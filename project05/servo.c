#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define BAUD 19200                
#define MYUBRR F_CPU/16/BAUD - 1

/* cycle length 40 000 ticks of prescaled clocks */
#define CYCLE_LENGTH 39999
#define OCR_MIN 934 
#define OCR_MAX 4300
#define OCR_CENTER (OCR_MAX+OCR_MIN)/2
#define ONE_DEGREE (OCR_MAX-OCR_MIN)/180

void USART_init(uint16_t);
void USART_write(char *);
void USART_transmit(uint8_t);

void ADC_init();
uint16_t ADC_read(uint8_t);

void PWM16_init();

int main(void){
  char t_chr[7];
  uint32_t v=0;

  memset(t_chr, '\0', sizeof(t_chr));

  USART_init(MYUBRR);
  /* Remove garbage from serial terminal */
  USART_transmit('\r');

  ADC_init();
  PWM16_init();

  while(1){
    v = ADC_read(0);
    snprintf(t_chr,sizeof(t_chr),"%u",v);

    USART_write(t_chr);
    USART_transmit(' ');

    v = (v * 180) / 1024;

    snprintf(t_chr,sizeof(t_chr),"%u",v);
    USART_write(t_chr);
    USART_transmit('\n');
    USART_transmit('\r');

    OCR1A = OCR_MIN + v * ONE_DEGREE;

    snprintf(t_chr,sizeof(t_chr),"%u",OCR1A);
    USART_write(t_chr);
    USART_transmit('\n');
    USART_transmit('\r');

    _delay_ms(500);
  }

  return 1;
}

void PWM16_init(){
  /* Set non-inverting mode for OC1A & OC1B*/
  TCCR1A |= _BV(COM1A1) | _BV(COM1B1);
  TCCR1A &= ~(_BV(COM1A0) | _BV(COM1B0));

  /* Configure wave-form generator mode: 16bit fast-PWM - for OC1x */
  /* ICR1 defines the TOP value for TCNT1 */
  TCCR1A |= _BV(WGM11);
  TCCR1B |= _BV(WGM13) | _BV(WGM12);

  /* set ICR1 to default cycle length*/
  ICR1 = CYCLE_LENGTH;
  /* Clear the compare registers and timers */
  OCR1A = OCR_CENTER;
  TCNT1 = 0;

  /* set the clock prescaling: 8 is a divider */
  TCCR1B |= _BV(CS11);
  
  /*
	 * The setup of the OCnx should be performed before setting the Data
	 * Direction Register for the port pin to output.
	 * 
   * Enable output driver for OC1A */
  DDRB |= _BV(PB1);
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

