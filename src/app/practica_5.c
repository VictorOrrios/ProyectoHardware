/**
 * @file practica_5.c
 * @ingroup APP_SIMON
 * @brief Simon Says Game Implementation
 * 
 * @details Implements a complete memory game with multiple features:
 * - Multiple difficulty levels with dynamic speed adjustment
 * - Sequence generation and validation
 * - Visual feedback through LED patterns
 * - Performance statistics collection
 * - System health monitoring
 * 
 * Key Components:
 * - State machine for game flow control
 * - Event-based input processing
 * - Watchdog timer integration
 * - Performance monitoring and statistics
 * - Power-efficient operation
 * 
 * Hardware Project 2024
 * EINA - University of Zaragoza
 * 
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 02/12/2024
 */

#include "practica_5.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "drv_botones.h"
#include "drv_rng.h"
#include "rt_GE.h"
#include "svc_alarma.h"
#include "drv_wdt.h"
#include "svc_log.h"
#include "rt_evento_t.h"
#include "rt_fifo.h"
#include "svc_stats.h"

#include "board.h"

/******************************************************************************
 * Game States
 *****************************************************************************/
enum
{
	ini_ON,			 // Initial state, all LEDs on
	ini_OFF,		 // Initial state, all LEDs off
	ini_CHECK,		 // Check initial sequence completion
	cpu_first,		 // Start of CPU sequence
	cpu_rellenar,	 // Fill next sequence item
	cpu_flash_ON,	 // Show sequence item (LED on)
	cpu_flash_OFF,	 // Show sequence item (LED off)
	cpu_flash_CHECK, // Check if sequence complete
	play_first,		 // Start of player turn
	play_idle,		 // Waiting for player input
	play_boton,		 // Button pressed
	play_ON,		 // Feedback LED on
	play_OFF,		 // Feedback LED off
	play_end_delay,	 // Delay after turn
	play_game_over,	 // Game over animation (on)
};

/******************************************************************************
 * Game Configuration
 *****************************************************************************/
// Timing constants (in milliseconds)
#define INI_ESPERA_SD 500		   // Wait between LED on/off in init sequence
#define INI_ANIMACION_MS_SD 200	   // Wait between LEDs in init animation
#define INI_REPETICIONES_SD 3	   // Number of init sequence repetitions
#define SHOW_DELAY_INITIAL_SD 700  // Initial delay between sequence items
#define PLAY_MAX_DELAY_SD 5000	   // Maximum time for player input
#define PLAY_FLASH_DELAY_SD 250	   // LED feedback duration after correct press
#define PLAY_END_DELAY_SD 500	   // Delay after sequence completion
#define GAME_OVER_ON_DELAY_SD 200  // Game over animation LED on time
#define GAME_OVER_MID_DELAY_SD 500 // Game over animation middle pause
#define GAME_OVER_DELAY_SD 1500	   // Game over animation total duration

// Difficulty settings (speed reduction factors)
#define DIFFICULTY_1_FACTOR_SD 0.95 // Easy mode speed reduction
#define DIFFICULTY_2_FACTOR_SD 0.90 // Medium mode speed reduction
#define DIFFICULTY_3_FACTOR_SD 0.80 // Hard mode speed reduction

// Watchdog configuration
#define DRV_WDT_TIMEOUT_MS_SD 1000	// Watchdog timeout period
#define WDT_FEED_INTERVAL_MS_SD 500 // Watchdog feed interval

/******************************************************************************
 * Global Variables
 *****************************************************************************/
// Initialization sequence
static uint8_t limitador_sd;	// Limit of LEDs/buttons (max 32)
static uint8_t ini_contador_sd; // Init sequence repetition counter

// CPU sequence
static uint8_t cpu_list_sd[64];				 // Sequence storage
static uint8_t cpu_tamg_sd;					 // Current sequence length
static uint8_t cpu_show_sd;					 // Items shown in current sequence
static uint32_t cpu_show_delay_sd;			 // Current sequence display speed
static volatile double difficulty_factor_sd; // Selected difficulty factor
static volatile uint32_t game_difficulty_sd; // Current game difficulty level

// Animation state
static volatile uint8_t led_animacion = 1;
static volatile uint8_t animacion_encendiendo = 1;

// Game over animation
static volatile uint8_t game_over_flash_count = 0;
static volatile uint8_t current_led = 1;

// Player state
static uint8_t play_index_sd; // Current position in sequence
static uint8_t play_b_sd;	  // Last button pressed

/******************************************************************************
 * Forwarded function declarations
 *****************************************************************************/
static void simon_animacion_handler(uint32_t evento, uint32_t aux);
static void simonSeleccionarDificultad(uint32_t evento, uint32_t botonPulsado);
static void reset_simon(uint32_t evento, uint32_t aux);
static void simonMaquinaEstados(uint32_t evento, uint32_t estado);
static void simonPulsarBoton(uint32_t evento, uint32_t botonPulsado);

/******************************************************************************
 * Internal Helper Functions
 *****************************************************************************/

/**
 * @brief Turn on LEDs from 1 to specified number
 *
 * @param ultima Highest LED number to turn on
 */
void encender_leds_desde_uno_sd(uint8_t ultima)
{
	for (int i = 1; i <= ultima; i++)
	{
		drv_led_encender(i);
	}
}

/**
 * @brief Turn off LEDs from 1 to specified number
 *
 * @param ultima Highest LED number to turn off
 */
void apagar_leds_desde_uno_sd(uint8_t ultima)
{
	for (int i = 1; i <= ultima; i++)
	{
		drv_led_apagar(i);
	}
}

/**
 * @brief Toggle LEDs from 1 to specified number
 *
 * @param ultima Highest LED number to toggle
 */
void conmutar_leds_desde_uno_sd(uint8_t ultima)
{
	for (int i = 1; i <= ultima; i++)
	{
		drv_led_conmutar(i);
	}
}

/**
 * @brief Fill sequence storage with random numbers
 */
void rellenarLista_sd()
{
	if (cpu_tamg_sd < sizeof(cpu_list_sd) / sizeof(cpu_list_sd[0]))
	{
		uint32_t result = 1;
		drv_rng_get_range(1, limitador_sd, &result);
		cpu_list_sd[cpu_tamg_sd] = result;
		cpu_tamg_sd++;
	}
}

/**
 * @brief Watchdog timer callback
 */
void wdt_callback_sd(void)
{
	LOG_ERROR("Watchdog about to bite real hard...");

	svc_log_procesar();
}

/******************************************************************************
 * Event Handlers
 *****************************************************************************/

/**
 * @brief Handle initial LED animation
 *
 * Controls the startup animation sequence, lighting LEDs in sequence
 * and then turning them off in reverse order.
 *
 * @param evento Event type (always ev_SIMON_GENERICO)
 * @param aux Additional data (unused)
 */
void simon_animacion_handler(uint32_t evento, uint32_t aux)
{
	if (animacion_encendiendo)
	{
		drv_led_encender(led_animacion);
		led_animacion++;

		if (led_animacion <= limitador_sd)
		{
			svc_alarma_activar(svc_alarma_codificar(0, INI_ANIMACION_MS_SD), ev_SIMON_GENERICO, 0);
		}
		else
		{
			animacion_encendiendo = 0;
			led_animacion = limitador_sd;
			svc_alarma_activar(svc_alarma_codificar(0, INI_ANIMACION_MS_SD), ev_SIMON_GENERICO, 0);
		}
	}
	else
	{
		drv_led_apagar(led_animacion);
		led_animacion--;

		if (led_animacion >= 1)
		{
			svc_alarma_activar(svc_alarma_codificar(0, INI_ANIMACION_MS_SD), ev_SIMON_GENERICO, 0);
		}
		else
		{
			// Animación completada, iniciar el juego
			svc_GE_cancelar(ev_SIMON_GENERICO, simon_animacion_handler);
			svc_GE_suscribir(ev_PULSAR_BOTON, simonSeleccionarDificultad);
		}
	}
}

/**
 * @brief Process difficulty selection at game start
 *
 * Handles button press to select game difficulty:
 * - Button 1: Easy (0.95 speed reduction)
 * - Button 2: Medium (0.90 speed reduction)
 * - Button 3: Hard (0.80 speed reduction)
 *
 * @param evento Event type (always ev_PULSAR_BOTON)
 * @param botonPulsado Button ID determining difficulty (1-3)
 */
void simonSeleccionarDificultad(uint32_t evento, uint32_t botonPulsado)
{
	// [NEW] Save button IRQ handling response time
	svc_stats_interrupt_end();
	
	game_difficulty_sd = botonPulsado <= 3 ? botonPulsado : 1;

	switch (botonPulsado)
	{
	case 1:
		LOG_INFO("Difficulty: Easy");
		difficulty_factor_sd = DIFFICULTY_1_FACTOR_SD;
		break;
	case 2:
		LOG_INFO("Difficulty: Medium");
		difficulty_factor_sd = DIFFICULTY_2_FACTOR_SD;
		break;
	case 3:
		LOG_INFO("Difficulty: Hard");
		difficulty_factor_sd = DIFFICULTY_3_FACTOR_SD;
		break;
	default:
		LOG_ERROR_F("Invalid difficulty %lu, defaulting to easy", botonPulsado);
		difficulty_factor_sd = DIFFICULTY_1_FACTOR_SD;
		break;
	}

	svc_log_procesar();
	drv_rng_init(drv_tiempo_actual_us());
	svc_GE_suscribir(ev_SIMON_GENERICO, simonMaquinaEstados);
	svc_GE_suscribir(ev_DOBLE_BOTON_PULSADO, reset_simon);
	svc_GE_cancelar(ev_PULSAR_BOTON, simonSeleccionarDificultad);
	rt_FIFO_encolar(ev_SIMON_GENERICO, ini_ON);
}

/**
 * @brief Process button press during gameplay
 *
 * Handles player input during sequence repetition phase.
 * Cancels button event subscription and queues button press event.
 *
 * @param evento Event type (always ev_PULSAR_BOTON)
 * @param botonPulsado ID of pressed button (1-4)
 */
static void simonPulsarBoton(uint32_t evento, uint32_t botonPulsado)
{
	// Save button IRQ handling response time
	svc_stats_interrupt_end();

	// NOTE: moved svc_stats_user_response_end(); to drv_botones_int_handler
	// to ensure exceptional measuring accuracy
	play_b_sd = botonPulsado;
	svc_GE_cancelar(ev_PULSAR_BOTON, simonPulsarBoton);
	rt_FIFO_encolar(ev_SIMON_GENERICO, play_boton);
}

/**
 * @brief Dump all statistics handler
 *
 * @param event Event type (always ev_DUMP_ALL_STATS)
 * @param auxData Additional data (unused)
 */
void dump_all_stats_handler(uint32_t event, uint32_t process_them)
{
	svc_stats_dump_all(process_them);
}

/**
 * @brief Reset game on double button press
 *
 * Handles game reset request:
 * 1. Cancels all Simon event subscriptions
 * 2. Turns off all LEDs
 * 3. Logs reset request
 * 4. Restarts game with same configuration
 *
 * @param evento Event type (always ev_DOBLE_BOTON_PULSADO)
 * @param aux Additional data (unused)
 */
void reset_simon(uint32_t evento, uint32_t aux)
{
	// Record game over with sequence length
	svc_stats_game_over(cpu_tamg_sd - 1);

	// Dump all statistics => NOTE: use event to avoid
	// saturating the thread
	rt_FIFO_encolar(ev_DUMP_ALL_STATS, 1);

	// Cancel all Simon event subscriptions
	svc_GE_cancelar(ev_SIMON_GENERICO, simonMaquinaEstados);
	svc_GE_cancelar(ev_PULSAR_BOTON, simonPulsarBoton);
	svc_GE_cancelar(ev_DOBLE_BOTON_PULSADO, reset_simon);

	// Turn off all LEDs
	apagar_leds_desde_uno_sd(limitador_sd);

	LOG_DEBUG("Game reset requested");

	// Restart game from the beginning
	simon_launcher(limitador_sd, limitador_sd, 1);
}

/******************************************************************************
 * State Machine Implementation
 *****************************************************************************/

/**
 * @brief Main game state machine handler
 *
 * Processes game state transitions and manages:
 * - Initialization sequence
 * - CPU sequence generation and display
 * - Player input processing
 * - Game over sequence
 *
 * @param evento Event that triggered state change
 * @param estado Current state to process
 */
void simonMaquinaEstados(uint32_t evento, uint32_t estado)
{
	switch (estado)
	{
	case ini_ON:
		encender_leds_desde_uno_sd(limitador_sd);
		svc_alarma_activar(svc_alarma_codificar(0, INI_ESPERA_SD), ev_SIMON_GENERICO, ini_OFF);
		break;
	case ini_OFF:
		apagar_leds_desde_uno_sd(limitador_sd);
		svc_alarma_activar(svc_alarma_codificar(0, INI_ESPERA_SD), ev_SIMON_GENERICO, ini_CHECK);
		ini_contador_sd++;
		break;
	case ini_CHECK:
		if (ini_contador_sd >= INI_REPETICIONES_SD)
		{
			rt_FIFO_encolar(ev_SIMON_GENERICO, cpu_first);
		}
		else
		{
			rt_FIFO_encolar(ev_SIMON_GENERICO, ini_ON);
		}
		break;
	case cpu_first:
		LOG_DEBUG_F("Starting new round, sequence length: %u", cpu_tamg_sd);
		// Record game start with difficulty level
		svc_stats_game_start(game_difficulty_sd);
		cpu_tamg_sd = 0;
		cpu_show_delay_sd = SHOW_DELAY_INITIAL_SD;
		rt_FIFO_encolar(ev_SIMON_GENERICO, cpu_rellenar);
		break;
	case cpu_rellenar:
		cpu_show_sd = 0;
		rellenarLista_sd();
		rt_FIFO_encolar(ev_SIMON_GENERICO, cpu_flash_ON);
		break;
	case cpu_flash_ON:
		drv_led_encender(cpu_list_sd[cpu_show_sd]);
		cpu_show_sd++;
		svc_alarma_activar(svc_alarma_codificar(0, cpu_show_delay_sd), ev_SIMON_GENERICO, cpu_flash_OFF);
		break;
	case cpu_flash_OFF:
		if (cpu_show_sd > 0)
			drv_led_apagar(cpu_list_sd[cpu_show_sd - 1]);
		if (cpu_show_sd >= cpu_tamg_sd)
		{
			// NOTE: improved player experience by removing small delay to detect button
			// 			 presses after switching off the last LED in the current sequence
			rt_FIFO_encolar(ev_SIMON_GENERICO, play_first);
		}
		else
		{
			svc_alarma_activar(svc_alarma_codificar(0, cpu_show_delay_sd), ev_SIMON_GENERICO, cpu_flash_ON);
		}
		break;
	case play_first:
		play_index_sd = 0;
		cpu_show_delay_sd = cpu_show_delay_sd * difficulty_factor_sd;
		rt_FIFO_encolar(ev_SIMON_GENERICO, play_idle);
		break;
	case play_idle:
		// Start measuring user response time just after the last led is switched off
		svc_stats_user_response_start(); // Start timing user response

		svc_alarma_activar(svc_alarma_codificar(0, PLAY_MAX_DELAY_SD), ev_SIMON_GENERICO, play_game_over);
		svc_GE_suscribir(ev_PULSAR_BOTON, simonPulsarBoton);
		break;
	case play_boton:
		LOG_DEBUG_F("Button pressed: %lu, Expected: %u", play_b_sd, cpu_list_sd[play_index_sd]);
		svc_alarma_activar(0, ev_SIMON_GENERICO, play_game_over);
		if (play_b_sd == cpu_list_sd[play_index_sd])
		{
			rt_FIFO_encolar(ev_SIMON_GENERICO, play_ON);
		}
		else
		{
			rt_FIFO_encolar(ev_SIMON_GENERICO, play_game_over);
		}
		break;
	case play_ON:
		drv_led_encender(play_b_sd);
		svc_alarma_activar(svc_alarma_codificar(0, PLAY_FLASH_DELAY_SD), ev_SIMON_GENERICO, play_OFF);
		break;
	case play_OFF:
		drv_led_apagar(play_b_sd);
		play_index_sd++;
		if (play_index_sd >= cpu_tamg_sd)
		{
			rt_FIFO_encolar(ev_SIMON_GENERICO, play_end_delay);
		}
		else
		{
			rt_FIFO_encolar(ev_SIMON_GENERICO, play_idle);
		}
		break;
	case play_end_delay:
		apagar_leds_desde_uno_sd(limitador_sd);
		svc_alarma_activar(svc_alarma_codificar(0, PLAY_END_DELAY_SD), ev_SIMON_GENERICO, cpu_rellenar);
		break;
	case play_game_over:
		if (current_led <= limitador_sd)
		{
			// Si current_led es impar, encendemos en orden normal
			// Si current_led es par, encendemos desde el final
			drv_led_conmutar(current_led % 2 == 0 ? limitador_sd - current_led / 2 + 1 : current_led / 2 + 1);
			current_led++;
			svc_alarma_activar(svc_alarma_codificar(0, GAME_OVER_ON_DELAY_SD), ev_SIMON_GENERICO, play_game_over);
		}
		else
		{
			game_over_flash_count++;
			current_led = 1;
			if (game_over_flash_count >= 3)
			{
				// 3 whole sequences completed
				LOG_INFO_F("Game Over! Score: %u sequences", cpu_tamg_sd - 1);

				// Count as game over event
				svc_stats_game_over(cpu_tamg_sd - 1);

				// Rest of required actions
				game_over_flash_count = 0;
				apagar_leds_desde_uno_sd(limitador_sd);
				svc_alarma_activar(svc_alarma_codificar(0, GAME_OVER_MID_DELAY_SD), ev_SIMON_GENERICO, ini_ON);
				ini_contador_sd = 0;
			}
			else
			{
				svc_alarma_activar(svc_alarma_codificar(0, GAME_OVER_ON_DELAY_SD), ev_SIMON_GENERICO, play_game_over);
			}
		}
		break;

	default:
		// Si llegamos aqui entonces algo hemos hecho mal
		while (1)
			;
	}
	svc_log_procesar();
}

/******************************************************************************
 * Public Interface Functions
 *****************************************************************************/

/**
 * @brief Launch Simon Says game
 *
 * @param num_leds Number of LEDs available
 * @param num_botones Number of buttons available
 * @param is_reset Flag indicating if this is a reset
 */
void simon_launcher(uint32_t num_leds, uint32_t num_botones, uint32_t is_reset)
{
	LOG_INFO_F("Simon Says starting with %lu LEDs and %lu buttons", num_leds, num_botones);

	if (!is_reset)
	{
		svc_stats_iniciar();

		// Subscribe to dump all stats event with the corresponding handler
		svc_GE_suscribir(ev_DUMP_ALL_STATS, dump_all_stats_handler);

		drv_wdt_iniciar(DRV_WDT_TIMEOUT_MS_SD, MONITOR3);
		drv_wdt_registrar_callback(wdt_callback_sd);

		LOG_DEBUG("Watchdog initialized");
	}

	// Determinar el limitador
	if (num_leds > num_botones)
	{
		limitador_sd = num_botones;
	}
	else
	{
		limitador_sd = num_leds;
	}
	if (limitador_sd > 32)
	{
		LOG_ERROR_F("Number of LEDs/buttons limited to 32 (was %lu)", limitador_sd);
		limitador_sd = 32;
	}

	svc_log_procesar();

	// Iniciar animación
	led_animacion = 1;
	animacion_encendiendo = 1;
	svc_GE_suscribir(ev_SIMON_GENERICO, simon_animacion_handler);
	svc_alarma_activar(svc_alarma_codificar(0, 100), ev_SIMON_GENERICO, 0);

	ini_contador_sd = 0;
	// NEW
	// cpu_show_delay_sd = SHOW_DELAY_INITIAL_SD;
}
