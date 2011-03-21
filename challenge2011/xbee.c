#include "xbee.h"

///////////////////////////////////////////////////////////////
void xbee_init()
{
    // Synchronization
    xQueueR          = xQueueCreate(16, sizeof(char));
    xQueueT          = xQueueCreate(16, sizeof(char));
    mutex = xSemaphoreCreateMutex();

    // enable clock for USART3
    RCC->APB1ENR    |=  RCC_APB1ENR_USART3EN;

    // configure GPIO for USART3: TX = PB10 output alternate push pull @2MHz, RX = PB11 floating input
    GPIOB->CRH      &= ~(GPIO_CRH_MODE10   | GPIO_CRH_CNF10   | GPIO_CRH_MODE11 | GPIO_CRH_CNF11  );
    GPIOB->CRH      |=   GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_1 |                   GPIO_CRH_CNF11_0 ;

    // UART settings for 9600bps & 8N1 (see procedure p.741)
    // 9.6kbps & Pclk=36MHz => Baud Rate Register = 234.375 (see pp. 747, 748 & 769)
    USART3->BRR      =     (uint16_t)(234.375 * 16);        // * 16 because fixed point representation: 12.4
    USART3->CR1     |=     USART_CR1_RE | USART_CR1_TE;     // Tx + Rx Enable
    USART3->CR1     |=  USART_CR1_UE | USART_CR1_RXNEIE;    // Enable USART & Reception interruption

    // Enable external interrupts, see p183
    NVIC->ISER[1]     |=  NVIC_ISER_SETENA_7;

    // Nested vectored interrupt control, need a priority < 255 (=kernel priority)
    NVIC->IP[39]      =   254;
}

///////////////////////////////////////////////////////////////
void USART3_IRQHandler()
{
    char c;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if (USART_SR_RXNE & USART3->SR)                     // Read Data Register Not Empty ?
    {
        c = USART3->DR & 0xFF;                          // read Data Register
        xQueueSendFromISR(xQueueR, &c, &xHigherPriorityTaskWoken); // http://www.freertos.org/a00119.html
    }
    else
    if (USART_SR_TXE & USART3->SR)                      // Transmit Data Register Empty ?
    {
        if (xQueueReceiveFromISR(xQueueT, &c, &xHigherPriorityTaskWoken) == pdTRUE) // www.freertos.org/a00120.html
            USART3->DR   =  c;                          // send the char from the queue
        else
            USART3->CR1 &= ~USART_CR1_TXEIE;            // disable TXE interrupt
    } 
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

///////////////////////////////////////////////////////////////
char xbee_getc()
{
    char c;
    xQueueReceive(xQueueR, &c, portMAX_DELAY);
    return c;
}

///////////////////////////////////////////////////////////////
void xbee_gets(char* s)
{
    char c;
    do {
        c = xbee_getc();
        *s = c;
        s++;
    } while( (c != '\r') );
}

///////////////////////////////////////////////////////////////
void xbee_puts(char* s)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    while(*s != '\0')
    {
        xQueueSendToBack(xQueueT, s, portMAX_DELAY);
        s++;
    }
    USART3->CR1 |= USART_CR1_TXEIE;                     // Enable TXE interruption
    xSemaphoreGive(mutex);
}

///////////////////////////////////////////////////////////////
void xbee_hello()
{
    led_parse("0L00FF00");

    vTaskDelay(1000);
    xbee_puts("+++");                                   // enter into Command Mode (p25)
    while (xbee_getc() != '\r');                        // wait for OK

    xbee_puts("ATMY4348\r");                            // 43 48 = C H (Cédric Honnet)
   // xbee_puts("ATDH0000\r");                            // Destination address 
    xbee_puts("ATDL4343\r");                            // Destination address 
    while (xbee_getc() != '\r');                        // wait for OK

    xbee_puts("ATCN\r");                                // exit command mode
    while (xbee_getc() != '\r');                        // wait for OK
    vTaskDelay(500);

    char c;
    while ( xQueueReceive(xQueueR, &c, 0) == pdTRUE );
    
    xbee_puts("\r");                                    // Ack
    led_parse("0LFF0000");
}


///////////////////////////////////////////////////////////////
void prvXbeeTask()
{ 
    char c, ack[] = " \r";

    xbee_hello();
    xbee_puts("\rSTART\r");

    led_parse("0L0000FF");
    while (1)
    {
        c = xbee_getc();
        xbee_gets( slots[c-'0'] );
        ack[0] = c;
        xbee_puts(ack);
        
        if(c == '0')
            execute( slots[c-'0'] );

        vTaskDelay(40);
    }
}
























///////////////////////////////////////////////////////////////
void execute(char* s)
{
    while(*s != '\r')
    {
        switch(*s++)
        {
            case 'P':
                xbee_puts("P\r");
                break;

            case 'L':
                // RRGGBB 
                led_parse(s);
                s+=6;
                break;

            case 'X':
                execute(slots[*s-'0']);
                s++;
                break;

            case 'U':
                xMessage.line = 1;
                strcpy(xMessage.string, s);
                xQueueSendToBack( xLCDQueue, &xMessage, portMAX_DELAY );
                break;

            case 'D':
                xMessage.line = 2;
                strcpy(xMessage.string, s);
                xQueueSendToBack( xLCDQueue, &xMessage, portMAX_DELAY );
                break;
        }
    }

}


