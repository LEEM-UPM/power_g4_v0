/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Pyro1Reg_EN_Pin GPIO_PIN_13
#define Pyro1Reg_EN_GPIO_Port GPIOC
#define OUT_11V_1_EN_Pin GPIO_PIN_14
#define OUT_11V_1_EN_GPIO_Port GPIOC
#define Pyro1A_EN_Pin GPIO_PIN_15
#define Pyro1A_EN_GPIO_Port GPIOC
#define ASensing_11V_Pin GPIO_PIN_1
#define ASensing_11V_GPIO_Port GPIOA
#define Pyro1_DAC_Pin GPIO_PIN_4
#define Pyro1_DAC_GPIO_Port GPIOA
#define VSensing_Pyro1_Pin GPIO_PIN_5
#define VSensing_Pyro1_GPIO_Port GPIOA
#define Pyro1A_Cont_Pin GPIO_PIN_6
#define Pyro1A_Cont_GPIO_Port GPIOA
#define Pyro1B_Cont_Pin GPIO_PIN_7
#define Pyro1B_Cont_GPIO_Port GPIOA
#define ASensing_Pyro1_Pin GPIO_PIN_0
#define ASensing_Pyro1_GPIO_Port GPIOB
#define Pyro1B_EN_Pin GPIO_PIN_1
#define Pyro1B_EN_GPIO_Port GPIOB
#define Pyro2A_EN_Pin GPIO_PIN_2
#define Pyro2A_EN_GPIO_Port GPIOB
#define Pyro2B_EN_Pin GPIO_PIN_10
#define Pyro2B_EN_GPIO_Port GPIOB
#define Pyro2_DAC_Pin GPIO_PIN_11
#define Pyro2_DAC_GPIO_Port GPIOB
#define VSensing_Pyro2_Pin GPIO_PIN_12
#define VSensing_Pyro2_GPIO_Port GPIOB
#define ASensing_Pyro2_Pin GPIO_PIN_13
#define ASensing_Pyro2_GPIO_Port GPIOB
#define VSensing_3V3_Pin GPIO_PIN_15
#define VSensing_3V3_GPIO_Port GPIOB
#define Pyro2B_Cont_Pin GPIO_PIN_8
#define Pyro2B_Cont_GPIO_Port GPIOA
#define Pyro2A_Cont_Pin GPIO_PIN_9
#define Pyro2A_Cont_GPIO_Port GPIOA
#define Pyro2Reg_EN_Pin GPIO_PIN_10
#define Pyro2Reg_EN_GPIO_Port GPIOA
#define OUT_11V_4_EN_Pin GPIO_PIN_3
#define OUT_11V_4_EN_GPIO_Port GPIOB
#define OUT_11V_3_EN_Pin GPIO_PIN_4
#define OUT_11V_3_EN_GPIO_Port GPIOB
#define OUT_11V_2_EN_Pin GPIO_PIN_9
#define OUT_11V_2_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
