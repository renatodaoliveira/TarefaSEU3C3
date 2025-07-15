/*
 * FreeRTOS V202111.00
 * Copyright (C) 2020 Amazon.com, Inc. ou suas afiliadas.  Todos os direitos reservados.
 *
 * Permissão é concedida, gratuitamente, a qualquer pessoa que obtenha uma cópia
 * deste software e dos arquivos de documentação associados (o "Software"), para lidar
 * com o Software sem restrição, incluindo, sem limitação, os direitos de
 * usar, copiar, modificar, mesclar, publicar, distribuir, sublicenciar e/ou vender cópias do
 * Software, e permitir que as pessoas a quem o Software é fornecido o façam,
 * sujeito às seguintes condições:
 *
 * O aviso de copyright acima e esta permissão devem ser incluídos em todas as
 * cópias ou partes substanciais do Software.
 *
 * O SOFTWARE É FORNECIDO "NO ESTADO EM QUE SE ENCONTRA", SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU
 * IMPLÍCITA, INCLUINDO MAS NÃO SE LIMITANDO ÀS GARANTIAS DE COMERCIALIZAÇÃO, ADEQUAÇÃO
 * PARA UM PROPÓSITO ESPECÍFICO E NÃO VIOLAÇÃO. EM NENHUMA HIPÓTESE OS AUTORES OU
 * DETENTORES DOS DIREITOS AUTORAIS SERÃO RESPONSÁVEIS POR QUALQUER REIVINDICAÇÃO, DANOS OU OUTRA
 * RESPONSABILIDADE, SEJA EM UMA AÇÃO DE CONTRATO, ATO ILÍCITO OU DE OUTRA FORMA, DECORRENTE DE,
 * FORA DE OU EM CONEXÃO COM O SOFTWARE OU O USO OU OUTRAS NEGOCIAÇÕES NO SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tabulação == 4 espaços!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Definições específicas da aplicação.
 *
 * Essas definições devem ser ajustadas para o seu hardware e
 * requisitos da aplicação.
 *
 * ESTES PARÂMETROS SÃO DESCRITOS NA SEÇÃO 'CONFIGURATION' DA
 * DOCUMENTAÇÃO DA API DO FreeRTOS DISPONÍVEL NO SITE FreeRTOS.org.
 *
 * Veja http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Relacionados ao escalonador */
#define configUSE_PREEMPTION                    1
#define configUSE_TICKLESS_IDLE                 0
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                    32
#define configMINIMAL_STACK_SIZE                ( configSTACK_DEPTH_TYPE ) 256
#define configUSE_16_BIT_TICKS                  0

#define configIDLE_SHOULD_YIELD                 1

/* Relacionados à sincronização */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_QUEUE_SETS                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_NEWLIB_REENTRANT              0
// necessário para compilar o lwip FreeRTOS sys_arch
#define configENABLE_BACKWARD_COMPATIBILITY     1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5

/* Sistema */
#define configSTACK_DEPTH_TYPE                  uint32_t
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t

/* Definições relacionadas à alocação de memória. */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   (128*1024)
#define configAPPLICATION_ALLOCATED_HEAP        0

/* Definições relacionadas a funções de hook. */
#define configCHECK_FOR_STACK_OVERFLOW          0
#define configUSE_MALLOC_FAILED_HOOK            0
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

/* Definições relacionadas à coleta de estatísticas de tempo de execução e tarefas. */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Definições relacionadas a co-rotinas. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         1

/* Definições relacionadas a temporizadores de software. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            1024

/* Configuração do comportamento de aninhamento de interrupções. */
/*
#define configKERNEL_INTERRUPT_PRIORITY         [dependente do processador]
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    [dependente do processador e aplicação]
#define configMAX_API_CALL_INTERRUPT_PRIORITY   [dependente do processador e aplicação]
*/

#if FREE_RTOS_KERNEL_SMP // definido pela porta SMP do RP2040 do FreeRTOS
/* Apenas para porta SMP */
#define configNUMBER_OF_CORES                   1
#define configTICK_CORE                         0
#define configRUN_MULTIPLE_PRIORITIES           1
#define configUSE_CORE_AFFINITY                 0
#endif

/* Específico do RP2040 */
#define configSUPPORT_PICO_SYNC_INTEROP         1
#define configSUPPORT_PICO_TIME_INTEROP         1

#include <assert.h>
/* Define para capturar erros durante o desenvolvimento. */
#define configASSERT(x)                         assert(x)

/* Defina as definições a seguir como 1 para incluir a função da API, ou zero
para excluir a função da API. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_xQueueGetMutexHolder            1

/* Um arquivo de cabeçalho que define macros de trace pode ser incluído aqui. */

#endif /* FREERTOS_CONFIG_H */