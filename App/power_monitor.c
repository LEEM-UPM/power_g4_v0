#include "power_monitor.h"

#include <stddef.h>

#include "analog.h"
#include "canNode.h"

/* Últimas medidas enviadas y respuestas que no se pudieron encolar
 * (inspeccionar desde el depurador). */
static PowerMonitor_Rails_t lastRails;
static uint32_t sendErrorCount;

/* ===========================================================================
 * Electrónica de medida (esquemático de power)
 * ======================================================================== */

/* INA138: V_out = I * R_shunt * G, con G = R_L / 5 kΩ.
 * R_L = 390 kΩ (R64 en "11V Current Sense Amp") -> G = 78. Es el mismo valor
 * que usaba current_sense.c para 3,3 y 5 V. */
#define INA138_RL_OHMS 390000.0f
#define INA138_GAIN    (INA138_RL_OHMS / 5000.0f)

/* Shunts, en microohmios (0 = sin definir: la medida sale como no válida).
 * 3,3 y 5 V: valor heredado de current_sense.c (5 mΩ), sin comprobar.
 * TODO: comprobar los tres en el esquemático (resistencia entre las redes
 * XV_Current+ y XV_Current-). */
#define SHUNT_3V3_MICROOHMS 5000U
#define SHUNT_5V_MICROOHMS  5000U
#define SHUNT_11V_MICROOHMS 5000U

/* Corrección empírica heredada de ThreeV_Current() / FiveV_Current(). Un
 * factor tan lejos de 1 apunta a que el shunt real no es el de arriba: con
 * el valor correcto debería quedar en 1,0 y el offset en 0. */
#define CAL_3V3_FACTOR    0.30356f
#define CAL_3V3_OFFSET_MA 5.0f
#define CAL_5V_FACTOR     0.372f
#define CAL_5V_OFFSET_MA  0.0f
#define CAL_11V_FACTOR    1.0f
#define CAL_11V_OFFSET_MA 0.0f

/* Divisores de tensión, en ohmios (0 = sin definir: medida no válida).
 * R_TOP: del raíl al pin. R_BOTTOM: del pin a GND.
 * V_raíl = V_pin * (R_TOP + R_BOTTOM) / R_BOTTOM.
 * TODO: valores del esquemático (redes +3V3_VSensing, +5V_VSensing y
 * +11V_VSensing). */
#define DIV_3V3_R_TOP_OHMS    10U
#define DIV_3V3_R_BOTTOM_OHMS 100U
#define DIV_5V_R_TOP_OHMS     10U
#define DIV_5V_R_BOTTOM_OHMS  15U
#define DIV_11V_R_TOP_OHMS    100U
#define DIV_11V_R_BOTTOM_OHMS 24U

#if (SHUNT_3V3_MICROOHMS == 0U) || (SHUNT_5V_MICROOHMS == 0U) || \
    (SHUNT_11V_MICROOHMS == 0U) || (DIV_3V3_R_BOTTOM_OHMS == 0U) || \
    (DIV_5V_R_BOTTOM_OHMS == 0U) || (DIV_11V_R_BOTTOM_OHMS == 0U)
#warning "power_monitor.c: faltan shunts o divisores del esquemático; esas medidas salen como 0xFFFF"
#endif

/* ===========================================================================
 * Conversión
 * ======================================================================== */

/* Redondea y satura a [0, POWER_MONITOR_MAX_VALUE] */
static uint16_t ToField(float value)
{
    if (value <= 0.0f)
    {
        return 0U;
    }
    if (value >= (float)POWER_MONITOR_MAX_VALUE)
    {
        return POWER_MONITOR_MAX_VALUE;
    }
    return (uint16_t)(value + 0.5f);
}

/* I = V_pin / (G * R_shunt), en mA, con la corrección de calibración */
static uint16_t ReadCurrent(Analog_Channel_t channel, uint32_t shunt_microohms,
                            float cal_factor, float cal_offset_ma)
{
    uint16_t pin_mv;
    if ((shunt_microohms == 0U) || !Analog_ReadPinMillivolts(channel, &pin_mv))
    {
        return POWER_MONITOR_INVALID;
    }

    /* 1 mV / 1 µΩ = 1e-3 V / 1e-6 Ω = 1e3 A = 1e6 mA  ->  mA = mV * 1e6 / µΩ */
    float current_ma = ((float)pin_mv * 1.0e6f) /
                       (INA138_GAIN * (float)shunt_microohms);

    return ToField((current_ma * cal_factor) + cal_offset_ma);
}

/* V_raíl = V_pin * (R_top + R_bottom) / R_bottom, en mV */
static uint16_t ReadVoltage(Analog_Channel_t channel, uint32_t r_top_ohms,
                            uint32_t r_bottom_ohms)
{
    uint16_t pin_mv;
    if ((r_bottom_ohms == 0U) || !Analog_ReadPinMillivolts(channel, &pin_mv))
    {
        return POWER_MONITOR_INVALID;
    }

    uint64_t rail_mv = ((uint64_t)pin_mv * ((uint64_t)r_top_ohms + r_bottom_ohms)) /
                       r_bottom_ohms;

    return (rail_mv > POWER_MONITOR_MAX_VALUE) ? POWER_MONITOR_MAX_VALUE
                                               : (uint16_t)rail_mv;
}

void PowerMonitor_ReadRails(PowerMonitor_Rails_t *rails)
{
    if (rails == NULL)
    {
        return;
    }

    rails->current_3v3_ma = ReadCurrent(ANALOG_I_3V3, SHUNT_3V3_MICROOHMS,
                                        CAL_3V3_FACTOR, CAL_3V3_OFFSET_MA);
    rails->current_5v_ma = ReadCurrent(ANALOG_I_5V, SHUNT_5V_MICROOHMS,
                                       CAL_5V_FACTOR, CAL_5V_OFFSET_MA);
    rails->current_11v_ma = ReadCurrent(ANALOG_I_11V, SHUNT_11V_MICROOHMS,
                                        CAL_11V_FACTOR, CAL_11V_OFFSET_MA);

    rails->voltage_3v3_mv =
        ReadVoltage(ANALOG_V_3V3, DIV_3V3_R_TOP_OHMS, DIV_3V3_R_BOTTOM_OHMS);
    rails->voltage_5v_mv =
        ReadVoltage(ANALOG_V_5V, DIV_5V_R_TOP_OHMS, DIV_5V_R_BOTTOM_OHMS);
    rails->voltage_11v_mv =
        ReadVoltage(ANALOG_V_11V, DIV_11V_R_TOP_OHMS, DIV_11V_R_BOTTOM_OHMS);
}

void PowerMonitor_Update(void)
{
    if (!CanNode_TakePowerRailsRequest())
    {
        return;
    }

    PowerMonitor_ReadRails(&lastRails);

    if (!CanNode_SendPowerRails(lastRails.current_3v3_ma, lastRails.current_5v_ma,
                                lastRails.current_11v_ma, lastRails.voltage_3v3_mv,
                                lastRails.voltage_5v_mv, lastRails.voltage_11v_mv))
    {
        sendErrorCount++;
    }
}
