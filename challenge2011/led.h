#ifndef LED_H
#define LED_H

#include<stm32f10x_conf.h>
#include <FreeRTOS.h>
#include "task.h"
#include "tools.h"

enum LED_COLOR {R,G,B,OFF};
enum LED_STATE {POS, NEG};

void led_init();
void led(char color, char state);
void led_parse(char * string);
void prvLEDTask();
void led_timers_init();

#endif

