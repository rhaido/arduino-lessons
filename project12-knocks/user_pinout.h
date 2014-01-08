#ifndef __user_pinout_h__
#define __user_pinout_h__

#include <avr/io.h>

struct bits {
  uint8_t b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
} __attribute__((__packed__));

#define SBIT_(port,pin) (((volatile struct bits*)&port)->b##pin)
#define SBIT(x,y) SBIT_(x,y)

#define LED_RED  SBIT(PORTD, PD5)
#define LED_YEL  SBIT(PORTD, PD4)
#define LED_GRN  SBIT(PORTD, PD3)
#define LOCK_BTN SBIT(PIND, PD2)

#define PWM_PIN SBIT(PORTB, PB1)

#define LED_RED_DDR   SBIT(DDRD, PD5)
#define LED_YEL_DDR   SBIT(DDRD, PD4)
#define LED_GRN_DDR   SBIT(DDRD, PD3)
#define LOCK_BTN_DDR  SBIT(DDRD, PD2)

#define PWM_PIN_DDR   SBIT(DDRB, PB1)

#endif
