#include <avr/io.h>
#include <util/delay.h>

int main(void){

  /* set dpin 2 to input; dpins 3,4,5 to output */
  DDRD &= ~(1<<PD2);
  DDRD |= (1<<PD3) | (1<<PD4) | (1<<PD5);

  /* set Arduino LED (13th pin) to output) */
  DDRB |= (1<<PB5);

  while(1){
    
    /* if dpin 2 sees LOW */
    if (! (PIND & (1<<PD2)) ) {

      /* turn-off onboard led */
      PORTB &= ~(1<<PB5);

      /* turn off red and yellow light */
      PORTD &= ~( (1<<PD4) | (1<<PD5) ) ;

      /* turn on green light - switch is off */
      PORTD |= (1<<PD3);
   
    } else {

      PORTB |= (1<<PB5);

      /* turn off green led */
      PORTD &= ~(1<<PD3);

      /* turn on and off yellow and red leds one after another */
      PORTD |= (1<<PD4);
      _delay_ms(100);

      PORTD &= ~(1<<PD4); 

      PORTD |= (1<<PD5);
      _delay_ms(100);

      PORTD &= ~(1<<PD5); 
    }
  }
  
  return 1;
}
