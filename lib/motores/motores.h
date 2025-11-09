#ifndef MOTORES_H
#define MOTORES_H

#include <avr/io.h>
#include <stdint.h>

/* Macros para manejar bits */
#define set_bit(reg, bit)    ((reg) |= (1 << (bit)))
#define clear_bit(reg, bit)  ((reg) &= ~(1 << (bit)))
#define invert(reg, bit)     ((reg) ^= (1 << (bit)))

void motores_init(void);

/* Inicia un movimiento no bloqueante en el motor indicado (0 o 1):
   - motor_idx: 0 o 1
   - angle_degrees: ángulo a girar (si >360 permite múltiples vueltas)
   - direction: 0 = una dirección, 1 = otra (ajusta según tu cableado)
   - steps_per_sec: pulsos por segundo (velocidad). */
uint8_t motor_move(uint8_t motor_idx, uint32_t angle_degrees, uint8_t direction, uint32_t steps_per_sec);

/* Llamar periódicamente desde ISR (ya configurado en main) */
void motor_task(void);

/* Consultas / control */
uint8_t motor_is_busy(uint8_t motor_idx);
void motor_abort(uint8_t motor_idx);

#ifndef STEPS_PER_REV
#define STEPS_PER_REV 200
#endif

#ifndef MICROSTEP
#define MICROSTEP 1
#endif

/* Número de motores soportados */
#ifndef MOTORS_COUNT
#define MOTORS_COUNT 2
#endif

#endif
