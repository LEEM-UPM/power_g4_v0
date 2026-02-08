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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

static void CAN1_Setup(void);
static void CAN1_SendCounter(uint8_t *data);  /* data: pointer to 8-byte array */
static void CAN2_Setup(void);
static void CAN2_SendCounter(uint8_t *data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static FDCAN_TxHeaderTypeDef txHeader;
static FDCAN_RxHeaderTypeDef rxHeader;
static uint8_t txData[8] = {"TETE"};
static uint8_t rxData[8];
static volatile uint8_t rxFlag = 0;
static uint32_t canErrorCounter = 0;
static uint8_t sendCounter = 0;  /* Contador interno que incrementa cada envío */


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
  MX_OPAMP1_Init();
  MX_OPAMP2_Init();
  MX_OPAMP3_Init();
  MX_OPAMP4_Init();
  MX_OPAMP5_Init();
  MX_OPAMP6_Init();
  /* USER CODE BEGIN 2 */

  CAN2_Setup();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Verificar si hay mensajes en el FIFO
    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan2, FDCAN_RX_FIFO0) > 0)
    {
      FDCAN_RxHeaderTypeDef rxh;
      uint8_t data[8] = {0};
      
      if (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &rxh, data) == HAL_OK)
      {
        
        // Si data[0] == 0x01, encender pyro 1 segundo
        if(data[0] == 0x01) {
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);  /// pin pirotecnico
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);  /// pin que habilita la alimentacion del pirotecnico
          HAL_Delay(1000);
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);   
        }
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
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

static void CAN1_Setup(void)
{
  FDCAN_FilterTypeDef filter;

  /* Enable FDCAN interrupts in NVIC */
  HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);

  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = 0x000;  // Acepta cualquier ID
  filter.FilterID2 = 0x000;  // Máscara 0 = no importa ningún bit

  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filter) != HAL_OK)
  {
    Error_Handler();
  }

  // Configuración global: ACEPTAR todos los mensajes que no pasen por filtros
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
                                   FDCAN_ACCEPT_IN_RX_FIFO0,  // Aceptar std IDs sin filtro
                                   FDCAN_ACCEPT_IN_RX_FIFO0,  // Aceptar ext IDs sin filtro
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan1,
                                     FDCAN_IT_RX_FIFO0_NEW_MESSAGE |
                                     FDCAN_IT_BUS_OFF |
                                     FDCAN_IT_ERROR_WARNING |
                                     FDCAN_IT_ERROR_PASSIVE,
                                     0) != HAL_OK)
  {
    Error_Handler();
  }

  txHeader.Identifier = 0x123;
  txHeader.IdType = FDCAN_STANDARD_ID;
  txHeader.TxFrameType = FDCAN_DATA_FRAME;
  txHeader.DataLength = FDCAN_DLC_BYTES_8;
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txHeader.MessageMarker = 0;

  for (uint8_t i = 0; i < sizeof(txData); i++)
  {
    txData[i] = 0;
  }
}

static void CAN1_SendCounter(uint8_t *data)
{
  HAL_StatusTypeDef status;

  /* Copy 8 bytes from input parameter to tx buffer */
  for (uint8_t i = 0; i < 8; i++)
  {
    txData[i] = data[i];
  }
  
  status = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData);
  
  if (status == HAL_OK)
  {
    sendCounter++;  /* Increment counter on successful send */
  }
  else
  {
    canErrorCounter++;
  }
}

static void CAN2_Setup(void)
{
  FDCAN_FilterTypeDef filter;

  // SIN interrupciones - solo polling

  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = 0x123;  // Solo acepta ID 0x123
  filter.FilterID2 = 0x7FF;  // Máscara completa - todos los bits deben coincidir

  if (HAL_FDCAN_ConfigFilter(&hfdcan2, &filter) != HAL_OK)
  {
    Error_Handler();
  }

  // Configuración global: RECHAZAR todos los mensajes que no pasen por filtros
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
                                   FDCAN_REJECT,
                                   FDCAN_REJECT,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
  {
    Error_Handler();
  }

  // NO activar interrupciones - usar polling solamente

  txHeader.Identifier = 0x123;
  txHeader.IdType = FDCAN_STANDARD_ID;
  txHeader.TxFrameType = FDCAN_DATA_FRAME;
  txHeader.DataLength = FDCAN_DLC_BYTES_8;
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txHeader.MessageMarker = 0;

  for (uint8_t i = 0; i < sizeof(txData); i++)
  {
    txData[i] = 0;
  }
}

static void CAN2_SendCounter(uint8_t *data)
{
  HAL_StatusTypeDef status;

  /* Copy 8 bytes from input parameter to tx buffer */
  for (uint8_t i = 0; i < 8; i++)
  {
    txData[i] = data[i];
  }
  
  status = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &txHeader, txData);
  
  if (status == HAL_OK)
  {
    sendCounter++;  /* Increment counter on successful send */
  }
  else
  {
    canErrorCounter++;
  }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if ((hfdcan->Instance == FDCAN1) && ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0U))
  {
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
      rxFlag = 1;
      /* Toggle LED to indicate message received */
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
    }
  }
  
  if ((hfdcan->Instance == FDCAN2) && ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0U))
  {
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
      rxFlag = 1;
      /* Toggle LED to indicate message received */
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
    }
  }
}

void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->Instance == FDCAN1)
  {
    canErrorCounter++;
  }
  if (hfdcan->Instance == FDCAN2)
  {
    canErrorCounter++;
  }
}

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  if (hfdcan->Instance == FDCAN1)
  {
    canErrorCounter++;
  }
  if (hfdcan->Instance == FDCAN2)
  {
    canErrorCounter++;
  }
}

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
