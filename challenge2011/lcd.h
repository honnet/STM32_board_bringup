#ifndef LCD_H
#define LCD_H

#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include <stm32f10x_conf.h>
#include <string.h>

// used to feed the queue
typedef struct {
    char line;
    char string[16]; // because of LCD width
} xLCD_message;


xLCD_message xMessage;
xQueueHandle xLCDQueue;


void lcd_init();
void reset();
void lcd_start();
void lcd_clear();
void lcd_put(char rs, char rw, char data);
void lcd_set_line(uint8_t n);
void lcd_puts(const char* str);
void timer_init();
void backlight(uint8_t val);
void prvLCDTask();

#endif

