#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <avr/io.h>

void encoder_pins_init(void);
uint8_t encoder_update(void);
uint8_t read_encoder_direction(void);
void button_edge_task(void);
uint8_t read_botton1(void);
#endif
