#include "hardware_init.h"
#include "button.h"
#include "lcd.h"
#include "led.h"
#include "stm32f10x.h"

void vHardwareInit()
{
  // Enable HSE:
  RCC->CR |= RCC_CR_HSEON;

  // Wait for HSE to be ready:
  while (!(RCC->CR & RCC_CR_HSERDY));

  // Disable PLL for configuration:
  RCC->CR &= ~RCC_CR_PLLON;

  // Set the PLL multiplicator to 9:
  RCC->CFGR |= (0x7 << 18);
  RCC->CFGR &= ~(1 << 21);
  //RCC->CFGR &= ~RCC_CFGR_PLLMULL9;

  // Set the PLL source to external clock:
  RCC->CFGR |= RCC_CFGR_PLLSRC;

  // Enable PLL:
  RCC->CR |= RCC_CR_PLLON;

  // Wait for PLL to be ready:
  while (!(RCC->CR & RCC_CR_PLLRDY));

  // Two wait states, if 48 MHz < SYSCLK <= 72 MHz:
  FLASH->ACR &= ~FLASH_ACR_LATENCY_0;
  FLASH->ACR |= FLASH_ACR_LATENCY_1;
  FLASH->ACR &= ~FLASH_ACR_LATENCY_2;

  // Set HSE as system clock:
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  // Disable HSI:
  RCC->CR &= ~RCC_CR_HSION;

  // Set APB high-speed prescaler (PPRE2), do not divide:
  RCC->CFGR &= ~RCC_CFGR_PPRE2_2 & ~RCC_CFGR_PPRE2_1 & ~RCC_CFGR_PPRE2_0;
  //RCC->CFGR &= ~(0x7 << 11);

  // Set APB low-speed prescaler (PPRE1), divide by 2:
  RCC->CFGR |= (0x4 << 8);
  RCC->CFGR &= ~(0x3 << 8);

  // Set AHB prescaler (HPRE), do not divide:
  RCC->CFGR &= ~(0xf << 4);

  // Enable clock for GPIO A, B and C:
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  // Enable clock for Alternate Function:
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  // Enable clock for TIMER3:
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  // Enable clock for TIMER1:
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  // 4 bits for groups, 4 bits for sub-groups:
  SCB->AIRCR = (0x5f << 16) | SCB_AIRCR_PRIGROUP3;
  // Write those simultaneously.

  // Set core clock as SYSTICK source:
  SysTick->CTRL |= SysTick_CTRL_CLKSOURCE;

  // Put vector interrupt table in RAM:
  //SCB->VTOR |= SCB_VTOR_TBLBASE;
}
