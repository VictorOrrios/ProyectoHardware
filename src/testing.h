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

#if defined(TESTING) || defined(DEBUG)

#include <stdint.h>

/**
 * @brief Test watchdog overflow behavior
 * 
 * Tests watchdog reset by intentionally causing a subscription overflow
 * in the event manager. The watchdog should trigger a system reset.
 * 
 * @param id_led LED to use for visual feedback
 */
void test_wdt_overflow(uint32_t id_led);

/**
 * @brief Test random number generation
 * 
 * Tests the random number generator by toggling random LEDs
 * at regular intervals.
 */
void test_random(void);

/**
 * @brief Test FIFO queue functionality
 * 
 * Tests FIFO queue operations including initialization,
 * normal operation, empty queue extraction, and overflow conditions.
 */
void test_fifo(void);

/**
 * @brief Test button functionality
 * 
 * Tests button interrupt handling by toggling an LED on button press.
 */
void test_boton(void);

/**
 * @brief Test UART functionality
 * 
 * Tests UART communication by sending test messages at different
 * log levels (INFO, DEBUG, ERROR).
 */
void test_logs(void);

/**
 * @brief Test critical section functionality
 * 
 * Tests nested critical sections and interrupt handling during
 * critical sections. Verifies proper operation of atomic operations.
 */
void test_critical_section(uint32_t main_task_interval_ms, uint32_t id_led_test,
                            uint32_t id_led_error, uint32_t target_depth);

#endif // TESTING
