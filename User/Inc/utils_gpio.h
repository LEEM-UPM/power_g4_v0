#ifndef UTILS_GPIO_H
#define UTILS_GPIO_H


#include "utils_can.h"
#include "main.h"
#include <stdbool.h>
#include "adc.h"

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

float ThreeV_Current(void);
float FiveV_Current(void);
float ElevenV_Current(void);

void Sys_init(void);


#ifdef __cplusplus
}
#endif

#endif /* UTILS_GPIO_H */
