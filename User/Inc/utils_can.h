#ifndef UTILS_CAN_H
#define UTILS_CAN_H

#include "main.h"
#include "fdcan.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *data;
    uint8_t len;
} BufferView;

HAL_StatusTypeDef CAN_Utils_SetupMaskFilter(FDCAN_HandleTypeDef *hfdcan,
                                            uint32_t filterIndex,
                                            uint32_t stdId,
                                            uint32_t stdMask,
                                            bool rejectNonMatching);

HAL_StatusTypeDef CAN_Utils_SendStd(FDCAN_HandleTypeDef *hfdcan,
                                    uint16_t stdId,
                                    const uint8_t *data,
                                    uint8_t len);

HAL_StatusTypeDef CAN_Utils_PollRx(FDCAN_HandleTypeDef *hfdcan,
                                   FDCAN_RxHeaderTypeDef *rxHeader,
                                   uint8_t *rxData,
                                   uint8_t rxDataSize,
                                   uint8_t *outLen);

uint8_t CAN_Utils_DlcToLen(uint32_t dataLength);

void CAN_Setup(void);
bool CAN_Test(void);
bool CAN1_Available(void);
uint8_t *CAN1_PollRx(void);
void CAN1_Send(const uint8_t *data, uint8_t len);
void CAN2_Send(const uint8_t *data, uint8_t len);



#ifdef __cplusplus
}
#endif

#endif /* UTILS_CAN_H */
