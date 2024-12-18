/**
 * @file svc_log.c
 * @ingroup SVC_LOG
 * @brief Logging Service Implementation
 * @details Implements the logging service defined in svc_log.h. Uses a circular
 *          buffer to queue log messages and provides thread-safe logging operations
 *          through critical sections.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "svc_log.h"
#include "drv_uart.h"
#include "drv_tiempo.h"
#include "drv_monitor.h"
#include "rt_evento_t.h"
#include "rt_GE.h"
#include "drv_consumo.h"
#include "hal_uart.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/*
 * NOTE: Critical sections ARE necessary in this logging module,
 * since if any logging function is called from an ISR or a function
 * called by a handler or its successors, race conditions could occur
 * on the shared structures.
 */

#define LOG_BUFFER_SIZE 128
#define LOG_MSG_SIZE 128
#define LOG_BUFFER_BYTE_SIZE (LOG_BUFFER_SIZE * LOG_MSG_SIZE)
#define ENDLINE "\r\n"
#define LOG_PRE_SIZE_INCLUDING_ENDLINE 23

// Change LogMessage structure to just a char buffer and a timestamp array
static struct
{
    char log_buffer[LOG_BUFFER_BYTE_SIZE]; // Buffer for log messages
    uint32_t count;                                  // Number of messages stored
    uint32_t current_position;                       // Current position in the log buffer
    uint32_t start_index_after_dumping;              // Index immediately after the end of the last message scheduled to be dumped
    uint32_t end_index_after_dumping;                // End of the buffer slice that was queued after a uart transmission started and before it finished
} log_queue;

static uint32_t log_monitor;                            // Monitor ID for the service
static volatile uint8_t uart_busy;                      // Flag to indicate if the UART is busy
static volatile uint8_t uart_freed_callback_fired;      // Flag to indicate if the UART freed callback has been fired
static volatile uint8_t process_log_after_uart_freed;   // Flag to indicate if the log processing should be done after the UART freed callback

/**
 * @brief Callback for UART freed events, will be called by
 *        the UART IRQHandler when the UART finishes a transaction.
 * @note  This function won't be called in handler mode, but rather in thread mode by the event dispatcher.
 */
static void uart_freed_subscriber(uint32_t evento, uint32_t auxData)
{
    if (auxData > 0 || uart_freed_callback_fired == 0) {
        uart_busy = 0;
        memcpy(&log_queue.log_buffer[0], &log_queue.log_buffer[log_queue.start_index_after_dumping], log_queue.end_index_after_dumping - log_queue.start_index_after_dumping);
        log_queue.current_position = log_queue.end_index_after_dumping - log_queue.start_index_after_dumping;
        log_queue.start_index_after_dumping = 0;
        log_queue.end_index_after_dumping = 0;
        if (auxData > 0) {
            // Means that we were waiting (drv_consumo_esperar()), then the UART IRQ handler
            // was triggered and this function was called directly from log_queue_message
            uart_freed_callback_fired = 1;
        }
        if (process_log_after_uart_freed) {
            process_log_after_uart_freed = 0;
            svc_log_procesar();
        }
    } else {
        // auxData == 0 && uart_freed_callback_fired != 0
        // Means that this function was called by the event dispatcher (rt_GE)
        // and the current function was already called by log_queue_message
        // Thus, we need to reset the flag to 0, so that in case this function is next
        // called by the event dispatcher first (not by log_queue_message), it will be able
        // to memcpy and reset the log struct data properly
        uart_freed_callback_fired = 0;
    }
}

/**
 * @brief Initialize the logging service
 * @param monitor Monitor ID for queue overflow detection
 * @param uart_freed_callback Callback function to be called when the UART is freed
 * @param uart_freed_event Event identifier to be passed to the callback
 */
void svc_log_iniciar(uint32_t monitor, void (*uart_freed_enqueue_callback)(), uint32_t uart_freed_event)
{
    drv_uart_iniciar(uart_freed_enqueue_callback, uart_freed_event);
    svc_GE_suscribir((EVENTO_T)uart_freed_event, uart_freed_subscriber);

    log_monitor = monitor;

    log_queue.current_position = 0;
    log_queue.start_index_after_dumping = 0;
    log_queue.end_index_after_dumping = 0;
    log_queue.count = 0;

    uart_busy = 0;
    uart_freed_callback_fired = 0;
    process_log_after_uart_freed = 0;

    LOG_INFO("Logging system initialized");
}

/**
 * @brief Queue a log message with level and timestamp
 * @param level Logging level string
 * @param msg Message to be logged
 * @note No need for critical sections, since all functions in this module are only called in thread mode
 */
static void log_queue_message(const char *level, const char *msg)
{
    // Calculate the size of the new log message
    uint32_t timestamp = drv_tiempo_actual_ms();
    // uint32_t message_size = snprintf(NULL, 0, "[%-6s] [%9u] %s" ENDLINE, level, timestamp, msg); // + 1; => NO, since we are going to store logs contiguously in the buffer
	uint32_t message_size = LOG_PRE_SIZE_INCLUDING_ENDLINE + strlen(msg);
	
    // While (not if) since we have to make sure that we don't enqueue
    // a message in case the buffer is already full (the watchdog will
    // bite if the program spends too much time here)
    while ((log_queue.current_position + message_size) > LOG_BUFFER_BYTE_SIZE)
    {
        drv_monitor_marcar(log_monitor);
        svc_log_procesar();
        // Wait for UART transactions to finish
        while (hal_uart_pendientes() != 0) {
            // Entering here means that there are pending bytes to be processed in the UART transaction in progress, or
            // that the UART transaction finish callback has not been called yet
            drv_consumo_esperar();
        }
        // If we get here, means that the UART is free and the buffer has been emptied,
        // thus we should execute uart_freed_subscriber() IMMEDIATELY in order to be
        // able to exit the current while loop and resume execution in thread mode
        uart_freed_subscriber(ev_UART_FREED, drv_tiempo_actual_ms()+1);
        drv_monitor_desmarcar(log_monitor);
    }
    
    // Append the log message to the log buffer
    timestamp = drv_tiempo_actual_ms();
    snprintf(&log_queue.log_buffer[log_queue.current_position], message_size+1, "[%-6s] [%9u] %s" ENDLINE,
             level, timestamp, msg); // message_size+1 since snprintf automatically adds a null terminator

    // Update the current position and count
    log_queue.current_position += message_size;
    log_queue.count++;

    if (uart_busy == 1) {
        // If the UART is busy, we need to update the after-dumping end index
        // to schedule the current message for rearrangement when uart_freed_callback is called
        log_queue.end_index_after_dumping = log_queue.current_position;
    }
}

/**
 * @brief Process and send queued log messages
 * @note Should be called periodically from main loop
 * @note Will retry automatically if UART is busy
 * @note This function should always be called in thread mode
 */
void svc_log_procesar(void)
{
    if (log_queue.count > 0)
    {
        int32_t pending = drv_uart_enviar_string(log_queue.log_buffer, log_queue.current_position);

        if (pending == 0) {
            // Transmission started successfully
            uart_busy = 1;
            log_queue.start_index_after_dumping = log_queue.current_position;
            log_queue.end_index_after_dumping = log_queue.current_position;
            log_queue.count = 0;
        } else if (pending > 0 || pending == -1) {
            // else: UART transaction is still in progress, schedule this module to
            // process logs just after the UART transaction finishes (the current transaction
            // might not flush all the current log_queue.log_buffer contents)
            process_log_after_uart_freed = 1;
        }
    }
}

/**
 * @brief Send a log message with specified level
 * @param level Log level string
 * @param msg Message to log
 * @note No need for critical sections, since all functions in this module are only called in thread mode
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
