#include "MKL46Z4.h"

volatile int door1_closed = 1;
volatile int door2_closed = 1;

void delay(void) {
    for (volatile int i = 0; i < 1000000; i++); // Retardo simple
}

void led_green_init() {  //PUERTAS CERRADAS
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; // Habilita reloj para el puerto D
    PORTD->PCR[5] = PORT_PCR_MUX(1);    // Configura PTD5 como GPIO
    GPIOD->PDDR |= (1 << 5);            // Configura PTD5 como salida
    GPIOD->PSOR = (1 << 5);             // Apaga el LED Verde (activo en bajo)

}


void led_red_init() {  // PUERTAS ABIERTAS
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Habilita reloj para el puerto E
    PORTE->PCR[29] = PORT_PCR_MUX(1);   // Configura PTE29 como GPIO
    GPIOE->PDDR |= (1 << 29);           // Configura PTE29 como salida
    GPIOE->PSOR = (1 << 29);            // Apaga el LED Rojo (activo en bajo)
}



void updateLEDs(void) {
  if (door1_closed && door2_closed) {
  
    GPIOD->PCOR = (1 << 5);
    GPIOE->PSOR = (1 << 29); 
      }
  else {
    GPIOE->PCOR = (1 << 29);
  GPIOD->PSOR = (1 << 5);
    }   
}

void switches_init(){

    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; // habilita el reloj Puerto C
    
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;  // CONFIGURAMOS EL PTC3 COMO GPIO // Pull-up activado
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; // Pull-up activado
    
    GPIOC->PDDR &= ~(1 << 3);  // Configura PC3 como entrada
    GPIOC->PDDR &= ~(1 << 12); // Configura PC12 como entrada
    
}

int pulsar_boton1() {
    
    return !(GPIOC->PDIR & (1 << 3));
    }
    
int pulsar_boton2() {
    
    return !(GPIOC->PDIR & (1 << 12));
    }
    

int main(void) {

    SIM->COPC = 0x00; // Desactiva el Watchdog Timer
    led_green_init();
    led_red_init();
    switches_init();
    updateLEDs();
    
    while (1) {
        if (pulsar_boton1()) {
             door1_closed = !door1_closed;// Pequeño retardo
            while (pulsar_boton1()); 
            }
        else if (pulsar_boton2()) {
            door2_closed = !door2_closed;
            while (pulsar_boton2()); 
            }
        updateLEDs();
    }
}