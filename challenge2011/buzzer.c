#include "buzzer.h"

///////////////////////////////////////////////////////////////
void buzzer_init()
{
    RCC->APB2ENR    |=  RCC_APB2ENR_TIM1EN;                    // Timer 1 clock enable
    GPIOA->CRH      &=  ~(GPIO_CRH_CNF8 | GPIO_CRH_MODE8);     // reset reg
    GPIOA->CRH      |=  GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8;      // PA8 as push-pull output
    TIM1->CCMR1     |=  TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2    // Set PWM mode 1 for TIM3 and enable preload
                                         | TIM_CCMR1_OC1PE;
    TIM1->CR1       |=  TIM_CR1_ARPE;                          // Enable auto-reload preload register
    TIM1->PSC        =  0;                                     // to avoid hearing it by default (will be changed)
    TIM1->ARR        =  1000;                                  // 1000 possible duty cycles (auto-reload reg)
    TIM1->CCR1       =  500;                                   // compare register, 50% by default
    TIM1->EGR       |=  TIM_EGR_UG;                            // Update Generation
    TIM1->CCER      |=  TIM_CCER_CC1E;                         // Enable the OC1
//    TIM1->BDTR      |=  TIM_BDTR_OSSR | TIM_BDTR_OSSI          // Off-state selection for Run mode & Idle mode
//                                      | TIM_BDTR_MOE;          // Output enable
    TIM1->CR1       |=  TIM_CR1_CEN;                           // Start timer
    buz_on          =  1;                                      // buzzer on by default
}

///////////////////////////////////////////////////////////////
/*
principle: Fout = Fin / (ARR * PSC)

Fin = 72 000 000 Hz
ARR = 1000
CRR1 = 500

=> PSC = 72000/Fout
*/
void buzzer_freq(uint16_t freq)
{
    if(!freq)
        TIM1->BDTR      &= ~TIM_BDTR_MOE;                      // Output disable
    else 
    {
        TIM1->PSC        =  72000/freq;
        if (buz_on)
            TIM1->BDTR  |=  TIM_BDTR_MOE;                      // Output enable
    }
}

///////////////////////////////////////////////////////////////
void buzzer_state(char state) 
{
    buz_on = state;
    if (buz_on)
        TIM1->BDTR      |=  TIM_BDTR_MOE;                      // Output enable
    else
        TIM1->BDTR      &= ~TIM_BDTR_MOE;                      // Output disable
}

///////////////////////////////////////////////////////////////
void buzzer_adc_volume() 
{
    TIM1->CCR1 = adc_get() >> 3;
}



