/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_BlockBased_TrustZone/Secure/Src/main.c
  * @author  MCD Application Team
  * @brief   Secure main application
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

/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table                             */
#define VTOR_TABLE_NS_START_ADDR  0x08040000UL

#define FLASH_USER_START_ADDR     ADDR_FLASH_PAGE_138                         /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR       (ADDR_FLASH_PAGE_255 + FLASH_PAGE_SIZE - 1) /* End @ of user Flash area */

#define FLASH_BANK_BBSEC_REGION_1 0xFFFFFC00UL   /* Pages 128 to 137 non-secure, pages 138 to 159 secure */
#define FLASH_BANK_BBSEC_REGION_2 0xFFFFFFFFUL   /* Pages 160 to 191 secure */
#define FLASH_BANK_BBSEC_REGION_3 0xFFFFFFFFUL   /* Pages 192 to 223 secure */
#define FLASH_BANK_BBSEC_REGION_4 0xFFFFFFFFUL   /* Pages 224 to 255 secure */

#define DATA_64                   ((uint64_t)0x1234567812345678)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t FirstPage = 0U, NbOfPages = 0U;
uint32_t Address = 0U, PageError = 0U;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

/*Variable used for Block-Based Security Program procedure*/
FLASH_BBAttributesTypeDef BBSecInitStruct;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void NonSecure_Init(void);
static void MX_GTZC_S_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t GetPage(uint32_t Address);

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
  /* SAU/IDAU, FPU and interrupts secure/non-secure allocation setup done */
  /* in SystemInit() based on partition_stm32l552xx.h file's definitions. */
  /* USER CODE BEGIN 1 */

  /* STM32L5xx **SECURE** HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Low Level Initialization
     */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* GTZC initialisation */
  MX_GTZC_S_Init();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* Initialize LED4 */
  BSP_LED_Init(LED4);

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Activate Block-based protection in user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
  BBSecInitStruct.Bank                  = FLASH_BANK_2;
  BBSecInitStruct.BBAttributesType      = FLASH_BB_SEC;
  BBSecInitStruct.BBAttributes_array[0] = FLASH_BANK_BBSEC_REGION_1;
  BBSecInitStruct.BBAttributes_array[1] = FLASH_BANK_BBSEC_REGION_2;
  BBSecInitStruct.BBAttributes_array[2] = FLASH_BANK_BBSEC_REGION_3;
  BBSecInitStruct.BBAttributes_array[3] = FLASH_BANK_BBSEC_REGION_4;
  
  if (HAL_FLASHEx_ConfigBBAttributes(&BBSecInitStruct) != HAL_OK)
  {
    /*
      Error occurred while configuring block-based attributes.
      User can add here some code to deal with this error.
    */
    Error_Handler();
  }

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Get the 1st page to erase */
  FirstPage = GetPage(FLASH_USER_START_ADDR);

  /* Get the number of pages to erase from 1st page */
  NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

  /* Fill EraseInit structure */
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = FLASH_BANK_2;
  EraseInitStruct.Page        = FirstPage;
  EraseInitStruct.NbPages     = NbOfPages;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PageError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    Error_Handler();
  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, DATA_64) == HAL_OK)
    {
      Address = Address + 8U;  /* increment to next double word */
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      Error_Handler();
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* End of Flash programmation by secure code */
  BSP_LED_On(LED4);

  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  HAL_SuspendTick();

  /* Configure as non-secure the pin used for non-secure LED */
  LED5_GPIO_CLK_ENABLE();
  HAL_GPIO_ConfigPinAttributes(LED5_GPIO_PORT, LED5_PIN, GPIO_PIN_NSEC);
 
	/* Leave the GPIO clocks enabled to let non-secure having I/Os control */

  /* USER CODE END 2 */

  /*************** Setup and jump to non-secure *******************************/

  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */
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
  * @brief  Non-secure call function
  *         This function is responsible for Non-secure initialization and switch
  *         to non-secure state
  * @retval None
  */
static void NonSecure_Init(void)
{
  funcptr_NS NonSecure_ResetHandler;

  SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();
}

/**
  * @brief GTZC_S Initialization Function
  * @param None
  * @retval None
  */
static void MX_GTZC_S_Init(void)
{

  /* USER CODE BEGIN GTZC_S_Init 0 */

  /* USER CODE END GTZC_S_Init 0 */

  MPCBB_ConfigTypeDef MPCBB_NonSecureArea_Desc = {0};

  /* USER CODE BEGIN GTZC_S_Init 1 */

  /* USER CODE END GTZC_S_Init 1 */
  MPCBB_NonSecureArea_Desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
  MPCBB_NonSecureArea_Desc.InvertSecureState = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[0] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[1] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[2] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[3] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[4] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[5] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[6] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[7] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[8] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[9] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[10] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[11] =   0xFFFFFFFF;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[12] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[13] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[14] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[15] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[16] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[17] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[18] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[19] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[20] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[21] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[22] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[23] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_LockConfig_array[0] =   0x00000000;
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM1_BASE, &MPCBB_NonSecureArea_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[0] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[1] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[2] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[3] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[4] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[5] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[6] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_SecConfig_array[7] =   0x00000000;
  MPCBB_NonSecureArea_Desc.AttributeConfig.MPCBB_LockConfig_array[0] =   0x00000000;
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM2_BASE, &MPCBB_NonSecureArea_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GTZC_S_Init 2 */

  /* USER CODE END GTZC_S_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
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

  /* Infinite loop */
  while (1)
  {
    /* Error detected. LED4 will blink with 1s period */
    BSP_LED_Toggle(LED4);
    HAL_Delay(500U);
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
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
