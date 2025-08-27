#include "lcd.h"

/* Variables globales para la máquina de estados */
volatile lcd_state_t lcd_state = LCD_IDLE;

#define LCD_QUEUE_SIZE 32

typedef struct {
    uint8_t value;
    uint8_t is_data;
} lcd_queue_item_t;

volatile lcd_queue_item_t lcd_queue[LCD_QUEUE_SIZE];
volatile uint8_t lcd_queue_head = 0;
volatile uint8_t lcd_queue_tail = 0;

/* Configura pines de control y datos como salida */
void lcd_pins_init(void) {
    set_bit(LCD_CTRL_DDR, RS_PIN);
    set_bit(LCD_CTRL_DDR, EN_PIN);

    set_bit(LCD_DATA_DDR, D4_PIN);
    set_bit(LCD_DATA_DDR, D5_PIN);
    set_bit(LCD_DATA_DDR, D6_PIN);
    set_bit(LCD_DATA_DDR, D7_PIN);
}

/* Escribe un nibble en D4-D7 y genera pulso de EN */
void lcd_write_nibble(uint8_t nibble) {
    if (nibble & 0x01) set_bit(LCD_DATA_PORT, D4_PIN); else clear_bit(LCD_DATA_PORT, D4_PIN);
    if (nibble & 0x02) set_bit(LCD_DATA_PORT, D5_PIN); else clear_bit(LCD_DATA_PORT, D5_PIN);
    if (nibble & 0x04) set_bit(LCD_DATA_PORT, D6_PIN); else clear_bit(LCD_DATA_PORT, D6_PIN);
    if (nibble & 0x08) set_bit(LCD_DATA_PORT, D7_PIN); else clear_bit(LCD_DATA_PORT, D7_PIN);

    set_bit(LCD_CTRL_PORT, EN_PIN);
    clear_bit(LCD_CTRL_PORT, EN_PIN);
}

/* Encola un byte para enviar al LCD (no bloqueante) */
void lcd_send(uint8_t value, uint8_t is_data) {
    uint8_t next = (lcd_queue_head + 1) % LCD_QUEUE_SIZE;
    if (next != lcd_queue_tail) { // Si la cola no está llena
        lcd_queue[lcd_queue_head].value = value;
        lcd_queue[lcd_queue_head].is_data = is_data;
        lcd_queue_head = next;
    }
    // Si la cola está llena, descarta el dato (puedes mejorar esto)
}

/* Envia comando al LCD */
void lcd_cmd(uint8_t cmd) {
    lcd_send(cmd, 0);
}

/* Envia un dato/caracter al LCD */
void lcd_data(uint8_t data) {
    lcd_send(data, 1);
}

/* Inicializa el LCD 16x2 en modo 4 bits */
void lcd_init(void) {
    lcd_pins_init();

    lcd_cmd(0x33);
    lcd_cmd(0x32);
    lcd_cmd(0x28); // 2 líneas, 5x8
    lcd_cmd(0x0C); // display ON, cursor OFF
    lcd_cmd(0x06); // auto-incremento
    lcd_cmd(0x01); // limpia pantalla
}

/* Posiciona cursor en columna/filas */
void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t address = (row == 0) ? col : (0x40 + col);
    lcd_cmd(0x80 | address);
}

/* Imprime string completo en LCD */
void lcd_print(const char *str) {
    while(*str) {
        lcd_data(*str++);
    }
}

/* Máquina de estados para envío no bloqueante */
void lcd_task(void) {
    static uint8_t phase = 0;
    static uint8_t current_value = 0;
    static uint8_t current_is_data = 0;
    static uint8_t sending = 0;

    if (!sending && lcd_queue_tail != lcd_queue_head) {
        // Hay datos en la cola
        current_value = lcd_queue[lcd_queue_tail].value;
        current_is_data = lcd_queue[lcd_queue_tail].is_data;
        lcd_queue_tail = (lcd_queue_tail + 1) % LCD_QUEUE_SIZE;
        sending = 1;
        lcd_state = LCD_SEND_HIGH;
    }

    switch(lcd_state) {
        case LCD_IDLE:
            // Espera nuevos datos
            break;

        case LCD_SEND_HIGH:
            if(current_is_data) set_bit(LCD_CTRL_PORT, RS_PIN);
            else clear_bit(LCD_CTRL_PORT, RS_PIN);

            lcd_write_nibble(current_value >> 4);
            phase = 1;
            lcd_state = LCD_SEND_LOW;
            break;

        case LCD_SEND_LOW:
            if(phase) {
                clear_bit(LCD_CTRL_PORT, EN_PIN);
                phase = 0;
            } else {
                lcd_write_nibble(current_value & 0x0F);
                clear_bit(LCD_CTRL_PORT, EN_PIN);
                lcd_state = LCD_IDLE;
                phase = 1;
                sending = 0;
            }
            break;
    }
}
