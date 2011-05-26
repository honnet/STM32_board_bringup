#include <string.h>

#include "FreeRTOS.h"
#include "stm32f10x.h"

#include "adc.h"
#include "button.h"
#include "buzzer.h"
#include "hardware_init.h"
#include "lcd.h"
#include "led.h"
#include "motor.h"
#include "queue.h"
#include "task.h"
#include "utils.h"
#include "uart1.h"

extern const char* version;

/////////////////////////////////////////////////////////////
void prvIdleTask(void *pvParameters)
{
  for (;;)
  {
    vLedToggle(RED);
    vTaskDelay(500);
  }
}

/////////////////////////////////////////////////////////////
void vUART2MotorTask()
{
    int angle = MIN_ANGLE-10;
    char angle_line[17] = "Command(deg):   ";
    char *angle_str = angle_line+13;

    for (;;)
    {
        // apply the command from the UART1:
        uart1_gets(angle_str);
        angle = atoi(angle_str);
        vMotorSetPosition(angle, V_MAX);

        // display angles on LCD:
        vLCDPuts(angle_line, LCD_LINE1, 0, 0);
    }
}

/////////////////////////////////////////////////////////////
void vADC2UART()
{
    adc_init();
    int adc_value;
    char adc_line[17] = "Mesure(deg):    ";
    char *adc_str = adc_line+13;

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const portTickType xPeriod = 50;

    for (;;)
    {
        // send the measure to the UART1
        adc_value = adc_to_angle();
        itoa(adc_value, adc_str);
        uart1_puts(adc_str);
        uart1_putc('\r');

        // display angles on LCD:
        vLCDPuts(adc_line,   LCD_LINE2, 0, 0);

        // wait to avoid sending to much data (synchro with sensorboard)
        vTaskDelayUntil( &xLastWakeTime, xPeriod );
    }
}

/////////////////////////////////////////////////////////////
int main(void)
{
  vHardwareInit();
  vLedsInit();
  vLCDInit();
//  vButtonsInit();
  vMotorInit();
  vUART1_init(); 
  adc_init_io();

  xTaskCreate(prvIdleTask,
              (signed portCHAR*) "Idle",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vLCDWriteTask,
              (signed portCHAR*) "LCDWrite",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 9, NULL);
  xTaskCreate(vLCDShiftTask1,
              (signed portCHAR*) "LCDWrite1",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 9, NULL);
  xTaskCreate(vLCDShiftTask2,
              (signed portCHAR*) "LCDWrite2",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 9, NULL);
/*
  xTaskCreate(vButtonLeftTask,
              (signed portCHAR*) "Button1",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vButtonRightTask,
              (signed portCHAR*) "Button2",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vADCTask,
              (signed portCHAR*) "ADC",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 4, NULL);
*/
  xTaskCreate(vMotorTask,
              (signed portCHAR*) "Motor",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 10, NULL);
  xTaskCreate(vUART2MotorTask,
              (signed portCHAR*) "UART to Motor",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 11, NULL);
  xTaskCreate(vADC2UART,
              (signed portCHAR*) "ADC to UART",
              configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 11, NULL);


  vTaskStartScheduler();

  return 0;
}
