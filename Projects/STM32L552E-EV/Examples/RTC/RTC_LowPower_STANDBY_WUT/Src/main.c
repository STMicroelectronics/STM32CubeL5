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

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

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
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON_RTC_ONLY;
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

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Main program NOT GENERATED BY CUBEMX
  * @param  None
  * @retval None
  */
int main(void)
{
  RTC_TimeTypeDef Time;
  RTC_DateTypeDef Date;

  /* STM32L5xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure LED_GREEN */
  BSP_LED_Init(LED_GREEN);

  /* Configure the system clock to 110 MHz */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_GPIO_Init();

  /* Uncomment to be able to debug after wake-up from Standby. Consuption will be increased */
  //HAL_DBGMCU_EnableDBGStandbyMode();

  /* Uncomment to freeze the RTC during debug */
  //__HAL_DBGMCU_FREEZE_RTC();

  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) == RESET)
  {
    /**** The system was not resumed from StandBy mode ****/
    /* Initialise the RTC from scratch thanks to the reset of the back up domain in the HAL_RTC_MspInit function */
    MX_RTC_Init();

    /* Set Calendar Ultra-Low power mode */
    if (HAL_RTCEx_SetLowPowerCalib(&hrtc, RTC_LPCAL_SET) != HAL_OK)
    {
      Error_Handler();
    }

    /* Use a backup register to save the start time a the beginning of the test */
    HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, Time.Seconds);

    /* Configure Wake up timer to wake up the system every 3 seconds with auto realod. Event instead of Interrupt */
    uint32_t wut_value = 3u * 32768u / 8u;
    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, wut_value, RTC_WAKEUPCLOCK_RTCCLK_DIV8, wut_value) != HAL_OK)
    {
      Error_Handler();
    }
  }
  else
  {
    /**** The system was resumed from StandBy mode ****/
    /* RTC is already running, msp init will execute the minimum operation */
    /* Enable RTC APB clock  */
    __HAL_RCC_RTCAPB_CLK_ENABLE();

    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

    /* Use a backup register to count the number of Wakeup */
    uint32_t number_of_wakeup = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) + 1;
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, number_of_wakeup);
    uint32_t start_time = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);

    /* Check that the current time is ok after 5 wake-up */
    if (number_of_wakeup == 5)
    {
      /* Wait for RTC synchro */
      __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
      if (HAL_RTC_WaitForSynchro(&hrtc) != HAL_OK)
      {
        Error_Handler();
      }
      __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);

      /* Get the RTC calendar time */
      HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
      HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN);

        /* 15 seconds = 5 wakeup after 3 seconds */
      if (((Time.Seconds - start_time) == 15) || ((Time.Seconds + 60 - start_time) == 15))
      {
        /* Clear Backup reg */
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0);

        /* the test is OK */
        BSP_LED_On(LED_GREEN);
        while(1);
      }
      else
      {
          Error_Handler();
      }
    }
    else
    {
      /* Toggle the LED_GREEN to indicate a wakeup from STANDBY */
      for (uint32_t i = 0; i < 4; i++)
      {
        BSP_LED_Toggle(LED_GREEN);
        HAL_Delay(100);
      }
    }
  }

  /* Enter the Standby mode */
  HAL_PWR_EnterSTANDBYMode();

  /* This code will be never reached */
  while (1)
  {
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
  while(1)
  {
    /* Toggle the LED_GREEN */
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(100);
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
