#ifndef PYRO_CONTROL_H
#define PYRO_CONTROL_H

/*
 * Pirotecnia mandada por el core por CAN1 (ICD, 6.3 a 6.5):
 *  - CMD_PYRO_FIRE: enciende el canal pedido y lo apaga solo pasado
 *    PYRO_ON_TIME_MS.
 *  - CMD_PYRO_CONTINUITY: mide los cuatro canales y responde con
 *    PYRO_CONTINUITY.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PYRO_ON_TIME_MS 1000U /* tiempo que los pirotécnicos permanecen encendidos */

/* Llamar en cada vuelta del bucle principal, después de CanNode_Poll(). */
void PyroControl_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* PYRO_CONTROL_H */
