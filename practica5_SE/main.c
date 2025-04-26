#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "MKL46Z4.h"
#include <stdlib.h>
#include "lcd.h"

#define MAX_PROD_CONS 5 // Máximo 5 produtores e 5 consumidores.
#define QUEUE_NUM_ELEMENTS 99 // Número máximo de elementos na fila.

volatile int num_producers = 0; //Número actual de produtores e consumidores activos.
volatile int num_consumers = 0; //Número actual de produtores e consumidores activos.

static TaskHandle_t producerHandles[MAX_PROD_CONS] = {NULL};  //Gardan os punteiros ás tarefas produtoras e consumidoras creadas. (para poder matalas despois)
static TaskHandle_t consumerHandles[MAX_PROD_CONS] = {NULL};

static QueueHandle_t queue = NULL; // Creamo-la cola de mensaxes compartida entre produtores e consumidores.

SemaphoreHandle_t xMutex = NULL; // Semáforo tipo mutex para protexer o acceso ao LCD (evitar que dous fíos actualicen o LCD á vez).

//-------------------------------------------------- DECLARACIONS DE FUNCIONS --------------------------------------------------//

void updateLCD(void);
void PORTC_PORTD_IRQHandler(void);
void sw_init(void);
void irclk_ini();
static void produce(void *pvParameters);
static void consume(void *pvParameters);
void queue_init(int queue_length, int queue_max);
void queue_read();
void queue_add(int num);
void manage_consumers();
void manage_producers();
void consumer_producer_task(void *pvParameters);

//-------------------------------------------------- FUNCIONS BÁSICAS--------------------------------------------------//

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHz; 1 = 4MHz
}

void sw_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IRQC(0xA);
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IRQC(0xA);
    GPIOC->PDDR &= ~((1 << 3) | (1 << 12));
}

void PORTDIntHandler(void) {
    if (PORTC->ISFR & (1 << 3)) {
        manage_consumers();
        PORTC->ISFR = (1 << 3);
    }
    if (PORTC->ISFR & (1 << 12)) {
        manage_producers();
        PORTC->ISFR = (1 << 12);
    }
}

//-------------------------------------------------- FUNCIONS DE XESTION DE COLAS --------------------------------------------------//

void manage_producers(void) {

    num_producers = (num_producers + 1) % (MAX_PROD_CONS + 1); // Incrementa o número de produtores activos, e se chega ao máximo, volve a 0.

    for (int i = 0; i < MAX_PROD_CONS; i++) {
        if (i < num_producers && producerHandles[i] == NULL) {
            xTaskCreate(produce, "Producer", configMINIMAL_STACK_SIZE, NULL, 0, &producerHandles[i]); // Crea un novo produtor se non hai espazo na cola.

        } else if (i >= num_producers && producerHandles[i] != NULL) {
            vTaskDelete(producerHandles[i]); // Elimina o produtor se non hai espazo na cola.
            producerHandles[i] = NULL;
        }
    }

    updateLCD();
}

void manage_consumers(void) {

    num_consumers = (num_consumers + 1) % (MAX_PROD_CONS + 1);

    for (int i = 0; i < MAX_PROD_CONS; i++) {
        if (i < num_consumers && consumerHandles[i] == NULL) {
            xTaskCreate(consume, "Consumer", configMINIMAL_STACK_SIZE, NULL, 0, &consumerHandles[i]);

        } else if (i >= num_consumers && consumerHandles[i] != NULL) {
            vTaskDelete(consumerHandles[i]);
            consumerHandles[i] = NULL;
        }
    }

    updateLCD();
}

// -------------------------------------------------- FUNCIONS DE XESTION DO LCD --------------------------------------------------//

void updateLCD(void) {
    uint8_t pendingMessages = uxQueueMessagesWaiting(queue); // Mensaxes pendentes na cola.
    lcd_display_time(pendingMessages, num_producers * 10 + num_consumers); // Actualiza o LCD co número de mensaxes pendentes e o número de produtores e consumidores activos.
}

//-------------------------------------------------- TAREFAS PRODUCTORAS E CONSUMIDORAS --------------------------------------------------//


static void produce(void *pvParameters) {
    while (1) {
        int value = 1;
        xQueueSend(queue, &value, portMAX_DELAY); // Envia un número á cola.
        vTaskDelay(500/portTICK_PERIOD_MS); // Simula un tempo de produción.

        xSemaphoreTake(xMutex, portMAX_DELAY); // Toma o semáforo para protexer o acceso ao LCD.
        updateLCD(); // Actualiza o LCD co número de mensaxes pendentes e o número de produtores e consumidores activos.
        xSemaphoreGive(xMutex); // Libera o semáforo.
    }
}

static void consume(void *pvParameters) {
    while (1) {
        int value;
        xQueueReceive(queue, &value, portMAX_DELAY);
        vTaskDelay(500/portTICK_PERIOD_MS);

        xSemaphoreTake(xMutex, portMAX_DELAY);
        updateLCD();
        xSemaphoreGive(xMutex);
    }
}

//-------------------------------------------------- FUNCION PRINCIPAL --------------------------------------------------//

int main(void) {

    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
    irclk_ini();
    sw_init();
    lcd_ini();

    xMutex = xSemaphoreCreateMutex(); // Crea o semáforo tipo mutex para protexer o acceso ao LCD.
    queue = xQueueCreate(QUEUE_NUM_ELEMENTS, sizeof(int)); // Crea a cola de mensaxes compartida entre produtores e consumidores.

    updateLCD(); // Actualiza o LCD co número de mensaxes pendentes e o número de produtores e consumidores activos.

    vTaskStartScheduler(); // Inicia o planificador de FreeRTOS.

    // Si llega aquí, houbo un erro ao iniciar o planificador. 
    for (;;);
    return 0;
}

// -------------------------------------------------- CODIGO BASE DE CHATGPT PARA COLLER IDEAS --------------------------------------------------//

// #include "FreeRTOS.h"
// #include "task.h"
// #include "queue.h"
// #include "semphr.h"
// #include "lcd.h"     // A túa libraría de LCD
// #include "buttons.h" // Funcións para ler botóns

// #define MAX_PRODUTORES 5
// #define MAX_CONSUMIDORES 5
// #define QUEUE_LENGTH 20

// QueueHandle_t queue;
// volatile uint8_t n_produtores = 1;
// volatile uint8_t n_consumidores = 1;

// TaskHandle_t produtores[MAX_PRODUTORES];
// TaskHandle_t consumidores[MAX_CONSUMIDORES];

// // Prototipos
// void produtor_task(void *pvParameters);
// void consumidor_task(void *pvParameters);
// void lcd_update_task(void *pvParameters);
// void button_check_task(void *pvParameters);

// void app_main(void) {
//     queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));

//     // Crear tasks de produtores
//     for (int i = 0; i < MAX_PRODUTORES; i++) {
//         xTaskCreate(produtor_task, "Produtor", 128, (void *)(intptr_t)i, 1, &produtores[i]);
//         vTaskSuspend(produtores[i]);
//     }

//     // Crear tasks de consumidores
//     for (int i = 0; i < MAX_CONSUMIDORES; i++) {
//         xTaskCreate(consumidor_task, "Consumidor", 128, (void *)(intptr_t)i, 1, &consumidores[i]);
//         vTaskSuspend(consumidores[i]);
//     }

//     xTaskCreate(lcd_update_task, "LCD_Update", 128, NULL, 1, NULL);
//     xTaskCreate(button_check_task, "Button_Check", 128, NULL, 1, NULL);

//     // Activar un produtor e un consumidor ao principio
//     vTaskResume(produtores[0]);
//     vTaskResume(consumidores[0]);

//     vTaskStartScheduler();

//     while(1); // Nunca debería chegar aquí
// }

// void produtor_task(void *pvParameters) {
//     uint32_t my_id = (uint32_t)pvParameters;
//     while (1) {
//         uint32_t data = my_id; // Datos arbitrarios
//         vTaskDelay(pdMS_TO_TICKS(500)); // Simular obtención de datos
//         xQueueSend(queue, &data, portMAX_DELAY);
//     }
// }

// void consumidor_task(void *pvParameters) {
//     uint32_t data;
//     while (1) {
//         if (xQueueReceive(queue, &data, portMAX_DELAY)) {
//             vTaskDelay(pdMS_TO_TICKS(700)); // Simular procesamento
//             // Aquí poderías facer algo máis co dato recibido
//         }
//     }
// }

// void lcd_update_task(void *pvParameters) {
//     while (1) {
//         uint32_t pending = uxQueueMessagesWaiting(queue);

//         // Mostrar no LCD
//         lcd_clear();
//         lcd_set_cursor(0, 0);

//         // Primeiro 2 díxitos: datos pendentes (modulo 100 para 2 díxitos)
//         lcd_print_digit((pending / 10) % 10);
//         lcd_print_digit(pending % 10);

//         // Logo: nº de produtores e consumidores activos
//         lcd_print_digit(n_produtores % 10);
//         lcd_print_digit(n_consumidores % 10);

//         vTaskDelay(pdMS_TO_TICKS(200)); // Actualizar cada 200ms
//     }
// }

// void button_check_task(void *pvParameters) {
//     while (1) {
//         if (button_left_pressed()) {
//             // Cambiar produtores
//             n_produtores = (n_produtores + 1) % (MAX_PRODUTORES + 1);
//             for (int i = 0; i < MAX_PRODUTORES; i++) {
//                 if (i < n_produtores) {
//                     vTaskResume(produtores[i]);
//                 } else {
//                     vTaskSuspend(produtores[i]);
//                 }
//             }
//         }

//         if (button_right_pressed()) {
//             // Cambiar consumidores
//             n_consumidores = (n_consumidores + 1) % (MAX_CONSUMIDORES + 1);
//             for (int i = 0; i < MAX_CONSUMIDORES; i++) {
//                 if (i < n_consumidores) {
//                     vTaskResume(consumidores[i]);
//                 } else {
//                     vTaskSuspend(consumidores[i]);
//                 }
//             }
//         }

//         vTaskDelay(pdMS_TO_TICKS(100)); // Polling cada 100ms
//     }
// }
