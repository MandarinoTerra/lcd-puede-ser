#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <avr/io.h>

uint8_t read_encoder_direction(void);
void encoder_pins_init(void);


#endif
