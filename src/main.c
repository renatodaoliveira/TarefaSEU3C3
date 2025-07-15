/* =========================
   Definições de Bibliotecas
   ========================= */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* =========================
   Definições de Hardware
   ========================= */
/* Canais ADC e GPIOs usados */
#define ADC_VRY_CHANNEL        0    // GPIO26 - Eixo vertical do joystick
#define ADC_VRX_CHANNEL        1    // GPIO27 - Eixo horizontal do joystick
#define BUTTON_PIN            22    // Botão do joystick (SW)
#define BUZZER_PIN            21    // Buzzer

/* =========================
   Temporizações das tarefas
   ========================= */
#define JOY_TASK_DELAY_MS    100    // Leitura do joystick a cada 100 ms
#define BUTTON_TASK_DELAY_MS  50    // Leitura do botão a cada 50 ms
#define BUZZER_ON_MS         100    // Buzzer ligado por 100 ms
#define BUZZER_OFF_MS         50    // Buzzer desligado por 50 ms

/* =========================
   Limiares do ADC
   ========================= */
#define ADC_VOLTAGE_REF     3.3f
#define ADC_RANGE           4096
#define ADC_ALARM_THRESHOLD \
    ((uint16_t)((3.0f/ADC_VOLTAGE_REF) * ADC_RANGE))  // ≈3723

/* =========================
   Tipos de mensagem para a fila
   ========================= */
typedef enum {
    MSG_JOYSTICK, // Mensagem de leitura do joystick
    MSG_BUTTON    // Mensagem de evento do botão
} MsgType;

/* Estrutura da mensagem da fila */
typedef struct {
    MsgType type; // Tipo da mensagem
    union {
        struct { uint16_t vrx, vry; } joystick; // Dados do joystick
        struct { bool pressed; }      button;   // Evento do botão
    } data;
} QueueMessage;

/* =========================
   Objetos do FreeRTOS
   ========================= */
static QueueHandle_t    xQueue;      // Fila para comunicação entre tarefas
static SemaphoreHandle_t xMutex;     // Mutex para proteger o printf
static SemaphoreHandle_t xBuzzerSem; // Semáforo contador para o buzzer

/* =========================
   Protótipos das tarefas
   ========================= */
void vTaskJoystick(void *pv);
void vTaskButton(void *pv);
void vTaskProcess(void *pv);
void vTaskBuzzer(void *pv);

/* =========================
   Inicialização dos GPIOs e ADC
   ========================= */
static void setup_gpio(void) {
    /* Inicializa ADC e configura os pinos analógicos */
    adc_init();
    adc_gpio_init(26);  // VRY no GPIO26 (ADC0)
    adc_gpio_init(27);  // VRX no GPIO27 (ADC1)

    /* Configura o botão como entrada com pull-up */
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    /* Configura o buzzer como saída e inicia desligado */
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

/* =========================
   Função principal
   ========================= */
int main(void) {
    stdio_init_all();   // Inicializa a saída padrão (UART)
    setup_gpio();       // Configura GPIOs e ADC

    /* Criação da fila, mutex e semáforo contador (máximo 2) */
    xQueue      = xQueueCreate(10, sizeof(QueueMessage));
    xMutex      = xSemaphoreCreateMutex();
    xBuzzerSem  = xSemaphoreCreateCounting(2, 0);

    /* Criação das tarefas do FreeRTOS */
    xTaskCreate(vTaskJoystick,  "Joystick", 256, NULL, 2, NULL);
    xTaskCreate(vTaskButton,    "Button",   256, NULL, 2, NULL);
    xTaskCreate(vTaskProcess,   "Process",  512, NULL, 1, NULL);
    xTaskCreate(vTaskBuzzer,    "Buzzer",   256, NULL, 3, NULL);

    vTaskStartScheduler(); // Inicia o escalonador do FreeRTOS

    for (;;); // Nunca deve chegar aqui
    return 0;
}

/* ===========================================================
 * Tarefa 1: Leitura do joystick a cada 100 ms
 * Envia os valores lidos dos eixos VRX e VRY para a fila.
 * =========================================================== */
void vTaskJoystick(void *pv) {
    QueueMessage msg;
    msg.type = MSG_JOYSTICK;

    for (;;) {
        /* Lê o valor do eixo vertical (VRY) */
        adc_select_input(ADC_VRY_CHANNEL);
        msg.data.joystick.vry = adc_read();

        /* Lê o valor do eixo horizontal (VRX) */
        adc_select_input(ADC_VRX_CHANNEL);
        msg.data.joystick.vrx = adc_read();

        /* Envia os dados para a fila */
        xQueueSend(xQueue, &msg, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(JOY_TASK_DELAY_MS));
    }
}

/* ===========================================================
 * Tarefa 2: Leitura do botão com debounce a cada 50 ms
 * Envia evento para a fila quando o botão é pressionado.
 * =========================================================== */
void vTaskButton(void *pv) {
    static bool last = true;
    QueueMessage msg;
    msg.type = MSG_BUTTON;

    for (;;) {
        bool cur = gpio_get(BUTTON_PIN); // Lê o estado atual do botão
        if (last && !cur) { // Detecta transição de solto para pressionado
            msg.data.button.pressed = true;
            xQueueSend(xQueue, &msg, portMAX_DELAY); // Envia evento para a fila
        }
        last = cur; // Atualiza o estado anterior
        vTaskDelay(pdMS_TO_TICKS(BUTTON_TASK_DELAY_MS));
    }
}

/* ===========================================================
 * Tarefa 3: Processa mensagens recebidas da fila, imprime no terminal protegido por mutex
 * e aciona o buzzer caso algum valor ultrapasse o limiar ou o botão seja pressionado.
 * =========================================================== */
void vTaskProcess(void *pv) {
    QueueMessage msg;
    for (;;) {
        // 📨 Aguarda e recebe uma mensagem da fila (pode ser do joystick ou do botão)
        if (xQueueReceive(xQueue, &msg, portMAX_DELAY) == pdPASS) {
            // 🔒 Garante acesso exclusivo ao terminal (evita prints simultâneos)
            xSemaphoreTake(xMutex, portMAX_DELAY);

            if (msg.type == MSG_JOYSTICK) {
                // 🎮 Se a mensagem for do joystick, extrai valores dos eixos
                uint16_t x = msg.data.joystick.vrx;
                uint16_t y = msg.data.joystick.vry;
                printf("🎮 Joy — X: %4u, Y: %4u\n", x, y);

                // 🚨 Verifica se algum eixo ultrapassou o limiar de alarme
                if (x > ADC_ALARM_THRESHOLD ||
                    x < (ADC_RANGE - ADC_ALARM_THRESHOLD) ||
                    y > ADC_ALARM_THRESHOLD ||
                    y < (ADC_RANGE - ADC_ALARM_THRESHOLD)) {
                    // 🔔 Solicita ativação do buzzer liberando o semáforo
                    xSemaphoreGive(xBuzzerSem);
                }
            }
            else {  // MSG_BUTTON
                // 🖲️ Se a mensagem for do botão, imprime aviso
                printf("🖲️ Botão pressionado!\n");
                // 🔔 Solicita ativação do buzzer liberando o semáforo
                xSemaphoreGive(xBuzzerSem);
            }

            // 🔓 Libera o mutex após terminar o acesso ao terminal
            xSemaphoreGive(xMutex);
        }
    }
}

/* ===========================================================
 * Tarefa 4: Controle do buzzer.
 * Aguarda o semáforo ser liberado para ativar o buzzer por 100 ms e depois desliga por 50 ms.
 * =========================================================== */
void vTaskBuzzer(void *pv) {
    for (;;) {
        // ⏳ Espera até que o semáforo seja liberado por outra tarefa
        if (xSemaphoreTake(xBuzzerSem, portMAX_DELAY) == pdTRUE) {
            // 🔊 Liga o buzzer (emite som)
            gpio_put(BUZZER_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(BUZZER_ON_MS));
            // 🔇 Desliga o buzzer
            gpio_put(BUZZER_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(BUZZER_OFF_MS));
        }
    }
}
