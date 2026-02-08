/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    opamp.c
  * @brief   This file provides code for the configuration
  *          of the OPAMP instances.
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
/* Includes ------------------------------------------------------------------*/
#include "opamp.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

OPAMP_HandleTypeDef hopamp1;
OPAMP_HandleTypeDef hopamp2;
OPAMP_HandleTypeDef hopamp3;
OPAMP_HandleTypeDef hopamp4;
OPAMP_HandleTypeDef hopamp5;
OPAMP_HandleTypeDef hopamp6;

/* OPAMP1 init function */
void MX_OPAMP1_Init(void)
{

  /* USER CODE BEGIN OPAMP1_Init 0 */

  /* USER CODE END OPAMP1_Init 0 */

  /* USER CODE BEGIN OPAMP1_Init 1 */

  /* USER CODE END OPAMP1_Init 1 */
  hopamp1.Instance = OPAMP1;
  hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
  hopamp1.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO1;
  hopamp1.Init.InternalOutput = ENABLE;
  hopamp1.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP1_Init 2 */

  /* USER CODE END OPAMP1_Init 2 */

}
/* OPAMP2 init function */
void MX_OPAMP2_Init(void)
{

  /* USER CODE BEGIN OPAMP2_Init 0 */

  /* USER CODE END OPAMP2_Init 0 */

  /* USER CODE BEGIN OPAMP2_Init 1 */

  /* USER CODE END OPAMP2_Init 1 */
  hopamp2.Instance = OPAMP2;
  hopamp2.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
  hopamp2.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp2.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
  hopamp2.Init.InternalOutput = ENABLE;
  hopamp2.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp2.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP2_Init 2 */

  /* USER CODE END OPAMP2_Init 2 */

}
/* OPAMP3 init function */
void MX_OPAMP3_Init(void)
{

  /* USER CODE BEGIN OPAMP3_Init 0 */

  /* USER CODE END OPAMP3_Init 0 */

  /* USER CODE BEGIN OPAMP3_Init 1 */

  /* USER CODE END OPAMP3_Init 1 */
  hopamp3.Instance = OPAMP3;
  hopamp3.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
  hopamp3.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp3.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
  hopamp3.Init.InternalOutput = ENABLE;
  hopamp3.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp3.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP3_Init 2 */

  /* USER CODE END OPAMP3_Init 2 */

}
/* OPAMP4 init function */
void MX_OPAMP4_Init(void)
{

  /* USER CODE BEGIN OPAMP4_Init 0 */

  /* USER CODE END OPAMP4_Init 0 */

  /* USER CODE BEGIN OPAMP4_Init 1 */

  /* USER CODE END OPAMP4_Init 1 */
  hopamp4.Instance = OPAMP4;
  hopamp4.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
  hopamp4.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp4.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
  hopamp4.Init.InternalOutput = ENABLE;
  hopamp4.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp4.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP4_Init 2 */

  /* USER CODE END OPAMP4_Init 2 */

}
/* OPAMP5 init function */
void MX_OPAMP5_Init(void)
{

  /* USER CODE BEGIN OPAMP5_Init 0 */

  /* USER CODE END OPAMP5_Init 0 */

  /* USER CODE BEGIN OPAMP5_Init 1 */

  /* USER CODE END OPAMP5_Init 1 */
  hopamp5.Instance = OPAMP5;
  hopamp5.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
  hopamp5.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp5.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
  hopamp5.Init.InternalOutput = ENABLE;
  hopamp5.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp5.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP5_Init 2 */

  /* USER CODE END OPAMP5_Init 2 */

}
/* OPAMP6 init function */
void MX_OPAMP6_Init(void)
{

  /* USER CODE BEGIN OPAMP6_Init 0 */

  /* USER CODE END OPAMP6_Init 0 */

  /* USER CODE BEGIN OPAMP6_Init 1 */

  /* USER CODE END OPAMP6_Init 1 */
  hopamp6.Instance = OPAMP6;
  hopamp6.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
  hopamp6.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp6.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC;
  hopamp6.Init.InternalOutput = DISABLE;
  hopamp6.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp6.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP6_Init 2 */

  /* USER CODE END OPAMP6_Init 2 */

}

void HAL_OPAMP_MspInit(OPAMP_HandleTypeDef* opampHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(opampHandle->Instance==OPAMP1)
  {
  /* USER CODE BEGIN OPAMP1_MspInit 0 */

  /* USER CODE END OPAMP1_MspInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**OPAMP1 GPIO Configuration
    PA3     ------> OPAMP1_VINP
    */
    GPIO_InitStruct.Pin = ASensing_5V_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ASensing_5V_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN OPAMP1_MspInit 1 */

  /* USER CODE END OPAMP1_MspInit 1 */
  }
  else if(opampHandle->Instance==OPAMP2)
  {
  /* USER CODE BEGIN OPAMP2_MspInit 0 */

  /* USER CODE END OPAMP2_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**OPAMP2 GPIO Configuration
    PB0     ------> OPAMP2_VINP
    */
    GPIO_InitStruct.Pin = ASensing_Pyro1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ASensing_Pyro1_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN OPAMP2_MspInit 1 */

  /* USER CODE END OPAMP2_MspInit 1 */
  }
  else if(opampHandle->Instance==OPAMP3)
  {
  /* USER CODE BEGIN OPAMP3_MspInit 0 */

  /* USER CODE END OPAMP3_MspInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**OPAMP3 GPIO Configuration
    PA1     ------> OPAMP3_VINP
    */
    GPIO_InitStruct.Pin = ASensing_11V_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ASensing_11V_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN OPAMP3_MspInit 1 */

  /* USER CODE END OPAMP3_MspInit 1 */
  }
  else if(opampHandle->Instance==OPAMP4)
  {
  /* USER CODE BEGIN OPAMP4_MspInit 0 */

  /* USER CODE END OPAMP4_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**OPAMP4 GPIO Configuration
    PB13     ------> OPAMP4_VINP
    */
    GPIO_InitStruct.Pin = ASensing_Pyro2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ASensing_Pyro2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN OPAMP4_MspInit 1 */

  /* USER CODE END OPAMP4_MspInit 1 */
  }
  else if(opampHandle->Instance==OPAMP5)
  {
  /* USER CODE BEGIN OPAMP5_MspInit 0 */

  /* USER CODE END OPAMP5_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**OPAMP5 GPIO Configuration
    PB14     ------> OPAMP5_VINP
    */
    GPIO_InitStruct.Pin = ASensing_3V3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ASensing_3V3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN OPAMP5_MspInit 1 */

  /* USER CODE END OPAMP5_MspInit 1 */
  }
  else if(opampHandle->Instance==OPAMP6)
  {
  /* USER CODE BEGIN OPAMP6_MspInit 0 */

  /* USER CODE END OPAMP6_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**OPAMP6 GPIO Configuration
    PB11     ------> OPAMP6_VOUT
    */
    GPIO_InitStruct.Pin = Pyro2_DAC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(Pyro2_DAC_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN OPAMP6_MspInit 1 */

  /* USER CODE END OPAMP6_MspInit 1 */
  }
}

void HAL_OPAMP_MspDeInit(OPAMP_HandleTypeDef* opampHandle)
{

  if(opampHandle->Instance==OPAMP1)
  {
  /* USER CODE BEGIN OPAMP1_MspDeInit 0 */

  /* USER CODE END OPAMP1_MspDeInit 0 */

    /**OPAMP1 GPIO Configuration
    PA3     ------> OPAMP1_VINP
    */
    HAL_GPIO_DeInit(ASensing_5V_GPIO_Port, ASensing_5V_Pin);

  /* USER CODE BEGIN OPAMP1_MspDeInit 1 */

  /* USER CODE END OPAMP1_MspDeInit 1 */
  }
  else if(opampHandle->Instance==OPAMP2)
  {
  /* USER CODE BEGIN OPAMP2_MspDeInit 0 */

  /* USER CODE END OPAMP2_MspDeInit 0 */

    /**OPAMP2 GPIO Configuration
    PB0     ------> OPAMP2_VINP
    */
    HAL_GPIO_DeInit(ASensing_Pyro1_GPIO_Port, ASensing_Pyro1_Pin);

  /* USER CODE BEGIN OPAMP2_MspDeInit 1 */

  /* USER CODE END OPAMP2_MspDeInit 1 */
  }
  else if(opampHandle->Instance==OPAMP3)
  {
  /* USER CODE BEGIN OPAMP3_MspDeInit 0 */

  /* USER CODE END OPAMP3_MspDeInit 0 */

    /**OPAMP3 GPIO Configuration
    PA1     ------> OPAMP3_VINP
    */
    HAL_GPIO_DeInit(ASensing_11V_GPIO_Port, ASensing_11V_Pin);

  /* USER CODE BEGIN OPAMP3_MspDeInit 1 */

  /* USER CODE END OPAMP3_MspDeInit 1 */
  }
  else if(opampHandle->Instance==OPAMP4)
  {
  /* USER CODE BEGIN OPAMP4_MspDeInit 0 */

  /* USER CODE END OPAMP4_MspDeInit 0 */

    /**OPAMP4 GPIO Configuration
    PB13     ------> OPAMP4_VINP
    */
    HAL_GPIO_DeInit(ASensing_Pyro2_GPIO_Port, ASensing_Pyro2_Pin);

  /* USER CODE BEGIN OPAMP4_MspDeInit 1 */

  /* USER CODE END OPAMP4_MspDeInit 1 */
  }
  else if(opampHandle->Instance==OPAMP5)
  {
  /* USER CODE BEGIN OPAMP5_MspDeInit 0 */

  /* USER CODE END OPAMP5_MspDeInit 0 */

    /**OPAMP5 GPIO Configuration
    PB14     ------> OPAMP5_VINP
    */
    HAL_GPIO_DeInit(ASensing_3V3_GPIO_Port, ASensing_3V3_Pin);

  /* USER CODE BEGIN OPAMP5_MspDeInit 1 */

  /* USER CODE END OPAMP5_MspDeInit 1 */
  }
  else if(opampHandle->Instance==OPAMP6)
  {
  /* USER CODE BEGIN OPAMP6_MspDeInit 0 */

  /* USER CODE END OPAMP6_MspDeInit 0 */

    /**OPAMP6 GPIO Configuration
    PB11     ------> OPAMP6_VOUT
    */
    HAL_GPIO_DeInit(Pyro2_DAC_GPIO_Port, Pyro2_DAC_Pin);

  /* USER CODE BEGIN OPAMP6_MspDeInit 1 */

  /* USER CODE END OPAMP6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
