#include "main.h"

// ver main.h para aclaraciones de structs, funcións, etc. Embaixo está o código inicial que non funciona pero
//serviume como guia do que fan as funcións, polo que o que no estea comentado eiquí hai que ver o final do código

const Led RED_LED   = {GPIOE, RED_LED_PIN};
const Led GREEN_LED = {GPIOD, GREEN_LED_PIN};

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0);
}

static inline void SegLCD_Col_Alterno(void) {  // alternamos o segmento decimal dunha pantalla Segment LCD
    SegLCD_Col_Toggle();  // Alternar o segmento decimal
    // Definimos no lcd.h o macro SegLCD_Col_Toggle() para alternar o segmento decimal
}

static inline void lcd_toggle_power(void) {
    LCD->GCR ^= ~LCD_GCR_LCDEN_MASK;    // Alternar apagado/encendido LCD No arquivo ,h da placa definea como (0x80U) liña 2036
}

void toggle_led(const Led *led) {
    led->port->PTOR = (1U << led->pin);
}

void set_led_on(const Led *led) {
    led->port->PCOR = (1U << led->pin);
}

void set_led_off(const Led *led) {
    led->port->PSOR = (1U << led->pin);
}

void refrescar_display(void) {
    lcd_display_time(temporizador.seg_alarma, temporizador.seg_temporizador);  //ver main.h
}


TipoBoton detectar_boton_pulsado(void) {
    static const uint8_t botones[] = { LEFT_BUTTON, RIGHT_BUTTON };
    static const TipoBoton botones_enum[] = { BUTTON_LEFT, BUTTON_RIGHT };

    for (int i = 0; i < 2; i++) {
        uint32_t pcr_valor = PORTC->PCR[botones[i]];
        if (((pcr_valor >> PORT_PCR_ISF_SHIFT) & 0x1U) != 0) {
            // Limpa-lo flag ISF escribindo un 1
            PORTC->PCR[botones[i]] |= PORT_PCR_ISF(1);
            return botones_enum[i];
        }
    }

    return BUTTON_NONE;
}



void PORTDIntHandler(void) {
    int8_t boton_pulsado = detectar_boton_pulsado();

    if (boton_pulsado == BUTTON_NONE) {
        // Non se pulsó ningún botón, saimos da interrupción
        return;
    }

    switch (temporizador.estado) {
        case ESTADO_INICIAL:
            // Reiniciar variables do temporizador
            temporizador.seg_alarma = 0;
            temporizador.seg_temporizador = 0;
            temporizador.ticks_restantes = 0;
            temporizador.estado = ESTADO_INTRODUCIR_TEMPO;
            break;

        case ESTADO_INTRODUCIR_TEMPO:
            if (boton_pulsado == BUTTON_LEFT) {
                // Aumenta-los segundos do temporizador (máximo TEMPO_MAX)
                temporizador.seg_temporizador = (temporizador.seg_temporizador + 1) % (TEMPO_MAX + 1);
            } else {
                // Pasar ó siguinte estado para introduci-la alarma
                temporizador.estado = ESTADO_INTRODUCIR_ALARMA;
            }
            break;

        case ESTADO_INTRODUCIR_ALARMA:
            if (boton_pulsado == BUTTON_LEFT) {
                // Aumenta-los segundos da alarma (máximo TEMPO_MAX)
                temporizador.seg_alarma = (temporizador.seg_alarma + 1) % (TEMPO_MAX + 1);
            } else {
                // Inicia-lo temporizador
                temporizador.estado = ESTADO_TEMPORIZADOR_INICIADO;
                acender_canle_pit(CANLE_TEMPORIZADOR_PRINCIPAL);  //Os canles están definidos en main.h
            }
            break;

        case ESTADO_TEMPORIZADOR_INICIADO:
            // Garda-lo tempo restante e pausa-lo PIT
            temporizador.ticks_restantes = PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].CVAL;
            apagar_canle_pit(CANLE_TEMPORIZADOR_PRINCIPAL);
            temporizador.estado = ESTADO_TEMPORIZADOR_PAUSADO;
            break;

        case ESTADO_TEMPORIZADOR_PAUSADO:
            // Restaura-lo tempo restante e reanuda-lo PIT
            PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].LDVAL = temporizador.ticks_restantes;
            acender_canle_pit(CANLE_TEMPORIZADOR_PRINCIPAL);
            temporizador.ticks_restantes = 0;
            temporizador.estado = ESTADO_TEMPORIZADOR_INICIADO;
            break;

        case ESTADO_FIN_TEMPORIZADOR:
            // Apagar parpadeo do display LCD
            LCD->AR = LCD_AR_BLINK(0) | LCD_AR_BRATE(0x00);

            if (boton_pulsado == BUTTON_LEFT) {
                temporizador.estado = ESTADO_INICIAL;
            } else {
                temporizador.estado = ESTADO_FIN_PROGRAMA;

                // Apagar LCD e alternar LED rojo
                lcd_toggle_power();
                toggle_led(&RED_LED);
            }
            break;

        default:
            // Non facer nada noutros estados
            break;
    }

    // Actualiza-lo contido do display
    refrescar_display();
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                 FUNCIÓN ANTIGA
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Manexador de interrupcións para os botóns
// void PORTDIntHandler(void) {
//     int8_t boton_pulsado = detectar_boton_pulsado(); //devolve o boton pulsado ou BUTTON_NONE

//     if(boton_pulsado == BUTTON_NONE) {
//         // Error
//         return;
//     }

//     switch(temporizador.estado) {
//         case ESTADO_INICIAL:
//             temporizador.seg_alarma = 0;
//             temporizador.seg_temporizador = 0;
//             temporizador.ticks_restantes = 0;
//             temporizador.estado = ESTADO_INTRODUCIR_TEMPO;
//         case ESTADO_INTRODUCIR_TEMPO:
//             if(boton_pulsado == BUTTON_LEFT) {
//                 temporizador.seg_temporizador = (temporizador.seg_temporizador + 1) % (TEMPO_MAX + 1); //engadimos os segundos
//             } else {
//                 temporizador.estado = ESTADO_INTRODUCIR_ALARMA; // se pulsa o boton dereinto pasamos a introduci-lo valor da alarma
//             }
//             break;
//         case ESTADO_INTRODUCIR_ALARMA:
//             if(boton_pulsado == BUTTON_LEFT) {
//                 temporizador.seg_alarma = (temporizador.seg_alarma + 1) % (TEMPO_MAX + 1);
//             } else {
//                 temporizador.estado = ESTADO_TEMPORIZADOR_INICIADO;
//                 acender_canle_pit(CANLE_TEMPORIZADOR_PRINCIPAL);
//             }
//             break;
//         case ESTADO_TEMPORIZADOR_INICIADO:
//             temporizador.ticks_restantes = PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].CVAL; //gardamos o valor de ticks (do tempo) restantes
//             apagar_canle_pit(CANLE_TEMPORIZADOR_PRINCIPAL);
//             temporizador.estado = ESTADO_TEMPORIZADOR_PAUSADO;
//             break;
//         case ESTADO_TEMPORIZADOR_PAUSADO:
//             PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].LDVAL = temporizador.ticks_restantes;
//             acender_canle_pit(CANLE_TEMPORIZADOR_PRINCIPAL);
//             PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].LDVAL = 48000000 / 4; //asinamos o valor de ticks
//             temporizador.ticks_restantes = 0;
//             temporizador.estado = ESTADO_TEMPORIZADOR_INICIADO;
//             break;
//         case ESTADO_FIN_TEMPORIZADOR:
//             LCD->AR = LCD_AR_BLINK(0) | LCD_AR_BRATE(0x00); //apaga-lo parpadeo
//             temporizador.estado = (boton_pulsado == BUTTON_LEFT) ? ESTADO_INICIAL : ESTADO_FIN_PROGRAMA;
//             if(boton_pulsado == 1) {
//                 lcd_toggle_power();
//                 toggle_led(&RED_LED);
//             }
//             break;
//         default:
//             break;
//     }

//     refrescar_display();
// }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




void setup_io(void) {
    // Habilitar reloxos de portos
    SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK |
                   SIM_SCGC5_PORTE_MASK |
                   SIM_SCGC5_PORTD_MASK);

    // Configurar LEDs
    PORTD->PCR[GREEN_LED_PIN] = PORT_PCR_MUX(1U);
    PORTE->PCR[RED_LED_PIN] = PORT_PCR_MUX(1U);
    GPIOE->PDDR = (1U << RED_LED_PIN);
    GPIOD->PDDR = (1U << GREEN_LED_PIN);

    // Apagar LEDs inicialmente
    GPIOE->PSOR = (1U << RED_LED_PIN);
    GPIOD->PSOR = (1U << GREEN_LED_PIN);

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

    // Habilitar interrupcións no NVIC
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
}

void disable_button_interrupts(void) {
    PORTC->PCR[LEFT_BUTTON] &= ~PORT_PCR_IRQC_MASK;   // Desactivar interrupción no botón esquerdo
    PORTC->PCR[RIGHT_BUTTON] &= ~PORT_PCR_IRQC_MASK;  // Desactivar interrupción no botón dereito
}

uint8_t contar_hacia_atras(void) {
    if(temporizador.seg_temporizador > 0) {
        comprobar_alarma();
        temporizador.seg_temporizador -= 1;
        comprobar_alarma();
    }

    return (temporizador.seg_temporizador <= 0);  //si o temporizador é menor ou igual a 0 devolve 1 se non devolve 0 (se quedan segundos)
}

void comprobar_alarma(void) {
    if(temporizador.seg_temporizador && temporizador.seg_temporizador == temporizador.seg_alarma) {
        acender_canle_pit(CANLE_TEMPORIZADOR_LEDS);
    }
}

void setup_led_blink_timer(void) {  //Creamos 2 canles do temporizador PIT, un para a lóxica ppal (1 segundo) e outro para o parpadeo do LED (41,6ms)
    // Habilitar PIT
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; //activamos registro de control do reloxo
    PIT->MCR &= ~PIT_MCR_MDIS_MASK; // Habilita o modulo PIT globalmente : MDIS (module Disable)

    // Configurar o canle 0 do PIT cun intervalo de 1000ms
    uint32_t intervalo_principal = FRECUENCIA_CPU / 4;  // 1000ms
    uint32_t intervalo_leds =      FRECUENCIA_CPU / 24;

    PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].LDVAL = PIT_LDVAL_TSV(intervalo_principal);
    PIT->CHANNEL[CANLE_TEMPORIZADOR_LEDS].LDVAL =      PIT_LDVAL_TSV(intervalo_leds);

    // Activar as interrupcións PIT
    PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].TCTRL |= PIT_TCTRL_TIE_MASK;
    PIT->CHANNEL[CANLE_TEMPORIZADOR_LEDS].TCTRL      |= PIT_TCTRL_TIE_MASK;

    // Habilitar interrupcións PIT no NVIC
    NVIC_EnableIRQ(PIT_IRQn);
}

void acender_canle_pit(uint8_t canle) {
    if (canle <= 1) {
        PIT->CHANNEL[canle].TCTRL |= PIT_TCTRL_TEN_MASK;
    }
}

void apagar_canle_pit(uint8_t canle) {
    if(canle <= 1) {
        PIT->CHANNEL[canle].TCTRL &= ~PIT_TCTRL_TEN_MASK;
    }
}

//------------------------------------------------------------------------------------------------------------------------------


// void lcd_draw_char_manual(char letra, int posicion) {
//     uint8_t seg_low = 0;
//     uint8_t seg_high = 0;

//     switch (letra) {
//         case 'B':
//         case 'b':
//             seg_low  = LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G;
//             seg_high = LCD_SEG_C;
//             break;
//         case 'Y':
//         case 'y':
//             seg_low  = LCD_SEG_D | LCD_SEG_F | LCD_SEG_G;
//             seg_high = LCD_SEG_B | LCD_SEG_C;
//             break;
//         case 'E':
//         case 'e':
//             seg_low  = LCD_SEG_A | LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G;
//             seg_high = 0;
//             break;
//         case ' ':
//             seg_low = 0;
//             seg_high = 0;
//             break;
//         default:
//             seg_low = 0;
//             seg_high = 0;
//             break;
//     }

//     uint8_t index_low  = LCD_FRONTPLANE0 + (posicion * 2);
//     uint8_t index_high = LCD_FRONTPLANE1 + (posicion * 2);

//     LCD->WF8B[index_low]  = seg_low;
//     LCD->WF8B[index_high] = seg_high;
// }

// void lcd_display_bye(void) {
//     lcd_draw_char_manual('B', 0);
//     lcd_draw_char_manual('Y', 1);
//     lcd_draw_char_manual('E', 2);
// }



//-------------------------------------------------------------------------------------------------------------------------------


void lcd_display_bye(void) {
     lcd_set(LCD_CLEAR, 1);
     lcd_set(0x0B,      2);  //Escribimo-la B
     lcd_set(LCD_CLEAR, 3);
     lcd_set(0x0E,      4); //Escribimo-la E

     // Apagar primeiro díxito
     LCD->WF8B[LCD_FRONTPLANE0] = 0x0;
     LCD->WF8B[LCD_FRONTPLANE1] = 0x0;

     // Mostrar "Y" no segundo díxito. Segmentos: B + C + D + F + G
     LCD->WF8B[LCD_FRONTPLANE4] = LCD_SEG_D | LCD_SEG_F | LCD_SEG_G;
     LCD->WF8B[LCD_FRONTPLANE5] = LCD_SEG_B | LCD_SEG_C;
 }

void xestionar_interrupcion_temporal() {
    // Máquina de estados
    switch(temporizador.estado) {
        case ESTADO_TEMPORIZADOR_INICIADO:
            if(contar_hacia_atras()) {
                if(temporizador.seg_temporizador <= 0) {
                    temporizador.estado = ESTADO_FIN_TEMPORIZADOR;

                    apagar_canle_pit(CANLE_TEMPORIZADOR_LEDS);

                    set_led_off(&RED_LED);
                    set_led_off(&GREEN_LED);

                    LCD->AR = LCD_AR_BLINK(1) | LCD_AR_BRATE(0x02);  // Acender o parpadeo do display LCD e a velocidade
                }
            }
            break;
        case ESTADO_FIN_PROGRAMA:
            lcd_toggle_power();
            lcd_display_bye();
            toggle_led(&GREEN_LED);
            disable_button_interrupts();
            temporizador.estado = ESTADO_APAGAR;
            return;
        case ESTADO_APAGAR:
            toggle_led(&RED_LED);
            toggle_led(&GREEN_LED);
            apagar_canle_pit(CANLE_TEMPORIZADOR_PRINCIPAL);
            lcd_toggle_power();
            return;
        default: //manexamos o resto dos estados
            // Non facer nada 
            break;   
    }

    refrescar_display();
}

void PITIntHandler(void) {
    if (PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].TFLG & PIT_TFLG_TIF_MASK) {
        PIT->CHANNEL[CANLE_TEMPORIZADOR_PRINCIPAL].TFLG &= PIT_TFLG_TIF_MASK;   // Limpar interrupción
        xestionar_interrupcion_temporal();
    }

    if (PIT->CHANNEL[CANLE_TEMPORIZADOR_LEDS].TFLG & PIT_TFLG_TIF_MASK) {
        PIT->CHANNEL[CANLE_TEMPORIZADOR_LEDS].TFLG &= PIT_TFLG_TIF_MASK;    // Limpar interrupciuón

        if (temporizador.estado != ESTADO_TEMPORIZADOR_INICIADO) {
            return;
        }

        toggle_led(&RED_LED);
        toggle_led(&GREEN_LED);
        SegLCD_Col_Alterno();
    }
}

/**
 *  Formato do display [aa:ss]
 *      - aa: Tempo de alarma      [0, 99] (segundos)
 *      - ss: Tempo de conta atrás [0, 99] (segundos)
 */
int main(void)
{
    irclk_ini();                    // Habilitar reloxo interno para LCD
    lcd_ini();                      // Inicializar LCD
    setup_io();                     // Configurar LEDs e botóns
    setup_led_blink_timer();        // Configurar PIT para pestanexar alarma

    refrescar_display();

    for(;;) {
        __WFI();
    }

    return 0;
}



//----------------------------------------------------------------------------------------
// --------------------- CÓDIGO PREVIO (NO FUNCIONA) -------------------------------------
//----------------------------------------------------------------------------------------

// #include "MKL46Z4.h"
// #include "lcd.h"
// #include "fsl_clock.h" // Parte do SDK de NXP para configura-lo reloxo.
// #include "fsl_pit.h"   // Parte do SDK de NXP para configura-lo temporizador PIT (Periodic Interrupt Timer).
// #include <sys/types.h> // Para o uso de tipos de datos como uint32_t.

// //Asocia os números de pin físicos a nomes lexibles.
// #define RED_LED_PIN 29U
// #define GREEN_LED_PIN 5U
// #define SW1 3U  // Botón dereito - LED verde
// #define SW3 12U // Botón esquerdo - LED vermello

// // Tipo para os LEDs
// // Definimos os LEDs como punteiros a estruturas que contén o porto e o pin
// // que se van a usar para controlar os LEDs. A estrutura Led contén un punteiro ao porto GPIO
// // e un número de pin. O porto GPIO é un tipo definido na biblioteca fsl_gpio.h, que representa un porto GPIO
// // no microcontrolador. O número de pin é un valor enteiro que representa o número do pin no porto GPIO.
// // A estrutura Led permite que o código sexa máis legible e fácil de manter, xa que podemos usar
// // a estrutura Led para referirnos aos LEDs en lugar de usar directamente os números de pin e porto.
// // Isto facilita a modificación do código se cambiamos os pines ou os portos que usamos para os LEDs.


// typedef struct {
//     GPIO_Type *port;  // Puntero ao porto GPIO
//     uint32_t pin;     // Número do pin
// } Led;


// // Definimos os LEDs como instancias da estrutura Led
// // O porto GPIO e o número de pin para cada LED son definidos aquí


// const Led RED_LED   = {GPIOE, RED_LED_PIN};  //Co estruct temos definido o porto e o pin
// const Led GREEN_LED = {GPIOD, GREEN_LED_PIN};

// //Definimos variables globáis

// volatile int estado = 0;
// volatile int alarm = 0, crono = 0;



// // Definición do temporizador PIT. Define configuraciones para el temporizador PIT (Periodic Interrupt Timer) y una bandera para indicar si se produjo una interrupción.


// #define PIT_LED_HANDLER PIT_IRQHandler // Definición dun alias para a función de interrupción para o temporizador PIT
// #define PIT_IRQ_ID PIT_IRQn // Definición dun alias para do ID da interrupción para o temporizador PIT


// /* Get source clock for PIT driver */
// #define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk) // Definición do reloxo de fonte para o controlador PIT. O controlador PIT usa o reloxo de bus como fonte de reloxo. 
// //CLOCK_GetFreq(kCLOCK_BusClk) é unha función do SDK de NXP que devolve a frecuencia dp Bus Clock (o reloxo interno que alimenta periféricos como GPIO, PIT, UART...).
// //PIT_SOURCE_CLOCK é o valor que se usará, por exemplo, para calcular cuántos ticks corresponden a 1 segundo o 1 milisegundo.

// // O struct de pit_config_t e da seguinte maneira (os campos mais comunes)
// // typedef struct {
// //     bool enableRunInDebug;  Este campo determina si el PIT debe seguir funcionando cuando el microcontrolador está en modo debug. Si es true, el PIT seguirá funcionando, incluso si el microcontrolador está en modo debug. Si es false, el PIT se detendrá en modo debug.
// // } pit_config_t;


// pit_config_t pitConfig; // Definición da configuración do temporizador PIT. pit_config_t é unha estrutura que contén as configuracións do temporizador PIT.
// // Esta estrutura é parte do SDK de NXP e contén campos como o modo de depuración, o modo de espera, etc.
// // pitConfig é unha variable que se usará para inicializar o temporizador PIT coas configuracións desexadas.
// volatile bool pitIsrFlag = false; // Definición da bandeira de interrupción do temporizador PIT. pitIsrFlag é unha variable booleana que se usará para indicar se se produciu unha interrupción no temporizador PIT. indica que, por defecto, la interrupción no ha ocurrido.


// // Enable IRCLK (Internal Reference Clock) Activa el reloj interno de 32 kHz para que pueda ser usado por la pantalla LCD.
// // see Chapter 24 in MCU documentation

// void irclk_ini() 
// {
//     MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
//     MCG->C2 = MCG_C2_IRCS(0);
// }

// // Configuración de los LEDs Dous duncions para encender e apagar os LEDs. Estas funcións usan o porto GPIO e o número de pin definidos na estrutura Led para controlar os LEDs.
// // A función set_led_on() pon o pin a 0 (enciende o LED) e a función set_led_off() pon o pin a 1 (apaga o LED).
// // Estas funcións son chamadas desde o manexador de interrupcións para os botóns (PORTDIntHandler) para controlar os LEDs en función do estado dos botóns.


// void set_led_on(const Led *led)  //creamos a funcion que chama o punteiro do struct onde se definiron o red_led e o green_led
// {
//     led->port->PCOR = (1U << led->pin); //PCOR: pon o pin a 0 (enciende el LED). pax. 835 do manual de referencia para o GPIOE_PCOR por exemplo (led RED)
// }

// void set_led_off(const Led *led)
// {
//     led->port->PSOR = (1U << led->pin); //PSOR: pon o pin a 1 (apaga el LED).
// }

// // Manexador de interrupcións para os botóns
// //Comproba si se pulsó un dos botóns (SW1 ou SW3) lendo a bandeira de interrupción ISF e actuando en consecuencia

// void PORTDIntHandler(void)  //PORTDIntHandler sirve tanto para PORTC como para PORTD, Ver startup.c
// {
//     // Botón dereito pulsado (SW1)
//     if ((PORTC->PCR[SW1] >> PORT_PCR_ISF_SHIFT) & 0x1U)  // Comprobamos se a bandeira de interrupción ISF está activa para o botón dereito (SW1)
//     // O operador >> desplaza os bits do valor de PCR[SW1] cara á dereita, e & 0x1U comprueba se o bit menos significativo é 1 (indica que a interrupción ocorreu)
//     // No manual, na páxina 193 vese que o pin do ISF é o 24, polo que o facer _SHIFT colocamonos nel.
//     {
//         // Xestionar botón dereito
//         lcd_display_time(00, 00);
//         set_led_on(&RED_LED);
//         set_led_on(&GREEN_LED);

//         PORTC->PCR[SW1] |= PORT_PCR_ISF(1); // Limpiar a bandeira de interrupción ISF para o botón dereito (SW1)
//         // O operador |= establece o bit ISF a 1, indicando que a interrupción foi tratada.
//         // O operador PORT_PCR_ISF(1) establece o bit ISF a 1, indicando que a interrupción foi tratada.
//     }

//     // Botón esquerdo pulsado (SW3)
//     if ((PORTC->PCR[SW3] >> PORT_PCR_ISF_SHIFT) & 0x1U)
//     {
//         // TODO: Xestionar botón esquerdo
//         lcd_display_time(88, 88);
//         set_led_off(&RED_LED);
//         set_led_off(&GREEN_LED);

//         PORTC->PCR[SW3] |= PORT_PCR_ISF(1);
//     }
// }

// void setup_io(void)
// {
//     // Habilitar reloxos de portos
//     SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTD_MASK);  //Habilita-los reloxos dos portos C, E e D

//     // Configurar LEDs
//     PORTD->PCR[GREEN_LED_PIN] = PORT_PCR_MUX(1U);
//     PORTE->PCR[RED_LED_PIN] = PORT_PCR_MUX(1U);
//     GPIOE->PDDR = (1U << RED_LED_PIN);
//     GPIOD->PDDR = (1U << GREEN_LED_PIN);

//     // Apagar LEDs inicialmente
//     GPIOE->PSOR = (1U << RED_LED_PIN);
//     GPIOD->PSOR = (1U << GREEN_LED_PIN);

//     // Configurar botóns
//     PORTC->PCR[SW1] = PORT_PCR_MUX(1U); // Configurar o mux para o botón dereito (SW1)
//     PORTC->PCR[SW3] = PORT_PCR_MUX(1U); // Configurar o mux para o botón esquerdo (SW3)
//     PORTC->PCR[SW1] |= (PORT_PCR_PE(1U) | PORT_PCR_PS(1U)); // Habilitar pull-up/pull-down para o botón dereito (SW1). PE = Pull Enable. PS = Pull Select sendo o 1 o pull-up
//     PORTC->PCR[SW3] |= (PORT_PCR_PE(1U) | PORT_PCR_PS(1U)); // Habilitar pull-up/pull-down para o botón esquerdo (SW3)
//     PORTC->PCR[SW1] |= PORT_PCR_IRQC(0xA); // Configurar interrupcións para o botón dereito (SW1). Usamos 0xA para configurar a interrupción "on falling edge". Páxina 194 do manual de referencia
//     PORTC->PCR[SW3] |= PORT_PCR_IRQC(0xA); // Configurar interrupcións para o botón esquerdo (SW3)
//     GPIOC->PDDR &= ~(1U << SW1); // Configurar o pin do botón dereito (SW1) como entrada
//     GPIOC->PDDR &= ~(1U << SW3); // Configurar o pin do botón esquerdo (SW3) como entrada

//     // Habilitar interrupcións
//     NVIC_EnableIRQ(PORTC_PORTD_IRQn);
// }

// void disable_button_interrupts(void)
// {
//     PORTC->PCR[SW1] &= ~PORT_PCR_IRQC_MASK;  // Desactivar interrupción en SW1. Invierte a máscara convertindo a 0 os bits de IRQC deixando o resto intacto. 
//     PORTC->PCR[SW3] &= ~PORT_PCR_IRQC_MASK;  // Desactivar interrupción en SW3
// }



// void PIT_LED_HANDLER(void)  //liña 46 do código. É a función que se executa cando se produce unha interrupción do temporizador PIT. O nome da función é PIT_IRQHandler, que é o nome estándar para o manexador de interrupcións do temporizador PIT no SDK de NXP.
// {
//     /* Clear interrupt flag.*/
//     PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag); // kPIT_chnl_0 é o canal 0 do temporizador PIT. kPIT_TimerFlag é a bandeira de interrupción do temporizador. Esta bandeira actívase cando o temporizador alcanza o valor configurado
//     pitIsrFlag = true; // Creamos unha bandeira para mostrar que detectouse unha interrupción e aumentamos co contador.
// }

// void setup_pit(void)
// {
    
//     PIT_GetDefaultConfig(&pitConfig); //Establece os valores predeterminados recomendados polo fabricante para a configuración inicial do PIT

//     /* Init pit module */
//     PIT_Init(PIT, &pitConfig); //inicializa o módulo PIT utilizando a configuración almacenada no pitConfig

//     /* Set timer period for channel 0 */
//     PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(1000000U, PIT_SOURCE_CLOCK)); // Esta función usase para establecer o periodo do temporizador do canle específico dun PIT. Ten a sitanxe: void PIT_SetTimerPeriod(PIT_Type *base, pit_channel_t channel, uint32_t period);
//     // UESEC_TO_COUNT convierte o valor de microsegundos a contadores do PIT. O primeiro argumento é o valor en microsegundos e o segundo argumento é a frecuencia do reloxo de fonte do PIT. Esta función devolve o número de contadores que corresponden ao valor de microsegundos dado.
//     // O valor devolto é usado para establecer o periodo do temporizador do canle 0 do PIT. O segundo argumento é o reloxo de fonte do PIT, que se define como PIT_SOURCE_CLOCK.
//     /* Enable timer interrupts for channel 0 */
//     PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable); //U tilizase para habilita-las interrupcions da canle 0 del PIT. void PIT_EnableInterrupts(PIT_Type *base, pit_channel_t channel, uint32_t mask); mask: Un valor de máscara que especifica qué tipo de interrupciones habilitar. En este caso, se usa kPIT_TimerInterruptEnable para habilitar la interrupción por desbordamiento del temporizador.
//     //kPIT_TimerInterruptEnable é unha constante definida no SDK de NXP que representa a máscara para habilitar a interrupción do temporizador. Esta máscara indica que queremos habilitar a interrupción por desbordamento do temporizador para o canle 0 do PIT.
//     // kPIT_TimerInterruptEnable es generalmente una constante que tiene un valor de máscara como PIT_TCTRL_TIE_MASK, que habilita la interrupción para el canal específico del PIT.

//     /* Enable at the NVIC */
//     EnableIRQ(PIT_IRQ_ID);  //Permite que o NVIC habilite unha interrupción específica no procesador

//     /* Start channel 0 */
//     PIT_StartTimer(PIT, kPIT_Chnl_0); //PIT_StartTimer(PIT_Type *base, pit_chnl_t channel). Arranca o temporizador do canle especificado. O primeiro argumento é o módulo PIT e o segundo argumento é o canle do PIT que queremos iniciar. En este caso, estamos iniciando o canle 0 do PIT.

//     while (true)
//     {
//         /* Check whether occur interupt and toggle LED */
//         if (true == pitIsrFlag)
//         {
//             // PRINTF("\r\n Channel No.0 interrupt is occured !");
//             crono += 1;
//             lcd_display_time(alarm, crono);
//             pitIsrFlag = false;
//         }
//     }
// }


// /**
//  *  Formato do display [aa:ss]
//  *      - aa: Tempo de alarma      [0, 99] (segundos)
//  *      - ss: Tempo de conta atrás [0, 99] (segundos)
//  *
//  *  Estado 0: Introducir 'aa'
//  *  Estado 1: Introducir 'ss'
//  *  Estado 2: Empeza conta atrás
//  */
// int main(void)
// {
//     irclk_ini(); // Habilitar reloxo interno para LCD
//     lcd_ini();   // Inicializar LCD
//     setup_io();  // Configurar LEDs e botóns

//     lcd_display_time(10, 77);

//     setup_pit();

//     disable_button_interrupts();

//     while(1) {

//     }

//     return 0;
// }
