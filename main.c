#include "MKL46Z4.h"
#include "lcd.h"
#include "components_functions.h" 

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICH
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Variables globales para almacenar
volatile unsigned int hits = 0; // cuenta los aciertos
volatile unsigned int misses = 0; //cuenta los fallos
volatile int current_led = 0; // 0: rojo, 1: verde


// Enable IRCLK (Internal Reference Clock) Activa el reloj interno de 32 kHz para que pueda ser usado por la pantalla LCD.
// see Chapter 24 in MCU doc
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay(void) // Se introduce una pausa en el programa para que los cambios en los LED sean visibles
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}


// Creamos las interrupciones:
void interrupts() {
    // Interrupción del SW1
    PORTC->PCR[3] |= (0xA << 16);

    // Interrupcion del SW2
    PORTC->PCR[12] |= (0xA << 16);

    NVIC_EnableIRQ(31); // Habilitar interrupción para el puerto D
}


// PORTD-Handler Routine

void PORTDIntHandler(void) {
    if (PORTC->ISFR & (1 << 3)) { // Si SW1 se presionó
        if (current_led == 1) { // Si el LED es verde (correcto)
            hits++; 
        } else { // Si el LED es rojo (incorrecto)
            misses++; 
        }
        PORTC->ISFR |= (1 << 3); // Limpiar la bandera de interrupción
    }

    if (PORTC->ISFR & (1 << 12)) { // Si SW2 se presionó
        if (current_led == 0) { // Si el LED es rojo (correcto)
            hits++; 
        } else { // Si el LED es verde (incorrecto)
            misses++; 
        }
        PORTC->ISFR |= (1 << 12); // Limpiar la bandera de interrupción
    }

    // Actualizar la pantalla LCD con los aciertos y fallos
    lcd_display_time(hits, misses);
}


int main(void) {
    irclk_ini(); // // Habilitar reloj interno
    lcd_ini(); // Inicializar LCD
    lcd_display_time(0, 0); // Estado Inicial LCD
    init_enviroment(); // Inicializa LEDs y botones
    interrupts(); // Configura y habilita -> Interrupciones

    // 'Random' sequence :-)
    volatile unsigned int sequence = 0x32B14D98;
    unsigned int index = 0;

    while (index < 32) {
        if (sequence & (1 << index)) { // Si el bit es 1. Usamos sequence como un patrón de 32 bits para encender LEDs de manera pseudoaleatoria.
            led_green_toggle();
            current_led = 1; // Estado del led: verde
        } else { // Si el bit es 0
            led_red_toggle();
            current_led = 0; // Estado del led: rojo
        }
        
        delay(); // Para hacer notorio el cambio de LED
        leds_off(); // Apaga los LEDs
        index++; // Avanzamos en la secuencia
    }


//  // Finalización del Juego::

    NVIC_DisableIRQ(31); // Deshabilitar las interrupciones

  
    // Mostrar 'End' en la pantalla LCD
    lcd_display_end();

    for(int i = 0; i < 4; i++) { //Para observar fin del juego
      delay();
    }

    while (1) {
        // Resultado
        lcd_display_time(hits, misses);
        delay();

        // Limpiar (Parpadeo)
        lcd_clear();
        delay();
    }

    return 0;
}
