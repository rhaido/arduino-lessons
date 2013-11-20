#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define FOSC 16000000
#define BAUD 19200                
#define MYUBRR FOSC/16/BAUD -1

void CTC_init(void);
void GPIO_init_input();
void GPIO_init_output();

int main(void){
  uint16_t cnt  = 0;
  uint8_t scnd = 0;
  int8_t  stt  = 0;
  uint8_t prev_diod = 0, cur_diod  = 0;

  GPIO_init_input();
  GPIO_init_output();

  CTC_init();

  while(1) {
    /* if the CTC timer equaled OC0RA compare register */
    loop_until_bit_is_set(TIFR0, OCF0A);

    cnt++;

    if (cnt == 999) PORTB |= _BV(PB5);
    /* if 1000 milliseconds was counted */
    else if (cnt == 1000) {
      PORTB &= ~(_BV(PB5));

      cnt = 0;

      /* PB0 is HIGH, clocks are turned over); */
      if (PINB & _BV(PB0)) stt = -1;
      else stt = 1;

      /* check limit conditions */
      if ( (stt == -1) && (scnd == 0) ) continue;
      if ( (stt == 1) && (scnd == 60) ) continue;

      /* add or remove 1 second from second counter depending on stt value */
      scnd += stt;

      if((scnd % 10) == 0) {
        cur_diod = scnd / 10;

        if (cur_diod == prev_diod) continue;

        /* turn off cur_diod */
        if (cur_diod > prev_diod)
          PORTD &= ~(1 << (1+cur_diod));
        else /* Turn on previous one */
          PORTD |= (1 << (1+prev_diod));

        prev_diod = cur_diod;
      }
    }

    /* Reset the flag */
    TIFR0 |= _BV(OCF0A);
  }

  return 1;
}

void GPIO_init_input() {
  DDRB &= ~(_BV(PB0));
}

void GPIO_init_output() {
  DDRD |= _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
  DDRB |= _BV(PB5);

  PORTD |= _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
  PORTB &= ~(_BV(PB5));
}

void CTC_init(){
  /* Configure CTC mode without chaging the state of OC0A;
   * OCR0A defines TOP value;
   * OC0A - disconnected, normal port operation */
  TCCR0A |= _BV(WGM01);

  TCCR0A &= ~( _BV(COM0A1) | _BV(COM0A0) | _BV(WGM00) );

  /* Set compare register to 249 in order to have 250 ticks;
   * Clear the timer */
  OCR0A = 249;
  TCNT0 = 0;

  /* set the clock prescaler to 64 */
  TCCR0B |= _BV(CS01) | _BV(CS00);
}

