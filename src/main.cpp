#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.c"
#include "encoder.c"

volatile uint16_t ms_counter = 0;
volatile bool show_tiago = true;

enum sexo_fernandez
{
  Manu1,
  velocidad,
  tempe,
  set_velo,
  set_tempe,
  programa_corriendo,
};
enum sexo_fernandez Menu = Manu1;
int main(void)
{
  // Configurar Timer0 CTC para llamar a lcd_task()
  TCCR0A = (1 << WGM01);  // modo CTC
  OCR0A = 99;             // ajuste para ~100us (depende de F_CPU)
  TIMSK0 = (1 << OCIE0A); // habilita interrupción
  TCCR0B = (1 << CS01);   // prescaler 8
  sei();                  // habilita interrupciones globales
  lcd_init();
  encoder_pins_init();  // <-- Inicializa los pines del encoder y el botón
  lcd_set_cursor(1, 0); // Primera línea
  lcd_print("VELOCIDAD");
  lcd_set_cursor(1, 1); // Segunda línea
  lcd_print("TEMPERATURA");
  _delay_ms(3);
  static uint8_t last_direction = 0xFF;
  while (1)
  {

    switch (Menu)
    {
    case Manu1:
    {
      uint8_t dir = read_encoder_direction();
      static uint8_t fresh = 1;
      if (fresh)
      {
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("VELOCIDAD");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("TEMPERATURA");
        fresh = 0;
      }

      if (dir != last_direction && dir != 0xFF)
      {
        last_direction = dir;
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("VELOCIDAD");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("TEMPERATURA");
        if (dir == 0)
        {
          lcd_set_cursor(0, 0); // Primera línea
          lcd_print(">");
        }
        else if (dir == 1)
        {
          lcd_set_cursor(0, 1); // Segunda línea
          lcd_print(">");
        }
      }
      if (read_botton1() == 1)
      {
        fresh = 1;
        if (last_direction == 0)
        {
          Menu = velocidad;
        }
        else
        {
          Menu = tempe;
        }
      }
    }
    break;
    case velocidad:
    {
      uint8_t dir = read_encoder_direction();
      static uint8_t fresh = 1;
      if (fresh)
      {
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("VELOCIDAD");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("ATRAS");
        fresh = 0;
      }
      if (dir != last_direction && dir != 0xFF)
      {
        last_direction = dir;
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("VELOCIDAD");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("ATRAS");
        if (dir == 0)
        {
          lcd_set_cursor(0, 0); // Primera línea
          lcd_print(">");
        }
        else if (dir == 1)
        {
          lcd_set_cursor(0, 1); // Segunda línea
          lcd_print(">");
        }
      }
      if (read_botton1() == 1)
      {
        fresh = 1;
        if (last_direction == 0)
        {
          Menu = Manu1;
        }
        else
        {
          Menu = Manu1;
        }
      }
    }
    break;
    case tempe:
    {
      uint8_t dir = read_encoder_direction();
      static uint8_t fresh = 1;
      if (fresh)
      {
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("TEMPERATURA");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("ATRAS");
        fresh = 0;
      }
      if (dir != last_direction && dir != 0xFF)
      {
        last_direction = dir;
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("TEMPERATURA");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("ATRAS");
        if (dir == 0)
        {
          lcd_set_cursor(0, 0); // Primera línea
          lcd_print(">");
        }
        else if (dir == 1)
        {
          lcd_set_cursor(0, 1); // Segunda línea
          lcd_print(">");
        }
      }
      if (read_botton1() == 1)
      {
        fresh = 1;
        if (last_direction == 0)
        {
          Menu = Manu1;
        }
        else
        {
          Menu = Manu1;
        }
      }
    }
    break;
    }
  }
}
ISR(TIMER0_COMPA_vect)
{
  lcd_task();
  button_edge_task();
  ms_counter++;
  if (ms_counter >= 10000)
  { // 500ms (100us * 5000 = 500ms)
    ms_counter = 0;
  }
}