# Traballo tutelado

## Alumno

Eduardo Manuel Caridad Cordeiro (eduardo.caridad.cordeiro@udc.es)


## Enunciado


TT2. PWM + LED + Sensor
Requisitos de finalización
Apertura: martes, 6 de mayo de 2025, 15:30
Cierre: viernes, 23 de mayo de 2025, 23:59

Programade unha aplicación para a placa FRDM-KL46Z que faga uso de PWM para acender os LEDs con maior ou menor intensidade, en función do valor dun sensor da placa.

Tedes tres opcións a escoller:

    A. Usar o slider: ao premer nun extremo do slider acéndese un led moi intenso nun dos extremos, minguando a intensidade segundo nos desprazamos cara o outro extremo, ao mesmo tempo que se incrementa a do outro LED, que pasará a ser o único acendido, coa súa máxima intensidade, ao chegar ao outro extremo.

    Como mostramos nas diapositivas (tema 9), na SDK de Kinetis tedes un exemplo de como empregar o módulo TSI (Touch Sensing Interface) integrado no MCU. Este módulo está deseñado especificamente para detectar cambios de capacitancia nos sensores táctiles (como o slider) e realiza as operacións necesarias para obter os valores.

    B. Sensor de luz da placa: con pouca luz, acenderemos os dous LEDs con máis luminosidade, e con moita luz estarán apagados (ou case). Como alternativa podedes facer, se queredes,  que un LED se acenda con moita luz e o outro con pouca.

    Para usar o sensor de luz precisaremos facer unha conversión analóxico->dixital (ADC). Os pasos básicos serían:
        Configurar o pin ao que conecta o sensor co ADC
        Configura o ADC (tedes algún exemplo na SDK)
        Accede á lectura do sensor a través da conversión do ADC
    C. Magnetómetro: Facer que a intensidade dun dos LEDs sexa moi forte nunha dirección, e moi baixa cando imos cara a contraria, ata apagarse, co outro LED comportándose ao contrario. O magnetómetro da nosa placa (MAG3110) está conectado por I2C.



## Resumen

Este código está diseñado para un sistema empotrado (neste caso, unha placa NXP MKL46Z4), co propósito de axustar automáticamente o brillo de dos LEDs (vermello e verde) en función da luz ambiental medida por un sensor LDR, usando PWM (modulación por ancho de pulso) e conversión analóxica a dixital (ADC).


## Anotacions

Si se quisiera recalibra-lo comportamiento dos LED según o rango de lecturas do sensor de luz, poderíanse seguir os seguintes pasos:

1. Compilar e executa-lo programa en modo debug, para seguir no LCD los valores leídos por el sensor:
    ```
    make debug
    make flash
    ```
2. Expo-lo sensor de luz da placa á fonte de luz máis brillante disponible, e anota-ĺo valor máis alto. Face-lo mesmo pero coa luz apagada (ou tapando o sensor co dedo) e apunta-lo valor máis baixo.
3. Colocar ditos valores no struct de configuración do `main.c`. Por exemplo:
    ```C
    const LightThresholds LIGHT_THRESHOLDS = {
        // Umbrales de luz para encender los LED
        .high = 4050,
        .low  = 3840
    };
    ```