#define MENU_LCD_H
#define MENU_LCD_H

#include <stdint.h>
//#include "pico/stdlib.h"

// Inicializa la pantalla LCD
void menu_lcd_init();

// Muestra la opción actual del menú en la LCD
void display_menu();

// Manejador de interrupción para el botón
//void button_callback(uint gpio, uint32_t events);

// Modifica un mensaje en el menú dinámicamente
void update_menu_option(int index, const char *title, const char *description);


