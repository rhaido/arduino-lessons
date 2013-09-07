#include <avr/io.h>
#include <util/delay.h>

/* define the Fosc, and baud rate for UBRR calculation */
#define FOSC 16000000
#define BAUD 19200                
#define MYUBRR FOSC/16/BAUD -1

void USART_init(uint16_t);
void USART_write(char *);
void USART_transmit(uint8_t);

int main(void){
  USART_init(MYUBRR);
  char str[] = "The quick brown fox jumps over the lazy dog. 1234567890\r\n";

  while(1){
    USART_write(str);

    _delay_ms(1000);
  }

  return 1;
}

void USART_init(uint16_t ubrr){
  /* we can use UBRR0 register instead of UBRR0H & UBRR0L */
  UBRR0 = ubrr;

  /* 8 data bit, 1 stop bit, no parity check*/
  UCSR0C = (0 << USBS0) | (3 << UCSZ00);

  /* Enable received & transmitter */
  UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

void USART_write(char *str){
  uint8_t i = 0;

  for(i = 0; str[i] != '\0'; i++)
    USART_transmit(str[i]);
}

void USART_transmit(uint8_t c){

  /* Wait until data register is empty */
  while( !(UCSR0A & (1<<UDRE0)) )
    ;
  
  UDR0 = c;
}

