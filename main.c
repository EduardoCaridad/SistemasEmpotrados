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


void switches_init(){

    // Primero tenemos que encender el reloj, luego activar el pin como GPIO, y hacer que sea "PULL ENABLE" con PE y PS

    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; // habilita el reloj Puerto C
    
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;  // CONFIGURAMOS EL PTC3 COMO GPIO 
    // Pull-up activado: Se habilitan las resistencias de pull-up en estos pines para mantenerlos en nivel alto cuando no se presionan
    
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; // Pull-up activado. Led Rojo
    
    
    // PDDR es el Puerto Data Direction Register (registro de dirección de datos del puerto). Este registro controla la dirección de los pines de un puerto específico. 1 como salida y 0 como entrada
    
    GPIOC->PDDR &= ~(1 << 3);  // Configura PC3 como entrada. ~(1 << 3): El operador ~ invierte los bits del valor anterior, cambiando el 1 en la posición 3 a 0, y dejando todos los demás bits como 1. Así, obtenemos 0b11110111
    GPIOC->PDDR &= ~(1 << 12); // Configura PC12 como entrada
    
}

int main(void) {

    SIM->COPC = 0x00; // Desactiva el Watchdog Timer
    led_green_init();
    led_red_init();
    switches_init();

    while (1) {
    
    
        // Verifica si el botón SW1 está presionado (activo en bajo)
        //GPIOC->PDIR es el registro de entrada de datos (Port Data Input Register) para el puerto C.
        //GPIOC->PDIR & (1 << 3) lee el estado del pin PC3: Si el resultado es 1, significa que el botón no está presionado (porque el pull-up lo mantiene en alto). Si el resultado es 0, significa que el botón está presionado (activo en bajo).
        // Por tanto, con !(...) Si devuelve 0 quiere decir que el botón está presionado y entra en el bucle. Si el resultado fuese 1 saldría del bucle.
        
        
        if (!(GPIOC->PDIR & (1 << 3))) {
            delay();                            // Pequeño retardo para evitar rebotes
            led_green_toggle();
            while (!(GPIOC->PDIR & (1 << 3))); // Espera a que se suelte el botón
        }

        // Verifica si el botón SW3 está presionado (activo en bajo)
        
        if (!(GPIOC->PDIR & (1 << 12))) {
            delay(); // Pequeño retardo para evitar rebotes
            led_red_toggle();
            while (!(GPIOC->PDIR & (1 << 12))); // Espera a que se suelte el botón
        }
    }

    return 0;
}
