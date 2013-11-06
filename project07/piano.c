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
  uint8_t f[5] = {239,213,189,179,0};
  uint8_t meandr[5] = {119,106,95,90,0};
  uint8_t tone = 4;
  char t_chr[7];
  uint16_t v = 0, v_old = 0;

  memset(t_chr, '\0', sizeof(t_chr));

  USART_init(MYUBRR);
  /* Remove garbage from serial terminal */
  USART_transmit('\r');

  ADC_init();
  PWM_init();

  while(1){
    v = ADC_read(0);  /* read from ADC0 */

    if (v != v_old) {
      v_old = v;

      snprintf(t_chr,sizeof(t_chr),"%u",v);

      USART_write(t_chr);
      USART_transmit('\n');
      USART_transmit('\r');
    }

    if (v > 1010) tone = 0;
    if (v < 1010 && v > 990) tone = 1;
    if (v > 500 && v < 520) tone = 2;
    if (v > 0 && v < 10) tone = 3;
    if (v == 0) tone = 4;

    OCR0A = f[tone];
    OCR0B = meandr[tone];

    _delay_ms(100);
  }

  return 1;
}

void PWM_init(){
  /* Configure wave-form generator mode: fast-PWM - for OC0x;
   * OCR0A defines TOP value; OCR0B - the compare value */
  TCCR0A |= _BV(WGM01) | _BV(WGM00);
  TCCR0B |= _BV(WGM02);

  /* Set non-inverting mode for OC0B; OCOA - disconnected*/
  TCCR0A |= _BV(COM0B1);
  TCCR0A &= ~(_BV(COM0B0));

  /* Clear the compare registers and timers */
  OCR0A = OCR0B = 0;

  TCNT0 = 0;

  /* set the clock prescaler to 256 */
  TCCR0B |= _BV(CS02);
  
  /*
	 * The setup of the OCnx should be performed before setting the Data
	 * Direction Register for the port pin to output.
	 *
   * Enable output drivers for OC0A & OC0B */
  DDRD |= _BV(PD5);
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

