// components_functions.h
#ifndef COMPONENTS_FUNCTIONS_H
#define COMPONENTS_FUNCTIONS_H

// Inicializa el LED verde (PTD5)
void led_green_init(void);

// Conmuta el estado del LED verde
void led_green_toggle(void);

// Inicializa el LED rojo (PTE29)
void led_red_init(void);

// Conmuta el estado del LED rojo
void led_red_toggle(void);

// Inicializa el botón SW1 (PTC3)
void sw1_init(void);

// Inicializa el botón SW2 (PTC12)
void sw2_init(void);

// Verifica si el botón SW1 está presionado
int is_pressed_sw1(void);

// Verifica si el botón SW2 está presionado
int is_pressed_sw2(void);

// Inicializa el entorno (LEDs y botones)
void init_enviroment(void);

// Apaga ambos LEDs
void leds_off(void);

#endif // COMPONENTS_FUNCTIONS_H