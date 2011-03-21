#include "lcd.h"


///////////////////////////////////////////////////////////////
void lcd_init()
{
    // LCD_D4:
    GPIOA->CRL &= ~GPIO_CRL_MODE0_0;
    GPIOA->CRL |= GPIO_CRL_MODE0_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF0_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF0_1;

    // LCD_D5:
    GPIOA->CRL &= ~GPIO_CRL_MODE1_0;
    GPIOA->CRL |= GPIO_CRL_MODE1_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF1_1;

    // LCD_D6:
    GPIOA->CRL &= ~GPIO_CRL_MODE2_0;
    GPIOA->CRL |= GPIO_CRL_MODE2_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_1;

    // LCD_D7:
    GPIOA->CRL &= ~GPIO_CRL_MODE3_0;
    GPIOA->CRL |= GPIO_CRL_MODE3_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF3_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF3_1;

    // LCD_E:
    GPIOA->CRL &= ~GPIO_CRL_MODE4_0;
    GPIOA->CRL |= GPIO_CRL_MODE4_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF4_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF4_1;

    // LCD_LED:
    GPIOA->CRL &= ~GPIO_CRL_MODE6_0;
    GPIOA->CRL |= GPIO_CRL_MODE6_1;// 2Mz
    GPIOA->CRL &= ~GPIO_CRL_CNF6_0;
    GPIOA->CRL |= GPIO_CRL_CNF6_1; // alternate push-pull

    // LCD_RW:
    GPIOC->CRH &= ~GPIO_CRH_MODE14_0;
    GPIOC->CRH |= GPIO_CRH_MODE14_1;
    GPIOC->CRH &= ~GPIO_CRH_CNF14_0;
    GPIOC->CRH &= ~GPIO_CRH_CNF14_1;

    // LCD_RS:
    GPIOC->CRH &= ~GPIO_CRH_MODE15_0;
    GPIOC->CRH |= GPIO_CRH_MODE15_1;
    GPIOC->CRH &= ~GPIO_CRH_CNF15_0;
    GPIOC->CRH &= ~GPIO_CRH_CNF15_1;

    timer_init();
    xLCDQueue = xQueueCreate(16, sizeof(xLCD_message));
}

///////////////////////////////////////////////////////////////
void reset()
{
    GPIOA->BRR = GPIO_BRR_BR4 | 0xf;
    GPIOC->BRR = GPIO_BRR_BR14 | GPIO_BRR_BR15; // RW | RS
}

///////////////////////////////////////////////////////////////
void lcd_start()
{
    reset();
    vTaskDelay(45);

    // Function set (Interface is 8 bits long):
    lcd_put(0, 0, 0x3);
    vTaskDelay(5);

    // Function set (Interface is 8 bits long):
    lcd_put(0, 0, 0x3);
    vTaskDelay(1);

    // Function set (Interface is 8 bits long):
    lcd_put(0, 0, 0x3);

    // Function set (Set interface to be 4 bits long):
    lcd_put(0, 0, 0x2);

    // Function set (Interface is 4 bits long. Specify the
    // number of display lines and character font.):
    lcd_put(0, 0, 0x2);
    lcd_put(0, 0, 0x8);
    // N = 1 <=> 2-line display
    // F = 0 <=> 5x8 dots

    // Display off:
    lcd_put(0, 0, 0x0);
    lcd_put(0, 0, 0x8);

    // Display clear:
    lcd_put(0, 0, 0x0);
    lcd_put(0, 0, 0x1);

    // Entry mode set:
    lcd_put(0, 0, 0x0);
    lcd_put(0, 0, 0x6);
    // I/D = 0 <=> Decrement
    // S = 1 <=> Shift

    // Display ON:
    lcd_put(0, 0, 0x0);
    lcd_put(0, 0, 0xc);
}

///////////////////////////////////////////////////////////////
void lcd_clear()
{
    // Display clear:
    lcd_put(0, 0, 0x0);
    lcd_put(0, 0, 0x1);
}

///////////////////////////////////////////////////////////////
void lcd_put(char rs, char rw, char data)
{
    GPIOA->BSRR = GPIO_BSRR_BS4; // E = 1.
    vTaskDelay(1);

    // Send data:
    GPIOC->BSRR = (rs ? GPIO_BSRR_BS15 : GPIO_BSRR_BR15)
        | (rw ? GPIO_BSRR_BS14 : GPIO_BSRR_BR14);

    GPIOA->BSRR = (data & 0xf) | (((~data) & 0xf) << 16);
    vTaskDelay(1);

    GPIOA->BRR = GPIO_BRR_BR4; // falling edge
    vTaskDelay(1);
}

///////////////////////////////////////////////////////////////
void lcd_set_line(uint8_t n)
{
    if (n == 1)
    {
        lcd_put(0, 0, 0x8);
        lcd_put(0, 0, 0);
    }
    else
    {
        lcd_put(0, 0, 0x8 | 0x2);
        lcd_put(0, 0, 0x8 | 0x7);
    }
}

///////////////////////////////////////////////////////////////
void lcd_puts(const char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        lcd_put(1, 0, (char)((str[i] >> 4) & 0xf));
        lcd_put(1, 0, (char)(str[i] & 0xf));
    }
}

///////////////////////////////////////////////////////////////
void timer_init()
{
    // Set PWM mode 1 for TIM3 and enable preload
    TIM3->CCMR1       |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    TIM3->CR1         |= TIM_CR1_ARPE;                            // Enable auto-reload preload register
    TIM3->PSC         = 59999;                                    // Divide clock by 60000 (clock prescaler)
    TIM3->ARR         = 19;                                       // 20 possible duty cycles (auto-reload reg)
    TIM3->CCR1        = 10;                                       // compare register, 50% by default
    TIM3->EGR         |= TIM_EGR_UG;
    TIM3->CCER        |= TIM_CCER_CC1E;                           // Enable the OC1
    TIM3->CR1         |= TIM_CR1_CEN;                             // Start timer
}

///////////////////////////////////////////////////////////////
void backlight(uint8_t val)
{
    TIM3->CCR1        =   val;                                    // duty cycle reg
}

///////////////////////////////////////////////////////////////
void prvLCDTask()
{
    lcd_start();

    while (1)
    {
        // read queue
        while( xQueueReceive(xLCDQueue, &xMessage, portMAX_DELAY) != pdPASS );

        // send to LCD
        lcd_set_line(xMessage.line);
        lcd_puts("                ");
        lcd_set_line(xMessage.line);
        lcd_puts(xMessage.string);

        taskYIELD();
    }
}

