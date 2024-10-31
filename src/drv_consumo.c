/**
 * @file drv_consumo.c
 * @brief Driver/Manejador de los modos de consumo del microcontrolador
 * 
 * Este módulo implementa la gestión de los diferentes modos de consumo,
 * permitiendo el control del consumo energético del sistema y monitorizando
 * los cambios de estado mediante marcadores.
 * 
 * @author P.H.2024
 * @date 2024
 * @version 1.0
 * @dependencies drv_monitor.h, hal_consumo.h
 */
  
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "hal_consumo.h"

/** Variables globales para almacenar los monitores de espera y modo dormir */
uint32_t mon_wait_global, mon_dormir_global;

/**
 * @brief Inicializa el driver de consumo
 * 
 * @param mon_wait Monitor para el modo de espera
 * @param mon_dormir Monitor para el modo dormir
 * 
 * @pre Los módulos hal_consumo y drv_monitor deben estar disponibles
 * @note Configura los monitores globales y inicializa los subsistemas necesarios
 */
void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir){
    mon_wait_global = mon_wait;
    mon_dormir_global = mon_dormir;
		drv_monitor_marcar(mon_wait);
		drv_monitor_marcar(mon_dormir);
    hal_consumo_iniciar();
}

/**
 * @brief Pone el sistema en modo de espera
 * 
 * Desmarca el monitor de espera, entra en modo de bajo consumo
 * y vuelve a marcar el monitor al despertar.
 * 
 * @pre El driver debe estar inicializado
 * @note Utiliza el monitor configurado en la inicialización
 */
void drv_consumo_esperar(void){
    drv_monitor_desmarcar(mon_wait_global);
    hal_consumo_esperar();
    drv_monitor_marcar(mon_wait_global);
}

/**
 * @brief Pone el sistema en modo dormir (sleep)
 * 
 * Desmarca el monitor de dormir, entra en modo de bajo consumo profundo
 * y vuelve a marcar el monitor al despertar.
 * 
 * @pre El driver debe estar inicializado
 * @note Utiliza el monitor configurado en la inicialización
 */
void drv_consumo_dormir(void){
    drv_monitor_desmarcar(mon_dormir_global);
    hal_consumo_dormir();
    drv_monitor_marcar(mon_dormir_global);
}
