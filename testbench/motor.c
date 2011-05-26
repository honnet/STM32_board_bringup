#include "motor.h"
#include "utils.h"
#include "lcd.h"
#include "led.h"

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


static int8_t currentAngle;
xQueueHandle xQueueAngleDelay;

typedef struct
{
    int8_t angle;
    uint16_t delay;
} xAngleDelay_t;

//////////////////////////////////////////////////////////////
void vMotorInit()
{
  GPIOB->CRH |= GPIO_CRH_MODE15_0;
  GPIOB->CRH |= GPIO_CRH_MODE15_1;
  GPIOB->CRH &= ~GPIO_CRH_CNF15_0;
  GPIOB->CRH |= GPIO_CRH_CNF15_1;

  // PWM mode 2:
  TIM1->CCMR2 |= TIM_CCMR2_OC3M_0;
  TIM1->CCMR2 |= TIM_CCMR2_OC3M_1;
  TIM1->CCMR2 |= TIM_CCMR2_OC3M_2;

  // Enable preload:
  TIM1->CCMR2 &= ~TIM_CCMR2_OC3PE;
  TIM1->CCMR2 |= TIM_CCMR2_OC3PE;
  // Update generation:
  TIM1->EGR &= ~TIM_EGR_UG;
  TIM1->EGR |= TIM_EGR_UG;
  // Enable output:
  TIM1->CCER |= TIM_CCER_CC3NE;
  TIM1->BDTR |= TIM_BDTR_MOE;

  // upcounting
  TIM1->CR1 &= ~TIM_CR1_DIR;
  // edge-aligned mode
  TIM1->CR1 &= ~TIM_CR1_CMS_1 & ~TIM_CR1_CMS_0;

  // Set frequency:
  TIM1->ARR = 0xffff;

  TIM1->PSC = 21;
  TIM1->CCR3 = 62914;

  TIM1->CR1 |= TIM_CR1_CEN;
  TIM1->EGR |= TIM_EGR_UG;

  // Assume we'll never go slower than V_MAX/16 :
  xQueueAngleDelay = xQueueCreate(2*MAX_ANGLE*16, sizeof(xAngleDelay_t));
  currentAngle = 0;
  vMotorSetPosition(currentAngle, V_MAX);
}

//////////////////////////////////////////////////////////////
void prvMotorAngle2PWM(int degres)
{
//  We made a linear regression on the experimental values to obtain a function
    const int a = 296758;
    const int b = 61601;
    const int precision = 10000;
    int result = (a * degres) / precision + b;

    TIM1->CCR3 = result;
}

//////////////////////////////////////////////////////////////
// Queue feeder: set angle to reach and angular velocity to use:
void vMotorSetPosition(int a, int v)
{
    // angle saturation if the command is insignificant
    int8_t command = (a>MAX_ANGLE)? MAX_ANGLE :
                     (a<MIN_ANGLE)? MIN_ANGLE : a;
    int8_t tmp, step = ANGLE_STEP;
    uint16_t delay;
    xAngleDelay_t ad;

    // flush the queue:
    while ( xQueueReceive(xQueueAngleDelay, &ad, 0) == pdTRUE );

    // discard insignificant values
    if (v >= V_MAX || v <= 0)
    {
        prvMotorAngle2PWM(command);
    }
    else
    {
        delay = 1000 / v; // 1000 because we use ms
        if (command < currentAngle)
            step = -step;

        tmp = currentAngle;
        while (command != tmp)
        {
            tmp += step;
            ad.angle = tmp;
            ad.delay = delay;
            xQueueSendToBack(xQueueAngleDelay, &ad, portMAX_DELAY);
        }
    }
}


//////////////////////////////////////////////////////////////
void vMotorTask() // queue consumer (call the angle setter)
{
    xAngleDelay_t ad;

    for (;;)
    {
        xQueueReceive(xQueueAngleDelay, &ad, portMAX_DELAY);
        prvMotorAngle2PWM(ad.angle);
        vTaskDelay(ad.delay);
        currentAngle = ad.angle; 
    }
}

