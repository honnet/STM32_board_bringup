#ifndef _ADC_H_
#define _ADC_H_

void adc_init_io();

void adc_init();
uint16_t adc_read();
int adc_to_angle();
void vADCTask();

#endif
