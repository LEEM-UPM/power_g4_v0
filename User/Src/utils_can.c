#include "utils_can.h"

#include <string.h>

#define CAN_STD_ID_MAX        0x7FFU
#define CAN_CLASSIC_MAX_BYTES 8U

static FDCAN_TxHeaderTypeDef txHeader;
static FDCAN_RxHeaderTypeDef rxHeader;
static uint8_t txData[8];
static uint8_t rxData[8];
static volatile uint8_t rxFlag = 0;
static uint32_t canErrorCounter = 0;

static uint32_t CAN_Utils_LenToDlc(uint8_t len)
{
    switch (len)
    {
        case 1U: return FDCAN_DLC_BYTES_1;
        case 2U: return FDCAN_DLC_BYTES_2;
        case 3U: return FDCAN_DLC_BYTES_3;
        case 4U: return FDCAN_DLC_BYTES_4;
        case 5U: return FDCAN_DLC_BYTES_5;
        case 6U: return FDCAN_DLC_BYTES_6;
        case 7U: return FDCAN_DLC_BYTES_7;
        case 8U:
        default:
            return FDCAN_DLC_BYTES_8;
    }
}

uint8_t CAN_Utils_DlcToLen(uint32_t dataLength)
{
    switch (dataLength)
    {
        case FDCAN_DLC_BYTES_1: return 1U;
        case FDCAN_DLC_BYTES_2: return 2U;
        case FDCAN_DLC_BYTES_3: return 3U;
        case FDCAN_DLC_BYTES_4: return 4U;
        case FDCAN_DLC_BYTES_5: return 5U;
        case FDCAN_DLC_BYTES_6: return 6U;
        case FDCAN_DLC_BYTES_7: return 7U;
        case FDCAN_DLC_BYTES_8: return 8U;
        default:
            return 0U;
    }
}

HAL_StatusTypeDef CAN_Utils_SetupMaskFilter(FDCAN_HandleTypeDef *hfdcan,
                                            uint32_t filterIndex,
                                            uint32_t stdId,
                                            uint32_t stdMask,
                                            bool rejectNonMatching)
{
    FDCAN_FilterTypeDef filter;
    uint32_t stdConfig;

    if (hfdcan == NULL)
    {
        return HAL_ERROR;
    }

    if ((stdId > CAN_STD_ID_MAX) || (stdMask > CAN_STD_ID_MAX))
    {
        return HAL_ERROR;
    }

    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = filterIndex;
    filter.FilterType = FDCAN_FILTER_MASK;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1 = stdId;
    filter.FilterID2 = stdMask;

    if (HAL_FDCAN_ConfigFilter(hfdcan, &filter) != HAL_OK)
    {
        return HAL_ERROR;
    }

    stdConfig = rejectNonMatching ? FDCAN_REJECT : FDCAN_ACCEPT_IN_RX_FIFO0;

    if (HAL_FDCAN_ConfigGlobalFilter(hfdcan,
                                     stdConfig,
                                     FDCAN_REJECT,
                                     FDCAN_REJECT_REMOTE,
                                     FDCAN_REJECT_REMOTE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef CAN_Utils_SendStd(FDCAN_HandleTypeDef *hfdcan,
                                    uint16_t stdId,
                                    const uint8_t *data,
                                    uint8_t len)
{
    FDCAN_TxHeaderTypeDef txHeader;
    uint8_t txData[CAN_CLASSIC_MAX_BYTES];

    if ((hfdcan == NULL) || (data == NULL) || (len == 0U))
    {
        return HAL_ERROR;
    }

    if (stdId > CAN_STD_ID_MAX)
    {
        return HAL_ERROR;
    }

    if (len > CAN_CLASSIC_MAX_BYTES)
    {
        len = CAN_CLASSIC_MAX_BYTES;
    }

    memset(txData, 0, sizeof(txData));
    memcpy(txData, data, len);

    txHeader.Identifier = stdId;
    txHeader.IdType = FDCAN_STANDARD_ID;
    txHeader.TxFrameType = FDCAN_DATA_FRAME;
    txHeader.DataLength = CAN_Utils_LenToDlc(len);
    txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    txHeader.BitRateSwitch = FDCAN_BRS_OFF;
    txHeader.FDFormat = FDCAN_CLASSIC_CAN;
    txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    txHeader.MessageMarker = 0U;

    return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &txHeader, txData);
}

HAL_StatusTypeDef CAN_Utils_PollRx(FDCAN_HandleTypeDef *hfdcan,
                                   FDCAN_RxHeaderTypeDef *rxHeader,
                                   uint8_t *rxData,
                                   uint8_t rxDataSize,
                                   uint8_t *outLen)
{
    HAL_StatusTypeDef status;

    if ((hfdcan == NULL) || (rxHeader == NULL) || (rxData == NULL) || (rxDataSize < CAN_CLASSIC_MAX_BYTES))
    {
        return HAL_ERROR;
    }

    if (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) == 0U)
    {
        return HAL_BUSY;
    }

    status = HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, rxHeader, rxData);
    if (status != HAL_OK)
    {
        return status;
    }

    if (outLen != NULL)
    {
        *outLen = CAN_Utils_DlcToLen(rxHeader->DataLength);
    }

    return HAL_OK;
}

static void CAN1_Setup(void)
{
  FDCAN_FilterTypeDef filter;

  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = 0b10000000000;
  filter.FilterID2 = 0b11111111111;

  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filter) != HAL_OK)
  {
    Error_Handler();
  }

  // Rechaza no filtrados: solo entran IDs que cumplan el filtro mascara
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
                                   FDCAN_REJECT,              // Std IDs sin filtro: rechazar
                                   FDCAN_REJECT,              // Ext IDs sin filtro: rechazar
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan1,
                                     FDCAN_IT_ERROR_WARNING |
                                     FDCAN_IT_ERROR_PASSIVE |
                                     FDCAN_IT_BUS_OFF |
                                     FDCAN_IT_ARB_PROTOCOL_ERROR |
                                     FDCAN_IT_DATA_PROTOCOL_ERROR,
                                     0U) != HAL_OK)
  {
    Error_Handler();
  }

  txHeader.Identifier = 0x123;
  txHeader.IdType = FDCAN_STANDARD_ID;
  txHeader.TxFrameType = FDCAN_DATA_FRAME;
  txHeader.DataLength = FDCAN_DLC_BYTES_8;
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txHeader.MessageMarker = 0;

  for (uint8_t i = 0; i < sizeof(txData); i++)
  {
    txData[i] = 0;
  }
}
static void CAN2_Setup(void)
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

  txHeader.Identifier = 0x123;
  txHeader.IdType = FDCAN_STANDARD_ID;
  txHeader.TxFrameType = FDCAN_DATA_FRAME;
  txHeader.DataLength = FDCAN_DLC_BYTES_8;
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txHeader.MessageMarker = 0;

  for (uint8_t i = 0; i < sizeof(txData); i++)
  {
    txData[i] = 0;
  }
}
void CAN_Setup(void) {
  CAN1_Setup();
  CAN2_Setup();
}

bool CAN_Test(void) {
    uint8_t payload[1] = { 1 };
    if (CAN_Utils_SendStd(&hfdcan2, 0b10000000000, payload, sizeof(payload)) != HAL_OK) {
        return false;
    }
    
    HAL_Delay(100);
    
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];
    uint8_t rxLen;
    if (CAN_Utils_PollRx(&hfdcan1, &rxHeader, rxData, sizeof(rxData), &rxLen) != HAL_OK) {
        return false;
    }
    
    if (rxLen != sizeof(payload)) {
        return false;
    }
    
    if (memcmp(payload, rxData, sizeof(payload)) != 0) {
        return false;
    }
    
    return true;
}

void CAN1_Send(const uint8_t *data, uint8_t len)
{
  HAL_StatusTypeDef status;
  uint8_t copyLen = len;

  if ((data == NULL) || (len == 0U))
  {
    canErrorCounter++;
    return;
  }

  /* Classic CAN supports up to 8 payload bytes. */
  if (copyLen > 8U)
  {
    copyLen = 8U;
  }

  for (uint8_t i = 0U; i < 8U; i++)
  {
    txData[i] = 0U;
  }

  for (uint8_t i = 0U; i < copyLen; i++)
  {
    txData[i] = data[i];
  }

  switch (copyLen)
  {
    case 1U:
      txHeader.DataLength = FDCAN_DLC_BYTES_1;
      break;
    case 2U:
      txHeader.DataLength = FDCAN_DLC_BYTES_2;
      break;
    case 3U:
      txHeader.DataLength = FDCAN_DLC_BYTES_3;
      break;
    case 4U:
      txHeader.DataLength = FDCAN_DLC_BYTES_4;
      break;
    case 5U:
      txHeader.DataLength = FDCAN_DLC_BYTES_5;
      break;
    case 6U:
      txHeader.DataLength = FDCAN_DLC_BYTES_6;
      break;
    case 7U:
      txHeader.DataLength = FDCAN_DLC_BYTES_7;
      break;
    case 8U:
    default:
      txHeader.DataLength = FDCAN_DLC_BYTES_8;
      break;
  }

  status = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData);

  if (status != HAL_OK)
  {
    canErrorCounter++;
  }
}
void CAN2_Send(const uint8_t *data, uint8_t len)
{
  HAL_StatusTypeDef status;
  uint8_t copyLen = len;

  if ((data == NULL) || (len == 0U))
  {
    canErrorCounter++;
    return;
  }

  /* Classic CAN supports up to 8 payload bytes. */
  if (copyLen > 8U)
  {
    copyLen = 8U;
  }

  for (uint8_t i = 0U; i < 8U; i++)
  {
    txData[i] = 0U;
  }

  for (uint8_t i = 0U; i < copyLen; i++)
  {
    txData[i] = data[i];
  }

  switch (copyLen)
  {
    case 1U:
      txHeader.DataLength = FDCAN_DLC_BYTES_1;
      break;
    case 2U:
      txHeader.DataLength = FDCAN_DLC_BYTES_2;
      break;
    case 3U:
      txHeader.DataLength = FDCAN_DLC_BYTES_3;
      break;
    case 4U:
      txHeader.DataLength = FDCAN_DLC_BYTES_4;
      break;
    case 5U:
      txHeader.DataLength = FDCAN_DLC_BYTES_5;
      break;
    case 6U:
      txHeader.DataLength = FDCAN_DLC_BYTES_6;
      break;
    case 7U:
      txHeader.DataLength = FDCAN_DLC_BYTES_7;
      break;
    case 8U:
    default:
      txHeader.DataLength = FDCAN_DLC_BYTES_8;
      break;
  }

  txHeader.Identifier = 0b10000000000;

  status = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &txHeader, txData);

  if (status != HAL_OK)
  {
    canErrorCounter++;
  }
}

uint8_t *CAN1_PollRx(void)
{
  if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0U)
  {
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
      rxFlag = 1;
      return rxData;
    }
    canErrorCounter++;
  }

  return NULL;
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
  if (hfdcan->Instance == FDCAN1)
  {
    canErrorCounter++;
  }
  if (hfdcan->Instance == FDCAN2)
  {
    canErrorCounter++;
  }
}

bool CAN1_Available(void) {
  return (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0U);
}