#include "led.h"

void led_init()
{
    // GPIO at 2 MHz 
    GPIOA->CRL |= GPIO_CRL_MODE7_1;    // PA7 red TIM3 channel 2
    GPIOA->CRL &= ~GPIO_CRL_MODE7_0;

    GPIOB->CRL |= GPIO_CRL_MODE0_1;    // PB0 green TIM3 channel 3
    GPIOB->CRL &= ~GPIO_CRL_MODE0_0;

    GPIOB->CRL |= GPIO_CRL_MODE1_1;    // PB1 blue TIM3 channel 4
    GPIOB->CRL &= ~GPIO_CRL_MODE1_0;

    // GPIO in alternate push-pull mode :
    GPIOA->CRL |= GPIO_CRL_CNF7_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF7_0;

    GPIOB->CRL |= GPIO_CRL_CNF0_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF0_0;

    GPIOB->CRL |= GPIO_CRL_CNF1_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF1_0;

    led_timers_init();
}

///////////////////////////////////////////////////////////////
void led_timers_init()
{
    TIM3->CR1         &= ~TIM_CR1_CEN;                            // Stop timer
    // Set PWM mode 1 for TIM3 and enable preload channels 2,3,4:
    TIM3->CCMR1       |= TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;
    TIM3->CCMR2       |= TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE;
    TIM3->CCMR2       |= TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE;
    TIM3->CR1         |= TIM_CR1_ARPE;                            // Enable auto-reload preload register
    TIM3->PSC         = 0xFFFF;                                   // Divide clock (clock prescaler)
    TIM3->ARR         = 0x12ED;                                   // auto reload
    TIM3->CCR2        = 0;                                        // compare register
    TIM3->CCR3        = 0;                                        // compare register
    TIM3->CCR4        = 0;                                        // compare register
    TIM3->EGR         |= TIM_EGR_UG;

    TIM3->CCER        |= TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;// Enable the OC1
    TIM3->CR1         |= TIM_CR1_CEN;                             // Start timer
}

///////////////////////////////////////////////////////////////
void led(char color, char state)
{
    switch(color)
    {
        case R:
            GPIOA->BSRR |= ~state ? GPIO_BSRR_BR7 : GPIO_BSRR_BS7; // RED ON
            GPIOB->BSRR |=  state ? GPIO_BSRR_BR0 : GPIO_BSRR_BS0;
            GPIOB->BSRR |=  state ? GPIO_BSRR_BR1 : GPIO_BSRR_BS1;
            break;
        case G:
            GPIOA->BSRR |=  state ? GPIO_BSRR_BR7 : GPIO_BSRR_BS7;
            GPIOB->BSRR |= ~state ? GPIO_BSRR_BR0 : GPIO_BSRR_BS0; // GREEN ON
            GPIOB->BSRR |=  state ? GPIO_BSRR_BR1 : GPIO_BSRR_BS1;
            break;
        case B:
            GPIOA->BSRR |=  state ? GPIO_BSRR_BR7 : GPIO_BSRR_BS7;
            GPIOB->BSRR |=  state ? GPIO_BSRR_BR0 : GPIO_BSRR_BS0;
            GPIOB->BSRR |= ~state ? GPIO_BSRR_BR1 : GPIO_BSRR_BS1; // BLUE ON
            break;
        default:                                                   // ALL OFF !
            GPIOA->BSRR |=  state ? GPIO_BSRR_BR7 : GPIO_BSRR_BS7;
            GPIOB->BSRR |=  state ? GPIO_BSRR_BR0 : GPIO_BSRR_BS0;
            GPIOB->BSRR |=  state ? GPIO_BSRR_BR1 : GPIO_BSRR_BS1;
    }
}

///////////////////////////////////////////////////////////////
void led_parse(char * string)
{
    // string 0LRRGGBB
    TIM3->CCR2 = ((baseX2int(string[2])<<4) + baseX2int(string[3]));
    TIM3->CCR3 = ((baseX2int(string[4])<<4) + baseX2int(string[5]));
    TIM3->CCR4 = ((baseX2int(string[6])<<4) + baseX2int(string[7]));
}

///////////////////////////////////////////////////////////////
void prvLEDTask()
{
    while (1)
    {
        led_parse("0L00GG00");
        vTaskDelay( 500 ); //ms
        led_parse("0LFF0000");
        vTaskDelay( 500 ); //ms
    }
}

