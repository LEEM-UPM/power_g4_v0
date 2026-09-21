#include "sys_init.h"

#include "analog.h"
#include "can2.h"
#include "canNode.h"
#include "current_sense.h"
#include "fdcan.h"
#include "main.h"

void Sys_init(void) {
    CurrentSense_Init();
    Analog_Init(); /* después de CurrentSense_Init(): ver analog.h */

    /* CAN1: bus común entre placas (ICD de third_party/can_protocol) */
    if (!CanNode_Init(&hfdcan1))
    {
        Error_Handler();
    }

    CAN2_Setup();
}
