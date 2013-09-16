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

int main(void){
  uint16_t v = 0;
  double t = 0.0;
  char t_chr[7];


  DDRD |= _BV(PD3) | _BV(PD4) | _BV(PD5);
  /* Turn off all connected leds, if by
   * accident they are working */
  PORTD &= ~(_BV(PD3) | _BV(PD4) | _BV(PD5));

  USART_init(MYUBRR);
  /* Remove garbage from serial terminal */
  USART_transmit('\r');

  ADC_init();

  while(1){
    v = ADC_read(0);  /* read from ADC0 */

    memset(t_chr, '\0', sizeof(t_chr));

    t = (((double)v * 5.0) / 1024.0 - 0.5) * 100;
    dtostrf(t, 5, 1, t_chr);

    if(t < 20.0)
      PORTD = _BV(PD5) | (PORTD & ~(_BV(PD4) | _BV(PD3)));
    else {
      if (t >= 20.0 && t <= 24.0 )
        PORTD = _BV(PD4) | (PORTD & ~(_BV(PD5) | _BV(PD3)));
      else
        PORTD = _BV(PD3) | (PORTD & ~(_BV(PD5) | _BV(PD4)));
    }

    USART_write(t_chr);
    USART_write("\r\n");

    _delay_ms(700);
  }

  return 1;
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

