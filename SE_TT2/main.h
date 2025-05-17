#ifndef MAIN_H
#define MAIN_H

#include "includes/MKL46Z4.h"
#include "includes/fsl_adc16.h"

// Configuración de Debug
#ifdef DEBUG
    #include "includes/lcd.h"
#endif

// Definicions xerais
#define TPM_MODULE_CLOCK 48000000  // Frecuencia do módulo TPM (48 MHz)
#define DELAY_CYCLES 1000
#define ADC_MAX_VALUE 4095 // 12 bits
#define PWM_MAX_VALUE 255  // 8 bits

// Estructuras de configuración
typedef struct {
    ADC_Type* base;
    uint8_t group;
    uint8_t channel;
} AdcConfig;

typedef struct {
    uint8_t redLedChannel;
    uint8_t greenLedChannel;
    uint8_t period;
} TpmConfig;

typedef struct {
    uint8_t greenLedPin;
    uint8_t redLedPin;
    uint8_t lightSensorPin;
} PinConfig;

typedef struct {
    uint32_t high;
    uint32_t low;
} LightThresholds;

// Configuración de hardware
extern const AdcConfig ADC_CONFIG;
extern const TpmConfig TPM_CONFIG;
extern const PinConfig PIN_CONFIG;
extern const LightThresholds LIGHT_THRESHOLDS;

// Funciones de inicialización
void LED_Init(void);
void TPM_Init(void);
void LDR_Init(void);
void System_Init(void);

static void Delay(volatile int cycles);
static inline uint8_t LimitBrightness(uint8_t brightness);
void LED_SetBrightness(uint8_t red_brightness, uint8_t green_brightness);
static uint32_t ReadLightSensor(void);
uint8_t CalculateBrightness(uint32_t lightValue);
uint8_t ApplyGammaCorrection(uint32_t adcValue);

#ifdef DEBUG
void LCD_Init(void);
void Debug_DisplayLightValue(uint32_t lightValue);
#endif

#endif // MAIN_H

