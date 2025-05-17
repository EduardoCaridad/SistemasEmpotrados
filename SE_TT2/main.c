// Sistemas Emppotrados - Traballo Tutelado 2.

#include "main.h"

// Definimo-los pins empregado:
const PinConfig PIN_CONFIG = {
    .greenLedPin    = 5U,   // PTD5
    .redLedPin      = 29U,  // PTE29
    .lightSensorPin = 22U   // PTE22
};


//Definimo-los canles TPM para PWM e os umbrales de luz

// Definimo-la configuración del TPM
const TpmConfig TPM_CONFIG = {
    .redLedChannel   = 2U,  // TPM0_CH2
    .greenLedChannel = 5U,  // TPM0_CH5
    .period = 255U
};

// Definimo-los umbrales de luz
const LightThresholds LIGHT_THRESHOLDS = {
    // Umbrales de luz para encender los LED
    .high = 4050,
    .low  = 3840
};


const AdcConfig ADC_CONFIG = {
    // ADC0_SE3
    .base    = ADC0,
    .group   = 0U,
    .channel = 3U
};

static adc16_channel_config_t adc16ChannelConfig;

/* LUT para corrección gamma (^2.2) frente a non linealidade dos LED.
Precalculada porque en execución resultaba demasiado costoso.
*/
static const uint8_t gammaLUT[256] = {
0,     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,
1,     1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,
3,     3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6,   6,
6,     7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  11,  11,  11,  12,
12,   13,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,
20,   20,  21,  22,  22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,
30,   30,  31,  32,  33,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,  41,
42,   43,  43,  44,  45,  46,  47,  48,  49,  49,  50,  51,  52,  53,  54,  55,
56,   57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
73,   74,  75,  76,  77,  78,  79,  81,  82,  83,  84,  85,  87,  88,  89,  90,
91,   93,  94,  95,  97,  98,  99, 100, 102, 103, 105, 106, 107, 109, 110, 111,
113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 127, 129, 130, 132, 133, 135,
137, 138, 140, 141, 143, 145, 146, 148, 149, 151, 153, 154, 156, 158, 159, 161,
163, 165, 166, 168, 170, 172, 173, 175, 177, 179, 181, 182, 184, 186, 188, 190,
192, 194, 196, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221,
223, 225, 227, 229, 231, 234, 236, 238, 240, 242, 244, 246, 248, 251, 253, 255
};


// Funciones de inicialización
// Configuramo-los pins como saidas PWM

void LED_Init(void) {
    // Desactivar Watchdog
    SIM->COPC &= ~0xC;

    // Habilitar reloxos para PORTD y PORTE
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    // Configurar pines como TPM
    PORTD->PCR[PIN_CONFIG.greenLedPin] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[PIN_CONFIG.greenLedPin] |= PORT_PCR_MUX(4); // TPM0_CH5

    PORTE->PCR[PIN_CONFIG.redLedPin] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[PIN_CONFIG.redLedPin] |= PORT_PCR_MUX(3);   // TPM0_CH2
}


//  Configuramo-lo temporizador TPM0 e os canles para os LEDs.

void TPM_Init(void) {
    // Habilitar reloxo para TPM0
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    TPM0->MOD = TPM_CONFIG.period;

    // Configurar canal 2 (LED Vermello)
    TPM0->CONTROLS[TPM_CONFIG.redLedChannel].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0->CONTROLS[TPM_CONFIG.redLedChannel].CnV = 0;

    // Configurar canal 5 (LED Verde)
    TPM0->CONTROLS[TPM_CONFIG.greenLedChannel].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0->CONTROLS[TPM_CONFIG.greenLedChannel].CnV = 0;

    // Habilitar TPM con preescaler de 1
    TPM0->SC = TPM_SC_CMOD(1) | TPM_SC_PS(0);
}

// Configuramo-lo ADC para ler o sensor de luz. 

void ADC16_Configuration(void) {
    adc16_config_t adc16Config;

    ADC16_GetDefaultConfig(&adc16Config);

    // Configurar para máxima resolución
    adc16Config.resolution = kADC16_ResolutionSE12Bit;
    adc16Config.enableContinuousConversion = false;

    ADC16_Init(ADC_CONFIG.base, &adc16Config);
    ADC16_EnableHardwareTrigger(ADC_CONFIG.base, false);

    // Configura-lo canle
    adc16ChannelConfig.channelNumber = ADC_CONFIG.channel;
    adc16ChannelConfig.enableInterruptOnConversionCompleted = false;
}


// Configuramo-lo pin do sensor de luz como entrada ADC

void LDR_Init() {
    // Habilitar reloxo para ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Configurar pin PTE22 como ADC0_SE4b
    PORTE->PCR[PIN_CONFIG.lightSensorPin] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[PIN_CONFIG.lightSensorPin] |= PORT_PCR_MUX(0);

    // Configurar ADC0
    ADC16_Configuration();
}

#ifdef DEBUG
void LCD_InternalClockInit() {
	MCG->C1 |= (1 << 1);  // IRCLKEN bit
	MCG->C1 |= (1 << 2);  // IREFSTEN bit
	MCG->C2 &= ~(1 << 0); // Clear IRCS bit for slow internal reference clock
}

void LCD_Init(void) {
    LCD_InternalClockInit();
    lcd_ini();
}

void Debug_DisplayValue(uint32_t value) {
    lcd_display_dec(value);
    Delay(1000 * DELAY_CYCLES); // Retardo mayor para leer el LCD
}
#endif

void System_Init(void) {
    LED_Init();
    TPM_Init();
    LDR_Init();

#ifdef DEBUG
    LCD_Init();
#endif
}

static void Delay(volatile int cycles) {
    for (volatile int delay = 0; delay < cycles; delay++);
}

static inline uint8_t LimitBrightness(uint8_t brightness) {
    return (brightness > TPM_CONFIG.period) ? TPM_CONFIG.period : brightness;
}


// Función para establecer el brillo de los LEDs. Cambia o ciclo de traballo PWM de ambos LEDs para axusta-lo brillo.
// O brillo é un valor entre 0 e 255, onde 0 é apagado e 255 é o máximo brillo.

void LED_SetBrightness(uint8_t red_brightness, uint8_t green_brightness) {
    // Actualizar valores de PWM
    TPM0->CONTROLS[TPM_CONFIG.redLedChannel].CnV = LimitBrightness(red_brightness);     // LED Vermello
    TPM0->CONTROLS[TPM_CONFIG.greenLedChannel].CnV = LimitBrightness(green_brightness); // LED Verde

    // Pequeno retardo para evitar lecturas demasiado rápidas
    Delay(DELAY_CYCLES);
}

// Función para ler o valor do sensor de luz. Inicia a conversión e espera ata que remate.

static uint32_t ReadLightSensor(void) {
    // Inicia a conversión
    ADC16_SetChannelConfig(ADC_CONFIG.base, ADC_CONFIG.group, &adc16ChannelConfig);

    // Esperar ata que a conversión remate
    while (!(ADC16_GetChannelStatusFlags(ADC_CONFIG.base, ADC_CONFIG.group) & kADC16_ChannelConversionDoneFlag));

    // Le-lo resultado da conversión
    return ADC16_GetChannelConversionValue(ADC_CONFIG.base, ADC_CONFIG.group);
}

// Función para calcular o brillo en función do valor do sensor de luz. Convirte o valor ADC nun brillo (0–255) lineal.


uint8_t CalculateBrightness(uint32_t lightValue) {
    if (lightValue >= LIGHT_THRESHOLDS.high) return 0;
    if (lightValue <= LIGHT_THRESHOLDS.low)  return 255;

    return (LIGHT_THRESHOLDS.high - lightValue) * 255 / (LIGHT_THRESHOLDS.high - LIGHT_THRESHOLDS.low);
}


// Función para aplicar a corrección gamma ao valor ADC. A corrección gamma é necesaria para compensar a non linearidade dos LEDs.
// Aplica-la corrección gamma á sinal para que o cambio de brillo sexa máis perceptivo visualmente (con LUT precomputada).

uint8_t ApplyGammaCorrection(uint32_t adcValue) {
    // 12 bits -> 8 bits
    uint8_t rawBrightness = CalculateBrightness(adcValue);

    return gammaLUT[rawBrightness];
}

int main(void) {
    uint32_t lightValue;
    uint8_t brightness;

    System_Init();

    while (1) {
        lightValue = ReadLightSensor();
        brightness = ApplyGammaCorrection(lightValue);
        LED_SetBrightness(brightness, brightness);

    #ifdef DEBUG
        // Mostrar valores no LCD para depurar.
        Debug_DisplayValue(lightValue);
    #endif
    }
}
