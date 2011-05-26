#ifndef LCD_H
# define LCD_H

# include "FreeRTOS.h"
# include "task.h"

# define LCD_LINE1 0x000
# define LCD_LINE2 0x040
# define LCD_LINE_SIZE 16

// N = 1 <=> 2-line display
// F = 0 <=> 5x8 dots

# define LCD_INIT_TOKEN    0x003
# define LCD_INIT_4BITS    0x002
# define LCD_DISPLAY_CLEAR 0x001
# define LCD_RETURN_HOME   0x002
# define LCD_DISPLAY_ON    0x00c
# define LCD_DISPLAY_OFF   0x008
# define LCD_FUNCTION_SET  0x028
# define LCD_SET_ADDRESS   0x080
# define LCD_ENTRY_MODE    0x006
# define LCD_CHAR_WRITE    0x200

# define LCD_BL_MIN_PWM 0x1fff
# define LCD_BL_MAX_PWM 0x5fff
# define LCD_BL_STEPS   16

# define LCD_WAIT_TIME  0x800
# define LCD_SHIFT_TOGGLE  0x400
# define LCD_SHIFT_WAIT  0x1000
# define LCD_SHIFT_WAIT_TIME 200

# define LCD_PRIORITY_LOW 0
# define LCD_PRIORITY_HIGH 0x100

# define LCD_DELAY 300

void vLCDInit();
void vLCDBacklightLevel(int level, int show_progressbar);
void vLCDBacklightInc(int show_progressbar);
void vLCDBacklightDec(int show_progressbar);
void vLCDClearScreen();
void vLCDClearLine(int line);
void vLCDPuts(const char* str, int line, int pos, char eol);
void vLCDWriteTask(void *pvParameters);
void vLCDShiftTask1(void *pvParameters);
void vLCDShiftTask2(void *pvParameters);

#endif
