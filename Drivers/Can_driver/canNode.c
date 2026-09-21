/**
 ******************************************************************************
 * @file    canNode.c
 * @brief   This board as a node of the common CAN1 bus between boards.
 ******************************************************************************
 */

#include "canNode.h"

#include "can_protocol/can_ids.h"
#include "can_protocol/can_pack.h"
#include "can_protocol/can_protocol_version.h"

/* CMD_PYRO_FIRE channel byte (ICD 6.3) */
#define PYRO_CHANNEL_1A 0x01u
#define PYRO_CHANNEL_1B 0x02u
#define PYRO_CHANNEL_2A 0x03u
#define PYRO_CHANNEL_2B 0x04u

/* One filter for two node fields: broadcast (0000) and power (0010) only
 * differ in bit 5 of the ID, so leaving that bit out of the mask accepts
 * 0x100-0x10F and 0x120-0x12F and nothing else. */
#define CAN_NODE_FILTER_ID CAN_ID(CAN_CLASS_COMMAND, CAN_NODE_BROADCAST, 0x0U)
#define CAN_NODE_FILTER_MASK \
  ((CAN_ID_CLASS_MASK | CAN_ID_NODE_MASK) & ~CAN_ID(0x0U, CAN_NODE_POWER, 0x0U))

/* G4 FDCAN message RAM elements are always 64 bytes, and
 * HAL_FDCAN_GetRxMessage() copies as many bytes as the frame's DLC says --
 * so the buffer must fit the largest frame. */
#define CAN_NODE_RX_BUFFER_BYTES 64u

static FDCAN_HandleTypeDef *canNodeHandle;

/* Only touched from the main loop. */
static bool awake;
static bool wakeUpPending;
static uint8_t pyroFirePending;
static bool continuityPending;
static bool powerRailsPending;

static uint32_t wakeUpCount;
static uint32_t incompatibleCount;
static uint32_t rejectedCount;
static uint32_t txErrorCount;

/* === Private helpers ====================================================== */

/* Queues one data frame, CAN FD without BRS. `dlc` must match `data`. */
static bool CanNode_Send(uint32_t id, uint32_t dlc, const uint8_t *data) {
  FDCAN_TxHeaderTypeDef header = {0};
  header.Identifier = id;
  header.IdType = FDCAN_STANDARD_ID;
  header.TxFrameType = FDCAN_DATA_FRAME;
  header.DataLength = dlc;
  header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  header.BitRateSwitch = FDCAN_BRS_OFF; /* bus is FD without BRS */
  header.FDFormat = FDCAN_FD_CAN;
  header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  header.MessageMarker = 0;

  if (HAL_FDCAN_AddMessageToTxFifoQ(canNodeHandle, &header,
                                    (uint8_t *)data) != HAL_OK) {
    txErrorCount++;
    return false;
  }
  return true;
}

static void CanNode_OnWakeUp(const FDCAN_RxHeaderTypeDef *header,
                             const uint8_t *data) {
  if (header->DataLength < FDCAN_DLC_BYTES_2) {
    rejectedCount++;
    return;
  }

  if (!CAN_PROTOCOL_IS_COMPATIBLE(can_get_u16_le(&data[0]))) {
    incompatibleCount++;
    return;
  }

  wakeUpCount++;
  awake = true;
  wakeUpPending = true;

  /* No data; nothing is copied, but the HAL still wants a buffer. */
  uint8_t unused = 0;
  (void)CanNode_Send(CAN_ID_WAKE_UP_ACK(CAN_NODE_POWER), FDCAN_DLC_BYTES_0,
                     &unused);
}

static void CanNode_OnPyroFire(const FDCAN_RxHeaderTypeDef *header,
                               const uint8_t *data) {
  if (!awake || header->DataLength < FDCAN_DLC_BYTES_1) {
    rejectedCount++;
    return;
  }

  switch (can_get_u8(&data[0])) {
    case PYRO_CHANNEL_1A:
      pyroFirePending |= CAN_NODE_PYRO_1A;
      break;
    case PYRO_CHANNEL_1B:
      pyroFirePending |= CAN_NODE_PYRO_1B;
      break;
    case PYRO_CHANNEL_2A:
      pyroFirePending |= CAN_NODE_PYRO_2A;
      break;
    case PYRO_CHANNEL_2B:
      pyroFirePending |= CAN_NODE_PYRO_2B;
      break;
    default:
      rejectedCount++; /* unknown channel: never guess which one to fire */
      break;
  }
}

/* === API =================================================================== */

bool CanNode_Init(FDCAN_HandleTypeDef *hfdcan) {
  canNodeHandle = hfdcan;

  FDCAN_FilterTypeDef filter = {0};
  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0; /* hfdcan1.Init.StdFiltersNbr = 1 in fdcan.c */
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = CAN_NODE_FILTER_ID;   /* 0x100 */
  filter.FilterID2 = CAN_NODE_FILTER_MASK; /* 0x7D0 */
  if (HAL_FDCAN_ConfigFilter(hfdcan, &filter) != HAL_OK) {
    return false;
  }

  /* The global filter can only be set before the peripheral is started. */
  if (HAL_FDCAN_ConfigGlobalFilter(hfdcan, FDCAN_REJECT, FDCAN_REJECT,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK) {
    return false;
  }

  return HAL_FDCAN_Start(hfdcan) == HAL_OK;
}

void CanNode_Poll(void) {
  if (canNodeHandle == NULL) {
    return;
  }

  /* Checking the fill level first keeps GetRxMessage() from ever hitting an
   * empty FIFO (which it would flag in hfdcan->ErrorCode). */
  while (HAL_FDCAN_GetRxFifoFillLevel(canNodeHandle, FDCAN_RX_FIFO0) > 0u) {
    FDCAN_RxHeaderTypeDef header;
    uint8_t data[CAN_NODE_RX_BUFFER_BYTES];
    if (HAL_FDCAN_GetRxMessage(canNodeHandle, FDCAN_RX_FIFO0, &header, data) !=
        HAL_OK) {
      break;
    }

    if (header.IdType != FDCAN_STANDARD_ID ||
        header.RxFrameType != FDCAN_DATA_FRAME) {
      rejectedCount++;
      continue;
    }

    switch (header.Identifier) {
      case CAN_ID_CMD_WAKE_UP:
        CanNode_OnWakeUp(&header, data);
        break;
      case CAN_ID_CMD_PYRO_FIRE:
        CanNode_OnPyroFire(&header, data);
        break;
      case CAN_ID_CMD_PYRO_CONTINUITY:
        if (awake) {
          continuityPending = true;
        } else {
          rejectedCount++;
        }
        break;
      case CAN_ID_CMD_POWER_RAILS:
        if (awake) {
          powerRailsPending = true;
        } else {
          rejectedCount++;
        }
        break;
      default:
        break; /* other commands through the same filter: not handled yet */
    }
  }
}

bool CanNode_PollWakeUp(void) {
  CanNode_Poll();

  bool pending = wakeUpPending;
  wakeUpPending = false;
  return pending;
}

bool CanNode_IsAwake(void) { return awake; }

uint8_t CanNode_TakePyroFire(void) {
  uint8_t pending = pyroFirePending;
  pyroFirePending = 0;
  return pending;
}

bool CanNode_TakeContinuityRequest(void) {
  bool pending = continuityPending;
  continuityPending = false;
  return pending;
}

bool CanNode_SendPyroContinuity(uint8_t status) {
  uint8_t data[1];
  can_put_u8(&data[0], status & (CAN_NODE_PYRO_1A | CAN_NODE_PYRO_1B |
                                 CAN_NODE_PYRO_2A | CAN_NODE_PYRO_2B));
  return CanNode_Send(CAN_ID_PYRO_CONTINUITY, FDCAN_DLC_BYTES_1, data);
}

bool CanNode_TakePowerRailsRequest(void) {
  bool pending = powerRailsPending;
  powerRailsPending = false;
  return pending;
}

bool CanNode_SendPowerRails(uint16_t current_3v3_ma, uint16_t current_5v_ma,
                            uint16_t current_11v_ma, uint16_t voltage_3v3_mv,
                            uint16_t voltage_5v_mv, uint16_t voltage_11v_mv) {
  /* ICD 6.7: 12 bytes, uint16 little-endian */
  uint8_t data[12] = {0};
  can_put_u16_le(&data[0], current_3v3_ma);
  can_put_u16_le(&data[2], current_5v_ma);
  can_put_u16_le(&data[4], current_11v_ma);
  can_put_u16_le(&data[6], voltage_3v3_mv);
  can_put_u16_le(&data[8], voltage_5v_mv);
  can_put_u16_le(&data[10], voltage_11v_mv);
  return CanNode_Send(CAN_ID_POWER_RAILS, FDCAN_DLC_BYTES_12, data);
}

uint32_t CanNode_WakeUpCount(void) { return wakeUpCount; }

uint32_t CanNode_IncompatibleCount(void) { return incompatibleCount; }

uint32_t CanNode_RejectedCount(void) { return rejectedCount; }

uint32_t CanNode_TxErrorCount(void) { return txErrorCount; }
