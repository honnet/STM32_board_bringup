#ifndef UART1_H
#define UART1_H

void vUART1_init();
void USART1_IRQHandler();
char uart1_getc();
void uart1_gets(char* s);
void uart1_putc(char c);
void uart1_puts(char* s);
void vUART1Task();

#endif

