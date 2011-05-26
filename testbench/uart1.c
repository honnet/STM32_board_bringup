#include "uart1.h"

#include <stm32f10x_conf.h>
#include <FreeRTOS.h>
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "led.h"


xSemaphoreHandle mutex;
xQueueHandle xQueueT;
xQueueHandle xQueueR;


//////////////////////////////////////////////////////////////
void vUART1_init()
{
    // Synchronization
	xQueueR          = xQueueCreate(16, sizeof(char));
	xQueueT          = xQueueCreate(16, sizeof(char));
	mutex            = xSemaphoreCreateMutex();

    // enable clock for USART1
    RCC->APB2ENR    |=  RCC_APB2ENR_USART1EN;

    // TX = PA9 output alternate push pull @2MHz, RX = PA10 floating input
	GPIOA->CRH      &= ~(GPIO_CRH_MODE9   | GPIO_CRH_CNF9   |
                         GPIO_CRH_MODE10  | GPIO_CRH_CNF10  );
    GPIOA->CRH      |=   GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1 |
                                            GPIO_CRH_CNF10_0 ;

    // UART convention: 115.2kbps & 8N1
    USART1->BRR      = 	0x0271;                         // (APB2clk=72MHz)
    USART1->CR1     |= 	USART_CR1_RE | USART_CR1_TE;    // Tx + Rx Enable
    USART1->CR1     |=  USART_CR1_UE | USART_CR1_RXNEIE;// Enable USART & Reception int

    // Enable external interrupts, see p183
    NVIC->ISER[1]   |=  NVIC_ISER_SETENA_5;

    // Nested vectored interrupt control, need a priority < 255 (=kernel priority)
    NVIC->IP[37]     =  254;
}

///////////////////////////////////////////////////////////////
void USART1_IRQHandler()
{
	char c;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if (USART_SR_RXNE & USART1->SR)                     // Read Data Reg Not Empty ?
	{
		c = USART1->DR & 0xFF;                          // read Data Register
		xQueueSendFromISR(xQueueR, &c, &xHigherPriorityTaskWoken);
	}
	else if (USART_SR_TXE & USART1->SR)                 // Transmit Data Reg Empty ?
	{
		if (xQueueReceiveFromISR(xQueueT, &c, &xHigherPriorityTaskWoken) == pdTRUE)
            USART1->DR   =  c;                          // send the char from the queue
		else
            USART1->CR1 &= ~USART_CR1_TXEIE;            // disable TXE interrupt
	} 
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

///////////////////////////////////////////////////////////////
char uart1_getc()
{
    char c;
	xQueueReceive(xQueueR, &c, portMAX_DELAY);
    return c;
}

///////////////////////////////////////////////////////////////
void uart1_gets(char* s)
{
    char c;
	do {
		c = uart1_getc();
		*s = c;
		s++;
	} while(c != '\r');

    *(--s) = '\0'; // end of string
}

///////////////////////////////////////////////////////////////
void uart1_putc(char c)
{
	xSemaphoreTake(mutex, portMAX_DELAY);
	xQueueSendToBack(xQueueT, &c, portMAX_DELAY);
	USART1->CR1 |= USART_CR1_TXEIE;                     // Enable TXE interruption
	xSemaphoreGive(mutex);
}

///////////////////////////////////////////////////////////////
void uart1_puts(char* s)
{
	xSemaphoreTake(mutex, portMAX_DELAY);
	while(*s != '\0')
    {
		xQueueSendToBack(xQueueT, s, portMAX_DELAY);
        s++;
    }
    USART1->CR1 |= USART_CR1_TXEIE;                     // Enable TXE interruption
	xSemaphoreGive(mutex);
}

