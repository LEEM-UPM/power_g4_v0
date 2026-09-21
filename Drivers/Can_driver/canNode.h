/**
 ******************************************************************************
 * @file    canNode.h
 * @brief   This board as a node of the common CAN1 bus between boards.
 *
 * Everything on CAN1 follows the ICD in third_party/can_protocol: 1 Mbit/s,
 * CAN FD without BRS, 11-bit IDs built with CAN_ID(), little-endian fields.
 * This board is CAN_NODE_POWER (0x2).
 *
 * Messages handled here (ICD section 6):
 *  - CMD_WAKE_UP (0x100, core -> all): answered with WAKE_UP_ACK (0x220).
 *  - CMD_PYRO_FIRE (0x120, core -> power): 1 byte, channel to fire.
 *  - CMD_PYRO_CONTINUITY (0x121, core -> power): no data, asks for
 *    PYRO_CONTINUITY (0x221, power -> core, 1 byte bitmask).
 *  - CMD_POWER_RAILS (0x122, core -> power): no data, asks for POWER_RAILS
 *    (0x222, power -> core, 12 bytes: rail currents and voltages).
 *
 * Commands other than CMD_WAKE_UP are ignored until the first valid
 * CMD_WAKE_UP has arrived, so nothing on the bus can fire a channel before
 * the core has woken the board up.
 *
 * Reception is polled, not interrupt-driven: CanNode_Poll() reads the FDCAN1
 * Rx FIFO 0 directly. The G4's FIFO only holds 3 frames, so call it on every
 * pass of the main loop. It only records what arrived; the main loop picks
 * it up with the CanNode_Take*() functions.
 ******************************************************************************
 */

#ifndef CAN_NODE_H
#define CAN_NODE_H

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Pyro channels, one bit each. Used both by CanNode_TakePyroFire() and in
 * the PYRO_CONTINUITY bitmask (same bit order as the ICD, 6.5). */
#define CAN_NODE_PYRO_1A (1u << 0)
#define CAN_NODE_PYRO_1B (1u << 1)
#define CAN_NODE_PYRO_2A (1u << 2)
#define CAN_NODE_PYRO_2B (1u << 3)

/* === API ================================================================== */

/* Associates this module with FDCAN1 (already initialised by
 * MX_FDCAN1_Init()), lets through broadcast commands (0x100-0x10F) and
 * commands to this board (0x120-0x12F) with a single filter, rejects
 * everything else and starts the peripheral. Returns true on success. */
bool CanNode_Init(FDCAN_HandleTypeDef *hfdcan);

/* Drains Rx FIFO 0. Answers every valid CMD_WAKE_UP right away and records
 * pyro commands for the CanNode_Take*() functions below. */
void CanNode_Poll(void);

/* CanNode_Poll() and then: true if a valid CMD_WAKE_UP arrived since the
 * previous call. Meant for the start-up wait loop:
 *   while (!CanNode_PollWakeUp()) {}
 * A CMD_WAKE_UP whose protocol version is not compatible
 * (CAN_PROTOCOL_IS_COMPATIBLE()) is neither answered nor reported. */
bool CanNode_PollWakeUp(void);

/* True once a valid CMD_WAKE_UP has been received. */
bool CanNode_IsAwake(void);

/* Channels (CAN_NODE_PYRO_* bits) requested by CMD_PYRO_FIRE since the
 * previous call; 0 if none. */
uint8_t CanNode_TakePyroFire(void);

/* True if a CMD_PYRO_CONTINUITY arrived since the previous call. */
bool CanNode_TakeContinuityRequest(void);

/* Sends PYRO_CONTINUITY with `status` (CAN_NODE_PYRO_* bits, 1 = continuity).
 * Returns false if it could not be queued. */
bool CanNode_SendPyroContinuity(uint8_t status);

/* True if a CMD_POWER_RAILS arrived since the previous call. */
bool CanNode_TakePowerRailsRequest(void);

/* Sends POWER_RAILS (ICD 6.7). Currents in mA, voltages in mV; 0xFFFF in a
 * field means that reading is not valid. Returns false if it could not be
 * queued. */
bool CanNode_SendPowerRails(uint16_t current_3v3_ma, uint16_t current_5v_ma,
                            uint16_t current_11v_ma, uint16_t voltage_3v3_mv,
                            uint16_t voltage_5v_mv, uint16_t voltage_11v_mv);

/* Diagnostics (watch them in the debugger). */
uint32_t CanNode_WakeUpCount(void);       /* valid CMD_WAKE_UP received */
uint32_t CanNode_IncompatibleCount(void); /* CMD_WAKE_UP with another version */
uint32_t CanNode_RejectedCount(void);     /* malformed or before wake-up */
uint32_t CanNode_TxErrorCount(void);      /* frames that could not be queued */

#ifdef __cplusplus
}
#endif

#endif /* CAN_NODE_H */
