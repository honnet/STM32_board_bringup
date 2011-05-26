#ifndef BUTTON_H
#define BUTTON_H

#include <FreeRTOS.h>
#include "stm32f10x.h"
#include "semphr.h"
#include "task.h"
#include "lcd.h"
#include "led.h"
#include "buzzer.h"

#define L_BUT   GPIO_BSRR_BS13 // PC13
#define R_BUT   GPIO_BSRR_BS5  // PB5
#define DELAY   30

xSemaphoreHandle l_button_sem;
xSemaphoreHandle r_button_sem;

void vButtonsInit();
void EXTI9_5_IRQHandler();
void EXTI15_10_IRQHandler();
void vButtonLeftTask();
void vButtonRightTask();
char button_paused();
void set_pause();

#endif
