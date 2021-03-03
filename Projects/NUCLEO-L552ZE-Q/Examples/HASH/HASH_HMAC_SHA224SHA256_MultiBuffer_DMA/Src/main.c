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
#define INPUT_TAB_SIZE    ((uint32_t) 261)/* The size of the input data "aInput" */
#define KEY_TAB_SIZE      ((uint32_t) 261)

#define HASHTimeout        0xFF
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
HASH_HandleTypeDef hhash;
DMA_HandleTypeDef hdma_hash_in;

/* USER CODE BEGIN PV */
__ALIGN_BEGIN uint8_t aInput[INPUT_TAB_SIZE] __ALIGN_END =
                        {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x32,0x20,0x2a,0x2a,0x2a};

__ALIGN_BEGIN uint8_t aKey[KEY_TAB_SIZE] __ALIGN_END=
                        {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x32,0x20,0x2a,0x2a,0x2a};

__ALIGN_BEGIN	static uint8_t aSHA224Digest[28] __ALIGN_END;

__ALIGN_BEGIN static uint8_t aExpectSHA224Digest[28] __ALIGN_END = {0x76, 0xcc, 0xdd, 0x1f, 0xde, 0x03, 0x6c, 0xde, 0x39,
                        0xc9, 0xd2, 0x3c, 0xe6, 0xbd, 0xb1, 0x69, 0xff, 0x74,
                        0x3b, 0x89, 0xd1, 0x57, 0x91, 0xbf, 0xba, 0x10, 0x9a, 0x55};

__ALIGN_BEGIN static uint8_t ExpectedHMAC224[28] __ALIGN_END  = {0x6b, 0xd3, 0x13, 0x2e, 0x89, 0xc7, 0x71, 0x5c, 0x9c,
                                                 0xba, 0xee, 0x01, 0x50, 0xd0, 0xb2, 0x5c, 0x57, 0x72,
                                                 0x97, 0xe4, 0x26, 0x40, 0x31, 0xca, 0xe9, 0x65, 0x60, 0x78};

__ALIGN_BEGIN static uint8_t aSHA256Digest[32] __ALIGN_END;

__ALIGN_BEGIN static uint8_t ExpectedHMAC256[32] __ALIGN_END   = {0xc5, 0x01, 0xfc, 0x6c, 0x17, 0x37, 0x78, 0xe6,
                                                  0x97, 0x44, 0xae, 0x06, 0x94, 0x4f, 0x09, 0x4c,
                                                  0x9e, 0xaf, 0x1c, 0x56, 0x2f, 0x58, 0xde, 0xbc,
                                                  0x47, 0x8c, 0xde, 0xa8, 0x5a, 0x42, 0x1f, 0x58};
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
  /*                                                                          */
  /*                Multi-buffer SHA224 digest computation                    */
  /*                                                                          */
  /****************************************************************************/
  /* Set multiple DMA transfer feature.
     MDMAT bit is set when hashing large files where
     multiple DMA transfers are needed. */
  __HAL_HASH_SET_MDMAT();

  /***********************************************/
  /* Start hashing a sequence of several buffers */
  /***********************************************/
  if (HAL_HASHEx_SHA224_Start_DMA(&hhash, aInput, 64) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  if (HAL_HASHEx_SHA224_Start_DMA(&hhash, aInput+64, 64) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  if (HAL_HASHEx_SHA224_Start_DMA(&hhash, aInput+128, 64) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  if (HAL_HASHEx_SHA224_Start_DMA(&hhash, aInput+192, 64) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);


  /************************************************************************/
  /* Before  hashing the last buffer, reset multiple DMA transfer feature */
  /************************************************************************/
  __HAL_HASH_RESET_MDMAT();

  /*******************************/
  /* Last buffer feed to HASH IP */
  /*******************************/
  if (HAL_HASHEx_SHA224_Start_DMA(&hhash, aInput+256, 5) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  /*******************/
  /* Retrieve digest */
  /*******************/
  if (HAL_HASHEx_SHA224_Finish(&hhash, aSHA224Digest, HASHTimeout) != HAL_OK)
  {
    Error_Handler();
  }

  /*********************************************/
  /* Compare computed digest with expected one */
  /*********************************************/
  if (memcmp(aSHA224Digest, aExpectSHA224Digest, sizeof(aExpectSHA224Digest) / sizeof(aExpectSHA224Digest[0])) != 0)
  {
    BSP_LED_On(LED_RED);
  }
  else
  {
    /****************************************************************************/
    /*                                                                          */
    /*            Multi-buffer SHA224 HMAC digest computation                   */
    /*                                                                          */
    /****************************************************************************/

    if (HAL_HASH_DeInit(&hhash) != HAL_OK)
    {
      Error_Handler();
    }
    hhash.Init.DataType = HASH_DATATYPE_8B;
    hhash.Init.pKey = aKey;
    hhash.Init.KeySize = 261;

    if (HAL_HASH_Init(&hhash) != HAL_OK)
    {
      Error_Handler();
    }

    /****************************************************************************/
    /* Start HMAC digest compution in feeding the first input buffer to the IP. */
    /* This achieves step 1 and starts step 2.                                  */
    /* Step 1: inner key feeding to the HASH IP                                 */
    /* Step 2: message feeding to the HASH IP                                   */
    /****************************************************************************/
    if (HAL_HMACEx_SHA224_Step1_2_DMA(&hhash, aInput, 64) != HAL_OK)
    {
      Error_Handler();
    }
    while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

    /*****************************************************************************/
    /* Carry out HMAC step 2 processing in feeding the sequence of input buffers */
    /*****************************************************************************/
    if (HAL_HMACEx_SHA224_Step2_DMA(&hhash, aInput+64, 64) != HAL_OK)
    {
      Error_Handler();
    }
    while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

    if (HAL_HMACEx_SHA224_Step2_DMA(&hhash, aInput+128, 64) != HAL_OK)
    {
      Error_Handler();
    }
    while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

    if (HAL_HMACEx_SHA224_Step2_DMA(&hhash, aInput+192, 64) != HAL_OK)
    {
      Error_Handler();
    }
    while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

    /****************************************************************************/
    /* Feed the last buffer and wraps up HMAC step 3 processing.                */
    /* Step 3: outer key (same as the inner key) feeding to the HASH IP         */
    /****************************************************************************/
    if (HAL_HMACEx_SHA224_Step2_3_DMA(&hhash, aInput+256, 5) != HAL_OK)
    {
      Error_Handler();
    }
    while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

    /*******************/
    /* Retrieve digest */
    /*******************/
    if (HAL_HASHEx_SHA224_Finish(&hhash, aSHA224Digest, HASHTimeout) != HAL_OK)
    {
      Error_Handler();
    }

    /*********************************************/
    /* Compare computed digest with expected one */
    /*********************************************/
    if(memcmp(aSHA224Digest, ExpectedHMAC224,  sizeof(aExpectSHA224Digest) / sizeof(aExpectSHA224Digest[0])) != 0)
    {
      BSP_LED_On(LED_RED);
    }
    else
    {
      BSP_LED_On(LED_GREEN);
    }
  }

  /****************************************************************************/
  /*                                                                          */
  /*            Multi-buffer SHA256 HMAC digest computation                   */
  /*                                                                          */
  /****************************************************************************/
  if (HAL_HASH_DeInit(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  hhash.Init.DataType = HASH_DATATYPE_8B;
  hhash.Init.pKey = aKey;
  hhash.Init.KeySize = 261;

  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }

  /*****************************************************************************/
  /* Start HMAC digest compution in feeding the first input buffers to the IP. */
  /* This achieves step 1 and starts step 2.                                   */
  /*****************************************************************************/
  if (HAL_HMACEx_SHA256_Step1_2_DMA(&hhash, aInput, 128) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  /******************************************************************************/
  /* There are only two buffers to hash, so wrap-up the step 2 computation in   */
  /* feeding the second and last buffer then carry out step 3.                  */
  /* When the file to hash is made of only two buffers (case illustrated here), */
  /* there is no need to call HAL_HMACEx_SHAxxx_Step2_DMA() API.                */
  /******************************************************************************/
  while (HAL_DMA_GetState(hhash.hdmain) != HAL_DMA_STATE_READY);
  if (HAL_HMACEx_SHA256_Step2_3_DMA(&hhash, aInput+128, 133) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  /*******************/
  /* Retrieve digest */
  /*******************/
  if (HAL_HASHEx_SHA256_Finish(&hhash, aSHA256Digest, HASHTimeout) != HAL_OK)
  {
    Error_Handler();
  }

  if(memcmp(aSHA256Digest, ExpectedHMAC256, sizeof(aSHA256Digest) / sizeof(aSHA256Digest[0])) != 0)
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
