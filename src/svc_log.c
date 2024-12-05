/* *****************************************************************************
 * Hardware Project 2024
 *
 * svc_log.c - Logging Service Implementation
 *
 * Description:
 *   Implements the logging service defined in svc_log.h. Uses a circular
 *   buffer to queue log messages and provides thread-safe logging operations
 *   through critical sections.
 *
 * Implementation Notes:
 *   - Uses critical sections for thread safety
 *   - Implements circular buffer for message queuing
 *   - Includes overflow protection
 *   - Supports variable argument formatting
 * *****************************************************************************/

#include "svc_log.h"
#include "drv_uart.h"
#include "drv_tiempo.h"
#include "hal_uart.h"
#include "drv_sc.h"
#include "drv_monitor.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/*
 * NOTE: En este módulo de logs SÍ que es necesaria la sección crítica,
 * puesto que si se llama a cualquier función de log desde una RSI o función
 * llamada por un handler o sus sucesoras, puede haber condiciones de carrera
 * sobre las estructuras compartidas.
 */

#define LOG_BUFFER_SIZE 80
#define LOG_MSG_SIZE 256
#define ENDLINE "\r\n"

typedef struct
{
    char msg[LOG_MSG_SIZE];
    uint32_t timestamp;
} LogMessage;

static struct
{
    LogMessage buffer[LOG_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} log_queue;

static uint32_t log_monitor; // Monitor ID for the service

/**
 * @brief Initialize the logging service
 * @param monitor Monitor ID for queue overflow detection
 */
void svc_log_iniciar(uint32_t monitor)
{
    drv_uart_iniciar();
    log_monitor = monitor;

    log_queue.head = 0;
    log_queue.tail = 0;
    log_queue.count = 0;

    LOG_INFO("Logging system initialized" ENDLINE);
}

/**
 * @brief Queue a log message with level and timestamp
 * @param level Logging level string
 * @param msg Message to be logged
 * @note Uses critical sections for thread safety
 */
static void log_queue_message(const char *level, const char *msg)
{
    if (log_queue.count >= LOG_BUFFER_SIZE)
    {
        drv_monitor_marcar(log_monitor);
        LOG_ERROR("Log buffer overflow");
        svc_log_procesar();
        while (1)
        ; // Buffer overflow - stay in infinite loop
    }

    LogMessage *entry = &log_queue.buffer[log_queue.head];
    entry->timestamp = drv_tiempo_actual_ms();

    snprintf(entry->msg, LOG_MSG_SIZE, "[%-6s] [%9lu] %s" ENDLINE,
             level, entry->timestamp, msg);

    drv_sc_entrar();
    log_queue.head = (log_queue.head + 1) % LOG_BUFFER_SIZE;
    log_queue.count++;
    drv_sc_salir();
}

/**
 * @brief Process and send queued log messages
 * @note Should be called periodically from main loop
 * @note Will not block if UART is busy
 */
void svc_log_procesar(void)
{
    while (log_queue.count > 0)
    {
        drv_sc_entrar();
        LogMessage *entry = &log_queue.buffer[log_queue.tail];
        log_queue.tail = (log_queue.tail + 1) % LOG_BUFFER_SIZE;
        log_queue.count--;
        drv_sc_salir();

        drv_uart_enviar_string(entry->msg);
    }
}

/**
 * @brief Send a log message with specified level
 * @param level Log level string
 * @param msg Message to log
 * @note Thread-safe through critical sections
 */
void svc_log_enviar(const char *level, const char *msg)
{
    log_queue_message(level, msg);
}

/**
 * @brief Send a formatted log message
 */
void svc_log_printf(const char *level, const char *format, ...)
{
    char buffer[LOG_MSG_SIZE];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    svc_log_enviar(level, buffer);
}
