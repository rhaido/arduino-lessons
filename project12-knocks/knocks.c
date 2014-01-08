#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "knocks.h"
#include "user_pinout.h"

void door_close();
void door_open();
void flash_yellow_led();

void main(){
  char t_chr[7];
  uint32_t v = 0;
  uint8_t knocks = 0, door_state = OPEN;

  LOCK_BTN_DDR = 0;
  LED_RED_DDR = 1;
  LED_YEL_DDR = 1;
  LED_GRN_DDR = 1;

  LED_RED = 0;
  LED_YEL = 0;
  LED_GRN = 1;

  memset(t_chr, '\0', sizeof(t_chr));

  USART_init(MYUBRR);
  /* Remove garbage from serial terminal */
  USART_transmit('\r');

  ADC_init();
  PWM16_init();

  while(1){
    v = ADC_read(0);
    
    if (v > 50) {
      snprintf(t_chr,sizeof(t_chr),"%u\n\r",v);
      USART_write(t_chr);
    }

    if (v > 200) {
      if (knocks < 2)
        knocks++; 
      else {
        knocks = 0;

        if(door_state == CLOSED){
          door_state = OPEN;
          door_open();
        }
      }

      snprintf(t_chr,sizeof(t_chr),"k %u\n\r",knocks);
      USART_write(t_chr);

      flash_yellow_led();
      _delay_ms(80);
    }

    if(LOCK_BTN == PRESSED) {
      if(door_state == OPEN){
        door_state = CLOSED;
        door_close();
        knocks = 0;
      } else {
        door_state = OPEN;
        door_open();
      }

      _delay_ms(500);
    }
  }
}

void door_close(){
  OCR1A = OCR_MAX;
  LED_RED = 1;
  LED_GRN = 0;
}

void door_open(){
  OCR1A = OCR_CENTER;
  LED_RED = 0;
  LED_GRN = 1;
}

void flash_yellow_led(){
  LED_YEL = 1;
  _delay_ms(20);
  LED_YEL = 0;
}

