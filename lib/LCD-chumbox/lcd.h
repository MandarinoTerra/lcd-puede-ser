#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <stdint.h>

/* Macros para manejar bits */
#define set_bit(reg, bit)    ((reg) |= (1 << (bit)))
#define clear_bit(reg, bit)  ((reg) &= ~(1 << (bit)))
#define invert(reg, bit)     ((reg) ^= (1 << (bit)))

/* Pines de control (PORTD) */
#define RS_PIN    PD2
#define EN_PIN    PD3
#define LCD_CTRL_PORT   PORTD
#define LCD_CTRL_DDR    DDRD

/* Pines de datos (PORTC) */
#define D4_PIN    PC3
#define D5_PIN    PC2
#define D6_PIN    PC1
#define D7_PIN    PC0
#define LCD_DATA_PORT   PORTC
#define LCD_DATA_DDR    DDRC

/* Máquina de estados interna */
typedef enum { LCD_IDLE, LCD_SEND_HIGH, LCD_SEND_LOW } lcd_state_t;

/* Variables globales usadas por la máquina de estados */
extern volatile lcd_state_t lcd_state;
extern volatile uint8_t lcd_byte;
extern volatile uint8_t lcd_is_data;

/* Prototipos de funciones */
void lcd_pins_init(void);
void lcd_init(void);
void lcd_write_nibble(uint8_t nibble);
void lcd_send(uint8_t value, uint8_t is_data);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_task(void); // máquina de estados, llamada desde ISR

#endif
