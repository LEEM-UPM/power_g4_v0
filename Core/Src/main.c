/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "fdcan.h"
#include "i2c.h"
#include "opamp.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32g4xx_hal_gpio.h"
#include "utils_gpio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define PYRO_ON_TIME_MS 1000  // tiempo en milisegundos que los pirotecnicos permanecen encendidos
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t pyro1A_on_counter = 0;  // variables que cronometraran el tiempo de encendido de los canales pirotecnicos
uint32_t pyro1B_on_counter = 0;
uint32_t pyro2A_on_counter = 0;
uint32_t pyro2B_on_counter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC1_Init();
  MX_DAC3_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_I2C1_Init();
  MX_OPAMP2_Init();
  MX_OPAMP3_Init();
  MX_OPAMP4_Init();
  MX_OPAMP6_Init();
  MX_ADC4_Init();
  /* USER CODE BEGIN 2 */

  Sys_init();

  AllPyroCheck();

  Pyro1_ON(); // encendemos la alimentacion de los pyrotecnicos
  Pyro2_ON();

  if (!CAN_Test()) // Test CAN1 con CAN2, han de estar conectados para que funcione
  {
    Error_Handler();
  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */



  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint8_t payload[1] = { 0x05 }; // seleccionar comando para hacer la prueba de envio
    CAN2_Send(payload, 1);

    if(CAN1_Available()) {
      uint8_t *rx = CAN1_PollRx();
      if (rx[0] == 0x01) { // activar pyro 1A
        pyro1A_on_counter = HAL_GetTick();
        Pyro1A_ON();
      }
      if(rx[0] == 0x02) { // activar pyro 1B
        pyro1B_on_counter = HAL_GetTick();
        Pyro1B_ON();
      }
      if(rx[0] == 0x03) { // activar pyro 2A
        pyro2A_on_counter = HAL_GetTick();
        Pyro2A_ON();
      }
      if (rx[0] == 0x04) { // activar pyro 2B
        pyro2B_on_counter = HAL_GetTick();
        Pyro2B_ON();
      }
      if (rx[0] == 0x05) { // continuity request

        bool continuity1A = Pyro1A_Continuity();
        bool continuity1B = Pyro1B_Continuity();
        bool continuity2A = Pyro2A_Continuity();
        bool continuity2B = Pyro2B_Continuity();

        uint8_t continuity_status = 0;
        if (continuity1A) continuity_status |= 0x01;
        if (continuity1B) continuity_status |= 0x02;
        if (continuity2A) continuity_status |= 0x04;
        if (continuity2B) continuity_status |= 0x08;

        CAN1_Send(&continuity_status, 1);
        
      }
    }

    {  // apagado automatico de los pirotecnicos despues de un tiempo
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

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
