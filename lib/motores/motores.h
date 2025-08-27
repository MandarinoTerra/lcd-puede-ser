#ifndef MOTORES_H
#define MOTORES_H

#include <avr/io.h>
#include <stdint.h>

/* Macros para manejar bits */
#define set_bit(reg, bit)    ((reg) |= (1 << (bit)))
#define clear_bit(reg, bit)  ((reg) &= ~(1 << (bit)))
#define invert(reg, bit)     ((reg) ^= (1 << (bit)))

void motores_init(void);
void motor1(uint8_t angulo, uint8_t direccion);
void motor2(uint8_t angulo, uint8_t direccion);
#endif
