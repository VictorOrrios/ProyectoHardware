/* *****************************************************************************
 * Hardware Project 2024
 * 
 * main.c - Main Program Entry Point
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
 *   Main program entry point. Initializes all hardware abstraction layers,
 *   drivers, and services. Provides selection of different test and practice
 *   functions through conditional compilation.
 * 
 * Implementation Notes:
 *   - Hardware initialization order is critical
 *   - GPIO must be initialized before any other drivers
 *   - Event system initialization required for most practices
 *   - Double button press timeout configurable through define
 * *****************************************************************************/

#include <stdint.h>

#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "drv_botones.h"
#include "rt_fifo.h"
#include "drv_rng.h"
#include "rt_GE.h"
#include "svc_alarma.h"
#include "svc_wdt.h"
#include "drv_wdt.h"
#include "hal_ext_int.h"
#include "svc_log.h"
#include "board.h"
#include "testing.h"

#include "app/practica_2.h"
#include "app/practica_3.h"
#include "app/practica_4.h"
#include "app/practica_5.h"

/******************************************************************************
 * Definitions
 *****************************************************************************/
#define DOUBLE_BUTTON_TIMEOUT_MS 3000  // Maximum time between presses for double press

/******************************************************************************
 * Main Function
 *****************************************************************************/

/**
 * @brief Program entry point
 * 
 * Initializes system components in the following order:
 * 1. Hardware abstraction layers (GPIO, external interrupts)
 * 2. Basic drivers (LEDs, monitors, timers)
 * 3. Runtime system (FIFO queue, event manager)
 * 4. Services (alarms, buttons, power management, logging)
 * 
 * Then launches the selected practice or test function if LEDs are available.
 * Finally enters the event processing loop.
 * 
 * @return int Program exit code (never returns)
 */
int main(void)
{
	#ifndef RELEASE
	
		uint32_t num_leds;
    uint32_t num_botones;
	
		// Program selection menu
		// ===Simple hello-world examples===
		// 1. blink_v1 										| Active wait based LED blink
		// 2. blink_v2 										| Timer based LED blink
		// 3. blink_v3 										| Event based LED blink
		// 4. blink_v4 										| Queue based LED blink
		// 5. blink_v3_bis 								| Runtime based LED blink
		// ===Test of kernel components===
		// 6. test_fifo 									| Test FIFO queue functionality
		// 7. test_boton 									| Test button functionality
		// 8. test_wdt_overflow 					| Test watchdog reset
		// 9. test_critical_section 			| Test critical sections
		// 10. test_logs 									| Test UART communication
		// 11. test_random 								| Test random number generation
		// ===Aplication example===
		// 12. bit_counterstrike_launcher | LED wack-a-mole like game
		// 13. simon_launcher							| Simon Says  

		// ==========================================
		// Put your selected number       HERE
		uint8_t program_selection = 13; // <---
		// ==========================================
	
		switch(program_selection){
			case 1:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
			
				if(num_leds >0) blink_v1(1);
			
				break;
			case 2:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
			
				if(num_leds >0) blink_v2(2);
			
				break;
			case 3:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
			
				if(num_leds >0) blink_v3(3);
			
				break;
			case 4:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
			
				if(num_leds >0) blink_v4(4);
			
				break;
			case 5:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				hal_ext_int_iniciar();
				drv_botones_iniciar(
					rt_FIFO_encolar,
					ev_PULSAR_BOTON,
					ev_RETARDO,
					ev_DOBLE_BOTON_PULSADO,
					DOUBLE_BUTTON_TIMEOUT_MS);
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				if(num_leds >0) blink_v3_bis(3);
			
				rt_GE_lanzador();
			
				break;
			case 6:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				if(num_leds >0) test_fifo();   

				rt_GE_lanzador();			
				break;
			case 7:
				hal_gpio_iniciar();
				hal_ext_int_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				num_botones = drv_botones_iniciar(rt_FIFO_encolar, 
																					 ev_PULSAR_BOTON, 
																					 ev_RETARDO, 
																					 ev_DOBLE_BOTON_PULSADO, 
																					 DOUBLE_BUTTON_TIMEOUT_MS);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				if(num_leds >0 && num_botones > 0) test_boton();   

				rt_GE_lanzador();			
				break;
			case 8:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				if(num_leds >0) test_wdt_overflow(1);

				rt_GE_lanzador();			
				break;
			case 9:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				#ifndef LPC2105_simulador
					svc_log_iniciar(MONITOR1); // On the LPC2105, it may overflow the fifo and crash the system
				#endif
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				if(num_leds >0) test_critical_section(500, 1, 3, 5);

				rt_GE_lanzador();			
				break;
			case 10:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				test_logs();

				rt_GE_lanzador();			
				break;
			case 11:
				hal_gpio_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				test_random();

				rt_GE_lanzador();		
				break;
			case 12:
				hal_gpio_iniciar();
				hal_ext_int_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				num_botones = drv_botones_iniciar(rt_FIFO_encolar, 
																					 ev_PULSAR_BOTON, 
																					 ev_RETARDO, 
																					 ev_DOBLE_BOTON_PULSADO, 
																					 DOUBLE_BUTTON_TIMEOUT_MS);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				if(num_leds >0 && num_botones > 0) bit_counterstrike_launcher(2000, num_leds, num_botones);   

				rt_GE_lanzador();			
				break;
			case 13:
				hal_gpio_iniciar();
				hal_ext_int_iniciar();
				num_leds = drv_leds_iniciar();
				drv_monitor_iniciar();
				drv_tiempo_iniciar();
				drv_consumo_iniciar(MONITOR4, MONITOR3);
				rt_FIFO_inicializar(MONITOR4);
				rt_GE_iniciar(MONITOR1);
				num_botones = drv_botones_iniciar(rt_FIFO_encolar, 
																					 ev_PULSAR_BOTON, 
																					 ev_RETARDO, 
																					 ev_DOBLE_BOTON_PULSADO, 
																					 DOUBLE_BUTTON_TIMEOUT_MS);
				svc_log_iniciar(MONITOR1);
				svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);
			
				if(num_leds >0 && num_botones > 0) simon_launcher(num_leds, num_botones, 0);   

				rt_GE_lanzador();			
				break;
			
			default:
				// You didn't choose a supported program if you end up at this line
				break;
		}
	
	
	#endif
	#ifdef RELEASE
    uint32_t num_leds;
    uint32_t num_botones;

    /* Initialize Hardware Abstraction Layer */
    hal_gpio_iniciar();        // Must be first - other drivers depend on GPIO
    hal_ext_int_iniciar();     // External interrupts for buttons
    
    /* Initialize Basic Drivers */
    num_leds = drv_leds_iniciar();
    drv_monitor_iniciar();
    drv_tiempo_iniciar();
		drv_consumo_iniciar(MONITOR4, MONITOR3);
    
    /* Initialize Runtime System */
    rt_FIFO_inicializar(MONITOR4);
    rt_GE_iniciar(MONITOR1);
    
    
    num_botones = drv_botones_iniciar(rt_FIFO_encolar, 
                                     ev_PULSAR_BOTON, 
                                     ev_RETARDO, 
                                     ev_DOBLE_BOTON_PULSADO, 
                                     DOUBLE_BUTTON_TIMEOUT_MS);
    

		/* Initialize Alarms service */
		svc_alarma_iniciar(MONITOR2, rt_FIFO_encolar, ev_T_PERIODICO);

    /* Launch selected practice/test if LEDs available */
    if (num_leds > 0 && num_botones > 0)
    {
        simon_launcher(num_leds, num_botones, 0);  					// Launcher for simon says
    }

    /* Enter event processing loop - never returns */
    rt_GE_lanzador();
	#endif
}
