#ifndef __4bit_lcd_h__
#define __4bit_lcd_h__

#include "4bit_lcd_pinout.h"

void lcd_init(void);
void lcd_nibble(uint8_t);
void lcd_byte(uint8_t);
void lcd_command(uint8_t);
void lcd_putc(uint8_t);
void lcd_puts(char *);
void lcd_overwrite(char *);

#define lcd_xy(x,y) lcd_command( (1<<LCD_DDRAM) + (0x40*y) + x)

#endif
