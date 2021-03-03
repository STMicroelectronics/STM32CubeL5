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
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* The size of the plaintext in Byte */
#define PLAINTEXT_SIZE          ((uint32_t)16)
#define AES_TEXT_SIZE           ((uint32_t)16)

#define ECB               1
#define CBC               2
#define CTR               3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRYP_HandleTypeDef hcryp;
__ALIGN_BEGIN static const uint32_t pKeyAES[4] __ALIGN_END = {
                            0x2B7E1516,0x28AED2A6,0xABF71588,0x09CF4F3C};
DMA_HandleTypeDef hdma_aes_in;
DMA_HandleTypeDef hdma_aes_out;

/* USER CODE BEGIN PV */
/* Plaintext */
__ALIGN_BEGIN uint8_t aPlaintext[4*PLAINTEXT_SIZE] __ALIGN_END =
														               {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
                                            0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                                            0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
                                            0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                                            0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
                                            0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                                            0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
                                            0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
                                         };

/* Cyphertext */
__ALIGN_BEGIN uint8_t aCyphertext[4*AES_TEXT_SIZE] __ALIGN_END =     
																				   {0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
                                            0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
                                            0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
                                            0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
                                            0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
                                            0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
                                            0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09,
                                            0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
                                           };

/* Expected text: Encrypted Data with AES 128 Mode ECB */
__ALIGN_BEGIN uint8_t aEncryptedtextExpected[4*AES_TEXT_SIZE] __ALIGN_END =
   																					      {0x3A, 0xD7, 0x7B, 0xB4, 0x0D, 0x7A,
                                                   0x36, 0x60, 0xA8, 0x9E, 0xCA, 0xF3,
                                                   0x24, 0x66, 0xEF, 0x97, 0xF5, 0xD3,
                                                   0xD5, 0x85, 0x03, 0xB9, 0x69, 0x9D,
                                                   0xE7, 0x85, 0x89, 0x5A, 0x96, 0xFD,
                                                   0xBA, 0xAF, 0x43, 0xB1, 0xCD, 0x7F,
                                                   0x59, 0x8E, 0xCE, 0x23, 0x88, 0x1B,
                                                   0x00, 0xE3, 0xED, 0x03, 0x06, 0x88, 
                                                   0x7B, 0x0C, 0x78, 0x5E, 0x27, 0xE8, 
                                                   0xAD, 0x3F, 0x82, 0x23, 0x20, 0x71, 
                                                   0x04, 0x72, 0x5D, 0xD4};

/* Expected text: Decrypted Data with AES 128 Mode ECB */
__ALIGN_BEGIN uint8_t aDecryptedtextExpected[4*PLAINTEXT_SIZE] __ALIGN_END =
																									 {0x6B, 0xC0, 0xBC, 0xE1, 0x2A, 0x45, 
                                                    0x99, 0x91, 0xE1, 0x34, 0x74, 0x1A, 
                                                    0x7F, 0x9E, 0x19, 0x25, 0xD8, 0x64, 
                                                    0x21, 0xFB, 0x9F, 0x1A, 0x1E, 0xDA, 
                                                    0x50, 0x5E, 0xE1, 0x37, 0x57, 0x46, 
                                                    0x97, 0x2C, 0x60, 0x4E, 0xD7, 0xDD, 
                                                    0xF3, 0x2E, 0xFD, 0xFF, 0x70, 0x20, 
                                                    0xD0, 0x23, 0x8B, 0x7C, 0x2A, 0x5D,
                                                    0x85, 0x21, 0xF2, 0xFD, 0x3C, 0x8E, 
                                                    0xEF, 0x2C, 0xDC, 0x3D, 0xA7, 0xE5, 
                                                    0xC4, 0x4E, 0xA2, 0x06};

/* Used for storing the encrypted text */
__ALIGN_BEGIN uint8_t aEncryptedText[4*AES_TEXT_SIZE] __ALIGN_END = {0};

/* Used for storing the decrypted text */
__ALIGN_BEGIN uint8_t aDecryptedText[4*PLAINTEXT_SIZE] __ALIGN_END = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_DMA_Init(void);
static void MX_AES_Init(void);
/* USER CODE BEGIN PFP */
static void Display_PlainData(uint32_t datalength);
static void Display_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength);
static void Display_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength);
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
  COM_InitTypeDef COM_Init;
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
  BSP_LED_Init(LED_RED);

  /* Configure COM port */
  COM_Init.BaudRate   = 115200;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.StopBits   = COM_STOPBITS_1;
  COM_Init.Parity     = COM_PARITY_NONE;
  COM_Init.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &COM_Init) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if (BSP_COM_SelectLogPort(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_DMA_Init();
  MX_AES_Init();
  /* USER CODE BEGIN 2 */
  /*#######################################################################*/
  /*                                                                       */
  /*##-         DMA-based AES 128 ECB encryption              #############*/
  /*                                                                       */
  /*#######################################################################*/

  /* Display Plain Data*/
  Display_PlainData(4*PLAINTEXT_SIZE);

  if (HAL_CRYP_Encrypt_DMA(&hcryp, (uint32_t *)aPlaintext, PLAINTEXT_SIZE, (uint32_t *)aEncryptedText) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler(); 
  }

  /*  Before starting a new process, the current state of the peripheral is checked; 
      as long as the state is not set back to READY, no new ciphering processing
      can be started.
      For simplicity's sake, this example is just waiting till the end of the 
      process, but application may perform other tasks while cihering operation
      is ongoing. */ 
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY)
  {
  }

  /* Display encrypted Data */
  Display_EncryptedData(ECB, 128, 4*AES_TEXT_SIZE);

  /*##- Compare the encrypted text with the expected one #####################*/
  if(memcmp(aEncryptedText, aEncryptedtextExpected, 4*PLAINTEXT_SIZE) != 0)
  {
    Error_Handler(); 
  }
  else
  {
    /* Correct encryption */
  }

  /*#######################################################################*/
  /*                                                                       */
  /*##-         DMA-based AES 128 ECB decryption              #############*/
  /*                                                                       */   
  /*#######################################################################*/  

  if (HAL_CRYP_Decrypt_DMA(&hcryp, (uint32_t *)aCyphertext, PLAINTEXT_SIZE, (uint32_t *)aDecryptedText) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler(); 
  }

  /*  Before starting a new process, the current state of the peripheral is checked; 
      as long as the state is not set back to READY, no new ciphering processing
      can be started.
      For simplicity's sake, this example is just waiting till the end of the 
      process, but application may perform other tasks while cihering operation
      is ongoing. */ 
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY)
  {
  }

  /* Display decrypted Data */
  Display_DecryptedData(ECB, 128, 4*PLAINTEXT_SIZE);

  /*##- Compare the decrypted text with the expected one #####################*/
  if(memcmp(aDecryptedText, aDecryptedtextExpected, 4*PLAINTEXT_SIZE) != 0)
  {
    Error_Handler(); 
  }
  else
  {
    /* Correct decryption */
  }

  printf("======================================================\n");
  printf("\n\r DMA-based AES 128 ECB encryption/decryption done.\n");
  printf("\n\r No issue detected.\n");

  /* Turn LED_GREEN on */
  BSP_LED_On(LED_GREEN);
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
  * @brief AES Initialization Function
  * @param None
  * @retval None
  */
static void MX_AES_Init(void)
{

  /* USER CODE BEGIN AES_Init 0 */

  /* USER CODE END AES_Init 0 */

  /* USER CODE BEGIN AES_Init 1 */

  /* USER CODE END AES_Init 1 */
  hcryp.Instance = AES;
  hcryp.Init.DataType = CRYP_DATATYPE_8B;
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey = (uint32_t *)pKeyAES;
  hcryp.Init.Algorithm = CRYP_AES_ECB;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN AES_Init 2 */

  /* USER CODE END AES_Init 2 */

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
  /* DMA2_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
  /* DMA2_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Display Plain Data
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_PlainData(uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;

  printf("\n\r =============================================================\n\r");
  printf(" ================== Crypt Using HW Cryp  =====================\n\r");
  printf(" =============================================================\n\r");
  printf(" ---------------------------------------\n\r");
  printf(" Plain Data :\n\r");
  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", aPlaintext[BufferCounter]);
    count++;

    if (count == 16)
    {
      count = 0;
      printf("  Block %lu \n\r", (unsigned long)(BufferCounter / 16));
    }
  }
}

/**
  * @brief  Display Encrypted Data
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;

  printf("\n\r =======================================\n\r");
  printf(" Encrypted Data with AES %d  Mode  ", keysize);

  if (mode == ECB)
  {
    printf("ECB\n\r");
  }
  else if (mode == CBC)
  {
    printf("CBC\n\r");
  }
  else /* if(mode == CTR)*/
  {
    printf("CTR\n\r");
  }

  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", aEncryptedText[BufferCounter]);

    count++;
    if (count == 16)
    {
      count = 0;
      printf("  Block %lu \n\r", (unsigned long)(BufferCounter / 16));
    }
  }
}

/**
  * @brief  Display Decrypted Data
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;

  printf("\n\r =======================================\n\r");
  printf(" Decrypted Data with AES %d  Mode  ", keysize);

  if (mode == ECB)
  {
    printf("ECB\n\r");
  }
  else if (mode == CBC)
  {
    printf("CBC\n\r");
  }
  else /* if(mode == CTR)*/
  {
    printf("CTR\n\r");
  }

  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", aDecryptedText[BufferCounter]);
    count++;

    if (count == 16)
    {
      count = 0;
      printf("  Block %lu \n\r", (unsigned long)(BufferCounter / 16));
    }
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
  printf("\n\r Error Detected...\n ");  

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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
