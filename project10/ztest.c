#include <avr/io.h>
#include <string.h>
#include <stdlib.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define FOSC 16000000
#define BAUD 19200                
#define MYUBRR FOSC/16/BAUD -1

#define PIN_ON_OFF PB1
#define PIN_CH_DIR PB0
#define PIN_BUTTON PINB

#define R_LEFT PD2 
#define R_RIGHT PD3
#define DRIVE PD6
#define OUT_P_DRIVE PORTD

void GPIO_init_input(void);
void GPIO_init_output(void);

int main(void) {
  GPIO_init_input();
  GPIO_init_output();

  PORTD &= ~(_BV(PD2)); 
  PORTD |= (_BV(PD3));

  while(1){
    if(PIN_BUTTON & _BV(PIN_ON_OFF)){
      PORTD |= _BV(PD6);

      PORTB |= _BV(PB5);
    } else {
      PORTD &= ~(_BV(PD6));
     
      PORTB &= ~(_BV(PB5));

    }
  }
}

void GPIO_init_input(void) {
  DDRB &= ~( _BV(PIN_ON_OFF)|_BV(PIN_CH_DIR) );
}

void GPIO_init_output(void) {
  DDRB |= _BV(PB5);
  DDRD |= _BV(PD2) | _BV(PD3) | _BV(PD6);

  PORTD &= ~( _BV(PD2) | _BV(PD3) | _BV(PD6) );
}

