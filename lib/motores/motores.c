#include "motores.h"

/* Pines por defecto (adáptalos) */
/* Motor 0: STEP PD2, DIR PD3
   Motor 1: STEP PD4, DIR PD5
   Ambos en PORTD por defecto; adapta si usas otros puertos. */
#define M0_STEP_PIN  PD2
#define M0_DIR_PIN   PD3
#define M1_STEP_PIN  PD4
#define M1_DIR_PIN   PD5
#define MOTOR_PORT   PORTD
#define MOTOR_DDR    DDRD

/* Tick del ISR en microsegundos (coincide con tu Timer0) */
#ifndef MOTOR_TICK_US
#define MOTOR_TICK_US 100u
#endif

/* Estado por motor */
static volatile uint32_t motor_remaining_steps[MOTORS_COUNT];
static volatile uint8_t  motor_busy_flag[MOTORS_COUNT];
static volatile uint16_t step_interval_ticks[MOTORS_COUNT];
static volatile uint16_t step_timer[MOTORS_COUNT];
static volatile uint8_t  step_pulse_state[MOTORS_COUNT]; /* 0 = listo, 1 = pulso alto pendiente de bajar */

/* Mapping de pines por motor (arrays constantes) */
static const uint8_t motor_step_pin[MOTORS_COUNT] = { M0_STEP_PIN, M1_STEP_PIN };
static const uint8_t motor_dir_pin [MOTORS_COUNT] = { M0_DIR_PIN,  M1_DIR_PIN  };

/* Inicializa pines */
void motores_init(void) {
    for (uint8_t i = 0; i < MOTORS_COUNT; i++) {
        /* Asegurar bajos */
        clear_bit(MOTOR_PORT, motor_step_pin[i]);
        clear_bit(MOTOR_PORT, motor_dir_pin[i]);
        /* Configurar como salidas */
        set_bit(MOTOR_DDR, motor_step_pin[i]);
        set_bit(MOTOR_DDR, motor_dir_pin[i]);
        /* Estado interno */
        motor_remaining_steps[i] = 0;
        motor_busy_flag[i] = 0;
        step_interval_ticks[i] = 1;
        step_timer[i] = 0;
        step_pulse_state[i] = 0;
    }
}

/* Convierte ángulo a pasos (acepta >360 para múltiples vueltas) */
static uint32_t angle_to_steps(uint32_t angle_deg) {
    uint64_t effective_steps = (uint64_t)STEPS_PER_REV * (uint64_t)MICROSTEP;
    uint64_t num = (uint64_t)angle_deg * effective_steps;
    uint32_t steps = (uint32_t)((num + 180) / 360); /* redondeo */
    return steps;
}

/* Inicia movimiento no bloqueante en motor_idx */
uint8_t motor_move(uint8_t motor_idx, uint32_t angle_degrees, uint8_t direction, uint32_t steps_per_sec) {
    if (motor_idx >= MOTORS_COUNT) return 0;
    if (motor_busy_flag[motor_idx]) return 0;
    if (angle_degrees == 0) return 0;
    if (steps_per_sec == 0) steps_per_sec = 1;

    uint32_t steps = angle_to_steps(angle_degrees);
    if (steps == 0) return 0;

    /* Dirección */
    if (direction) set_bit(MOTOR_PORT, motor_dir_pin[motor_idx]);
    else clear_bit(MOTOR_PORT, motor_dir_pin[motor_idx]);

    /* Calcular intervalo (ticks ISR) = ceil((1e6 / steps_per_sec) / MOTOR_TICK_US) */
    uint32_t period_us = 1000000UL / steps_per_sec;
    if (period_us == 0) period_us = 1;
    uint32_t ticks = (period_us + MOTOR_TICK_US - 1) / MOTOR_TICK_US;
    if (ticks < 1) ticks = 1;
    if (ticks > 0xFFFF) ticks = 0xFFFF;

    step_interval_ticks[motor_idx] = (uint16_t)ticks;
    step_timer[motor_idx] = 0;
    step_pulse_state[motor_idx] = 0;
    motor_remaining_steps[motor_idx] = steps;
    motor_busy_flag[motor_idx] = 1;
    /* Asegurar STEP bajo antes de empezar */
    clear_bit(MOTOR_PORT, motor_step_pin[motor_idx]);
    return 1;
}

/* motor_task: llamar desde ISR periódica. No bloquea. */
void motor_task(void) {
    for (uint8_t i = 0; i < MOTORS_COUNT; i++) {
        if (!motor_busy_flag[i]) continue;

        if (step_pulse_state[i]) {
            /* Bajar pulso y contabilizar paso */
            clear_bit(MOTOR_PORT, motor_step_pin[i]);
            step_pulse_state[i] = 0;
            if (motor_remaining_steps[i] > 0) motor_remaining_steps[i]--;
            step_timer[i] = 0;
            if (motor_remaining_steps[i] == 0) {
                motor_busy_flag[i] = 0;
            }
            continue;
        }

        /* Esperar intervalo */
        if (step_timer[i]++ >= step_interval_ticks[i]) {
            /* Generar pulso (alto por 1 tick) */
            set_bit(MOTOR_PORT, motor_step_pin[i]);
            step_pulse_state[i] = 1;
            /* step_timer se reseteará cuando baje el pulso */
        }
    }
}

/* Estado/Control */
uint8_t motor_is_busy(uint8_t motor_idx) {
    if (motor_idx >= MOTORS_COUNT) return 0;
    return motor_busy_flag[motor_idx];
}

void motor_abort(uint8_t motor_idx) {
    if (motor_idx >= MOTORS_COUNT) return;
    motor_remaining_steps[motor_idx] = 0;
    motor_busy_flag[motor_idx] = 0;
    step_pulse_state[motor_idx] = 0;
    step_timer[motor_idx] = 0;
    clear_bit(MOTOR_PORT, motor_step_pin[motor_idx]);
}