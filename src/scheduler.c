#include "rt_fifo.h"
#include "drv_leds.h"
#include "drv_consumo.h"
#include "drv_tiempo.h"
#include "board.h"

/**
 * @brief Ejecuta el planificador basado en eventos.
 * 
 * Inicializa la cola de eventos y configura un temporizador periódico para encolar eventos.
 * Comprueba continuamente si hay eventos para procesar, conmutando un LED si se detecta
 * un evento periódico. Si no hay eventos presentes, pone el procesador en un estado
 * de espera de bajo consumo.
 */
void scheduler_run() {
    EVENTO_T EV_ID_evento;
    uint32_t EV_auxData;
    Tiempo_us_t EV_TS;

    rt_FIFO_inicializar(MONITOR4);  // NOTA: al compilar, ya sea "BOARD_PCA10056" o "LPC2105_simulador"
                                    // estará correctamente definido (ver board.h). Así que no te preocupes, compilará bien
    drv_led_encender(4);
    drv_tiempo_periodico_ms(500, rt_FIFO_encolar, ev_T_PERIODICO);

    while (1) {
				EV_ID_evento = 0;
				if(rt_FIFO_extraer(&EV_ID_evento, &EV_auxData, &EV_TS)){
					if (EV_ID_evento == ev_T_PERIODICO) {
							drv_led_conmutar(4);
					}
				}else{
					drv_consumo_esperar();
				}
    }
}
