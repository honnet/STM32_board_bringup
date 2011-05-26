#include "led.h"
#include "stm32f10x.h"
#include "task.h"

#include "FreeRTOS.h"
#include "queue.h"

static xQueueHandle xLedsQueue;
static int iLedsFlickerStarted = 0;
static int iLedOn[3] = {0};

void vLedsInit()
{
  // LED R:
  GPIOA->CRL |= GPIO_CRL_MODE7_0;
  GPIOA->CRL |= GPIO_CRL_MODE7_1;
  GPIOA->CRL &= ~GPIO_CRL_CNF7_0;
  GPIOA->CRL |= GPIO_CRL_CNF7_1;
  // PWM mode 2:
  TIM3->CCMR1 |= TIM_CCMR1_OC2M_0;
  TIM3->CCMR1 |= TIM_CCMR1_OC2M_1;
  TIM3->CCMR1 |= TIM_CCMR1_OC2M_2;
  // Enable preload:
  TIM3->CCMR1 |= TIM_CCMR1_OC2PE;
  // Set duty cycle:
  TIM3->CCER |= TIM_CCER_CC2E;
  TIM3->CCR2 = LED_MAX_CNT;

  // LED G:
  GPIOB->CRL |= GPIO_CRL_MODE0_0;
  GPIOB->CRL |= GPIO_CRL_MODE0_1;
  GPIOB->CRL &= ~GPIO_CRL_CNF0_0;
  GPIOB->CRL |= GPIO_CRL_CNF0_1;
  // PWM mode 2:
  TIM3->CCMR2 |= TIM_CCMR2_OC3M_0;
  TIM3->CCMR2 |= TIM_CCMR2_OC3M_1;
  TIM3->CCMR2 |= TIM_CCMR2_OC3M_2;
  // Enable preload:
  TIM3->CCMR2 |= TIM_CCMR2_OC3PE;
  // Set duty cycle:
  TIM3->CCER |= TIM_CCER_CC3E;
  TIM3->CCR3 = LED_MAX_CNT;

  // LED B:
  GPIOB->CRL |= GPIO_CRL_MODE1_0;
  GPIOB->CRL |= GPIO_CRL_MODE1_1;
  GPIOB->CRL &= ~GPIO_CRL_CNF1_0;
  GPIOB->CRL |= GPIO_CRL_CNF1_1;
  // PWM mode 2:
  TIM3->CCMR2 |= TIM_CCMR2_OC4M_0;
  TIM3->CCMR2 |= TIM_CCMR2_OC4M_1;
  TIM3->CCMR2 |= TIM_CCMR2_OC4M_2;
  // Enable preload:
  TIM3->CCMR2 |= TIM_CCMR2_OC4PE;
  // Set duty cycle:
  TIM3->CCER |= TIM_CCER_CC4E;
  TIM3->CCR4 = LED_MAX_CNT;

  vLedsOff();

  // Set frequency:
  TIM3->ARR = 0xffff;
  // Update generation:
  TIM3->EGR |= TIM_EGR_UG;
  // Enable autoreload:
  TIM3->CR1 |= TIM_CR1_ARPE;
  // Enable TIMER3:
  TIM3->CR1 |= TIM_CR1_CEN;

  xLedsQueue = xQueueCreate(128, sizeof(enum LED));
}

void vLedIntensity(enum LED l, int value)
{
  if (value == 0)
    iLedOn[l] = 0;
  else
    iLedOn[l] = 1;

  switch (l)
  {
    case RED:
      TIM3->CCR2 = value * LED_MAX_CNT / 0xff;
      break;
    case GREEN:
      TIM3->CCR3 = value * LED_MAX_CNT / 0xff;
      break;
    case BLUE:
      TIM3->CCR4 = value * LED_MAX_CNT / 0xff;
      break;
  }
  TIM3->EGR |= TIM_EGR_UG;
}

void vLedOn(enum LED l)
{
  vLedIntensity(l, 0xff);
}

void vLedOff(enum LED l)
{
  vLedIntensity(l, 0);
}

void vLedToggle(enum LED l)
{
  if (iLedOn[l])
    vLedOff(l);
  else
    vLedOn(l);
}

void vLedsOff()
{
  vLedOff(RED);
  vLedOff(GREEN);
  vLedOff(BLUE);
}

void vLedFlicker(enum LED led)
{
  portBASE_TYPE reschedNeeded = pdFALSE;
  if (iLedsFlickerStarted)
  {
    xQueueSendFromISR(xLedsQueue, &led, &reschedNeeded);
    portEND_SWITCHING_ISR(reschedNeeded);
  }
}

void vLedsFlickerTask()
{
  enum LED led;
  iLedsFlickerStarted = 1;
  for(;;)
  {
    xQueueReceive(xLedsQueue, &led, portMAX_DELAY);
    vLedOn(led);
    vTaskDelay(LED_FLICKER_TIME_MS);
    vLedOff(led);
  }
}
