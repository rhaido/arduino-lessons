#include <avr/io.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define FOSC 16000000

#define PIN_ON_OFF PB1
#define PIN_CH_DIR PB0
#define PIN_BUTTON PINB

#define R_LEFT PD2 
#define R_RIGHT PD3
#define DRIVE PD6
#define OUT_P_DRIVE PORTD

void GPIO_init_input(void);
void GPIO_init_output(void);
void PWM_init(void);
void PWM_set_width_8(uint8_t);
void ADC_init(void);
uint16_t ADC_read(uint8_t);

int main(void) {
  uint16_t v = 0, speed = 0, prev_speed = 0;

  ADC_init();
  PWM_init();
  GPIO_init_input();
  GPIO_init_output();

  while(1){
    if(PIN_BUTTON & _BV(PIN_ON_OFF)){

      v = ADC_read(0);

      speed = v / 4;

      if (speed != prev_speed) {
        PWM_set_width_8((uint8_t)speed);
        prev_speed = speed;
      }

      if( !(PIN_BUTTON & _BV(PIN_CH_DIR) )){
        OUT_P_DRIVE &= ~(_BV(R_RIGHT)); 
        OUT_P_DRIVE |= (_BV(R_LEFT));
      } else {
        OUT_P_DRIVE &= ~(_BV(R_LEFT)); 
        OUT_P_DRIVE |= (_BV(R_RIGHT));
      }

      DDRD |= _BV(DRIVE);
      PORTB |= _BV(PB5);

    } else {
      if ( DDRD & _BV(DRIVE) ) {

        PORTB &= ~(_BV(PB5));

        DDRD &= ~(_BV(DRIVE)); 
        OUT_P_DRIVE &= ~(_BV(R_LEFT) | _BV(R_RIGHT));
      }
    }
  }
}

void GPIO_init_input(void) {
  DDRB &= ~( _BV(PIN_ON_OFF)|_BV(PIN_CH_DIR) );
}

void GPIO_init_output(void) {
  DDRB |= _BV(PB5);

  DDRD |= _BV(PD2) | _BV(PD3);

  PORTD &= ~( _BV(PD2)|_BV(PD3) );
}

void PWM_init(void) {
 /* Configure wave-form generator mode: fast-PWM - for OC0A */
  TCCR0A |= _BV(WGM01) | _BV(WGM00);

  /* Set non-inverting mode for OC0A*/
  TCCR0A |= _BV(COM0A1);
  TCCR0A &= ~(_BV(COM0A0));

  OCR0A = 0;
  TCNT0 = 0;

  /* No prescaling */
  TCCR0B |= _BV(CS00);
}

void PWM_set_width_8(uint8_t width) {
  OCR0A = width;
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

