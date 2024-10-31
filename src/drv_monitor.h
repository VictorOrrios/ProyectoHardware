/**
 * @file drv_monitor.h
 * @brief Driver/Manejador de los monitores de depuración
 * 
 * Este módulo define la interfaz para gestionar pines GPIO utilizados
 * como monitores para depuración. Permite seguir la ejecución temporal
 * del programa mediante señales digitales.
 * 
 * @author P.H.2024
 * @date 2024
 * @version 1.0
 * @dependencies stdint.h
 */

#ifndef DRV_MONITOR
#define DRV_MONITOR

#include <stdint.h>

/**
 * @brief Inicializa los monitores de depuración
 * 
 * @return Número de monitores disponibles en la plataforma
 * @pre MONITOR_NUMBER y MONITOR_LIST deben estar definidos en board.h
 */
uint32_t drv_monitor_iniciar(void);

/**
 * @brief Activa (marca) un monitor específico
 * 
 * @param id Identificador del monitor (1 a MONITOR_NUMBER)
 * @pre El monitor debe haber sido inicializado
 */
void drv_monitor_marcar(uint32_t id);

/**
 * @brief Desactiva (desmarca) un monitor específico
 * 
 * @param id Identificador del monitor (1 a MONITOR_NUMBER)
 * @pre El monitor debe haber sido inicializado
 */
void drv_monitor_desmarcar(uint32_t id);

#endif
