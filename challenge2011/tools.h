#ifndef TOOLS_H
#define TOOLS_H

#include <stm32f10x_conf.h>
#include <string.h>

uint32_t atoi(char* s);
int baseX2int(char c);
void itoa(uint16_t x, char* s);

void wait_us(int x);

#endif
