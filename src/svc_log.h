/**
 * @file svc_log.h
 * @ingroup SVC_LOG
 * @brief Logging Service Interface
 * @details Provides a configurable logging system with multiple severity levels
 *          and formatted output support. Includes compile-time log level filtering
 *          and queue-based message handling.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef SVC_LOG
#define SVC_LOG

#include <stdint.h>

// Log levels
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_NONE  0

#if defined(DEBUG) && defined(WITH_DEBUG_LOGS)
    #define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#ifdef TESTING
    #define LOG_LEVEL LOG_LEVEL_INFO
#endif

#ifdef RELEASE
    #define LOG_LEVEL LOG_LEVEL_NONE
#endif

// Configure current log level
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

// Log macros
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(msg) svc_log_enviar("DEBUG", msg)
#else
    #define LOG_DEBUG(msg)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
    #define LOG_INFO(msg) svc_log_enviar("INFO", msg)
#else
    #define LOG_INFO(msg)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
    #define LOG_ERROR(msg) svc_log_enviar("ERROR", msg)
#else
    #define LOG_ERROR(msg)
#endif

/**
 * @brief Initialize the logging service
 * @param monitor Monitor ID for queue overflow detection
 * @param uart_freed_callback Callback function to be called when the UART is freed
 * @param uart_freed_event Event identifier to be passed to the callback
 */
void svc_log_iniciar(uint32_t monitor, void (*uart_freed_callback)(), uint32_t uart_freed_event);

/**
 * @brief Send a log message with level
 * @param level Log level string
 * @param msg Message to log
 */
void svc_log_enviar(const char* level, const char* msg);

/**
 * @brief Process pending log messages
 * Must be called periodically from main loop
 */
void svc_log_procesar(void);

/**
 * @brief Send a formatted log message
 * @param level Log level string ("DEBUG", "INFO", "ERROR")
 * @param format Printf-style format string
 * @param ... Variable arguments for format string
 */
void svc_log_printf(const char* level, const char* format, ...);

// Add formatted logging macros
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG_F(...) svc_log_printf("DEBUG", __VA_ARGS__)
#else
    #define LOG_DEBUG_F(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
    #define LOG_INFO_F(...) svc_log_printf("INFO", __VA_ARGS__)
#else
    #define LOG_INFO_F(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
    #define LOG_ERROR_F(...) svc_log_printf("ERROR", __VA_ARGS__)
#else
    #define LOG_ERROR_F(...)
#endif

#endif 
