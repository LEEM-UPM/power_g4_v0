#ifndef ANALOG_H
#define ANALOG_H

/*
 * Lectura de las entradas analógicas de medida de la placa: tensión en el
 * PIN del micro, en milivoltios. Pasar de esa tensión a la magnitud física
 * (tensión del raíl, corriente) depende de los divisores, shunts y
 * amplificadores del esquemático, y NO se hace aquí.
 *
 * Mapa de señales (esquemático de power y Power_MK2.ioc):
 *
 *   Canal            Pin    Red del esquemático  Camino             ADC
 *   ANALOG_V_3V3     PB15   +3V3_VSensing        directo            ADC2_IN15
 *   ANALOG_V_5V      PA0    +5V_VSensing         directo            ADC1_IN1  (*)
 *   ANALOG_V_11V     PA2    +11V_VSensing        directo            ADC1_IN3  (*)
 *   ANALOG_V_PYRO1   PA5    VSensing_Pyro1       directo            ADC2_IN13
 *   ANALOG_V_PYRO2   PB12   VSensing_Pyro2       directo            ADC1_IN11
 *   ANALOG_I_3V3     PB14   +3V3_ASensing        INA138             ADC4_IN4
 *   ANALOG_I_5V      PA3    +5V_ASensing         INA138             ADC1_IN4
 *   ANALOG_I_11V     PA1    +11V_ASensing        OPAMP3 seguidor    ADC2 (VOPAMP3)
 *   ANALOG_I_PYRO1   PB0    ASensing_Pyro1       OPAMP2 seguidor    ADC2 (VOPAMP2)
 *   ANALOG_I_PYRO2   PB13   ASensing_Pyro2       OPAMP4 seguidor    ADC5 (VOPAMP4)
 *
 * Fuera de CubeMX (sobreviven a una regeneración, pero no hay que usar estos
 * recursos en CubeMX para otra cosa sin revisar este módulo):
 *  - (*) PA0 y PA2 no están en el .ioc: Analog_Init() los pone en analógico.
 *  - ADC5 no está activado en CubeMX: lo inicializa Analog_Init() (handle y
 *    reloj propios en analog.c).
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ANALOG_V_3V3 = 0,
  ANALOG_V_5V,
  ANALOG_V_11V,
  ANALOG_V_PYRO1,
  ANALOG_V_PYRO2,
  ANALOG_I_3V3,
  ANALOG_I_5V,
  ANALOG_I_11V,
  ANALOG_I_PYRO1,
  ANALOG_I_PYRO2,
  ANALOG_COUNT
} Analog_Channel_t;

/* Pone PA0 y PA2 en analógico, calibra ADC2, inicializa y calibra ADC5 y
 * arranca los OPAMP 2, 3 y 4.
 * Llamar una vez después de CurrentSense_Init() (que calibra ADC1 y ADC4).
 * Llama a Error_Handler() si algo falla. */
void Analog_Init(void);

/* Lee `channel` (media de varias conversiones) y escribe en `*millivolts` la
 * tensión en el pin, suponiendo VDDA = 3,3 V. Bloqueante: unos pocos ms.
 * Devuelve false si el ADC falla. */
bool Analog_ReadPinMillivolts(Analog_Channel_t channel, uint16_t *millivolts);

#ifdef __cplusplus
}
#endif

#endif /* ANALOG_H */
