#include "button.h"

static char paused = 0;

///////////////////////////////////////////////////////////////
void vButtonsInit()
{
    // Port PC13 input pull-up/pull-down (left button)
    GPIOC->CRH        &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOC->CRH        |=   GPIO_CRH_CNF13_1;
    GPIOC->BSRR       =    L_BUT; //pull-up
    // Port PB5 input pull-up/pull-down (right button)
    GPIOB->CRL        &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
    GPIOB->CRL        |=   GPIO_CRL_CNF5_1;
    GPIOB->BSRR       =    R_BUT; //pull-up

    // External interupt on ports B and C
    //PC13 = L_BUT
    AFIO->EXTICR[4-1] &=  ~AFIO_EXTICR4_EXTI13;
    AFIO->EXTICR[4-1] |=   AFIO_EXTICR4_EXTI13_PC;
    //PB5 = R_BUT
    AFIO->EXTICR[2-1] &=  ~AFIO_EXTICR2_EXTI5;
    AFIO->EXTICR[2-1] |=   AFIO_EXTICR2_EXTI5_PB;

    // falling edge interruption enable
    EXTI->FTSR        |=   R_BUT | L_BUT;

    // Enable external interrupts, see p183
    NVIC->ISER[0]     |=  NVIC_ISER_SETENA_23; // EXTI Line[9:5] (PB5 => EXTI9_5)
    NVIC->ISER[1]     |=  NVIC_ISER_SETENA_8;  // EXTI Line[15:10] (PC13 => EXTI15_10)

    // Nested vectored interrupt control, need a priority < 255 (=kernel priority)
    NVIC->IP[23]      =   254;
    NVIC->IP[8+32]    =   254; 

    // Interrupt mask
    EXTI->IMR         |= R_BUT | L_BUT;


    // Initialize left & right button semaphores (no need to inherit priority with mutex)
    vSemaphoreCreateBinary(l_button_sem);
    vSemaphoreCreateBinary(r_button_sem);

    // Take the semaphores that are initialized at 1 by default
    xSemaphoreTake(l_button_sem, portMAX_DELAY);
    xSemaphoreTake(r_button_sem, portMAX_DELAY);
}

///////////////////////////////////////////////////////////////
void EXTI15_10_IRQHandler() // LEFT BUTTON INTERRUPTION HANDLER
{
    static signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    // clear the pending register
    EXTI->PR = L_BUT;

    // release the semaphore
    xSemaphoreGiveFromISR(l_button_sem, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

///////////////////////////////////////////////////////////////
void EXTI9_5_IRQHandler() // RIGHT BUTTON INTERRUPTION HANDLER
{
    static signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    // clear the pending register
    EXTI->PR = R_BUT;
    
    // release the semaphore
    xSemaphoreGiveFromISR(r_button_sem, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

///////////////////////////////////////////////////////////////
void vButtonLeftTask()
{
    vLedToggle(RED);

    while(1)
    {
        xSemaphoreTake(l_button_sem, portMAX_DELAY);            // wait for a button interrupt
        vTaskDelay(DELAY);                                      // avoid bounces
        xSemaphoreTake(l_button_sem, 0);                        // cancel bounce period interruptions
        if ( !(GPIOC->IDR & GPIO_IDR_IDR13) )
        {
            vLedToggle(BLUE);
            vTaskDelay(100);
            vLedToggle(BLUE);
            paused = 0;                                        // unpause
        }
    }
}

///////////////////////////////////////////////////////////////
void vButtonRightTask()                                         // play with the LCD brightness
{
    /*
    char i = 4;
    backlight(i);
    while(1)
    {
        xSemaphoreTake(r_button_sem, portMAX_DELAY);            // wait for a button interrupt
        vTaskDelay(DELAY);                                      // avoid bounces
        xSemaphoreTake(r_button_sem, 0);                        // cancel bounce period interruptions
        if ( !(GPIOB->IDR & GPIO_IDR_IDR5) )
        {
            i = (i + 5) % 20;
            backlight(i);
        }
    }
    */
}

///////////////////////////////////////////////////////////////
char button_paused()
{
    return paused;
}

///////////////////////////////////////////////////////////////
void set_pause()
{
    paused = 1;
}
