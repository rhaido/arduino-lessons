#include <avr/io.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define FOSC 16000000
#define BAUD 19200                

void GPIO_init_input();
void GPIO_init_output();

int main(void) {
  GPIO_init_input();
  GPIO_init_output();

  while(1){
    if(PIND & _BV(PD2))
      PORTB |= _BV(PB1);
    else
      PORTB &= ~(_BV(PB1));
  }
}

void GPIO_init_input() {
  DDRD &= ~(_BV(PD2));
}

void GPIO_init_output() {
  DDRB |= _BV(PB1);

  PORTB &= ~(_BV(PB1));
}

