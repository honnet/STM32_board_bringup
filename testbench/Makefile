PROGRAM = main

# One of "ram" or "flash"
MODE = flash

# One of "hd" or "md"
DENSITY = md

# Object files
OBJS = hardware_init.o main.o  \
       led.o lcd.o button.o version.o adc.o utils.o motor.o buzzer.o uart1.o \
       port.o queue.o tasks.o list.o heap_1.o

# Uncomment to allow debugging
# CFLAGS += -DDEBUG

include ../Makefile.inc

version.c::
	echo "const char* version = \"$(shell git log --format='%h' -n1)\";" > $@
