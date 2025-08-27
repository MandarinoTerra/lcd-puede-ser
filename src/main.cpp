#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.c"


int main(void)
{
  // Configurar Timer0 CTC para llamar a lcd_task()
  TCCR0A = (1 << WGM01);  // modo CTC
  OCR0A = 99;             // ajuste para ~100us (depende de F_CPU)
  TIMSK0 = (1 << OCIE0A); // habilita interrupción
  TCCR0B = (1 << CS01);   // prescaler 8
  sei();                  // habilita interrupciones globales
  lcd_init();
  while (1)
  {
  }
}

ISR(TIMER0_COMPA_vect)
{
  lcd_task();
}
