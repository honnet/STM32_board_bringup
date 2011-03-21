#include "adc.h"


///////////////////////////////////////////////////////////////
void adc_init()
{
    RCC->CFGR       =   RCC_CFGR_ADCPRE_DIV8;               // PCLK2 divided by 8
    RCC->APB2ENR    |=  RCC_APB2ENR_ADC1EN;                 // ADC 1 interface clock enable 
    ADC1->CR1       |=  ADC_CR1_SCAN;                       // Scan mode
    ADC1->CR2       |=  ADC_CR2_CAL | ADC_CR2_RSTCAL;       // reset & start calibration
    wait_us(10);
    //vTaskDelay(1);
    ADC1->SQR3      &=  ADC_SQR3_SQ1;                       // reset
    ADC1->SQR3      |=  5;                                  // channel5 
    ADC1->CR2       |=  ADC_CR2_CONT | ADC_CR2_ADON;        // set continuous mode, start 
    wait_us(10);
    //vTaskDelay(1);
    ADC1->CR2       |=  ADC_CR2_ADON; //turn on conversion    
}

///////////////////////////////////////////////////////////////
uint16_t adc_get()
{
	return ( (ADC1->DR) & 0xFFF ); // 12 bits precision
}


// WORK IN PROGRESS !!!
