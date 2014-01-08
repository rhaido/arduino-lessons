#include <util/delay.h>
#include "4bit_lcd.h"
#include <stdlib.h>

void init_timer();

void main(void) {
  int8_t reply;

  lcd_init();
  init_timer();

  lcd_puts("Ask the");
  lcd_xy(0,1);
  lcd_puts("Crystall Ball!");

  while(1){
    if(PIND & _BV(PD6)) {
      srand(TCNT0);

      reply = random() % 8;
    }

    if ( reply != -1 ) {
      switch(reply){
        case 0:
          lcd_overwrite("Yes");
          break;
        case 1:
          lcd_overwrite("Most likely");
          break;
        case 2:
          lcd_overwrite("Certainly");
          break;
        case 3:
          lcd_overwrite("Outlook good");
          break;
        case 4:
          lcd_overwrite("Unsure");
          break;
        case 5:
          lcd_overwrite("Ask again");
          break;
        case 6:
          lcd_overwrite("Doubtful");
          break;
        case 7:
          lcd_overwrite("No");
          break;
        default:
          break;
      }

      reply = -1;
    }
  }
}

void init_timer(void){
  TCNT0 = 0;

  /* No prescaling */
  TCCR0B |= _BV(CS00);
}
