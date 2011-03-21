#ifndef XBEE_H
#define XBEE_H

#include <stm32f10x_conf.h>
#include <FreeRTOS.h>
#include "semphr.h"
#include "queue.h"
#include "lcd.h"
#include "led.h"
#include "buzzer.h"
#include <string.h>
#include "tools.h"

extern xLCD_message xMessage;
extern xQueueHandle xLCDQueue;


#define SLOT_SIZE 50
#define SLOT_QTTY 33
char slots[SLOT_QTTY][SLOT_SIZE];

xSemaphoreHandle mutex;
xQueueHandle xQueueT;
xQueueHandle xQueueR;

void xbee_init();
void USART3_IRQHandler();

char xbee_getc();
void xbee_gets(char* s);
void xbee_puts(char* s);

void xbee_hello();
void prvXbeeTask();

void execute(char* s);



#endif

