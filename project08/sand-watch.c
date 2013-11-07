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

void ADC_init();
uint16_t ADC_read(uint8_t);

void CTC_init(void);

int main(void){
  uint16_t cnt  = 0;
  uint8_t scnd = 0;
  int8_t  stt  = 0;
  uint8_t prev_diod = 0, cur_diod  = 0;

  USART_init(MYUBRR);
  CTC_init();
  ADC_init();

  while(1) {
    loop_until_bit_is_set(TIFR0, OCF0A);

    cnt++;

    stt = ADC_read(0);

    if (stt) stt = 1;
    else stt = -1;

    if (cnt == 1000) {
      if ( (stt == -1) && (scnd == 0) )
        continue;
      scnd += stt;
      cnt = 0;
    }

    if((scnd % 10) == 0) {
      cur_diod = scnd / 10;

      if (cur_diod == prev_diod) continue;
      if (cur_diod > prev_diod) ; // turn_on(cur_diod);
      else ; // turn_off(prev_diod)

      prev_diod = cur_diod;
    }

    TIMSK0 |= _BV(OCF0A);
  }

  return 1;
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

void ADC_init(){
	/* unless otherwise configured, arduinos use the internal Vcc
	 * reference. MUX 0x0f samples the ground (0.0V) - safety setting. */
  ADMUX = _BV(REFS0) | 0x0f;
  DIDR0 = _BV(ADC0D);  /* Reduce 'digital' noise on analog pin by disabling digital input */

  /* Set divider of prescaler to 128 to receive 125 kHz *
   * Enable ADC */
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

  /* Do the 'warm up': first conversion occupies 26 cycles first;
   * each next only 13 */
  ADCSRA |= _BV(ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
}

uint16_t ADC_read(uint8_t pin){
  uint16_t ADC_val = 0;
  /* Preserve Previous REFS0 setting;
   * Clear previous MUX channels;
   * Set channel to read from; */
  ADMUX = (ADMUX & 0xf0) | pin;

  /* Start conversion - go-go-go! */
  ADCSRA |= _BV(ADSC);

  /* when the conversion is ended,  */
  loop_until_bit_is_clear(ADCSRA, ADSC);

  ADC_val = (uint16_t)ADCL; /* reading of ADCL blocks data registers ADCL/ADCH */
  ADC_val += (ADCH<<8);     /* reading of ADCH unblocks them again */

  return ADC_val;
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

