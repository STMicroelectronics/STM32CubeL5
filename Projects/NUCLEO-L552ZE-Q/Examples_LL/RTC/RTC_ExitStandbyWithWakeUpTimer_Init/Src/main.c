/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This code example shows how to configure the RTC in order to work
  *          with the WUT through the STM32L5xx RTC LL API.
  *          Peripheral initialization done using LL unitary services functions.
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

/* Oscillator time-out values */
#define LSI_TIMEOUT_VALUE          ((uint32_t)100)   /* 100 ms */
#define LSE_TIMEOUT_VALUE          ((uint32_t)5000)  /* 5 s */
#define RTC_TIMEOUT_VALUE          ((uint32_t)1000)  /* 1 s */

/* Defines related to Clock configuration */
/* Uncomment to enable the adequate Clock Source */
//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE

#ifdef RTC_CLOCK_SOURCE_LSI
/* ck_apre=LSIFreq/(ASYNC prediv + 1) = 256Hz with LSIFreq=32 kHz RC */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7C)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0xFF)
/* Value defined for WUT */
#define RTC_WUT_TIME               ((uint32_t)48001)     /* 3 s */
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
/* ck_apre=LSEFreq/(ASYNC prediv + 1) = 256Hz with LSEFreq=32768Hz */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00FF)
/* Value defined for WUT */
#define RTC_WUT_TIME               ((uint32_t)49153)     /* 3 s */
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  
#if (USE_TIMEOUT == 1)
uint32_t Timeout = 0; /* Variable used for Timeout management */
#endif /* USE_TIMEOUT */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
void     Configure_RTC(void);
void     EnterStandbyMode(void);
uint32_t WaitForSynchro_RTC(void);
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
  uint32_t seconds ;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);

  /* System interrupt init*/

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  LL_PWR_DisableUCPDDeadBattery();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Uncomment to be able to debug after wake-up from Standby. Consuption will be increased */
  //__DBGMCU_EnableDBGStandbyMode();

  /* Uncomment to freeze the RTC during debug */
  //__DBGMCU_FREEZE_RTC();  
  /* Check and handle if the system was resumed from StandBy mode */
  if (LL_PWR_IsActiveFlag_SB() != 1)
  {

    /* ##### Run after normal reset ##### */
    /* Configure RTC to use WUT */
    Configure_RTC();
    LL_RTC_DisableWriteProtection(RTC);
      if (WaitForSynchro_RTC() != RTC_ERROR_NONE)
      {
        Error_Handler();
      }
    LL_RTC_EnableWriteProtection(RTC);
    /* Use a backup register to save the start time at the beginning of the test */
    seconds = (uint8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
    LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR0, seconds);
    
  }
  else
  {
    /* ##### Run after standby mode ##### */
    /* Enable RTC APB clock  */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);
    /* Clear Standby flag */
    LL_PWR_ClearFlag_SB();

    /* Use a backup register to count the number of Wakeup */
    LL_PWR_EnableBkUpAccess();
    uint32_t number_of_wakeup = LL_RTC_BKP_GetRegister(RTC, LL_RTC_BKP_DR1) + 1;
    LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR1, number_of_wakeup);
    uint32_t start_time = LL_RTC_BKP_GetRegister(RTC, LL_RTC_BKP_DR0);    
    /* Check that the current time is ok after 5 wake-up */
    if (number_of_wakeup == 5)
    {
      /* Wait for RTC synchro */
      LL_RTC_DisableWriteProtection(RTC);
      if (WaitForSynchro_RTC() != RTC_ERROR_NONE)
      {
        Error_Handler();
      }
      LL_RTC_EnableWriteProtection(RTC);

      /* Get the RTC calendar time */
      seconds = (uint8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));

        /* 15 seconds = 5 wakeup after 3 seconds */
      if (((seconds - start_time) == 15) || ((seconds + 60 - start_time) == 15))
      {
        /* Clear Backup reg */
        LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR0, 0);
        LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR1, 0);

        /* the test is OK */
        LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin);
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
        LL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        LL_mDelay(100);
      }
    }
    
  }  
    
  /* Enable wake-up timer and enter in standby mode */
  EnterStandbyMode();
  /* This code will be never reached */
  Error_Handler();

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
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  LL_RCC_MSI_Enable();

   /* Wait till MSI is ready */
  while(LL_RCC_MSI_IsReady() != 1)
  {
  }

  LL_RCC_MSI_EnableRangeSelection();
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
  LL_RCC_MSI_SetCalibTrimming(0);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 55, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

   /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Insure 1µs transition state at intermediate medium speed clock based on DWT*/
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < 100);

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(110000000);

  LL_SetSystemCoreClock(110000000);
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
  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  /**/
  LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);

  /**/
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
  * Brief   This function configures RTC.
  * Param   None
  * Retval  None
  */
void Configure_RTC(void)
{
  LL_RTC_InitTypeDef RTC_InitStruct = {0};
  /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
  /* To change the source clock of the RTC feature (LSE, LSI), you have to:
     - Enable the power clock
     - Enable write access to configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain
     - Configure the needed RTC clock source */
  LL_PWR_EnableBkUpAccess();
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();  

  /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
#ifdef RTC_CLOCK_SOURCE_LSE
  /* Enable LSE only if disabled.*/
  if (LL_RCC_LSE_IsReady() == 0)
  {   
   
    LL_RCC_LSE_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSE_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSE_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSE activation error */
        Error_Handler();
      }
#endif /* USE_TIMEOUT */
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();
  }
#elif defined(RTC_CLOCK_SOURCE_LSI)
  if (LL_RCC_LSI_IsReady() == 0)
  {    
    
    LL_RCC_LSI_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSI_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSI_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSI activation error */
        Error_Handler();
      }
#endif /* USE_TIMEOUT */
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();
  }

#else
#error "configure clock for RTC"
#endif

  /*##-4- Configure RTC ######################################################*/
/* Enable RTC APB clock  */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);

  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
  RTC_InitStruct.AsynchPrescaler = RTC_ASYNCH_PREDIV;
  RTC_InitStruct.SynchPrescaler = RTC_SYNCH_PREDIV;
  LL_RTC_Init(RTC, &RTC_InitStruct);
  LL_RTC_SetBackupRegisterPrivilege(RTC, LL_RTC_PRIVILEGE_BKUP_ZONE_NONE);
  LL_RTC_SetBackupRegProtection(RTC, LL_RTC_BKP_DR0, LL_RTC_BKP_DR0);

  /* Wait until it is allow to Enable Calibration Low Power */
  while (LL_RTC_IsActiveFlag_RECALP(RTC) != 0)
  {
  }

  /* Enable Calibration Low Power */
  LL_RTC_CAL_LowPower_Enable(RTC);

  /* Disable wake up timer to modify it */
  LL_RTC_WAKEUP_Disable(RTC);

  /* Wait until it is allow to modify wake up reload value */
#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      /* LSI activation error */
      Error_Handler();
    }
#endif /* USE_TIMEOUT */
  }

  /* Configure the Wake up timer to wake up the system every 3 seconds with auto realod.*/    
  LL_RTC_WAKEUP_SetAutoReload(RTC, (uint32_t)(RTC_WUT_TIME | (RTC_WUT_TIME << RTC_WUTR_WUTOCLR_Pos)));

  /* As Wake up auto clear value (WakeUpAutoClr), we configure EXTI as EVENT instead of Interrupt*/
  /* to avoid any RTC software execution after Wake Up (useless IRQ handler)*/
  __LL_RTC_WAKEUPTIMER_EXTI_ENABLE_EVENT();  
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_DIV_2);

  /* Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);

}
/**
  * @brief  Function to configure and enter in STANDBY Mode.
  * @param  None
  * @retval None
  */
void EnterStandbyMode(void)
{
  /* ######## ENABLE WUT #################################################*/
  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  /* Enable wake up counter and wake up interrupt */
  /* Note: Periodic wakeup interrupt should be enabled to exit the device
     from low-power modes.*/
  LL_RTC_EnableIT_WUT(RTC);
  LL_RTC_WAKEUP_Enable(RTC);

  /* Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);

  /* ######## ENTER IN STANDBY MODE ######################################*/
  /** Request to enter STANDBY mode
    * Following procedure describe in STM32L5xx Reference Manual
    * See PWR part, section Low-power modes, Standby mode
    */
  /* No need to Reset Internal Wake up flag since it is cleared by hardware*/
  //LL_RTC_ClearFlag_WUT(RTC);

  /* Set Standby mode */
  LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep();

  /* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM)
  __force_stores();
#endif

  /* Request Wait For Interrupt */
  __WFI();
}
/**
  * @brief  Wait until the RTC Time and Date registers (RTC_TR and RTC_DR) are
  *         synchronized with RTC APB clock.
  * @param  None
  * @retval RTC_ERROR_NONE if no error (RTC_ERROR_TIMEOUT will occur if RTC is
  *         not synchronized)
  */
uint32_t WaitForSynchro_RTC(void)
{
  /* Clear RSF flag */
  LL_RTC_ClearFlag_RS(RTC);

#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  /* Wait the registers to be synchronised */
  while (LL_RTC_IsActiveFlag_RS(RTC) != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      return RTC_ERROR_TIMEOUT;
    }
#endif /* USE_TIMEOUT */
  }
  return RTC_ERROR_NONE;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while (1)
  {
    LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin);
    LL_mDelay(50);
    LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);
    LL_mDelay(50);
    LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin);
    LL_mDelay(50);
    LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);
    LL_mDelay(850);
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
