/**
 * @file drv_botones.c
 * @ingroup DRV_BUTTONS
 * @brief Button Driver Implementation
 * @details Implementation of button state management, debouncing logic, and event handling.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "drv_botones.h"
#include "hal_gpio.h"
#include "hal_ext_int.h"
#include "svc_alarma.h"
#include "rt_GE.h"
#include "drv_tiempo.h"
#include "rt_evento_t.h"
#include "board.h"
#include "svc_stats.h"

// Button debounce times (in ms)

/**
 * @brief Button debounce time in milliseconds
 */
#define TRP_MS 10 // Button press debounce time

/**
 * @brief Button polling interval in milliseconds
 */
#define TEP_MS 5 // Button polling interval

/**
 * @brief Button release debounce time in milliseconds
 */
#define TRD_MS 10 // Button release debounce time

/**
 * @brief Greatest Common Divisor of all timings in milliseconds
 */
#define GCD_TIMING 5 // ms, Greatest Common Divisor of all timings

typedef enum
{
    BOTON_IDLE,             // Initial state, waiting for an interrupt
    BOTON_REBOTE_PRESION,   // Waiting for press debounce to pass
    BOTON_PULSADO,          // Button stabilized in pressed
    BOTON_REBOTE_LIBERACION // Waiting for release debounce to pass
} estado_boton_t;

#if BUTTONS_NUMBER > 0
static const uint8_t button_list[BUTTONS_NUMBER] = BUTTONS_LIST;
static estado_boton_t estado_botones[BUTTONS_NUMBER];
static int32_t tiempo_ultima_pulsacion[BUTTONS_NUMBER];
#endif

static EVENTO_T evento_pulsacion;
static EVENTO_T evento_retardo;
static EVENTO_T evento_doble_pulsacion;
static void (*callback_pulsacion_encolar)(EVENTO_T evento, uint32_t auxData);
static uint32_t timeout_doble_pulsacion_ms;
static uint32_t alarma_activa = 0;

// Prototipos de funciones internas
void drv_botones_schedule_rebounce_alarm(uint32_t evento, uint32_t button_index);
static void drv_botones_int_handler(uint32_t pin);
static void procesar_estado_boton(void);
static uint32_t obtener_indice_boton(uint32_t pin);
void drv_botones_tratar(uint32_t evento, uint32_t auxData);

uint32_t doble_boton_time_stamp = 0;

/**
 * @brief Initialize the button driver
 *
 * @param callback Callback function for button events
 * @param ID_evento_pulsar Event ID for single press
 * @param ID_evento_retardo Event ID for periodic checks
 * @param ID_evento_doble Event ID for double press
 * @param timeout_doble_ms Maximum time between presses to consider it a double press (in ms)
 * @return uint32_t Number of buttons initialized
 */
uint32_t drv_botones_iniciar(void (*callback)(),
                             uint32_t ID_evento_pulsar,
                             uint32_t ID_evento_retardo,
                             uint32_t ID_evento_doble,
                             uint32_t timeout_doble_ms)
{
#if BUTTONS_NUMBER > 0
    evento_pulsacion = (EVENTO_T)ID_evento_pulsar;
    evento_retardo = (EVENTO_T)ID_evento_retardo;
    evento_doble_pulsacion = (EVENTO_T)ID_evento_doble;
    timeout_doble_pulsacion_ms = timeout_doble_ms;
    callback_pulsacion_encolar = callback;

    // Subscribe to handle our own events
    // Marks a warning due to the compiler not knowing that EVENTO_T maps to uint32_t
    // svc_GE_suscribir(evento_pulsacion, drv_botones_tratar);
    svc_GE_suscribir(evento_retardo, drv_botones_tratar);

    // Register our interrupt handler with hal_ext_int
    hal_ext_int_registrar_callback(drv_botones_int_handler);

    // Subscribe to schedule button rebounce alarm events with the corresponding callback
    svc_GE_suscribir(ev_ENABLE_BUTTON_REBOUNCE_ALARM, drv_botones_schedule_rebounce_alarm);

    // Inicializar estados y configurar pines
    for (uint32_t i = 0; i < BUTTONS_NUMBER; i++)
    {
        estado_botones[i] = BOTON_IDLE;
        tiempo_ultima_pulsacion[i] = -1;

        // Configure GPIO as input
        hal_gpio_sentido(button_list[i], HAL_GPIO_PIN_DIR_INPUT);

        // Configure external interrupts
        hal_ext_int_habilitar_int(button_list[i]);
        hal_ext_int_habilitar_despertar(button_list[i]);
    }
#endif
    return BUTTONS_NUMBER;
}

/**
 * @brief Process button state machine
 *
 * Handles button state transitions and event generation based on
 * current state and timing conditions.
 */
void drv_botones_tratar(uint32_t evento, uint32_t auxData)
{
#if BUTTONS_NUMBER > 0
    if ((EVENTO_T)evento == evento_retardo)
    {
        procesar_estado_boton(); // Remove parameter as we'll check all buttons
    }
#endif
}

/**
 * @brief Schedule a button rebounce alarm
 * 
 * @param evento Event type (always ev_ENABLE_BUTTON_REBOUNCE_ALARM)
 * @param button_index Index of the button that needs rebounce checking
 * 
 * @note This function is called when a button interrupt occurs to start
 *       the periodic checking process for button debouncing
 * @note Uses the alarm service to schedule periodic checks with GCD_TIMING interval
 */
void drv_botones_schedule_rebounce_alarm(uint32_t evento, uint32_t button_index) {
    svc_alarma_activar(svc_alarma_codificar(0, GCD_TIMING), evento_retardo, button_index);
}

/**
 * @brief Button interrupt handler
 *
 * Called when a button interrupt occurs. Disables further interrupts
 * for the pin and starts the debounce process.
 *
 * @param pin GPIO pin that triggered the interrupt
 */
static void drv_botones_int_handler(uint32_t pin)
{
#if BUTTONS_NUMBER > 0
    uint32_t indice = obtener_indice_boton(pin);
    if (indice < BUTTONS_NUMBER && estado_botones[indice] == BOTON_IDLE)
    {
        // End timing user response (at the beginning, to ensure exceptional measuring accuracy)
        svc_stats_user_response_end();

        /*
         * No need for critical section here, since if there's another button
         * interrupt, having the same priority as the previous one, it will
         * remain pending until this one finishes. And even if it had higher
         * priority, it would modify a different vector index.
         * In such case, the !alarma_activa conditional would be a slightly
         * more delicate situation, but the worst that could happen would be
         * reprogramming the evento_retardo alarm with the ID of the last
         * pressed button
         */
        hal_ext_int_deshabilitar_int(pin);

        estado_botones[indice] = BOTON_REBOTE_PRESION;
        tiempo_ultima_pulsacion[indice] = drv_tiempo_actual_ms();
			
		// Start periodic checks if not already running
        if (!alarma_activa)
        {
            callback_pulsacion_encolar(ev_ENABLE_BUTTON_REBOUNCE_ALARM, indice);
            alarma_activa = 1;
        }

		// Enqueue the button pressed event after the enable rebounce alarm event
        // (to ensure the alarm is scheduled ASAP)
        callback_pulsacion_encolar(evento_pulsacion, indice + 1);

        // Record interrupt start time (at the end, to be fair with the measurement)
        svc_stats_interrupt_start();
    }
#endif
}

/**
 * @brief Process current button state
 *
 * Updates button states based on timing and current pin values.
 * Handles debouncing and event generation.
 */
static void procesar_estado_boton(void)
{
#if BUTTONS_NUMBER > 0
    uint32_t tiempo_actual = drv_tiempo_actual_ms();

    // Gestión de dobles pulsaciones
    uint32_t botones_timeout = 0;

    for (uint32_t i = 0; i < BUTTONS_NUMBER; i++)
    {
        if (estado_botones[i] == BOTON_PULSADO &&
            tiempo_ultima_pulsacion[i] != -1 &&
            (tiempo_actual - tiempo_ultima_pulsacion[i]) >= timeout_doble_pulsacion_ms)
        {
            botones_timeout++;
            if (botones_timeout >= 2 && callback_pulsacion_encolar)
            {
                callback_pulsacion_encolar(evento_doble_pulsacion, i + 1);
                tiempo_ultima_pulsacion[i] = -1; // Reset del tiempo tras notificar
            }
        }
    }

    // Procesamiento normal de los botones
    uint32_t hay_botones_activos = 0;
    for (uint32_t id_boton = 0; id_boton < BUTTONS_NUMBER; id_boton++)
    {
        uint32_t pin = button_list[id_boton];
        uint32_t valor_pin = hal_ext_int_get_estado_pin(pin);
        int32_t tiempo_transcurrido = tiempo_actual - tiempo_ultima_pulsacion[id_boton];

        switch (estado_botones[id_boton])
        {
        case BOTON_REBOTE_PRESION:
            if (tiempo_transcurrido >= TRP_MS)
            {
                if (valor_pin == BUTTONS_ACTIVE_STATE)
                {
                    estado_botones[id_boton] = BOTON_PULSADO;
                    hay_botones_activos = 1;
                }
                else
                {
                    estado_botones[id_boton] = BOTON_REBOTE_LIBERACION;
										tiempo_ultima_pulsacion[id_boton] = tiempo_actual;
										hay_botones_activos = 1;
                }
            }
            else
            {
                hay_botones_activos = 1;
            }
            break;

        case BOTON_PULSADO:
            hay_botones_activos = 1;
            if (valor_pin != BUTTONS_ACTIVE_STATE)
            {
                estado_botones[id_boton] = BOTON_REBOTE_LIBERACION;
                tiempo_ultima_pulsacion[id_boton] = tiempo_actual;
            }
            break;

        case BOTON_REBOTE_LIBERACION:
            if (tiempo_transcurrido >= TRD_MS)
            {
                estado_botones[id_boton] = BOTON_IDLE;
                tiempo_ultima_pulsacion[id_boton] = -1;
                hal_ext_int_habilitar_int(pin);
            }
            else
            {
                hay_botones_activos = 1;
            }
            break;
        case BOTON_IDLE:
            break;
        }
    }

    if (hay_botones_activos)
    {
        svc_alarma_activar(svc_alarma_codificar(0, GCD_TIMING), evento_retardo, 0);
        alarma_activa = 1;
    }
    else
    {
        alarma_activa = 0;
    }
#endif
}

/**
 * @brief Get button index from pin number
 *
 * @param pin GPIO pin number
 * @return uint32_t Button index or BUTTONS_NUMBER if invalid
 */
static uint32_t obtener_indice_boton(uint32_t pin)
{
#if BUTTONS_NUMBER > 0
    for (uint32_t i = 0; i < BUTTONS_NUMBER; i++)
    {
        if (button_list[i] == pin)
        {
            return i;
        }
    }
#endif
    return BUTTONS_NUMBER; // Valor inválido
}
