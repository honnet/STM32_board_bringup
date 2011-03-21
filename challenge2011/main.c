#include "hardware_init.h"
#include <string.h>


///////////////////////////////////////////////////////////////
int main()
{
    hardware_init();
/*
    // create a LED task
    xTaskCreate( prvLEDTask, ( signed portCHAR * ) "LED",
            configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
*/
    // create a LCD task to display the queue content
    xTaskCreate( prvLCDTask, ( signed portCHAR * ) "LCD",
            configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    // create a Xbee task 
    xTaskCreate( prvXbeeTask, ( signed portCHAR * ) "prvXbeeTask",
            configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    // create left & right button tasks
    xTaskCreate( prvL_buttonTask, ( signed portCHAR * ) "L_button",
            configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
    xTaskCreate( prvR_buttonTask, ( signed portCHAR * ) "R_button",
            configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    // start the scheduler
    vTaskStartScheduler();
    return 0; 
}


