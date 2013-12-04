#include <avr/io.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define FOSC 16000000

void GPIO_init_input(void);
void GPIO_init_output(void);
void PWM_init(void);
void ADC_init(void);
uint16_t ADC_read(uint8_t);

int main(void) {
  GPIO_init_input();
  GPIO_init_output();

  while(1){
    ;
  }
}

void GPIO_init_input(void) {
  ;
}

void GPIO_init_output(void) {
  ;
}

void PWM_init(void) {
  ;
}

void ADC_init(void) {
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

