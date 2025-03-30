#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 9
#define LCD_ADDR 0x27
#define BUTTON_PIN 10

volatile int menuIndex = 0;
#define MAX_MENU_ITEMS 4
#define MAX_TEXT_LENGTH 16  // Longitud máxima de cada línea

char menuOptions[MAX_MENU_ITEMS][2][MAX_TEXT_LENGTH] = {
    {"Menu 1", "Opcion 1"},
    {"Menu 2", "Opcion 2"},
    {"Menu 3", "Opcion 3"},
    {"Menu 3", "Opcion 3"}
    
};

// Función para escribir en el LCD a través de I2C
void i2c_write_byte(uint8_t addr, uint8_t val) {
    i2c_write_blocking(I2C_PORT, addr, &val, 1, false);
}

// Activa la señal de "Enable" del LCD
void lcd_toggle_enable(uint8_t val) {
    sleep_us(600);
    i2c_write_byte(LCD_ADDR, val | 0x04);
    sleep_us(600);
    i2c_write_byte(LCD_ADDR, val & ~0x04);
    sleep_us(600);
}

// Enviar un byte al LCD (modo comando o datos)
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | 0x08;
    uint8_t low = mode | ((val << 4) & 0xF0) | 0x08;
    i2c_write_byte(LCD_ADDR, high);
    lcd_toggle_enable(high);
    i2c_write_byte(LCD_ADDR, low);
    lcd_toggle_enable(low);
}

// Borra la pantalla del LCD
void lcd_clear() {
    lcd_send_byte(0x01, 0);
}

// Posiciona el cursor en la línea y columna deseada
void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, 0);
}

// Escribe una cadena en el LCD
void lcd_string(const char *s) {
    while (*s) {
        lcd_send_byte(*s++, 1);
    }
}

// Inicializa el LCD
void lcd_init() {
    lcd_send_byte(0x03, 0);
    lcd_send_byte(0x03, 0);
    lcd_send_byte(0x03, 0);
    lcd_send_byte(0x02, 0);
    lcd_send_byte(0x28, 0);  // Modo 4 bits, 2 líneas
    lcd_send_byte(0x0C, 0);  // Display ON, cursor OFF
    lcd_send_byte(0x06, 0);  // Incremento del cursor
    lcd_clear();
}

// Función para actualizar los mensajes del menú
void update_menu_option(int index, const char *title, const char *description) {
    if (index >= 0 && index < MAX_MENU_ITEMS) {
        strncpy(menuOptions[index][0], title, MAX_TEXT_LENGTH - 1);
        strncpy(menuOptions[index][1], description, MAX_TEXT_LENGTH - 1);
        menuOptions[index][0][MAX_TEXT_LENGTH - 1] = '\0'; // Asegurar terminación
        menuOptions[index][1][MAX_TEXT_LENGTH - 1] = '\0';
    }
}

// Muestra la opción actual del menú en la LCD
void display_menu() {
    lcd_clear();
    lcd_set_cursor(0, 3);
    lcd_string(menuOptions[menuIndex][0]);
    lcd_set_cursor(1, 0);
    lcd_string(menuOptions[menuIndex][1]);
}

// Función de interrupción del botón para cambiar de menú
void button_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN) {
        menuIndex = (menuIndex + 1) % MAX_MENU_ITEMS;  // Cambia al siguiente menú
        display_menu();
    }
}

// Inicializa el menú y la pantalla LCD
void menu_lcd_init() {
    i2c_init(I2C_PORT, 100000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    lcd_init();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_RISE, true, &button_callback);

    display_menu();  // Mostrar primera opción del menú
}
