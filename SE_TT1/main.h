#ifndef _MAIN_H_
#define _MAIN_H_

#include "MKL46Z4.h"
#include "lcd.h"
#include <sys/types.h>

#define RED_LED_PIN   29u
#define GREEN_LED_PIN 5u
#define LEFT_BUTTON   12u
#define RIGHT_BUTTON  3u

#define TEMPO_MAX     (99)
#define CANLE_TEMPORIZADOR_PRINCIPAL (0)
#define CANLE_TEMPORIZADOR_LEDS      (1)

#define FRECUENCIA_CPU 48000000U 

// Estrutura para representar un LED
typedef struct {
    GPIO_Type *port;  // Porto
    uint32_t pin;     // Pin
} Led;

typedef enum {
    ESTADO_INICIAL,
    ESTADO_INTRODUCIR_TEMPO,
    ESTADO_INTRODUCIR_ALARMA,
    ESTADO_TEMPORIZADOR_INICIADO,
    ESTADO_TEMPORIZADOR_PAUSADO,
    ESTADO_FIN_TEMPORIZADOR,
    ESTADO_FIN_PROGRAMA,
    ESTADO_APAGAR
} EstadoTemporizador;

typedef enum {
    BUTTON_NONE = -1,
    BUTTON_LEFT = 0,
    BUTTON_RIGHT = 1
} TipoBoton;

static volatile struct {
    EstadoTemporizador estado;
    uint32_t ticks_restantes;
    uint8_t seg_alarma;
    uint8_t seg_temporizador;
} temporizador = {
    .estado = ESTADO_INICIAL,
    .ticks_restantes = 0,
    .seg_alarma = 0,
    .seg_temporizador = 0
};

void irclk_ini(void);                               // Initialize internal clock
void set_led_on(const Led *led);                    // Turn on an LED
void set_led_off(const Led *led);                   // Turn off an LED
void refrescar_display(void);                       // Refresh the display
void PORTDIntHandler(void);                         // Interrupt handler for buttons
void PITIntHandler(void);                           // Interrupt handler for PIT
void setup_io(void);                                // Setup I/O for buttons and LEDs
void setup_led_blink_timer(void);                   // Set up LED blink timer
void disable_button_interrupts(void);               // Disable button interrupts
void interrupcion_LEFT_BUTTON(void);                // Left button interrupt handler
void interrupcion_RIGHT_BUTTON(void);               // Right button interrupt handler
void xestionar_interrupcion_temporal(void);         // PIT interrupt handler
void introducir_alarma(void);                       // Introduce alarm time
void iniciar_conta_atras(void);                     // Start countdown
void pausar_conta_atras(void);                      // Pause countdown
uint8_t contar_hacia_atras(void);                   // Count backwards
void comprobar_alarma(void);                        // Check alarm
void acender_canle_pit(uint8_t canle);              // Enable PIT channel
void apagar_canle_pit(uint8_t canle);               // Disable PIT channel

#endif  // _MAIN_H_
