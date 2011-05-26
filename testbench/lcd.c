#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "task.h"

#include "button.h"
#include "lcd.h"
#include "led.h"

// FIXME: Deal with busy flag.
// FIXME: remove useless eol arguments.
// FIXME: Remanent messages queueing with each others.
// FIXME: Message expiration, "f=...", and typical receive frequency.
// FIXME: One cannot be able to flood the LCDQueue and DOS the program.

static int iPriority;
static xQueueHandle xLCDQueue;
static xQueueHandle xLCDShiftQueue1;
static xQueueHandle xLCDShiftQueue2;
static xSemaphoreHandle xLCDMutex;
static xSemaphoreHandle xLCDMutex1;
static xSemaphoreHandle xLCDMutex2;
static int iLCDBacklightLevel;
static int iLCDCurrentLine;
static char cLCDCurrentAddress = 0;
static char current_line1[50] = {' '};
static char current_line2[50] = {' '};
static int line1_len = 0;
static int line2_len = 0;

static void lcd_init();
static void lcd_reset();
static void lcd_start();
static void lcd_sample();
static void lcd_data(char data);
static void lcd_control(char rs, char rw);
static void lcd_command(short command);
//static void lcd_puts(const char* str, char eol);
static void lcd_shift_wait();
//static void lcd_shift_toggle();
static void lcd_set_position(int line, int pos);
static void lcd_clear_screen();

void lcd_init()
{
  iPriority = LCD_PRIORITY_LOW;
  //iDrawing = 0;

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

  lcd_reset();

  // LCD BACKLIGHT:
  GPIOA->CRL |= GPIO_CRL_MODE6_0;
  GPIOA->CRL |= GPIO_CRL_MODE6_1;
  GPIOA->CRL &= ~GPIO_CRL_CNF6_0;
  GPIOA->CRL |= GPIO_CRL_CNF6_1;

  // Set frequency:
  TIM3->ARR = 0xffff;
  // Set duty cycle:
  TIM3->CCR1 = 0x0fff;

  // PWM mode 2:
  TIM3->CCMR1 |= TIM_CCMR1_OC1M_0;
  TIM3->CCMR1 |= TIM_CCMR1_OC1M_1;
  TIM3->CCMR1 |= TIM_CCMR1_OC1M_2;

  // Enable preload:
  TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
  // Update generation:
  TIM3->EGR |= TIM_EGR_UG;
  // Enable output:
  TIM3->CCER |= TIM_CCER_CC1E;
  // Enable autoreload:
  TIM3->CR1 |= TIM_CR1_ARPE;
  // Enable TIMER3:
  TIM3->CR1 |= TIM_CR1_CEN;

  iLCDBacklightLevel = LCD_BL_STEPS - 1;
}

void lcd_reset()
{
  GPIOA->BRR = GPIO_BRR_BR4 | 0xf;
  GPIOC->BRR = GPIO_BRR_BR14 | GPIO_BRR_BR15;
}

void lcd_start()
{
  /*-----------------.
  | Init procedure.  |
  `-----------------*/

  vTaskDelay(45);
  // Function set (Interface is 8 bits long):
  lcd_control(0, 0);
  lcd_data(LCD_INIT_TOKEN);
  vTaskDelay(5);
  lcd_data(LCD_INIT_TOKEN);
  vTaskDelay(1);
  lcd_data(LCD_INIT_TOKEN);
  // Function set (Set interface to be 4 bits long):
  lcd_data(LCD_INIT_4BITS);

  /*-----------.
  | Settings.  |
  `-----------*/

  // Function set (Interface is 4 bits long. Specify the
  // number of display lines and character font.):
  lcd_command(LCD_FUNCTION_SET);
  lcd_command(LCD_DISPLAY_OFF);
  lcd_command(LCD_DISPLAY_CLEAR);
  lcd_command(LCD_ENTRY_MODE);
  lcd_command(LCD_DISPLAY_ON);
}

void lcd_sample()
{
  // Sample on falling edge:
  GPIOA->BSRR = GPIO_BSRR_BS4; // E = 1.
  vTaskDelay(1);
  GPIOA->BRR = GPIO_BRR_BR4; // E = 0.
  vTaskDelay(1);
}

void lcd_data(char data)
{
  GPIOA->BSRR = (data & 0x0f) | (((~data) & 0x0f) << 16);
  vTaskDelay(1);
  lcd_sample();
}

void lcd_control(char rs, char rw)
{
  GPIOC->BSRR =
    (rs ? GPIO_BSRR_BS15 : GPIO_BSRR_BR15)
    | (rw ? GPIO_BSRR_BS14 : GPIO_BSRR_BR14);
}

void lcd_command(short command)
{
  if (!command)
    return;

  if (command & LCD_SET_ADDRESS)
    cLCDCurrentAddress = command & 0x7f;

  const char rs = (command >> 9) & 1;
  const char rw = (command >> 8) & 1;
  const char data1 = (char)((command & 0xf0) >> 4);
  const char data2 = (char)((command & 0x0f) >> 0);
  lcd_control(rs, rw);
  lcd_data(data1);
  lcd_data(data2);
}

void lcd_set_position(int line, int pos)
{
  const short line_address = line;
  const short position_command =
    LCD_SET_ADDRESS | ((line_address + pos) & 0x7f) | iPriority;
  xQueueSendToBack(xLCDQueue, &position_command, portMAX_DELAY);
  iLCDCurrentLine = line;
}

void lcd_shift_wait()
{
  const short wait_command =
    LCD_SHIFT_WAIT | iPriority;
  xQueueSendToBack(xLCDQueue, &wait_command, portMAX_DELAY);
}

void lcd_shift_toggle()
{
  const short shift_command =
    LCD_SHIFT_TOGGLE | iPriority;
  xQueueSendToBack(xLCDQueue, &shift_command, portMAX_DELAY);
}

void lcd_puts(const char* str, char eol)
{
  short write_command;
  int i = 0, shift = 0, len = 0;

  // FIXME: may be optimized out...
  while (str[len++] != eol);

  while (str[i] != eol)
  {
    write_command = LCD_CHAR_WRITE | str[i] | iPriority;
    xQueueSendToBack(xLCDQueue, &write_command, portMAX_DELAY);
    i++;
    if (i - shift >= LCD_LINE_SIZE && len - shift > LCD_LINE_SIZE )
    {
      lcd_shift_wait();
      lcd_set_position(iLCDCurrentLine, 0);
      i = shift++;
    }
  }

  if (len > LCD_LINE_SIZE)
  {
    return;
  }

  for (int j = i; j < LCD_LINE_SIZE; j++)
  {
    write_command = LCD_CHAR_WRITE | ' ' | iPriority;
    xQueueSendToBack(xLCDQueue, &write_command, portMAX_DELAY);
  }
}

void lcd_clear_screen()
{
  short command = LCD_DISPLAY_CLEAR | iPriority;
  xQueueSendToBack(xLCDQueue, &command, portMAX_DELAY);
}

void lcd_backlight_level(int level, int menu)
{
  iLCDBacklightLevel = level;
  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM3->CCR1 = (LCD_BL_STEPS - level)
    * (LCD_BL_MAX_PWM - LCD_BL_MIN_PWM)
    / (LCD_BL_STEPS - 1) + LCD_BL_MIN_PWM;
  TIM3->CR1 |= TIM_CR1_CEN;
}

void vLCDInit()
{
  xLCDQueue = xQueueCreate(128, sizeof(short));
  xLCDShiftQueue1 = xQueueCreate(64, sizeof(short));
  xLCDShiftQueue2 = xQueueCreate(64, sizeof(short));
  xLCDMutex = xSemaphoreCreateMutex();
  xLCDMutex1 = xSemaphoreCreateMutex();
  xLCDMutex2 = xSemaphoreCreateMutex();
  memset(current_line1, ' ', sizeof current_line1);
  memset(current_line2, ' ', sizeof current_line2);
  lcd_init();
}

void vLCDBacklightLevel(int level, int show_progressbar)
{
  lcd_backlight_level(level, show_progressbar);
}

void vLCDBacklightInc(int show_progressbar)
{
  int new = iLCDBacklightLevel + 1;
  lcd_backlight_level(
    (new < LCD_BL_STEPS) ? new : LCD_BL_STEPS, show_progressbar);
}

void vLCDBacklightDec(int show_progressbar)
{
  int new = iLCDBacklightLevel - 1;
  lcd_backlight_level((new >= 0) ? new : 0, show_progressbar);
}

void vLCDClearScreen()
{
  xSemaphoreTake(xLCDMutex1, portMAX_DELAY);
  xSemaphoreTake(xLCDMutex2, portMAX_DELAY);
  lcd_clear_screen();
  xSemaphoreGive(xLCDMutex2);
  xSemaphoreGive(xLCDMutex1);
}

void vLCDClearLine(int line)
{
  char fill[17] = {0};
  for (int i = 0; i < 16; i++)
    fill[i] = ' ';
  vLCDPuts(fill, line, 0, 0);
}

void vLCDPuts(const char* str, int line,
                  int pos, char eol)
{
  if (line == LCD_LINE1)
  {
    memset(current_line1, ' ', sizeof current_line1);
    strncpy(current_line1, str, strlen(str));
    line1_len = strlen(str);
  }
  else
  {
    memset(current_line2, ' ', sizeof current_line2);
    strncpy(current_line2, str, strlen(str));
    line2_len = strlen(str);
  }
}

#define LCD_TASK(N)                                             \
  void vLCDShiftTask##N(void *pvParameters)                     \
  {                                                             \
    int idx = -1;                                               \
    for (;;)                                                    \
    {                                                           \
      xSemaphoreTake(xLCDMutex, portMAX_DELAY);                 \
      idx++;                                                    \
      int start = idx * 16;                                     \
      if (start >= line##N##_len)                               \
        idx = start = 0;                                        \
      lcd_set_position((N) == 1 ? LCD_LINE1 : LCD_LINE2, 0);    \
      for (int i = start; i < start + 16; i++)                  \
      {                                                         \
        short command = LCD_CHAR_WRITE | current_line##N[i];    \
        xQueueSendToBack(xLCDQueue, &command, portMAX_DELAY);   \
      }                                                         \
      xSemaphoreGive(xLCDMutex);                                \
      vTaskDelay(1000);                                         \
    }                                                           \
  }

LCD_TASK(1)
LCD_TASK(2)

void vLCDWriteTask(void *pvParameters)
{
  short command;
  lcd_start();

  for (;;)
  {
    xQueueReceive(xLCDQueue, &command, portMAX_DELAY);
    if (command & LCD_SHIFT_WAIT)
      ;
    else
      lcd_command(command); // Eventually execute command
    vTaskDelay(10);
  }
}
