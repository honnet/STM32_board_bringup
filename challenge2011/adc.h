#ifndef ADC_H
#define ADC_H

#include <stm32f10x_conf.h>
#include <FreeRTOS.h>
#include "task.h"
#include <string.h>
#include "tools.h"

void adc_init();
uint16_t adc_get();

#endif

