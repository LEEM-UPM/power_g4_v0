#include "utils_can.h"

#include <string.h>

#define CAN_STD_ID_MAX        0x7FFU
#define CAN_CLASSIC_MAX_BYTES 8U
/* Los FDCAN van en modo CAN FD: HAL_FDCAN_GetRxMessage() copia tantos bytes
 * como indique el DLC de la trama recibida, hasta 64. Todo buffer de
 * recepción debe tener este tamaño para no desbordarse. */
#define CAN_FD_MAX_BYTES      64U

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

    if ((hfdcan == NULL) || (rxHeader == NULL) || (rxData == NULL) || (rxDataSize < CAN_FD_MAX_BYTES))
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
