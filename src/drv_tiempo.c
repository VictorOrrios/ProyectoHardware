/**
 * @file drv_tiempo.c
 * @brief Driver/Manejador de los temporizadores
 * 
 * Este módulo implementa la gestión de temporizadores y funciones de tiempo,
 * proporcionando una capa de abstracción sobre el hardware específico.
 * 
 * @author P.H.2024
 * @date 2024
 * @version 1.0
 * @dependencies hal_tiempo.h
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

/** Factor de conversión entre ticks del sistema y microsegundos */
uint64_t factorTickToUs;

/** Variables globales para el manejo de eventos periódicos */
static uint32_t ID_evento_global;
static void(*drv_callback)();

/**
 * @brief Inicializa el reloj del sistema
 * 
 * Configura el temporizador del sistema y obtiene el factor de conversión
 * entre ticks y microsegundos.
 */
void drv_tiempo_iniciar(void){
    factorTickToUs = hal_tiempo_iniciar_tick();
}

/**
 * @brief Obtiene el tiempo actual en microsegundos
 * 
 * @return Tiempo transcurrido desde el inicio en microsegundos
 * @pre El driver debe estar inicializado
 */
Tiempo_us_t drv_tiempo_actual_us(void){
    return hal_tiempo_actual_tick()/factorTickToUs;
}

/**
 * @brief Obtiene el tiempo actual en milisegundos
 * 
 * @return Tiempo transcurrido desde el inicio en milisegundos
 * @pre El driver debe estar inicializado
 */
Tiempo_ms_t drv_tiempo_actual_ms(void){
    return drv_tiempo_actual_us()/(1000);    
}

/**
 * @brief Genera un retardo del tiempo especificado
 * 
 * @param ms Tiempo a esperar en milisegundos
 * @pre El driver debe estar inicializado
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
    while(drv_tiempo_actual_ms() < ms);
}

/**
 * @brief Espera hasta alcanzar un tiempo específico
 * 
 * @param ms Tiempo objetivo en milisegundos
 * @return Tiempo actual tras la espera
 * @pre El driver debe estar inicializado
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
    drv_tiempo_esperar_ms(ms);
    return drv_tiempo_actual_ms();
	
}

/**************************************************/
/* Funciones auxiliares internas del driver */
/**************************************************/
/**
 * @brief Callback interno para manejar eventos periódicos
 */
void hal_callback_no_params(void) {
    drv_callback(ID_evento_global, drv_tiempo_actual_ms());
}
/**
 * @brief Convierte milisegundos a ticks del sistema
 * 
 * @param periodo_ms Periodo en milisegundos
 * @return Periodo equivalente en ticks del sistema
 */
uint32_t periodo_en_tick(Tiempo_ms_t periodo_ms) {
    return (uint32_t)(periodo_ms * 1000 * factorTickToUs);
}


/**
 * @brief Configura una llamada periódica a una función
 * 
 * @param periodo_ms Periodo entre llamadas en milisegundos
 * @param funcion_encolar_evento Función a llamar periódicamente
 * @param ID_evento Identificador del evento a generar
 * @pre El driver debe estar inicializado
 */
void drv_tiempo_periodico_ms(Tiempo_ms_t periodo_ms, void(*funcion_encolar_evento)(), uint32_t ID_evento){
    ID_evento_global = ID_evento;
    drv_callback = funcion_encolar_evento;
    hal_tiempo_reloj_periodico_tick(periodo_en_tick(periodo_ms), hal_callback_no_params);
}

