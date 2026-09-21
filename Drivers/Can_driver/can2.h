#ifndef CAN2_H
#define CAN2_H

/*
 * FDCAN2. CAN1 (bus común entre placas) va en canNode.h.
 */

#include <stdint.h>

#include "fdcan.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Acepta todas las tramas en la RX FIFO0, activa las interrupciones de error
 * y arranca FDCAN2. Llama a Error_Handler() si falla. */
void CAN2_Setup(void);

/* Envía hasta 8 bytes con el ID 0x400 en formato CAN clásico. */
void CAN2_Send(const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* CAN2_H */
