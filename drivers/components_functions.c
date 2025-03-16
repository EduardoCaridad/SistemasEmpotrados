#include "MKL46Z4.h"
#include "components_functions.h"

// LED_GREEN = PTD5
void led_green_init()
{
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; // Habilitar el reloj para el puerto D
    PORTD->PCR[5] = PORT_PCR_MUX(1);    // Configurar PTD5 como GPIO
    GPIOD->PDDR |= (1 << 5);            // Configurar PTD5 como salida
    GPIOD->PSOR |= (1 << 5);            // Apagar Inicialmente
}

void led_green_toggle()
{
    GPIOD->PTOR |= (1 << 5);            // Conmutar el LED verde
}

// LED_RED = PTE29
void led_red_init()
{
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Habilitar el reloj para el puerto E
    PORTE->PCR[29] = PORT_PCR_MUX(1);   // Configurar PTE29 como GPIO
    GPIOE->PDDR |= (1 << 29);           // Configurar PTE29 como salida
    GPIOE->PSOR |= (1 << 29);           // Apagar Inicialmente
}

void led_red_toggle()
{
    GPIOE->PTOR |= (1 << 29);           // Conmutar el LED rojo
}

void sw1_init(){
    SIM->SCGC5 |= (1 << 11);  		// PORT-C
    PORTC->PCR[3] &= ~0x700;            // Limpiar 3 bits MUX
    PORTC->PCR[3] |= (1 << 8);      	// Poner MUX a 1

    PORTC->PCR[3] |= (1 << 1);      	// 1 --> PULL UP / 0 --> PULL DOWN
    GPIOC->PDDR &= ~(1 << 3); 		// Configurar PTC3 como entrada
}

void sw2_init(){
    SIM->SCGC5 |= (1 << 11);  		// PORT-C
    PORTC->PCR[12] &= ~0x700;        	// Limpiar 3 BITS MUX
    PORTC->PCR[12] |= (1 << 8);      	// PONER 1 EN EL MUX

    PORTC->PCR[12] |= (1 << 1);      	// 1 --> PULL UP / 0 --> PULL DOWN
    GPIOC->PDDR &= ~(1 << 12); 		// Configurar PTC12 como entrada
}

int is_pressed_sw1(){
    return !(GPIOC->PDIR & (1 << 3));  	// Retorna 1 si el botón está presionado (nivel bajo)
}

int is_pressed_sw2(){
    return !(GPIOC->PDIR & (1 << 12));  // Retorna 1 si el botón está presionado (nivel bajo)
}

void init_enviroment(){
     led_green_init();
     led_red_init();
     sw1_init();
     sw2_init();
}

void leds_off() {
    GPIOD->PSOR |= (1 << 5);  // Apagar el LED verde
    GPIOE->PSOR |= (1 << 29); // Apagar el LED rojo
}
