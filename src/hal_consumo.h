/* *****************************************************************************
 * P.H.2024: hal_consumo, interfaz que nos independiza del hardware para el manejo
 * del consumo energético del microcontrolador.
 * 
 * Esta interfaz proporciona funciones para gestionar los diferentes modos de
 * consumo energético del microcontrolador, permitiendo optimizar el uso de energía
 * según las necesidades de la aplicación.
 */
 
#ifndef HAL_CONSUMO
#define HAL_CONSUMO

#include <stdint.h>

/**
 * @brief Inicializa el sistema de gestión de consumo energético.
 * 
 * Configura los registros necesarios para permitir los diferentes
 * modos de bajo consumo del microcontrolador.
 */
void hal_consumo_iniciar(void);

/**
 * @brief Pone al microcontrolador en modo de espera (wait).
 * 
 * En este modo, el procesador detiene su ejecución pero mantiene
 * los periféricos activos. Se despierta ante cualquier interrupción.
 * Consumo moderado de energía.
 */
void hal_consumo_esperar(void);

/**
 * @brief Pone al microcontrolador en modo de bajo consumo (sleep).
 * 
 * Modo de consumo mínimo donde se detienen la mayoría de los periféricos
 * y el reloj principal. Solo se mantienen activos los periféricos esenciales
 * configurados para despertar al sistema.
 */
void hal_consumo_dormir(void);

#endif
