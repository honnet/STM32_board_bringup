#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

#include "led.h"
#include "lcd.h"
#include "button.h"
#include "xbee.h"
#include "buzzer.h"
#include "adc.h"

#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include <stm32f10x_conf.h>


/*
~/TPfreeRTOS/STM32/STM32_USB-FS-Device_Lib_V3.1.0/Libraries/CMSIS/Core/CM3/stm32f10x.h
*/

void hardware_init();

#endif

