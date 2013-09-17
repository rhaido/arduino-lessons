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

void PWM_init();
void PWM_write(uint8_t, uint8_t);

int main(void){
  uint16_t v0,v1,v2;
  double t0,t1,t2;
  char t_chr[7];

  memset(t_chr, '\0', sizeof(t_chr));

  USART_init(MYUBRR);
  /* Remove garbage from serial terminal */
  USART_transmit('\r');

  ADC_init();
  PWM_init();

  while(1){
    v0 = ADC_read(0);  /* read from ADC0 */
    v1 = ADC_read(1);  /* read from ADC0 */
    v2 = ADC_read(2);  /* read from ADC0 */

    t0 = v0 / 4;
    t1 = v1 / 4;
    t2 = v2 / 4;

    dtostrf(t0, 5, 1, t_chr);
    USART_write("0: ");
    USART_write(t_chr);
    USART_write("\r\n");

    dtostrf(t1, 5, 1, t_chr);
    USART_write("1: ");
    USART_write(t_chr);
    USART_write("\r\n");

    dtostrf(t2, 5, 1, t_chr);
    USART_write("2: ");
    USART_write(t_chr);
    USART_write("\r\n");

    OCR0A = t0;
    OCR0B = t1;
    OCR2A = t2;

    _delay_ms(500);
  }

  return 1;
}

void PWM_init(){
  /* Configure wave-form generator mode: fast-PWM - for OC0x */
  TCCR0A |= _BV(WGM01) | _BV(WGM00);

  /* Set non-inverting mode for OC0A & OC0B*/
  TCCR0A |= _BV(COM0A1) | _BV(COM0B1);
  TCCR0A &= ~(_BV(COM0A0) | _BV(COM0B0));

  /* Configure wave-form generator mode: fast-PWM - for OC2x */
  TCCR2A |= _BV(WGM21) | _BV(WGM20);

  /* Set non-inverting mode for OC2 */
  TCCR2A |= _BV(COM2A1);
  TCCR2A &= ~(_BV(COM2A0));


  /* Clear the compare registers and timers */
  OCR0A = OCR0B = 0;
  OCR2A = 0;

  TCNT0 = 0;
  TCNT2 = 0;

  /* set the clock prescaling */
  TCCR0B |= _BV(CS02) | _BV(CS00);
  TCCR2B |= _BV(CS20);
  
  /*
	 * The setup of the OCnx should be performed before setting the Data
	 * Direction Register for the port pin to output.
	 *
   * Enable output driver for OC2A */
  DDRB |= _BV(PB3);
  /* Enable output drivers for OC0A & OC0B */
  DDRD |= _BV(PD6) | _BV(PD5);
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

