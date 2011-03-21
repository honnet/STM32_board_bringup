#ifndef BUZZER_H
#define BUZZER_H

#include <stm32f10x_conf.h>
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "adc.h"

char buz_on;

void buzzer_init();
void buzzer_freq(uint16_t freq);
void buzzer_state(char state);
void buzzer_adc_volume();


#endif

