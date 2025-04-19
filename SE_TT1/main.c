#include "main.h"
#include "funcions_basicas.h"

// ver main.h para aclaraciones de structs, funcións, etc. 
// Para as aclaracions ve-lo primeiro main subido que ten o código antigo e as súas aclaracións
// Nesta versión depurei o código para que sexa mais lexible

const Led RED_LED   = {GPIOE, RED_LED_PIN};
const Led GREEN_LED = {GPIOD, GREEN_LED_PIN};

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0);
}

static inline void SegLCD_Col_Alterno(void) {  // alternamos (prendemos ou apagamos) o segmento decimal dunha pantalla Segment LCD
    SegLCD_Col_Toggle();  // Alternar o segmento decimal
    // Definimos no lcd.h o macro SegLCD_Col_Toggle() para alternar o segmento decimal
}

static inline void lcd_toggle_power(void) { // Encendido ou apagado do LCD
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



void setup_io(void) {  // ver funcions_basicas.c
    // Habilitar reloxos de portos
    habilitar_reloxo();

    // Configurar LEDs
    configurar_leds();

    // Configurar botóns
    configurar_botons();
    // Habilitar interrupcións no NVIC
    configurar_interrupcions();
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

