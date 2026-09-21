#include "can2.h"

#include "utils_can.h"

#define CAN2_TX_ID 0x400U

static uint32_t canErrorCounter = 0;

void CAN2_Setup(void)
{
  FDCAN_FilterTypeDef filter;

  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = 0x000;  // Acepta cualquier ID
  filter.FilterID2 = 0x000;  // Máscara 0 = no importa ningún bit

  if (HAL_FDCAN_ConfigFilter(&hfdcan2, &filter) != HAL_OK)
  {
    Error_Handler();
  }

  // Configuración global: ACEPTAR todos los mensajes que no pasen por filtros
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
                                   FDCAN_ACCEPT_IN_RX_FIFO0,
                                   FDCAN_ACCEPT_IN_RX_FIFO0,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan2,
                                     FDCAN_IT_ERROR_WARNING |
                                     FDCAN_IT_ERROR_PASSIVE |
                                     FDCAN_IT_BUS_OFF |
                                     FDCAN_IT_ARB_PROTOCOL_ERROR |
                                     FDCAN_IT_DATA_PROTOCOL_ERROR,
                                     0U) != HAL_OK)
  {
    Error_Handler();
  }
}

void CAN2_Send(const uint8_t *data, uint8_t len)
{
  /* Rellena con ceros hasta 8 bytes como máximo y envía en CAN clásico */
  if (CAN_Utils_SendStd(&hfdcan2, CAN2_TX_ID, data, len) != HAL_OK)
  {
    canErrorCounter++;
  }
}

void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->Instance == FDCAN1)
  {
    canErrorCounter++;
  }
  if (hfdcan->Instance == FDCAN2)
  {
    canErrorCounter++;
  }
}

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  (void)ErrorStatusITs;

  if (hfdcan->Instance == FDCAN1)
  {
    canErrorCounter++;
  }
  if (hfdcan->Instance == FDCAN2)
  {
    canErrorCounter++;
  }
}
