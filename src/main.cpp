#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.c"
#include "encoder.c"
#include "motores.c"

volatile uint16_t ms_counter = 0;
volatile bool show_tiago = true;

enum sexo_fernandez
{
  Manu,
  inicio,
  config,
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

  // variables default tempe velo
  int temperatura = 50;
  int velocimetro = 1;
  // variables default motores
  motores_init();
  uint8_t seq_state = 8;
  const uint8_t DIR_FWD = 1;
  const uint8_t DIR_REV = 0;
  const uint32_t SPEED = 800; // pasos por segundo (ajusta)
  const uint32_t ANG0 = 120;  // grados para motor 0
  const uint32_t ANG1 = 20;   // grados para motor 1

  while (1)
  {

  
    switch (Menu)
    {
    case Manu:
    {
      uint8_t dir = read_encoder_direction();
      static uint8_t fresh = 1;
      if (fresh)
      {
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("Inicio");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("Configuracion");
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
        fresh = 0;
      }

      if (dir != last_direction && dir != 0xFF)
      {
        last_direction = dir;
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        lcd_print("Inicio");
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("Configuracion");
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
          Menu = inicio;
        }
        else
        {
          Menu = Manu1;
        }
      }
    }

    break;
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
      static char strwachin[16];
      static uint8_t fresh = 1;
      if (fresh)
      {
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
        fresh = 0;
      }
      if (dir != last_direction && dir != 0xFF)
      {
        last_direction = dir;
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        sprintf(strwachin, "VELOCIDAD:%d", velocimetro);
        lcd_print(strwachin);
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
          Menu = set_velo;
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
      static char strwachin[16];
      if (fresh)
      {
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        sprintf(strwachin, "TEMPERATURA:%d", temperatura);
        lcd_print(strwachin);
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
        fresh = 0;
      }
      if (dir != last_direction && dir != 0xFF)
      {
        last_direction = dir;
        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        sprintf(strwachin, "TEMPERATURA:%d", temperatura);
        lcd_print(strwachin);
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
          Menu = set_tempe;
        }
        else
        {
          Menu = Manu1;
        }
      }
    }
    break;
    case set_tempe:
    {
      static char strwachin[16];
      temperatura = encoder_update();
      if (ms_counter >= 3000) // Actualiza
      {
        ms_counter = 0;

        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        sprintf(strwachin, "TEMPERATURA:%d", temperatura);
        lcd_print(strwachin);
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("ATRAS");
      }
      if (read_botton1() == 1)
      {
        Menu = tempe;
      }
    }
    break;
    case set_velo:
    {
      static char strwachin[16];
      velocimetro = encoder_update();
      if (ms_counter >= 3000) // Actualiza
      {
        ms_counter = 0;

        lcd_cmd(0x01);        // Limpia pantalla
        lcd_set_cursor(1, 0); // Primera línea
        sprintf(strwachin, "VELOCIDAD:%d", velocimetro);
        lcd_print(strwachin);
        lcd_set_cursor(1, 1); // Segunda línea
        lcd_print("ATRAS");
      }
      if (read_botton1() == 1)
      {
        Menu = velocidad;
      }
    }
    break;
    }

    
    switch (seq_state)
    {
    case 0: // iniciar motor0 adelante
      if (motor_move(0, ANG0, DIR_FWD, SPEED))
        seq_state = 1;
      break;

    case 1: // esperar fin motor0 adelante
      if (!motor_is_busy(0))
        seq_state = 2;
      break;

    case 2: // iniciar motor0 vuelta
      if (motor_move(0, ANG0, DIR_REV, SPEED))
        seq_state = 3;
      break;

    case 3: // esperar fin motor0 vuelta
      if (!motor_is_busy(0))
        seq_state = 4;
      break;

    case 4: // iniciar motor1 adelante
      if (motor_move(1, ANG1, DIR_FWD, SPEED))
        seq_state = 5;
      break;

    case 5: // esperar fin motor1 adelante
      if (!motor_is_busy(1))
        seq_state = 6;
      break;

    case 6: // iniciar motor1 vuelta
      if (motor_move(1, ANG1, DIR_REV, SPEED))
        seq_state = 7;
      break;

    case 7: // esperar fin motor1 vuelta -> repetir
      if (!motor_is_busy(1))
        seq_state = 0;
      break;
    }


  }
}
ISR(TIMER0_COMPA_vect)
{
  lcd_task();
  button_edge_task();
  motor_task();
  ms_counter++;
  if (ms_counter >= 10000)
  { // 500ms (100us * 5000 = 500ms)
    ms_counter = 0;
  }
}