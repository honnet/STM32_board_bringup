#ifndef LED_H
# define LED_H

# include "FreeRTOS.h"

# define LED_FLICKER_TIME_MS 10
# define LED_MAX_CNT 0xffff

enum LED { RED = 0, GREEN = 1, BLUE = 2 };

void vLedsInit();
void vLedOn(enum LED l);
void vLedOff(enum LED l);
void vLedToggle(enum LED l);
void vLedFlicker(enum LED l);
void vLedsOff();
void vLedsFlickerTask();
void vLedIntensity(enum LED l, int value);

#endif
