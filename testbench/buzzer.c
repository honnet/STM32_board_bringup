#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lcd.h"
#include "buzzer.h"
#include "utils.h"

static xQueueHandle xBuzzerQueue;

#ifdef BUZZER_ENABLED
static void buzzer_frequency_set(int f);
#endif

void vBuzzerInit()
{
#ifdef BUZZER_ENABLED
  // LCD BACKLIGHT:
  GPIOA->CRH |= GPIO_CRH_MODE8_0;
  GPIOA->CRH |= GPIO_CRH_MODE8_1;
  GPIOA->CRH &= ~GPIO_CRH_CNF8_0;
  GPIOA->CRH |= GPIO_CRH_CNF8_1;

  // PWM mode 1:
  TIM1->CCMR1 &= ~TIM_CCMR1_OC1M_0;
  TIM1->CCMR1 |= TIM_CCMR1_OC1M_1;
  TIM1->CCMR1 |= TIM_CCMR1_OC1M_2;

  // Enable preload:
  TIM1->CCMR1 &= ~TIM_CCMR1_OC1PE;
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
  // Update generation:
  TIM1->EGR &= ~TIM_EGR_UG;
  TIM1->EGR |= TIM_EGR_UG;
  // Enable output:
  TIM1->CCER |= TIM_CCER_CC1E;
  TIM1->BDTR |= TIM_BDTR_MOE;
  // Enable autoreload:
  //TIM1->CR1 |= TIM_CR1_ARPE | TIM_CR1_DIR;

  // upcounting
  TIM1->CR1 &= ~TIM_CR1_DIR;
  // edge-aligned mode
  TIM1->CR1 &= ~TIM_CR1_CMS_1 & ~TIM_CR1_CMS_0;

  // Set frequency:
  TIM1->ARR = BUZZER_CNT_MAX;

  vBuzzerOff();
  buzzer_frequency_set(A3);
  vBuzzerVolumeSet(BUZZER_VOLUME_STEPS - 1, 0);
  xBuzzerQueue = xQueueCreate(256, sizeof(char));
#else
  xBuzzerQueue = xQueueCreate(0, sizeof(char));
#endif
}

void vBuzzerOn()
{
#ifdef BUZZER_ENABLED
  // Enable TIMER1:
  TIM1->CR1 |= TIM_CR1_CEN;
#endif
}

void vBuzzerOff()
{
#ifdef BUZZER_ENABLED
  // Disable TIMER1:
  TIM1->CR1 &= ~TIM_CR1_CEN;
#endif
}

void vBuzzerTask(void *pvParameters)
{
#ifdef BUZZER_ENABLED
# ifdef BUZZER_INIT_SOUND
  enum Notes startup[9] = {E4, E4f, E4, E4f, E4, B3, D4, C4, A3};
  for (int i = 0; i < 9; i++)
  {
    vBuzzerFrequencySet(startup[i]);
    vTaskDelay(70);
  }
  vBuzzerOff();
# endif

  char v;
  for (;;)
  {
    xQueueReceive(xBuzzerQueue, &v, portMAX_DELAY);
    //vLCDShowProgressbar("Buzzer Volume", (int)v);
  }
#else
  vTaskDelay(portMAX_DELAY);
#endif
}

void vBuzzerVolumeSet(int v, int show_progressbar)
{
#ifdef BUZZER_ENABLED
  //FIXME: check it works
  static int iBuzzerVolume = -1;
  if (iBuzzerVolume != v)
  {
    if (v >= BUZZER_VOLUME_STEPS)
      v = BUZZER_VOLUME_STEPS - 1;
    if (v < 0)
      v = 0;

    // Set duty cycle:
    //FIXME: see if "v" or "BUZZER_VOLUME_STEPS - v" ?!
    //FIXME: use logarithm
    TIM1->CCR1 = v * (BUZZER_MAX_PSC - BUZZER_MIN_PSC)
      / BUZZER_VOLUME_STEPS + BUZZER_MIN_PSC;

    /*if (show_progressbar)
      xQueueSend(xBuzzerQueue, &v, 10);*/

    iBuzzerVolume = (int)v;
  }
#endif
}

#ifdef BUZZER_ENABLED
void buzzer_frequency_set(int f)
{
  TIM1->PSC =
    ((72000000L / BUZZER_CNT_MAX) / f - 1)
    & TIM_PSC_PSC;
}
#endif

void vBuzzerFrequencySet(int f)
{
#ifdef BUZZER_ENABLED
  static int iBuzzerFrequency = -1;
  if (iBuzzerFrequency != f)
  {
    if (f == 0)
      vBuzzerOff();
    else
    {
      vBuzzerOn();
      iBuzzerFrequency = f;
      buzzer_frequency_set(f);
      TIM1->EGR |= TIM_EGR_UG;
    }
  }
#endif
}
