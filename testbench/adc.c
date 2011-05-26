#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "stm32f10x.h"
#include "adc.h"
#include "lcd.h"
#include "utils.h"

#define MS_TO_TICKS(time_ms) ((portTickType)((time_ms) / portTICK_RATE_MS))
#define ABS(x) (((x) < 0) ? (-(x)) : (x))

#define ADC_M60 663
#define ADC_0   1617 
#define ADC_P60 2622


static xQueueHandle xADCQueue;

/////////////////////////////////////////////////////////////////
void ADC1_2_IRQHandler(void)
{
  signed portBASE_TYPE xHigherPriorityTaskWoken;
  uint16_t in_buffer = ADC1->DR & 0x0000ffff;

  // Put the data in the queue
  xQueueSendToBackFromISR(xADCQueue, &in_buffer, &xHigherPriorityTaskWoken);

  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/////////////////////////////////////////////////////////////////
void adc_init_io()
{
  // Enable IO clock for port A
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  // Enable AFIO clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  // Set PA5 (ADC) as floating input
  GPIOA->CRL &= 0xff0fffff;
  GPIOA->CRL |= 0x00400000;

  // Enable ADC1 clock
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  // Create the ADC queue
  xADCQueue = xQueueCreate(8, sizeof (uint16_t));
}

/////////////////////////////////////////////////////////////////
void adc_init()
{
  // Turn the ADC on
  ADC1->CR2 = ADC_CR2_ADON;
 
  // Wait until it stabilizes
  vTaskDelay(MS_TO_TICKS(1));
  
  // Calibrate
  ADC1->CR2 |= ADC_CR2_RSTCAL;
  while (ADC1->CR2 & ADC_CR2_RSTCAL);
  
  ADC1->CR2 |= ADC_CR2_CAL;
  while (ADC1->CR2 & ADC_CR2_CAL);

  // Configure channel sequence length = 1 channel
  ADC1->SQR1 &= ~ADC_SQR1_L;

  // Configure first channel as channel 5
  ADC1->SQR3 &= ~ADC_SQR3_SQ1;
  ADC1->SQR3 |= 0x00000005;

  // Enable EOC interrupt
  ADC1->CR1     |= ADC_CR1_EOCIE;
  NVIC->ISER[0] |= NVIC_ISER_SETENA_18;
  NVIC->IP[18]   = configKERNEL_INTERRUPT_PRIORITY - 1;
}

/////////////////////////////////////////////////////////////////
unsigned short adc_read()
{
  uint16_t in_buffer;

  // Start the measure
  ADC1->CR2 |= ADC_CR2_ADON;

  // Get new data from the queue
  xQueueReceive(xADCQueue, &in_buffer, portMAX_DELAY);

  return in_buffer; 
}

/////////////////////////////////////////////////////////////////
int adc_to_angle()
{
//  We made a linear regression on the experimental values to obtain a function
    int32_t angle = 0;
    
    for (int i = 0; i < 128; i++)
        angle += adc_read();

    angle /= 128;

    const int32_t a = 6586;
    const int32_t b = 15547345;
    const int32_t precision = 100000;
    angle = (a * angle - b) / precision;

    return angle;
}

/////////////////////////////////////////////////////////////////
void vADCTask()
{
    int adc_value;
    char adc_str[16] = "Mesure(deg):    ";
    adc_init();

    for (;;)
    {
        adc_value = adc_to_angle();
        // display angle on 2nd line of LCD:
        itoa(adc_value, adc_str+13);

        vLCDPuts(adc_str, LCD_LINE2, 0, 0);
        vTaskDelay(10);        
    }
}
