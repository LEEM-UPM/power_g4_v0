#include "pyro_control.h"

#include <stdint.h>

#include "canNode.h"
#include "main.h"
#include "pyro.h"

/* Instante de encendido de cada canal (HAL_GetTick()); 0 = apagado */
static uint32_t pyro1A_on_counter = 0;
static uint32_t pyro1B_on_counter = 0;
static uint32_t pyro2A_on_counter = 0;
static uint32_t pyro2B_on_counter = 0;

static void FireRequested(void);
static void ContinuityRequested(void);
static void AutoOff(void);

void PyroControl_Update(void)
{
  FireRequested();
  ContinuityRequested();
  AutoOff();
}

/* CMD_PYRO_FIRE (0x120): el byte de datos indica el canal */
static void FireRequested(void)
{
  uint8_t fire = CanNode_TakePyroFire();

  if (fire & CAN_NODE_PYRO_1A) {
    pyro1A_on_counter = HAL_GetTick();
    Pyro1A_ON();
  }
  if (fire & CAN_NODE_PYRO_1B) {
    pyro1B_on_counter = HAL_GetTick();
    Pyro1B_ON();
  }
  if (fire & CAN_NODE_PYRO_2A) {
    pyro2A_on_counter = HAL_GetTick();
    Pyro2A_ON();
  }
  if (fire & CAN_NODE_PYRO_2B) {
    pyro2B_on_counter = HAL_GetTick();
    Pyro2B_ON();
  }
}

/* CMD_PYRO_CONTINUITY (0x121) -> PYRO_CONTINUITY (0x221) */
static void ContinuityRequested(void)
{
  if (!CanNode_TakeContinuityRequest()) {
    return;
  }

  uint8_t continuity_status = 0;
  if (Pyro1A_Continuity()) continuity_status |= CAN_NODE_PYRO_1A;
  if (Pyro1B_Continuity()) continuity_status |= CAN_NODE_PYRO_1B;
  if (Pyro2A_Continuity()) continuity_status |= CAN_NODE_PYRO_2A;
  if (Pyro2B_Continuity()) continuity_status |= CAN_NODE_PYRO_2B;

  CanNode_SendPyroContinuity(continuity_status);
}

/* Apagado automático de los pirotécnicos después de PYRO_ON_TIME_MS */
static void AutoOff(void)
{
  if ((pyro1A_on_counter != 0U) && ((HAL_GetTick() - pyro1A_on_counter) >= PYRO_ON_TIME_MS)) {
    Pyro1A_OFF();
    pyro1A_on_counter = 0;
  }
  if ((pyro1B_on_counter != 0U) && ((HAL_GetTick() - pyro1B_on_counter) >= PYRO_ON_TIME_MS)) {
    Pyro1B_OFF();
    pyro1B_on_counter = 0;
  }
  if ((pyro2A_on_counter != 0U) && ((HAL_GetTick() - pyro2A_on_counter) >= PYRO_ON_TIME_MS)) {
    Pyro2A_OFF();
    pyro2A_on_counter = 0;
  }
  if ((pyro2B_on_counter != 0U) && ((HAL_GetTick() - pyro2B_on_counter) >= PYRO_ON_TIME_MS)) {
    Pyro2B_OFF();
    pyro2B_on_counter = 0;
  }
}
