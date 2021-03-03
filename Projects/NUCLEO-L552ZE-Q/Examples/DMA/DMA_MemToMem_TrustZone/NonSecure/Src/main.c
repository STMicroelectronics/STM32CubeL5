/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DMA/DMA_MemToMem_TrutZone/NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   Non-secure main application
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
#include <string.h>     /* for memcmp() */

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
DMA_HandleTypeDef hdma_memtomem_dma1_channel4;
/* USER CODE BEGIN PV */

static const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

static uint32_t aDST_Buffer1[BUFFER_SIZE];   /* Non-secure buffer to receive secure data */
static uint32_t aDST_Buffer2[BUFFER_SIZE];   /* Non-secure buffer copy */

static __IO uint32_t transferCompleteDetected; /* Set to 1 if transfer is correctly completed */
static __IO uint32_t transferErrorDetected; /* Set to 1 if an error transfer is detected */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void NonSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1);
static void NonSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1);
/* Following callabcks are called by secure */
static void SecureNonSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx);
static void NonSecureSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx);
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

  /* STM32L5xx **NON-SECURE** HAL library initialization:
       - Non-secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */

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
  MX_DMA_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize LEDs */
  BSP_LED_Init(LED2);

  /* Step 1 */
  /* Get secure data into non-secure RAM area */
  /* through secure DMA channels via Non-Secure Callable secure service */

  /* Reset transferCompleteDetected to 0, it will be set to 1 if a transfer is correctly completed */
  transferCompleteDetected = 0;

  if (SECURE_DMA_Secure_To_NonSecure((uint32_t *)aDST_Buffer1,
                                     BUFFER_SIZE,
                                     (void *)SecureNonSecureTransferCompleteCallback) == ERROR)
  {
    Error_Handler();
  }

  /* Wait for notification completion */
  while (transferCompleteDetected == 0);

  /* Check copied RAM buffer expected content */
  /* Compare the secure destination verus the source */
  if (memcmp((uint8_t *)aDST_Buffer1,
             (uint8_t *)aSRC_Const_Buffer,
             sizeof(uint32_t) * BUFFER_SIZE) != 0)
  {
    Error_Handler();   /* Infinite loop */
  }

  /* Step 2 */
  /* Copy data into second non-secure RAM area */

  /* Reset transferCompleteDetected to 0, it will be set to 1 if a transfer is correctly completed */
  transferCompleteDetected = 0;
  /* Reset transferErrorDetected to 0, it will be set to 1 if a transfer error is detected */
  transferErrorDetected = 0;

  /* Select Callbacks functions called after Transfer complete and Transfer error */
  HAL_DMA_RegisterCallback(&hdma_memtomem_dma1_channel4, HAL_DMA_XFER_CPLT_CB_ID, NonSecureTransferComplete);
  HAL_DMA_RegisterCallback(&hdma_memtomem_dma1_channel4, HAL_DMA_XFER_ERROR_CB_ID, NonSecureTransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA channel transfer */
  /* Enable DMA TC and TE interrupts */
  if (HAL_DMA_Start_IT(&hdma_memtomem_dma1_channel4,
                       (uint32_t)&aDST_Buffer1,
                       (uint32_t)&aDST_Buffer2,
                       BUFFER_SIZE) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();
  }

  /* Wait for end of DMA transfer */
  while ((transferCompleteDetected == 0) &&
         (transferErrorDetected == 0));

  if (transferErrorDetected == 1)
  {
    Error_Handler();  /* Infinite loop */
  }

  /* Check copied RAM buffer expected content */
  /* Compare the secure destination verus the source */
  if (memcmp((uint8_t *)aDST_Buffer2,
             (uint8_t *)aDST_Buffer1,
             sizeof(uint32_t) * BUFFER_SIZE) != 0)
  {
    Error_Handler();   /* Infinite loop */
  }

  /* Step 3 */
  /* Provide non-secure data to secure */
  /* through secure DMA channels via Non-Secure Callable secure service */

  /* Reset transferCompleteDetected to 0, it will be set to 1 if a transfer is correctly completed */
  transferCompleteDetected = 0;

  if (SECURE_DMA_NonSecure_To_Secure((uint32_t *)aDST_Buffer2,
                                     BUFFER_SIZE,
                                     (void *)NonSecureSecureTransferCompleteCallback) == ERROR)
  {
    Error_Handler();
  }

  /* Wait for notification completion */
  while (transferCompleteDetected == 0);

  /* Step 4 */
  /* Secure data coherency check */
  if (SECURE_DATA_Last_Buffer_Compare() == SUCCESS)
  {
    /* LED2 is on for end of test */
    BSP_LED_On(LED2);
  }
  else
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
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma1_channel4
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* Configure DMA request hdma_memtomem_dma1_channel4 on DMA1_Channel4 */
  hdma_memtomem_dma1_channel4.Instance = DMA1_Channel4;
  hdma_memtomem_dma1_channel4.Init.Request = DMA_REQUEST_MEM2MEM;
  hdma_memtomem_dma1_channel4.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma1_channel4.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma1_channel4.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma1_channel4.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_memtomem_dma1_channel4.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_memtomem_dma1_channel4.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma1_channel4.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_memtomem_dma1_channel4) != HAL_OK)
  {
    Error_Handler( );
  }

  /*  */
  if (HAL_DMA_ConfigChannelAttributes(&hdma_memtomem_dma1_channel4, DMA_CHANNEL_NPRIV) != HAL_OK)
  {
    Error_Handler( );
  }

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief  DMA non-secure transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void NonSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1)
{
  transferCompleteDetected = 1;
}

/**
  * @brief  DMA non-secure transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void NonSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1)
{
  transferErrorDetected = 1;
}

/**
  * @brief  DMA secure to non-secure transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void SecureNonSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx)
{
  transferCompleteDetected = 1;
}

/**
  * @brief  DMA non-secure to secure transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void NonSecureSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx)
{
  transferCompleteDetected = 1;
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
  while (1)
  {
    BSP_LED_Toggle(LED2);
    HAL_Delay(500);
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
