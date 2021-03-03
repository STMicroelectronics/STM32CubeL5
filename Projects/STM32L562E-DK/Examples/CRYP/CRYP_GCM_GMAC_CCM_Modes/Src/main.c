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
#define AES_PAYLOAD_SIZE_GCM            4  /* in 32-bit long words */
#define AES_PAYLOAD_SIZE_CCM            8  /* in 32-bit long words */
#define AES_ACTUAL_PAYLOAD_SIZE_CCM    17  /* Plaintext_CCM[] message is actually 17-byte long */

#define AES_HEADER_SIZE_GMAC           17  /* in 32-bit long words */
#define AES_HEADER_SIZE_CCM             4  /* in 32-bit long words */

#define TAG_SIZE                        4  /* in 32-bit long words */

#define KEY_256_SIZE                    8  /* in 32-bit long words */
#define IV_SIZE                         4  /* in 32-bit long words */

#define TIMEOUT_VALUE                0xFF


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRYP_HandleTypeDef hcryp;
__ALIGN_BEGIN static const uint32_t pKeyAES[4] __ALIGN_END = {
                            0xC939CC13,0x397C1D37,0xDE6AE0E1,0xCB7C423C};
__ALIGN_BEGIN static const uint32_t pInitVectAES[4] __ALIGN_END = {
                            0xB3D8CC01,0x7CBB89B3,0x9E0F67E2,0x00000002};
__ALIGN_BEGIN static const uint32_t HeaderAES[4] __ALIGN_END = {
                            0x24825602,0xBD12A984,0xE0092D3E,0x448EDA5F};

/* USER CODE BEGIN PV */

uint32_t AES256Key[KEY_256_SIZE] = {
                            0x691D3EE9, 0x09D7F541, 0x67FD1CA0, 0xB5D76908,
                            0x1F2BDE1A, 0xEE655FDB, 0xAB80BD52, 0x95AE6BE7};

uint32_t AES256Key_CCM[KEY_256_SIZE] = {
                            0xD346D11A, 0x7117CE04, 0x08089570, 0x7778287C,
                            0x40F5F473, 0xA9A8F2B1, 0x570F6137, 0x4669751A};

/* Initialization vector */
uint32_t AESIV_GMAC[IV_SIZE]    = {
                            0xF0761E8D, 0xCD3D0001, 0x76D457ED, 0x00000002};

/* AES_GCM ===================================================================*/

/* Plaintext */
uint32_t aPlaintext[AES_PAYLOAD_SIZE_GCM] = {
                            0xc3b3c41f, 0x113a31b7, 0x3d9a5cd4, 0x32103069};

/* Expected text: Encrypted Data with AES 128 Mode GCM */
uint32_t Cyphertext[AES_PAYLOAD_SIZE_GCM] = {
                            0x93FE7D9E, 0x9BFD1034, 0x8A5606E5, 0xCAFA7354};

uint32_t ExpectedTAG_GCM[TAG_SIZE] = {
                            0x0032A1DC, 0x85F1C978, 0x6925A2E7, 0x1D8272DD};

/* AES_GMAC ==================================================================*/

uint32_t HeaderMessage_GMAC[AES_HEADER_SIZE_GMAC] = {
                             0xE20106D7, 0xCD0DF076, 0x1E8DCD3D, 0x88E54000,
                             0x76D457ED, 0x08000F10, 0x11121314, 0x15161718,
                             0x191A1B1C, 0x1D1E1F20, 0x21222324, 0x25262728,
                             0x292A2B2C, 0x2D2E2F30, 0x31323334, 0x35363738,
                             0x393A0003 };

uint32_t ExpectedTAG_GMAC[TAG_SIZE] = {
                            0x35217C77, 0x4BBC31B6, 0x3166BCF9, 0xD4ABED07};

/* AES_CCM ===================================================================*/
uint32_t BlockB0[IV_SIZE] = {0x7A05C8CC, 0x7732B3B4, 0x7F08AF1D, 0xAF000011};
uint32_t BlockB1[AES_HEADER_SIZE_CCM] = {
                             0x00073421, 0x5F032567, 0x0B000000, 0x00000000};

/* Actual payload is 17-byte long */
uint32_t Plaintext_CCM[AES_PAYLOAD_SIZE_CCM] = {
                             0xBBD88334, 0x000075F6, 0xF4E89F9D, 0xDA50F5EA,
                             0xB1000000, 0x00000000, 0x00000000, 0x00000000};

uint32_t Cyphertext_CCM[AES_PAYLOAD_SIZE_CCM] = {
                             0xA7B7653C, 0x5D600AF3, 0x9CA0DB48, 0x0F4F5CCE,
                             0x99000000, 0x00000000, 0x00000000, 0x00000000};

uint32_t ExpectedTAG_CCM[TAG_SIZE] = {
                             0x352C36D3, 0x935B8894, 0x0426A004, 0x3BBAB7EE};


/*============================================================================*/

/* Used to store the encrypted text */
/* (AES_PAYLOAD_SIZE_CCM chosen since it is the largest) */
uint32_t aEncryptedtext[AES_PAYLOAD_SIZE_CCM] ={0};

/* Used to store the computed tag */
uint32_t TAG[TAG_SIZE]={0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_AES_Init(void);
/* USER CODE BEGIN PFP */
static void data_cmp(uint32_t *EncryptedText, uint32_t *RefText, uint8_t Size);
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
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_AES_Init();
  /* USER CODE BEGIN 2 */


  /******************************************************************************/
  /*                             AES mode GCM                                   */
  /******************************************************************************/

  /*****************  AES 128   ****************/
  /* Start encrypting aPlaintext, the ciphered data are available in aEncryptedtext */
  if (HAL_CRYP_Encrypt_IT(&hcryp, aPlaintext, AES_PAYLOAD_SIZE_GCM, aEncryptedtext) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY);

  /* Compare the encrypted text with the expected one *************************/
  data_cmp(aEncryptedtext, Cyphertext, AES_PAYLOAD_SIZE_GCM);

  /* Compute the authentication TAG */
  if (HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, TAG, TIMEOUT_VALUE) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /* Compare the derived tag with the expected one *************************/
  data_cmp(TAG, ExpectedTAG_GCM, TAG_SIZE);


  /******************************************************************************/
  /*                             AES mode GMAC                                  */
  /******************************************************************************/

  /*****************  AES 256   ****************/
  hcryp.Init.KeySize       = CRYP_KEYSIZE_256B;
  hcryp.Init.pKey          = (uint32_t *)AES256Key;
  hcryp.Init.pInitVect     = (uint32_t *)AESIV_GMAC;
  hcryp.Init.Algorithm     = CRYP_AES_GCM_GMAC;
  hcryp.Init.Header        = (uint32_t *)HeaderMessage_GMAC;
  hcryp.Init.HeaderSize    = AES_HEADER_SIZE_GMAC;

  /* Set the CRYP parameters */
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* GMAC mode, so no payload processed */
  if (HAL_CRYP_Encrypt_IT(&hcryp, NULL, 0, NULL)!= HAL_OK)
  {
    Error_Handler();
  }
  /* Wait for processing to be done */
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY);

  /* Compute the authentication TAG */
  if (HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, TAG, TIMEOUT_VALUE) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /* Compare the derived tag with the expected one *************************/
  data_cmp(TAG, ExpectedTAG_GMAC, TAG_SIZE);


  /******************************************************************************/
  /*                              AES mode CCM                                  */
  /******************************************************************************/

  /*****************  AES 256   ****************/
  hcryp.Init.KeySize       = CRYP_KEYSIZE_256B;
  hcryp.Init.pKey          = (uint32_t *)AES256Key_CCM;
  hcryp.Init.Algorithm     = CRYP_AES_CCM;
  hcryp.Init.B0            = (uint32_t *)BlockB0;
  hcryp.Init.Header        = (uint32_t *)BlockB1;
  hcryp.Init.HeaderSize    = AES_HEADER_SIZE_CCM;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE; /* for payload padding on a byte-basis */

  /* Set the CRYP parameters */
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  if (HAL_CRYP_Encrypt_IT(&hcryp, Plaintext_CCM, AES_ACTUAL_PAYLOAD_SIZE_CCM, aEncryptedtext) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY);

  /* Compare the encrypted text with the expected one *************************/
  aEncryptedtext[(AES_ACTUAL_PAYLOAD_SIZE_CCM/4)] = aEncryptedtext[(AES_ACTUAL_PAYLOAD_SIZE_CCM/4)]&0xFF000000; /* to compare only first byte of last word */
  data_cmp(aEncryptedtext, Cyphertext_CCM, ((AES_ACTUAL_PAYLOAD_SIZE_CCM/4)+1));

  /* Compute the authentication TAG */
  if (HAL_CRYPEx_AESCCM_GenerateAuthTAG(&hcryp, TAG, TIMEOUT_VALUE) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /* Compare the derived tag with the expected one *************************/
  data_cmp(TAG, ExpectedTAG_CCM, TAG_SIZE);

  /* No issue detected if code reaches this point */
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
  hcryp.Init.DataType = CRYP_DATATYPE_32B;
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey = (uint32_t *)pKeyAES;
  hcryp.Init.pInitVect = (uint32_t *)pInitVectAES;
  hcryp.Init.Algorithm = CRYP_AES_GCM_GMAC;
  hcryp.Init.Header = (uint32_t *)HeaderAES;
  hcryp.Init.HeaderSize = 4;
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

/* USER CODE BEGIN 4 */
/**
  * @brief  buffer data comparison
  * @param
  * @retval None
  */
static void data_cmp(uint32_t *EncryptedText, uint32_t *RefText, uint8_t Size)
{
  /*  Before starting a new process, you need to check the current state of the peripheral;
      if it’s busy you need to wait for the end of current transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      process, but application may perform other tasks while transfer operation
      is ongoing. */
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY)
  {
  }

  /*##-3- Check the encrypted text with the expected one #####################*/
  if(memcmp(EncryptedText, RefText, 4*Size) != 0)
  {
    Error_Handler();
  }
  else
  {
    /* Right encryption */
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
