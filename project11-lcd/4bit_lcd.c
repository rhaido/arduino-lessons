#include <util/delay.h>
#include "4bit_lcd.h"

void lcd_init(void){
  /* define pins as OUTPUT ports in the corresponding DDRs */
  LCD_DDR_D4 = 1;
  LCD_DDR_D5 = 1;
  LCD_DDR_D6 = 1;
  LCD_DDR_D7 = 1;
  LCD_DDR_RS = 1;
  LCD_DDR_EN = 1;

  LCD_EN = 0;
  LCD_RS = 0; /* Send Commands */

  /* run delay after power-on for at least 15ms */
  _delay_ms(15);

  lcd_nibble(0x30); /* 0b00110000 - sets the DD RAM address */
  _delay_ms(4.5);

  lcd_nibble(0x30);
  _delay_ms(1);

  lcd_nibble(0x30);
  _delay_us(50);

  lcd_nibble(0x20); /* 0b00100000 - set the CG RAM address, enable 4-bit mode */
  _delay_us(50);

  lcd_command(0x28);    /* 0b00101000 - 4-bit width, 2 lines, 5x7 dots each symbol */
  lcd_command(0x08);    /* 0b00001000 - tunr off display, the cursor, and blink
                           of the character on the cursor position */
  lcd_command(0x01);    /* Clear display, requires ~2 ms delay */
  lcd_command(0x06);    /* Cursor moves and moves right*/
  lcd_command(0x0C);    /* 0b00001100 - display on; no cursor, no blinking; */
}

void lcd_nibble(uint8_t n){
  LCD_D4 = (n & 1 << 4) ? 1 : 0;
  LCD_D5 = (n & 1 << 5) ? 1 : 0;
  LCD_D6 = (n & 1 << 6) ? 1 : 0;
  LCD_D7 = (n & 1 << 7) ? 1 : 0;

  LCD_EN = 1;
  _delay_us(1);
  LCD_EN = 0;
}

void lcd_byte(uint8_t b){
  lcd_nibble(b);
  lcd_nibble(b << 4);
  _delay_us(50);
}

void lcd_command(uint8_t d){
  LCD_RS = 0;

  lcd_byte(d);

  if (d <= 3)
    _delay_ms(2);
}

void lcd_putc(uint8_t c){
  LCD_RS = 1;

  lcd_byte(c);
}

void lcd_puts(char *s){
  while(*s != '\0'){
    lcd_putc(*s);
    s++;
  }
}

void lcd_overwrite(char *s){
  lcd_command(0x01);

  lcd_puts(s);
}

