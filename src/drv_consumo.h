/**
 * @file drv_consumo.h
 * @brief Driver/Manejador de los modos de consumo del microcontrolador
 * 
 * Este módulo define la interfaz para gestionar los diferentes modos de consumo
 * del microcontrolador, permitiendo optimizar el consumo energético del sistema.
 * 
 * @author P.H.2024
 * @date 2024
 * @version 1.0
 * @dependencies stdint.h
 */
 
#ifndef DRV_CONSUMO
#define DRV_CONSUMO

#include <stdint.h>

/**
 * @brief Inicializa el driver de consumo
 * 
 * @param mon_wait Identificador del monitor para el modo de espera
 * @param mon_dormir Identificador del monitor para el modo dormir
 * 
 * @pre Los módulos hal_consumo y drv_monitor deben estar disponibles
 */
void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir);

/**
 * @brief Pone el sistema en modo de espera
 * 
 * Coloca el sistema en un estado de bajo consumo del que puede
 * despertar rápidamente mediante interrupciones.
 * 
 * @pre El driver debe estar inicializado mediante drv_consumo_iniciar()
 */
void drv_consumo_esperar(void);

/**
 * @brief Pone el sistema en modo dormir
 * 
 * Coloca el sistema en un estado de bajo consumo profundo.
 * El tiempo de despertar es mayor que en el modo de espera.
 * 
 * @pre El driver debe estar inicializado mediante drv_consumo_iniciar()
 */
void drv_consumo_dormir(void);

#endif
