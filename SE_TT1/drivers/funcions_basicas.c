#include "MKL46Z4.h"
#include "funcions_basicas.h"

#define RED_LED_PIN   29u
#define GREEN_LED_PIN 5u
#define LEFT_BUTTON   12u
#define RIGHT_BUTTON  3u

void habilitar_reloxo(void) {  // Habilitar reloxos de portos
    SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK |
                   SIM_SCGC5_PORTE_MASK |
                   SIM_SCGC5_PORTD_MASK);
}

void configurar_leds(void) {
    // Configurar LEDs
    PORTD->PCR[GREEN_LED_PIN] = PORT_PCR_MUX(1U);
    PORTE->PCR[RED_LED_PIN] = PORT_PCR_MUX(1U);
    GPIOE->PDDR = (1U << RED_LED_PIN);
    GPIOD->PDDR = (1U << GREEN_LED_PIN);

    // Apagar LEDs inicialmente
    GPIOE->PSOR = (1U << RED_LED_PIN);
    GPIOD->PSOR = (1U << GREEN_LED_PIN);
}
void configurar_botons(void) {
    // Configurar botóns
    PORTC->PCR[LEFT_BUTTON] = PORT_PCR_MUX(1U) |
                              PORT_PCR_PE(1U) |
                              PORT_PCR_PS(1U) |
                              PORT_PCR_IRQC(0xA);

    PORTC->PCR[RIGHT_BUTTON] = PORT_PCR_MUX(1U) |
                               PORT_PCR_PE(1U) |
                               PORT_PCR_PS(1U) |
                               PORT_PCR_IRQC(0xA);

    GPIOC->PDDR &= ~(1U << RIGHT_BUTTON);
    GPIOC->PDDR &= ~(1U << LEFT_BUTTON);
}

void configurar_interrupcions(void) {
    // Habilitar interrupcións no NVIC
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
}



