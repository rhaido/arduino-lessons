#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

int main(void){
  DDRB |= (1<<PB5);
  unsigned char i=0;

  while(1){
    for(i=0;i<=2;i++){
      PORTB = (1<<PB5);
      _delay_ms(300);
      PORTB &= ~(1<<PB5);
      _delay_ms(300);
    }

    for(i=0;i<=2;i++){
      PORTB = (1<<PB5);
      _delay_ms(1000);
      PORTB &= ~(1<<PB5);
      _delay_ms(1000);
    }

    for(i=0;i<=2;i++){
      PORTB = (1<<PB5);
      _delay_ms(300);
      PORTB &= ~(1<<PB5);
      _delay_ms(300);
    }

    _delay_ms(5000);
  }

  return 1;
}
