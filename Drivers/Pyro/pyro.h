#ifndef PYRO_H
#define PYRO_H

/*
 * Canales pirotécnicos: alimentación de 7 V de cada par (Pyro1/Pyro2), MOSFET
 * de cada canal (1A, 1B, 2A, 2B) y lectura de continuidad.
 * Documentación de cada función en README_UTILIDADES.txt.
 */

#include <stdbool.h>

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void Pyro1_ON(void);
void Pyro1_OFF(void);
void Pyro2_ON(void);
void Pyro2_OFF(void);

void Pyro1A_ON(void);
void Pyro1A_OFF(void);
void Pyro1B_ON(void);
void Pyro1B_OFF(void);
void Pyro2A_ON(void);
void Pyro2A_OFF(void);
void Pyro2B_ON(void);
void Pyro2B_OFF(void);

bool Pyro1A_Continuity(void);
bool Pyro1B_Continuity(void);
bool Pyro2A_Continuity(void);
bool Pyro2B_Continuity(void);

void AllPyroCheck(void);

#ifdef __cplusplus
}
#endif

#endif /* PYRO_H */
