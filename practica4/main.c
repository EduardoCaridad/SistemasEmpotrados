#include "MKL46Z4.h"
#include "lcd.h"

// Declarar la función de ensamblador a usar
extern unsigned int reverse_int(unsigned int in);

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc
void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32 kHz, 1 = 4 MHz
}

// Función de retardo
void delay(void) {
    volatile int i;
    for (i = 0; i < 1000000; i++);
}

int main(void) {
    irclk_ini();   
    lcd_ini();     

    unsigned int original_value = 1234;  // Valor Original
    unsigned int reversed_value;
    unsigned int truncated_value;

    // Realizar la inversión de bits
    reversed_value = reverse_int(original_value);

    // Muestro el Número en Decimal
    lcd_display_dec(original_value);
      
    // Pausa doble para apreciarlo mejor  
    delay();
    delay(); 

    // Truncar el resultado a los últimos 4 dígitos
    truncated_value = reversed_value % 10000;

    // Mostrar el valor truncado en la pantalla LCD
    lcd_display_dec(truncated_value);

    // Bucle infinito para observar el resultado
    while (1) {
        delay();
    }

    return 0;
}