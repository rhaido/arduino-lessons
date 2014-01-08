#include <avr/io.h>

#include "knocks.h"
#include "user_pinout.h"

void PWM16_init(void){
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
  PWM_PIN_DDR = 1;
}

