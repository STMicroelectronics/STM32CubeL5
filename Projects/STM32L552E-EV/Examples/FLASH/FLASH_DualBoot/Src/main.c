/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_DualBoot/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to boot from bank1 or bank2
  *          of the STM32L5xx FLASH.
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
const LCD_UTILS_Drv_t LCD_Driver =
{
  BSP_LCD_DrawBitmap,
  BSP_LCD_FillRGBRect,
  BSP_LCD_DrawHLine,
  BSP_LCD_DrawVLine,
  BSP_LCD_FillRect,
  BSP_LCD_ReadPixel,
  BSP_LCD_WritePixel,
  BSP_LCD_GetXSize,
  BSP_LCD_GetYSize,
  NULL,
  BSP_LCD_GetFormat
};

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
FLASH_OBProgramInitTypeDef    OBInit;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void Display_ExampleDescription(void);

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
  /* STM32L5xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* Initialize LED4 and LED5 */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Configure BUTTON_WAKEUP */
  BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);

  /* Display messages on LCD */
  Display_ExampleDescription();

  /* Turn on LEDs */
  BSP_LED_On(LED4);
  BSP_LED_On(LED5);

  /*--- If WKUP push-button is pushed, Set or reset SWAP_BANK option bit to enable or disable
  boot from Bank2 (active after next reset), w/ Boot pins set in Boot from Flash
  memory position ---*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Wait for WKUP push-button is released */
    if (BSP_PB_GetState(BUTTON_WAKEUP) == SET)
    {
      while (BSP_PB_GetState(BUTTON_WAKEUP) == SET);

      /* Allow Access to Flash control registers and user Flash */
      HAL_FLASH_Unlock();

      /* Allow Access to option bytes sector */
      HAL_FLASH_OB_Unlock();

      /* Get the Dual boot configuration status */
      HAL_FLASHEx_OBGetConfig(&OBInit);

      /* Enable/Disable dual boot feature */
      OBInit.OptionType = OPTIONBYTE_USER;
      OBInit.USERType   = OB_USER_SWAP_BANK;

      if (((OBInit.USERConfig) & (OB_SWAP_BANK_ENABLE)) == OB_SWAP_BANK_ENABLE)
      {
        OBInit.USERConfig = OB_SWAP_BANK_DISABLE;
      }
      else
      {
        OBInit.USERConfig = OB_SWAP_BANK_ENABLE;
      }

      if(HAL_FLASHEx_OBProgram (&OBInit) != HAL_OK)
      {
        /*
        Error occurred while setting option bytes configuration.
        User can add here some code to deal with this error.
        To know the code error, user can call function 'HAL_FLASH_GetError()'
        */
        /* Infinite loop */
        while (1)
        {
          /* Make LED4 and LED5 blink (1s on, 1s off) to indicate error */
          BSP_LED_Toggle(LED4);
          BSP_LED_Toggle(LED5);
          HAL_Delay(1000);
        }
      }

      /* Start the Option Bytes programming process */
      if (HAL_FLASH_OB_Launch() != HAL_OK)
      {
        /*
        Error occurred while reloading option bytes configuration.
        User can add here some code to deal with this error.
        To know the code error, user can call function 'HAL_FLASH_GetError()'
        */
        /* Infinite loop */
        while (1)
        {
          /* Make LED4 and LED5 blink (1s on, 1s off) to indicate error */
          BSP_LED_Toggle(LED4);
          BSP_LED_Toggle(LED5);
          HAL_Delay(1000);
        }
      }

      /* Prevent Access to option bytes sector */
      HAL_FLASH_OB_Lock();

      /* Disable the Flash option control register access (recommended to protect
      the option Bytes against possible unwanted operations) */
      HAL_FLASH_Lock();

    }
    else
    {
#ifdef FLASH_BANK1
      /* Toggle LED4 */
      BSP_LED_Toggle(LED4);
#else
      /* Toggle LED5 */
      BSP_LED_Toggle(LED5);
#endif

      /* Insert 50 ms delay */
      HAL_Delay(50);
    }

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
  * @brief  Display main example messages
  * @param  None
  * @retval None
  */
static void Display_ExampleDescription(void)
{
  uint32_t x_size;

  /* Initialize the LCD */
  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  UTIL_LCD_SetDevice(0);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  BSP_LCD_DisplayOn(0);
  BSP_LCD_GetXSize(0, &x_size);
#ifdef FLASH_BANK1
  UTIL_LCD_FillRect(0, 0, x_size, 80, UTIL_LCD_COLOR_BLUE);
#else
  UTIL_LCD_FillRect(0, 0, x_size, 80, UTIL_LCD_COLOR_RED);
#endif

  /* Display LCD messages */
#ifdef FLASH_BANK1
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
#else
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_RED);
#endif
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)MESSAGE1, CENTER_MODE);

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)MESSAGE2, CENTER_MODE);

#ifdef FLASH_BANK1
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
#else
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
#endif
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 95,  (uint8_t *)MESSAGE3, CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 110, (uint8_t *)MESSAGE4, CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)MESSAGE5, CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 155, (uint8_t *)MESSAGE6, CENTER_MODE);
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
  while(1)
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
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
