PROGRAM = main

# One of "ram" or "flash"
MODE = flash

# One of "hd" or "md"
DENSITY = md

# Object files # led.o lcd.o buzzer.o switch.o zigbee.o 
OBJS = hardware_init.o led.o lcd.o button.o xbee.o buzzer.o adc.o tools.o main.o \
	   port.o queue.o tasks.o list.o heap_1.o # FreeRTOS files

# Uncomment to allow debugging
# CFLAGS += -DDEBUG

include ../Makefile.inc
