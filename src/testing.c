/* *****************************************************************************
 * Hardware Project 2024
 *
 * testing.h - Test Suite Interface
 *
 * Authors:
 *   - Víctor Orrios Barón (NIA: 840994)
 *   - José Miguel Quílez Vergara (NIA: 873499)
 *
 * EINA - University of Zaragoza
 * Computer Science and Engineering
 * Course: 3rd year, 1st semester
 *
 * Date: 02/12/2024
 *
 * Description:
 *   Interface for the test suite module. Provides functions to test different
 *   components of the system including watchdog, random number generation,
 *   FIFO queues, buttons, UART, and critical sections.
 * *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "board.h"
#include "svc_alarma.h"
#include "drv_rng.h"
#include "rt_GE.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "svc_wdt.h"
#include "hal_wdt.h"
#include "rt_evento_t.h"
#include "rt_FIFO.h"
#include "svc_log.h"
#include "drv_sc.h"
#include "testing.h"

/******************************************************************************
 * Definitions
 *****************************************************************************/
#define WDT_TEST_TIMEOUT_MS 5000

/******************************************************************************
 * Watchdog Test Functions
 *****************************************************************************/

/**
 * @brief Notify watchdog that task is alive
 *
 * @param evento Event type
 * @param wdt_handle_id Watchdog handle for the task
 */
void notify_task_alive_test_wdt(uint32_t evento, uint32_t wdt_handle_id)
{
    svc_wdt_tarea_viva(wdt_handle_id);
}

// Function to subscribe multiple times
void dummy_wdt_handler(uint32_t evento, uint32_t auxdata)
{
    drv_led_conmutar(auxdata);
}

/**
 * @brief Test watchdog overflow behavior
 *
 * Tests watchdog reset by intentionally causing a subscription overflow
 * in the event manager. The watchdog should trigger a system reset.
 *
 * @param id_led LED to use for visual feedback
 */
void test_wdt_overflow(uint32_t id_led)
{
    // Initialize watchdog service with 5 second timeout
    svc_wdt_iniciar(WDT_TEST_TIMEOUT_MS, MONITOR3);

    // Register this task with watchdog
    int32_t wdt_handle = svc_wdt_registrar_tarea(1);

    drv_led_encender(id_led);

    // Try to subscribe more than rt_GE_MAX_SUSCRITOS times
    for (int i = 0; i < rt_GE_MAX_SUSCRITOS + 1; i++)
    {                                                         // rt_GE_MAX_SUSCRITOS is 4
        svc_GE_suscribir(ev_PULSAR_BOTON, dummy_wdt_handler); // This will halt on 5th subscription
    }

    // Code below won't execute because rt_GE will halt
    // Watchdog will then trigger system reset
    svc_GE_suscribir(ev_FEED_WDT, notify_task_alive_test_wdt);
    svc_alarma_activar(svc_alarma_codificar(1, WDT_TEST_TIMEOUT_MS / 2), ev_FEED_WDT, wdt_handle);
}

/******************************************************************************
 * Random Number Generator Test Functions
 *****************************************************************************/

static uint32_t random_test_prev_led_id = 0;

/**
 * @brief Random number test callback
 *
 * @param id_evento Event ID
 * @param auxdata Additional data (unused)
 */
void testRandomCallback(uint32_t id_evento, uint32_t auxdata)
{
    uint32_t result = 1;
    drv_rng_get_range(1, LEDS_NUMBER, &result);
    if (random_test_prev_led_id)
        drv_led_apagar(random_test_prev_led_id);
    drv_led_conmutar(result);
    random_test_prev_led_id = result;
}

/**
 * @brief Test random number generation
 *
 * Tests the random number generator by toggling random LEDs
 * at regular intervals.
 */
void test_random(void)
{

    drv_rng_init(drv_tiempo_actual_us());

    svc_GE_suscribir(ev_TEST, testRandomCallback);
    svc_alarma_activar(svc_alarma_codificar(1, 1000), ev_TEST, 0);
}

/******************************************************************************
 * FIFO Queue Test Functions
 *****************************************************************************/

/**
 * @brief Test FIFO queue functionality
 *
 * Tests FIFO queue operations including initialization,
 * normal operation, empty queue extraction, and overflow conditions.
 */
void test_fifo(void)
{
    EVENTO_T EV_ID_evento;
    uint32_t EV_auxData;
    Tiempo_us_t EV_TS;

    rt_FIFO_inicializar(MONITOR4);
    drv_tiempo_periodico_ms(500, rt_FIFO_encolar, ev_T_PERIODICO);

    // Test normal operation
    for (int j = 0; j <= EVENT_QUEUE_SIZE; j++)
    {
        for (int i = 0; i < j; i++)
        {
            rt_FIFO_encolar(0, i);
        }

        for (int i = 0; i < j; i++)
        {
            rt_FIFO_extraer(&EV_ID_evento, &EV_auxData, &EV_TS);
            if (EV_auxData != i)
            {
                drv_led_encender(1);
                while (1)
                    ;
            }
        }
    }

    // Test null extraction
    if (rt_FIFO_extraer(&EV_ID_evento, &EV_auxData, &EV_TS) != 0)
    {
        drv_led_encender(2);
        while (1)
            ;
    }

    // Test overflow
    drv_led_encender(3);
    for (int i = 0; i < EVENT_QUEUE_SIZE + 1; i++)
    {
        rt_FIFO_encolar(0, i);
    }
    drv_led_encender(4);
    while (1)
        ;
}

/******************************************************************************
 * Button Test Functions
 *****************************************************************************/

/**
 * @brief Button test callback
 *
 * @param evento Event ID
 * @param id LED ID to toggle
 */
void conmutar_led_task_boton(uint32_t evento, uint32_t id)
{
    drv_led_conmutar(id);
}

/**
 * @brief Test button functionality
 *
 * Tests button interrupt handling by toggling an LED on button press.
 */
void test_boton(void)
{
    svc_GE_suscribir(ev_PULSAR_BOTON, conmutar_led_task_boton);
}

/******************************************************************************
 * UART Test Functions
 *****************************************************************************/
/**
 * @brief Test UART functionality
 *
 * Tests UART communication by sending test messages at different
 * log levels (INFO, DEBUG, ERROR).
 */
void test_logs(void)
{
    LOG_INFO_F("Hola caracola INFO, tiempo actual en microsegundos: %lu", drv_tiempo_actual_us());
    LOG_DEBUG_F("Hola caracola DEBUG, tiempo actual en microsegundos: %lu", drv_tiempo_actual_us());
    LOG_ERROR_F("Hola caracola ERROR, tiempo actual en microsegundos: %lu", drv_tiempo_actual_us());
    drv_led_encender(1);
    svc_log_procesar();
		
}

/******************************************************************************
 * Critical Section Test Functions
 *****************************************************************************/

/* Global variables for critical section test */
static volatile uint32_t test_cs_interrupt_counter = 0;
static volatile uint32_t test_cs_expected_value = 0;
static volatile uint8_t test_cs_finished = 0;

static volatile uint32_t test_cs_current_depth = 0;
static volatile uint32_t test_cs_current_target_depth = 0;
static volatile uint32_t test_cs_target_depth = 0;

static volatile uint32_t test_cs_waittime_ms = 0;
static volatile uint32_t test_cs_id_led_error = 0;
static volatile uint32_t test_cs_id_led_ok = 0;

static volatile uint32_t fake_in_cs_global = 0;

/**
 * @brief Critical section test interrupt handler
 *
 * @param evento Event type
 * @param auxdata Additional data (unused)
 */
void test_cs_interrupt_handler(uint32_t evento, uint32_t auxdata)
{
    if (test_cs_current_depth > 0) {
        if (evento == ev_T_PERIODICO) {
            test_cs_interrupt_counter++;
            LOG_ERROR_F("Timer interrupt triggered while in CS, depth: %lu", test_cs_current_depth);
        }
        svc_log_procesar();
        drv_led_encender(test_cs_id_led_error);
        while (1)
            ;
    }

    if (evento == ev_T_PERIODICO) {
        // Forward event and auxdata to rt_FIFO_encolar
        rt_FIFO_encolar(evento, auxdata);
        test_cs_interrupt_counter++;
        if (!fake_in_cs_global) {
            LOG_INFO_F("Timer interrupt triggered while not in CS (OK), total timer interrupts count=%lu", test_cs_interrupt_counter);
        } else {
            LOG_INFO("~~ WARNING ~~ Timer interrupt triggered deliberately while waiting in handler but not actually in CS (OK)");
        }
    }

    svc_log_procesar();
}

/**
 * @brief Main test loop handler
 *
 * @param evento Event type
 * @param auxdata Test value from queue
 */
void test_cs_main_handler(uint32_t evento, uint32_t fake_in_cs)
{
    test_cs_current_depth = 0;

    if (test_cs_current_target_depth <= test_cs_target_depth) {

        fake_in_cs_global = fake_in_cs;
        uint32_t total_interrupts_before_entering_cs = test_cs_interrupt_counter;

        if (!fake_in_cs) {
            for (uint32_t i = 0; i < test_cs_current_target_depth; i++) {
                test_cs_current_depth = drv_sc_entrar();
                test_cs_current_depth++;
            }
        } else {
            LOG_INFO("~~ WARNING ~~ Deliberately not entering CS");
        }

        if (test_cs_current_depth != test_cs_current_target_depth && !fake_in_cs) {
            drv_led_encender(test_cs_id_led_error);
            LOG_ERROR_F("Current CS depth: %lu, expected current CS (target) depth: %lu", test_cs_current_depth, test_cs_current_target_depth);
            svc_log_procesar();
            while (1)
                ;
        }

        if (!fake_in_cs) {
            LOG_INFO_F("Successfully entered CS with depth: %lu, proceeding to wait %lu ms", test_cs_current_depth, test_cs_waittime_ms);
        } else {
            LOG_INFO_F("~~ WARNING ~~ Did not actually enter CS, proceeding to wait %lu ms", test_cs_waittime_ms);
        }

        drv_tiempo_esperar_hasta_ms(drv_tiempo_actual_ms() + test_cs_waittime_ms);
        test_cs_current_target_depth++;

        // Set current depth to 0 before exiting CS (an interrupt could occur just
        // after the last drv_sc_salir() and it would see the wrong depth, > 0)
        test_cs_current_depth = 0;
        uint32_t total_interrupts_while_in_cs = test_cs_interrupt_counter - total_interrupts_before_entering_cs;

        if (fake_in_cs) {
            fake_in_cs_global = 0;
            LOG_INFO_F("~~ WARNING ~~ As it can be seen, %lu interrupts occurred while waiting and not actually in CS", total_interrupts_while_in_cs);
            svc_log_procesar();
        } else {
            LOG_INFO_F("Exiting CS with depth: %lu, %lu interrupts occurred while in CS", test_cs_current_target_depth-1, total_interrupts_while_in_cs);
            svc_log_procesar();
            for (uint32_t i = 0; i < test_cs_current_target_depth-1; i++) {
                drv_sc_salir();
            }
        }

        uint32_t total_in_cs_after_all_exits = drv_sc_entrar();
        drv_sc_salir();

        if (total_in_cs_after_all_exits != 0) {
            drv_led_encender(test_cs_id_led_error);
            LOG_ERROR_F("Total in CS after all exits: %lu (expected 0)", total_in_cs_after_all_exits);
            svc_log_procesar();
            while (1)
                ;
        }
    } else {
        LOG_INFO_F("***PASS*** CS Test finished successfully, current target depth (excess by 1): %lu, target depth: %lu, total interrupts count: %lu", test_cs_current_target_depth, test_cs_target_depth, test_cs_interrupt_counter);
        svc_log_procesar();

        svc_GE_cancelar(ev_TEST_CS_MAIN, test_cs_main_handler);

        // Cancel alarm to stop test
        svc_alarma_activar(svc_alarma_codificar(0, 0), ev_TEST_CS_MAIN, 0);
        drv_led_apagar(test_cs_id_led_ok);

        // Execute again but without actually entering CS, to proof that interrupts can
        // occur while waiting at drv_tiempo_esperar_hasta_ms(drv_tiempo_actual_ms() + test_cs_waittime_ms);
        test_cs_current_target_depth = test_cs_target_depth;
        test_cs_main_handler(ev_TEST_CS_MAIN, 1);

        // Resume normal alarm service activity
        svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
    }
}

/**
 * @brief Test critical section functionality
 *
 * Tests nested critical sections and interrupt handling during
 * critical sections. Verifies proper operation of atomic operations.
 */
void test_critical_section(uint32_t main_task_interval_ms, uint32_t id_led_test,
                            uint32_t id_led_error, uint32_t target_depth)
{
    test_cs_finished = 0;
    test_cs_interrupt_counter = 0;

    test_cs_current_depth = 0;
    test_cs_current_target_depth = 1;
    test_cs_target_depth = target_depth;
    test_cs_id_led_error = id_led_error;
    test_cs_id_led_ok = id_led_test;
    uint32_t periodic_timer_interval = main_task_interval_ms / 8 > 0 ? main_task_interval_ms / 8 : 1;
    test_cs_waittime_ms = main_task_interval_ms / 2 > 4 ? main_task_interval_ms / 2 : 4;
    main_task_interval_ms = main_task_interval_ms > 8 ? main_task_interval_ms : 8;

    fake_in_cs_global = 0;

    LOG_INFO_F("Starting critical section test, main task interval: %lu ms, periodic timer interval: %lu ms, wait time: %lu ms, target depth: %lu",
        main_task_interval_ms, periodic_timer_interval, test_cs_waittime_ms, target_depth);

    svc_log_procesar();

    // Subscribe handlers
    svc_alarma_activar(svc_alarma_codificar(1, main_task_interval_ms), ev_TEST_CS_MAIN, 0);

    // Hijack periodic timer to trigger test_cs_interrupt_handler instead of rt_FIFO_encolar
    // directly (test_cs_interrupt_handler will internally call rt_FIFO_encolar)
    drv_tiempo_periodico_ms(periodic_timer_interval, test_cs_interrupt_handler, ev_T_PERIODICO);

    // Subscribe main handler to ev_TEST_CS_MAIN event
    svc_GE_suscribir(ev_TEST_CS_MAIN, test_cs_main_handler);

    // First ev_TEST_CS_MAIN event enqueued to execute before any ev_T_PERIODICO triggers test_cs_interrupt_handler
    rt_FIFO_encolar(ev_TEST_CS_MAIN, 0);

    drv_led_encender(id_led_test); // Indicate test is running
}
