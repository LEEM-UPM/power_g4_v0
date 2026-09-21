#ifndef CURRENT_SENSE_H
#define CURRENT_SENSE_H

/*
 * Medida de corriente de las ramas de alimentación: shunt + INA138 leídos por
 * ADC (3.3 V por ADC4, 5 V por ADC1).
 * Documentación de cada función en README_UTILIDADES.txt.
 */

#include <stdbool.h>

#include "adc.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Calibra y configura ADC1 y ADC4 y descarta las primeras muestras. Llamar
 * una vez tras MX_ADC1_Init() y MX_ADC4_Init() (lo hace Sys_init()). */
void CurrentSense_Init(void);

float ThreeV_Current(void);
float FiveV_Current(void);
float ElevenV_Current(void); /* TODO: declarada pero sin implementar */

#ifdef __cplusplus
}
#endif

#endif /* CURRENT_SENSE_H */
