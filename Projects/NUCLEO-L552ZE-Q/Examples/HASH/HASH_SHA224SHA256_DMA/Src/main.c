/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
HASH_HandleTypeDef hhash;
DMA_HandleTypeDef hdma_hash_in;

/* USER CODE BEGIN PV */
__ALIGN_BEGIN const uint8_t aInput[] __ALIGN_END = "The STM32 L5 series is the result of a perfect symbiosis of the real-time control capabilities of an MCU and the signal processing performance of a DSP, and thus complements the STM32 portfolio with a new class of devices, digital signal controllers (DSC).";

__ALIGN_BEGIN static uint8_t aSHA224Digest[28] __ALIGN_END;
__ALIGN_BEGIN static uint8_t aExpectSHA224Digest[28] __ALIGN_END = {0x6a, 0xfb, 0xfa, 0x7a, 0x56, 0x37, 0x25, 0x34,
                                                                    0xce, 0x4a, 0x25, 0xf1, 0x02, 0xd0, 0x69, 0x37,
                                                                    0x7d, 0x53, 0x4b, 0x00, 0xd5, 0x92, 0x7e, 0x7a,
                                                                    0xf4, 0x36, 0x5e, 0x22
                                                                    };

__ALIGN_BEGIN static uint8_t aSHA256Digest[32] __ALIGN_END;
__ALIGN_BEGIN static uint8_t aExpectSHA256Digest[32] __ALIGN_END = {0x00, 0xc8, 0x90, 0xec, 0x15, 0x30, 0xb7, 0xb5, 
                                                                    0xda, 0xea, 0x53, 0xe0, 0xb5, 0x98, 0x3b, 0x88, 
                                                                    0x3e, 0x3d, 0xa7, 0x34, 0x28, 0xc3, 0x6d, 0x89,
                                                                    0xf0, 0xbc, 0xb5, 0xca, 0xc2, 0xcd, 0x1e, 0x09
                                                                    };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_ICACHE_Init(void);
static void MX_HASH_Init(void);
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
  /* Configure LEDs */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_ICACHE_Init();
  MX_HASH_Init();
  /* USER CODE BEGIN 2 */
  /****************************************************************************/
  /****************************** SHA224 **************************************/
  /****************************************************************************/

  /* Start HASH computation using DMA transfer */
  if (HAL_HASHEx_SHA224_Start_DMA(&hhash, (uint8_t*)aInput, strlen((char const*)aInput)) != HAL_OK)
  {
    Error_Handler();
  }
  /* Wait for DMA transfer to complete */ 
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  /* Get the computed digest value */
  if(HAL_HASHEx_SHA224_Finish(&hhash, aSHA224Digest, 0xFF) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare computed digest with expected one */
  if(memcmp(aSHA224Digest, aExpectSHA224Digest, sizeof(aExpectSHA224Digest)/sizeof(aExpectSHA224Digest[0])) != 0)
  {
    BSP_LED_On(LED_RED);
  }
  else
  {
    BSP_LED_On(LED_GREEN);
  }

  /****************************************************************************/
  /***************************** SHA256 ***************************************/
  /****************************************************************************/
  if(HAL_HASH_DeInit(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  hhash.Init.DataType = HASH_DATATYPE_8B;
  
  if(HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* Start HASH computation using DMA transfer */
  if (HAL_HASHEx_SHA256_Start_DMA(&hhash, (uint8_t*)aInput, strlen((char const*)aInput)) != HAL_OK)
  {
    Error_Handler();
  }
  /* Wait for DMA transfer to complete */ 
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  /* Get the computed digest value */
  if (HAL_HASHEx_SHA256_Finish(&hhash, aSHA256Digest, 0xFF) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Compare computed digest with expected one */
  if(memcmp(aSHA256Digest, aExpectSHA256Digest, sizeof(aExpectSHA256Digest)/sizeof(aExpectSHA256Digest[0])) != 0)
  {
    Error_Handler();
  }
  else
  {
    BSP_LED_On(LED_BLUE);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief HASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_HASH_Init(void)
{

  /* USER CODE BEGIN HASH_Init 0 */

  /* USER CODE END HASH_Init 0 */

  /* USER CODE BEGIN HASH_Init 1 */

  /* USER CODE END HASH_Init 1 */
  hhash.Init.DataType = HASH_DATATYPE_8B;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HASH_Init 2 */

  /* USER CODE END HASH_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */
  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel7_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel7_IRQn);

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
  /* Turn LED_RED on */
  BSP_LED_On(LED_RED);
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
