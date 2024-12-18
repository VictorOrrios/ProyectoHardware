/**
 * @file practica_4.c
 * @ingroup APP_ADVANCED
 * @brief Advanced LED Control Implementation
 * 
 * @details Implements advanced LED control patterns and interactive features:
 * - Limited blink sequences with deep sleep
 * - Interactive LED patterns with button control
 * - Watchdog timer integration
 * - Random pattern generation
 * 
 * Features:
 * - Version 3 Bis: Limited blink sequence with power management
 * - Counterstrike Mode: Interactive LED pattern game
 * - Dynamic timing adjustments
 * - System health monitoring
 * 
 * Hardware Project 2024
 * EINA - University of Zaragoza
 * 
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 02/12/2024
 */

#include "practica_4.h"
#include "drv_tiempo.h"
#include "drv_leds.h"
#include "drv_consumo.h"
#include "rt_evento_t.h"
#include "rt_GE.h"
#include "svc_alarma.h"
#include "drv_wdt.h"
#include "svc_log.h"
#include "board.h"

/******************************************************************************
 * Definitions
 *****************************************************************************/
#define NUM_BLINKS_BLINK_V3_BIS 10
#define RETARDO_MS_BLINK_V3_BIS 500
#define RETARDO_MS_COUNTERSTRIKE 500
#define DRV_WDT_TIMEOUT_MS_CS 1000
#define WDT_FEED_INTERVAL_MS_CS 500

/******************************************************************************
 * Global Variables
 *****************************************************************************/
static volatile uint32_t blink_count = 0;
static volatile uint8_t estado_leds_cs[32];
static volatile uint32_t limitador_cs;
static volatile uint32_t delay_cs;
static volatile uint32_t iterations_cs;
static volatile uint32_t num_leds_cs;

/******************************************************************************
 * LED Control Functions
 *****************************************************************************/

/**
 * @brief LED toggle callback for blink_v3_bis
 *
 * @param id_evento Event type
 * @param auxdata LED ID to toggle
 */
void conmutar_led_blink_v3_bis(uint32_t id_evento, uint32_t auxdata)
{
    // *2 because we count both on and off states
    if (blink_count < (NUM_BLINKS_BLINK_V3_BIS * 2) - 1)
    {
        drv_led_conmutar(auxdata);
        blink_count++;
    }
    else
    {
        // After 10 blinks, turn off all the LEDs
        for (uint8_t i = 1; i <= 4; i++)
        {
            drv_led_apagar(i);
        }
        drv_consumo_dormir(); // Enter deep sleep, no code after this will execute
    }
}

/**
 * @brief Toggle all LEDs in counterstrike mode
 *
 * @param id_evento Event type
 * @param auxdata Additional data (unused)
 */
void conmutar_todas_leds_cs(uint32_t id_evento, uint32_t auxdata)
{
    for (int i = 1; i <= num_leds_cs; i++)
    {
        drv_led_conmutar(i);
    }
}

/**
 * @brief Turn off LED task for counterstrike mode
 *
 * @param evento Event type
 * @param id LED ID to turn off
 */
void apagar_led_task_cs(uint32_t evento, uint32_t id)
{
    drv_led_apagar(id);
    estado_leds_cs[id - 1] = 0;
}

/**
 * @brief Main game logic for counterstrike mode
 *
 * @param id_evento Event type
 * @param auxdata Additional data (unused)
 */
void bit_counterstrike_logic(uint32_t id_evento, uint32_t auxdata)
{
    uint32_t aleatorio = drv_tiempo_actual_us() % limitador_cs;
    uint32_t apagado = 33;
    uint32_t num_false = 0;
    for (uint32_t i = 0; i < limitador_cs; i++)
    {
        if (estado_leds_cs[(aleatorio + i) % limitador_cs] == 0)
        {
            apagado = (aleatorio + i) % limitador_cs;
            break;
        }
        else
        {
            num_false++;
        }
    }
    if (apagado != 33)
    {
        estado_leds_cs[apagado] = 1;
        drv_led_encender(apagado + 1);
        // Cada diez iteraciones restamos 200ms del delay
        if (iterations_cs > 10 && iterations_cs % 10 == 0 && delay_cs > 200)
        {
            delay_cs -= 200;
        }
        iterations_cs++;
        svc_alarma_activar(svc_alarma_codificar(0, delay_cs + (iterations_cs % 4)), ev_COUNTER_STRIKE, 3);
    }
    else
    {
        for (int i = 1; i <= num_leds_cs; i++)
        {
            drv_led_apagar(i);
        }
        svc_GE_cancelar(ev_COUNTER_STRIKE, bit_counterstrike_logic);
        svc_GE_cancelar(ev_PULSAR_BOTON, apagar_led_task_cs);
        svc_GE_suscribir(ev_COUNTER_STRIKE, conmutar_todas_leds_cs);
        svc_alarma_activar(svc_alarma_codificar(1, RETARDO_MS_COUNTERSTRIKE), ev_COUNTER_STRIKE, 3);
    }
}

/******************************************************************************
 * Public Interface Functions
 *****************************************************************************/

/**
 * @brief Enhanced version of blink_v3 with limited blinks
 *
 * @param id LED identifier to blink
 */
void blink_v3_bis(uint32_t id)
{
    // Switch LED on
    drv_led_encender(id);
    blink_count = 0;

    // Subscribe to blink events and button events
    svc_GE_suscribir(ev_BLINK, conmutar_led_blink_v3_bis);

    // Start periodic alarm (500ms) with ev_BLINK event
    svc_alarma_activar(svc_alarma_codificar(1, RETARDO_MS_BLINK_V3_BIS), ev_BLINK, id);
}

/**
 * @brief Feed watchdog timer periodically
 * 
 * Prevents system reset by feeding watchdog timer at regular intervals.
 * Logs each feed operation for debugging.
 * 
 * @param evento Event type (always ev_FEED_WDT)
 * @param handler_id Watchdog handler ID to feed
 */
void feedWDT_cs(uint32_t evento, uint32_t handler_id)
{
	LOG_DEBUG("Feeding watchdog");
	svc_log_procesar();
	drv_wdt_feed();
}

/**
 * @brief Interactive LED pattern game launcher
 *
 * @param retardoSpawnInicial Initial spawn delay in milliseconds
 * @param num_leds Number of LEDs available
 * @param num_botones Number of buttons available
 */
void bit_counterstrike_launcher(uint32_t retardoSpawnInicial, uint32_t num_leds, uint32_t num_botones)
{
    // Initialize watchdog driver
    drv_wdt_iniciar(DRV_WDT_TIMEOUT_MS_CS, MONITOR3);

    // Subscribe to watchdog feed event
    svc_GE_suscribir(ev_FEED_WDT, feedWDT_cs);

    // Activate watchdog alarm
    svc_alarma_activar(svc_alarma_codificar(1, WDT_FEED_INTERVAL_MS_CS), ev_FEED_WDT, 0);
    LOG_DEBUG("Watchdog initialized");

    // Escoger cual es menor si el n de botones o de leds y como maximo 32
    uint32_t limitador = 0;
    if (num_botones < num_leds)
    {
        limitador = num_botones;
    }
    else
    {
        limitador = num_leds;
    }
    if (limitador > 32)
    {
        limitador = 32;
    }
    limitador_cs = limitador;
    delay_cs = retardoSpawnInicial;
    iterations_cs = 0;
    num_leds_cs = num_leds;
    // Inicializar el estado del juego
    for (int i = 0; i < limitador; i++)
        estado_leds_cs[i] = 0;

    svc_GE_suscribir(ev_PULSAR_BOTON, apagar_led_task_cs);
    svc_GE_suscribir(ev_COUNTER_STRIKE, bit_counterstrike_logic);
    svc_alarma_activar(svc_alarma_codificar(0, retardoSpawnInicial), ev_COUNTER_STRIKE, 3);
}
