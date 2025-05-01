/**
 * @file practica_3.c
 * @ingroup APP_EVENT
 * @brief Event-Based LED Control Implementation
 * 
 * @details Implements LED control using event-based programming:
 * - Timer interrupt handling
 * - Event queue management
 * - Power-efficient scheduling
 * - Runtime system integration
 * 
 * Features:
 * - Version 3: Timer interrupt-based LED control
 * - Version 4: Event queue-based LED control
 * - Power management integration
 * - Event scheduler implementation
 * 
 * Hardware Project 2024
 * EINA - University of Zaragoza
 * 
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 02/12/2024
 */

#include "practica_3.h"
#include "drv_tiempo.h"
#include "drv_leds.h"
#include "drv_consumo.h"
#include "rt_fifo.h"
#include "rt_evento_t.h"
#include "board.h"

/******************************************************************************
 * Definitions
 *****************************************************************************/
#define RETARDO_MS_BLINK_V3 500

/******************************************************************************
 * Event Handler Functions
 *****************************************************************************/

uint8_t blinkv3_led_id;

/**
 * @brief LED toggle callback for blink_v3
 * 
 * @param evento Event type
 * @param id LED ID to toggle
 */
void conmutar_led_task_blink(uint32_t evento, uint32_t id)
{
    drv_led_conmutar(blinkv3_led_id);
}

/**
 * @brief Run the event scheduler
 * 
 * Initializes event queue and configures periodic timer for event queueing.
 * Continuously checks for events to process, toggling LED on periodic events.
 * Puts processor in low power state when no events are present.
 */
void scheduler_run(uint32_t led_id)
{
    EVENTO_T EV_ID_evento;
    uint32_t EV_auxData;
    Tiempo_us_t EV_TS;

    rt_FIFO_inicializar(MONITOR4); // NOTA: when compilig, whether it's "BOARD_PCA10056" or "LPC2105_simulador"
                                   // will be properly defined (see board.h). So no worries, it will compile just fine
    drv_led_encender(led_id);
    drv_tiempo_periodico_ms(500, rt_FIFO_encolar, ev_T_PERIODICO);

    while (1)
    {
        EV_ID_evento = ev_VOID;
        if (rt_FIFO_extraer(&EV_ID_evento, &EV_auxData, &EV_TS))
        {
            if (EV_ID_evento == ev_T_PERIODICO)
            {
                drv_led_conmutar(led_id);
            }
        }
        else
        {
            drv_consumo_esperar();
        }
    }
}

/******************************************************************************
 * LED Control Functions
 *****************************************************************************/

/**
 * @brief Timer interrupt-based LED blinking
 * 
 * Implements LED blinking using timer interrupts and power management.
 * 
 * @param id LED identifier to blink
 */
void blink_v3(uint32_t id)
{
	// NOTE: removed "id" param
	drv_led_encender(id);
	blinkv3_led_id = id;
	drv_tiempo_periodico_ms(RETARDO_MS_BLINK_V3, conmutar_led_task_blink, id);
	while (1)
	{
		// NOTE: will run past this line every time a clock interruption occurs
		drv_consumo_esperar();
	}
}

/**
 * @brief Event queue-based LED blinking
 * 
 * Implements LED blinking using the runtime system with event queue
 * management and scheduler for event processing.
 * 
 * @param id LED identifier to blink
 */
void blink_v4(uint32_t id)
{
	// Initialize the event queue and scheduler
	scheduler_run(id);

	// The scheduler will handle:
	// 1. Setting up the periodic timer (500ms)
	// 2. Processing events from the queue
	// 3. Toggling LED based on periodic events
	// 4. Managing low power modes when idle
}
