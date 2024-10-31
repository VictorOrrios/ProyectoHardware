/* *****************************************************************************
  * P.H.2024: Driver/Manejador de los Leds
	*
	* blink practica 2 de proyecto hardware 2024
 */
 
#include <stdint.h>
#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "scheduler.h"
// Solo para el test de fifo
#include "board.h"

#define RETARDO_MS 500 		//retardo blink en milisegundos

/* *****************************************************************************
 * BLINK, parpadeo de un led conmutando on/off 
 * retardo por bucle de instrucciones, solo usa el manejador del led
 * para realizar la primera sesin de la practica
 */
void blink_v1(uint32_t id){
	drv_led_encender(id);
	while (1) {
		uint32_t volatile tmo;
		
		tmo = 10000000;
		while (tmo--);
		drv_led_conmutar(id);     
	}		
}

/* *****************************************************************************
 * BLINK, parpadeo de un led conmutando on/off 
 * activacion por tiempo, usa tanto manejador del led como el del tiempo
 * para realizar en la segunda sesin de la practica, version a entregar
 */
void blink_v2(uint32_t id){
	
	// NOTE: drv_tiempo_iniciar() already called in main function
	Tiempo_ms_t siguiente_activacion;	
	
	drv_led_encender(id);

	siguiente_activacion = drv_tiempo_actual_ms();
	
	/* Toggle LEDs. */
	while (true) {
		siguiente_activacion += RETARDO_MS; //ms
		drv_tiempo_esperar_hasta_ms(siguiente_activacion);
		drv_led_conmutar(id);
		//otras cosas
	}
}

void conmutar_led(uint32_t id, uint32_t time){
	drv_led_conmutar(id);
}

void blink_v3(uint32_t id){
	// NOTE: removed "id" param
	drv_led_encender(id);
	drv_tiempo_periodico_ms(RETARDO_MS, conmutar_led, id);
	while(1){
		// NOTE: will run past this line every time a clock interruption occurs
		drv_consumo_esperar();  
	}
}

/* *****************************************************************************
 * BLINK v4: LED blinking using event queue and scheduler
 * This version uses the runtime system with event queue management
 * and the scheduler for event processing
 */
void blink_v4(uint32_t id) {
	// TODO: use parameter
    // Initialize the event queue and scheduler
    scheduler_run();
    
    // The scheduler will handle:
    // 1. Setting up the periodic timer (500ms)
    // 2. Processing events from the queue
    // 3. Toggling LED based on periodic events
    // 4. Managing low power modes when idle
}

// Unidad de tests para la cola fifo
void test_fifo(){
		EVENTO_T EV_ID_evento;
    uint32_t EV_auxData;
    Tiempo_us_t EV_TS;

    rt_FIFO_inicializar(MONITOR4);
    drv_tiempo_periodico_ms(500, rt_FIFO_encolar, ev_T_PERIODICO);

		
		// Test de funcionamiento normal
    for(int j =0; j<=64; j++){
			for(int i = 0; i<j; i++){
				rt_FIFO_encolar(0,i);
			}
			
			for(int i = 0; i<j; i++){
				rt_FIFO_extraer(&EV_ID_evento,&EV_auxData,&EV_TS);
				if(EV_auxData != i){
					drv_led_encender(1);
					while(1);
				}
			}
		}
		
		// drv_led_encender(1);
		
		// Test de extraccion nula
		if(rt_FIFO_extraer(&EV_ID_evento,&EV_auxData,&EV_TS) != 0){
			drv_led_encender(2);
			while(1);
		}
		
		// Test de overflow
		drv_led_encender(3);
		for(int i = 0; i<65;i++){
				rt_FIFO_encolar(0,i);
		}
		drv_led_encender(4);
		while(1);
}

/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar a blink_v2
 */
int main(void){
	// TODO: check usage
	uint32_t Num_Leds;
	
	/* Initialize hal_gpio */
	hal_gpio_iniciar();			// Initialize GPIO before the drivers
	
	/* Configure LED */
	Num_Leds = drv_leds_iniciar();

	/* Initialize drv_tiempo (initializes the underlying system timer) */
	drv_tiempo_iniciar(); 		// DONE
	
	/* Initialize drv_monitor (for debuggin drv_consumo and rt_fifo) */
	drv_monitor_iniciar();
	
	/* Initialize drv_consumo (initializes the system for going idle and going to sleep) */
	drv_consumo_iniciar(3,3);
	

	
	if (Num_Leds > 0){
		// blink_v1(1);			// DONE
		// blink_v2(2);			// DONE
		 blink_v3(3);
		// blink_v4(4);
		// test_fifo();
	}
}
