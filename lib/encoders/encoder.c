#include "encoder.h"

volatile uint8_t button_state = 0;
volatile uint8_t button_debounced = 0;
volatile uint8_t button_counter = 0;

static volatile uint8_t last_button = 1; // Estado anterior (pull-up, inicia en 1)
static volatile uint8_t falling_edge_detected = 0;

#define BUTTON_PIN PD7
#define DEBOUNCE_TIME 5 // número de llamadas a ISR para considerar estable (~0.5ms si ISR cada 0.1ms)

void encoder_pins_init(void)
{
    DDRD &= ~(1 << PD5); // PD5 como entrada (CLK)
    DDRD &= ~(1 << PD6); // PD6 como entrada (DATA)
    DDRD &= ~(1 << PD7); // PD7 como entrada (BUTTON)
    PORTD |= (1 << PD7); // Pull-up interno para botón
}
uint8_t read_encoder_direction()
{
    static uint8_t last_clk = 0;
    uint8_t clk = (PIND & (1 << PD5)) ? 1 : 0;
    uint8_t data = (PIND & (1 << PD6)) ? 1 : 0;
    uint8_t direction = 0xFF; // Sin movimiento

    if (clk != last_clk)
    { // Detecta flanco
        if (clk == 1)
        {                             // Flanco de subida
            direction = data ? 0 : 1; // 0: derecha, 1: izquierda
        }
    }
    last_clk = clk;
    return direction; // 0: derecha, 1: izquierda, 0xFF: sin movimiento
}


// Llama esta función en la ISR del Timer0
void button_edge_task(void) {
    uint8_t current = (PIND & (1 << PD7)) ? 1 : 0;
    if (last_button == 1 && current == 0) {
        falling_edge_detected = 1; // Flanco de bajada detectado
    }
    last_button = current;
}

// Llama esta función en tu loop principal
uint8_t read_botton1(void) {
    if (falling_edge_detected) {
        falling_edge_detected = 0;
        return 1;
    }
    return 0;
}


