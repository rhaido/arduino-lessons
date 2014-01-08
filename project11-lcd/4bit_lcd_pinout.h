#ifndef __4bit_lcd_pinout_h__
#define __4bit_lcd_pinout_h__

#include <avr/io.h>

struct bits {
  uint8_t b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
} __attribute__((__packed__));

#define SBIT_(port,pin) (((volatile struct bits*)&port)->b##pin)
#define SBIT(x,y) SBIT_(x,y)

#define LCD_EN      SBIT(PORTB, PB3)
#define LCD_RS      SBIT(PORTB, PB4)

#define LCD_D4      SBIT(PORTD, PD5)
#define LCD_D5      SBIT(PORTD, PD4)
#define LCD_D6      SBIT(PORTD, PD3)
#define LCD_D7      SBIT(PORTD, PD2)

#define LCD_DDR_EN  SBIT(DDRB,  PB3)
#define LCD_DDR_RS  SBIT(DDRB,  PB4)

#define LCD_DDR_D4  SBIT(DDRD, PD5)
#define LCD_DDR_D5  SBIT(DDRD, PD4)
#define LCD_DDR_D6  SBIT(DDRD, PD3)
#define LCD_DDR_D7  SBIT(DDRD, PD2)

#define LCD_DDRAM   7
#define LCD_LINES   2

#endif
