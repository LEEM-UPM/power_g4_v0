#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

/*
 * Tensión y corriente de los raíles de 3,3, 5 y 11 V en unidades físicas,
 * listas para el mensaje POWER_RAILS (ICD 6.7).
 *
 * Parte de los tensiones en el pin que da Analog_ReadPinMillivolts() y
 * aplica la electrónica de cada medida (INA138 + shunt para las corrientes,
 * divisor resistivo para las tensiones). Las constantes están al principio
 * de power_monitor.c.
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Valor de un campo cuya lectura no es válida (ICD 6.7) */
#define POWER_MONITOR_INVALID 0xFFFFU

/* Mayor valor válido: 0xFFFF queda reservado para "no válido" */
#define POWER_MONITOR_MAX_VALUE 0xFFFEU

typedef struct {
  uint16_t current_3v3_ma;
  uint16_t current_5v_ma;
  uint16_t current_11v_ma;
  uint16_t voltage_3v3_mv;
  uint16_t voltage_5v_mv;
  uint16_t voltage_11v_mv;
} PowerMonitor_Rails_t;

/* Lee las 6 medidas (bloqueante, unos milisegundos). Cada campo vale
 * POWER_MONITOR_INVALID si su lectura falla o si su electrónica aún no está
 * definida en power_monitor.c. */
void PowerMonitor_ReadRails(PowerMonitor_Rails_t *rails);

/* Si el core ha pedido CMD_POWER_RAILS (0x122), mide y responde con
 * POWER_RAILS (0x222). Llamar en cada vuelta del bucle principal, después de
 * CanNode_Poll(). Solo mide cuando hay petición: leer los 6 canales tarda
 * unos milisegundos. */
void PowerMonitor_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* POWER_MONITOR_H */
