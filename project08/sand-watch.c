#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define FOSC 16000000
#define BAUD 19200                
#define MYUBRR FOSC/16/BAUD -1

void USART_init(uint16_t);
void USART_write(char *);
void USART_transmit(uint8_t);

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

  USART_init(MYUBRR);
  CTC_init();

  while(1) {
    /* if the CTC timer equaled OC0RA compare register */
    loop_until_bit_is_set(TIFR0, OCF0A);

    cnt++;

    /* if 1000 milliseconds was counted */
    if (cnt == 998) {
      PORTB |= _BV(PB5);

      cnt = 0;

      /* PB0 is HIGH, clocks are turned over); */
      if (PINB & _BV(PB0)) stt = -1;
      else stt = 1;

      _delay_ms(1);

      PORTB &= ~(_BV(PB5));

      /* check limit conditions */
      if ( (stt == -1) && (scnd == 0) ) continue;
      if ( (stt == 1) && (scnd == 60) ) continue;

      /* add or remove 1 second from second counter depending on stt value */
      scnd += stt;

      if((scnd % 10) == 0) {
        cur_diod = scnd / 10;

        if (cur_diod == prev_diod) continue;
        
        if (cur_diod > prev_diod) { /* turn on diod (cur_diod - 1) in from diod_array*/
          PORTD |= (1 << (1+cur_diod));
        }
        else PORTD &= ~(1 << (1+prev_diod)); // turn_off(prev_diod)

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
  PORTD = 0;
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

void USART_init(uint16_t ubrr){
  /* we can use UBRR0 register instead of UBRR0H & UBRR0L */
  UBRR0 = ubrr;

  /* 8 data bit, 1 stop bit, no parity check*/
  UCSR0C |= (3 << UCSZ00);

  /* Enable receiver & transmitter */
  UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
}

void USART_write(char *str){
  uint8_t i = 0;

  for(i = 0; str[i] != '\0'; i++)
    USART_transmit(str[i]);
}

void USART_transmit(uint8_t c){

  /* Wait until data register is empty */
  loop_until_bit_is_set(UCSR0A, UDRE0);
  
  UDR0 = c;
}

