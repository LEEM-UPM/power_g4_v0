#include "pyro.h"
#include "stm32g4xx_hal_gpio.h"

#define PYRO1_GPIO_PORT GPIOC
#define PYRO1_GPIO_PIN GPIO_PIN_13
#define PYRO2_GPIO_PORT GPIOA
#define PYRO2_GPIO_PIN GPIO_PIN_10
#define PYRO1A_GPIO_PORT GPIOC
#define PYRO1A_GPIO_PIN GPIO_PIN_15
#define PYRO1B_GPIO_PORT GPIOB
#define PYRO1B_GPIO_PIN GPIO_PIN_1
#define PYRO2A_GPIO_PORT GPIOB
#define PYRO2A_GPIO_PIN GPIO_PIN_2
#define PYRO2B_GPIO_PORT GPIOB
#define PYRO2B_GPIO_PIN GPIO_PIN_10

#define PYRO1A_CONTINUITY_GPIO_PORT GPIOA
#define PYRO1A_CONTINUITY_GPIO_PIN GPIO_PIN_6
#define PYRO1B_CONTINUITY_GPIO_PORT GPIOA
#define PYRO1B_CONTINUITY_GPIO_PIN GPIO_PIN_7
#define PYRO2A_CONTINUITY_GPIO_PORT GPIOA
#define PYRO2A_CONTINUITY_GPIO_PIN GPIO_PIN_9
#define PYRO2B_CONTINUITY_GPIO_PORT GPIOA
#define PYRO2B_CONTINUITY_GPIO_PIN GPIO_PIN_8

void Pyro1_ON(void)
{
    HAL_GPIO_WritePin(PYRO1_GPIO_PORT, PYRO1_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro1_OFF(void)
{
    HAL_GPIO_WritePin(PYRO1_GPIO_PORT, PYRO1_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro2_ON(void)
{
    HAL_GPIO_WritePin(PYRO2_GPIO_PORT, PYRO2_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro2_OFF(void)
{
    HAL_GPIO_WritePin(PYRO2_GPIO_PORT, PYRO2_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro1A_ON(void)
{
    HAL_GPIO_WritePin(PYRO1A_GPIO_PORT, PYRO1A_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro1A_OFF(void)
{
    HAL_GPIO_WritePin(PYRO1A_GPIO_PORT, PYRO1A_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro1B_ON(void)
{
    HAL_GPIO_WritePin(PYRO1B_GPIO_PORT, PYRO1B_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro1B_OFF(void)
{
    HAL_GPIO_WritePin(PYRO1B_GPIO_PORT, PYRO1B_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro2A_ON(void)
{
    HAL_GPIO_WritePin(PYRO2A_GPIO_PORT, PYRO2A_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro2A_OFF(void)
{
    HAL_GPIO_WritePin(PYRO2A_GPIO_PORT, PYRO2A_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro2B_ON(void)
{
    HAL_GPIO_WritePin(PYRO2B_GPIO_PORT, PYRO2B_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro2B_OFF(void)
{
    HAL_GPIO_WritePin(PYRO2B_GPIO_PORT, PYRO2B_GPIO_PIN, GPIO_PIN_RESET);
}

void AllPyroCheck(void)
{
    Pyro1_ON();
    Pyro2_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro1A_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro1B_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro2A_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro2B_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)

    Pyro1_OFF();
    Pyro2_OFF();
    Pyro1A_OFF();
    Pyro1B_OFF();
    Pyro2A_OFF();
    Pyro2B_OFF();
}

bool Pyro1A_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO1A_CONTINUITY_GPIO_PORT, PYRO1A_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}

bool Pyro1B_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO1B_CONTINUITY_GPIO_PORT, PYRO1B_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}

bool Pyro2A_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO2A_CONTINUITY_GPIO_PORT, PYRO2A_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}

bool Pyro2B_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO2B_CONTINUITY_GPIO_PORT, PYRO2B_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}
