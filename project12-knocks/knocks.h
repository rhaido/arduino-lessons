#ifndef __knocks_h__
#define __knocks_h__
/* define the Fosc, and baud rate for UBRR calculation */
#define BAUD 19200                
#define MYUBRR F_CPU/16/BAUD - 1

/* cycle length 40 000 ticks of prescaled clocks */
#define CYCLE_LENGTH 39999
#define OCR_MIN 925 
#define OCR_MAX 4300
#define OCR_CENTER (OCR_MAX + OCR_MIN - 550)/2
#define ONE_DEGREE (OCR_MAX-OCR_MIN)/180

#define OPEN 1
#define CLOSED 0

#define PRESSED 1

void USART_init(uint16_t);
void USART_write(char *);
void USART_transmit(uint8_t);

void ADC_init(void);
uint16_t ADC_read(uint8_t);

void PWM16_init(void);

#endif
