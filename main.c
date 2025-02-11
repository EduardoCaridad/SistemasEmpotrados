#include "MKL46Z4.h"

void delay(void) {
    for (volatile int i = 0; i < 1000000; i++); // Retardo simple
}

void led_green_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; // Habilita reloj para el puerto D
    PORTD->PCR[5] = PORT_PCR_MUX(1);    // Configura PTD5 como GPIO
    GPIOD->PDDR |= (1 << 5);            // Configura PTD5 como salida
    GPIOD->PSOR = (1 << 5);             // Apaga el LED Verde (activo en bajo)
}

void led_green_toggle() {
    GPIOD->PTOR = (1 << 5); // Alterna el estado del LED Verde
}

void led_red_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Habilita reloj para el puerto E
    PORTE->PCR[29] = PORT_PCR_MUX(1);   // Configura PTE29 como GPIO
    GPIOE->PDDR |= (1 << 29);           // Configura PTE29 como salida
    GPIOE->PSOR = (1 << 29);            // Apaga el LED Rojo (activo en bajo)
}

void led_red_toggle() {
    GPIOE->PTOR = (1 << 29); // Alterna el estado del LED Rojo
}

int main(void) {
    SIM->COPC = 0x00; // Desactiva el Watchdog Timer
    led_green_init();
    led_red_init();

    while (1) {
        led_green_toggle();
        delay();
        led_red_toggle();
        delay();
    }

    return 0;
}
