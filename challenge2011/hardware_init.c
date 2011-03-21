#include "hardware_init.h"

void hardware_init()
{
    // set HSE L (default CR = 0x0000 XX83)
    RCC->CR         |=  RCC_CR_HSEON;                           // External high-speed clock enabled
    while ( !(RCC_CR_HSERDY & RCC->CR) );                       // wait for the clock to be stable

    // set PLL (default CFGR = 0)
    RCC->CFGR       |=  RCC_CFGR_PLLSRC;                        // HSE oscillator clock selected as PLL input clock
    RCC->CFGR       |=  RCC_CFGR_PLLMULL9;                      // multiply the 8MHz by 9
    RCC->CR         |=  RCC_CR_PLLON;                           // enable the PLL
    while ( !(RCC_CR_PLLRDY & RCC->CR) );                       // wait for the PLL to be stable
    FLASH->ACR      =  (FLASH->ACR & 0xFFFFFFF8) | 2; // FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2; // Prefetch Buffer Enable + Latency 2 (wait state)

    RCC->CFGR       |=  RCC_CFGR_SW_PLL;                        // PLL used as system clock
    RCC->CR         &= ~RCC_CR_HSION;                           // disable HSI clock

    // set APB (Advanced Peripheral Bus, default: 0)
    RCC->CFGR       |=  RCC_CFGR_PPRE1_DIV2;                    // APB low-speed prescaler: HCLK divided by 2 to not exceed 36 MHz
    RCC->APB2ENR    |=  RCC_APB2ENR_IOPAEN;                     // enable clock for port A
    RCC->APB2ENR    |=  RCC_APB2ENR_IOPBEN;                     // enable clock for port B
    RCC->APB2ENR    |=  RCC_APB2ENR_IOPCEN;                     // enable clock for port C

    RCC->APB2ENR    |=  RCC_APB2ENR_AFIOEN;                     // alternate function clock enable
    RCC->APB1ENR    |=  RCC_APB1ENR_TIM3EN;                     // timer3 clock enable

    SysTick->CTRL   |=  SysTick_CTRL_CLKSOURCE;                 // OS clock

//    SCB->VTOR       =   SCB_VTOR_TBLBASE;                       // Set vector table base in RAM (0x20000000)
    SCB->AIRCR      =   (0x5FA<<16) | SCB_AIRCR_PRIGROUP3;      // IRQ priority: 4bit pre-emption priority, 4 bits subpriority
    
    led_init();
    lcd_init();
    but_init();
    xbee_init();
    buzzer_init();
    //adc_init();
}


