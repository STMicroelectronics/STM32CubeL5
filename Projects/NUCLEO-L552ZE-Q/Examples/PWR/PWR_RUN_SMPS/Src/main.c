  /**
  ******************************************************************************
  * @file    PWR/PWR_RUN_SMPS/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use SMPS PWR API to enter
  *          SMPS in RUN mode.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup PWR_RUN_SMPS
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

typedef struct
{
uint32_t power_mode_in_use;
uint32_t hclkfreq_in_use;
uint32_t vos_in_use;
} config_in_useTypeDef;

/* Private define ------------------------------------------------------------*/
// #define ADD_EXTRA_DELAY
#define ADD_EXTRA_DELAY_MEASUREMENTS
#define DELAY_FOR_POWER_MEASUREMENTS 20000 /* in ms */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* List of power configuration as per test step */
config_in_useTypeDef config_in_use_example [] =
{
  { PWR_SMPS_HIGH_POWER,                                            /* Step_1 */
    4000000,
    PWR_REGULATOR_VOLTAGE_SCALE2
  },
  { PWR_SMPS_LOW_POWER,                                             /* Step_2 */
    4000000,
    PWR_REGULATOR_VOLTAGE_SCALE2
  },
  { PWR_SMPS_LOW_POWER,                                             /* Step_3 */
    24000000,
    PWR_REGULATOR_VOLTAGE_SCALE2
  },
  { PWR_SMPS_HIGH_POWER,                                            /* Step_4 */
    24000000,
    PWR_REGULATOR_VOLTAGE_SCALE2
  },
  { PWR_SMPS_HIGH_POWER,                                            /* Step_5 */
    24000000,
    PWR_REGULATOR_VOLTAGE_SCALE1
  },
  { PWR_SMPS_HIGH_POWER,                                            /* Step_6 */
    80000000,
    PWR_REGULATOR_VOLTAGE_SCALE1
  },
  { PWR_SMPS_HIGH_POWER,                                             /* Step_7 */
    80000000,
    PWR_REGULATOR_VOLTAGE_SCALE0
  },
  { PWR_SMPS_HIGH_POWER,                                            /* Step_8 */
    24000000,
    PWR_REGULATOR_VOLTAGE_SCALE0
  },
  { PWR_SMPS_HIGH_POWER,                                            /* Step_9 */
    110000000,
    PWR_REGULATOR_VOLTAGE_SCALE0
  },
  { PWR_SMPS_HIGH_POWER,                                           /* Step_10 */
    24000000,
    PWR_REGULATOR_VOLTAGE_SCALE0
  },
  { PWR_SMPS_HIGH_POWER,                                           /* Step_11 */
    24000000,
    PWR_REGULATOR_VOLTAGE_SCALE2
  },
  { PWR_SMPS_LOW_POWER,                                            /* Step_12 */
    24000000,
    PWR_REGULATOR_VOLTAGE_SCALE2
  },
  { /* Unused step */
    PWR_SMPS_BYPASS,                                               /* Step_13 */
    0,
    PWR_REGULATOR_VOLTAGE_SCALE0
  }
};

uint32_t config_check_status;

uint32_t power_mode_in_use;
uint32_t hclkfreq_in_use;
uint32_t vos_in_use;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_110MHz(void);
void SystemClock_80MHz(void);
void SystemClock_24MHz(void);

/**
  * @brief This function provides minimum delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay  specifies the delay time length, in milliseconds.
  * @retval None
  */

#if defined ( __ICCARM__ )
#pragma location="While1Section"
#pragma optimize=no_inline
void HAL_Delay(uint32_t Delay)
#else
__attribute__((section(".While1Section")))
void HAL_Delay(uint32_t Delay) __attribute__((noinline));
void HAL_Delay(uint32_t Delay)
#endif
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a period to guaranty minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait++;
  }

  while((HAL_GetTick() - tickstart) < wait)
  {
  }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
   int main(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

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

  /* Enable Instruction cache in 1-way direct (default 2-ways set associative cache) */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);

  if(HAL_ICACHE_Enable() != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* ------------------ GPIO ------------------ */

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* Upon standby or shutdown exit, the PULLUP is default setting */
  /* Hence same applied here for coherence */

  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();

  __HAL_RCC_PWR_CLK_ENABLE();

  /*  USB Power Delivery */
  HAL_PWREx_DisableUCPDDeadBattery();

  /* -------------------------------------------- */
  /* Step_0                                       */
  /* SMPS: Bypass/HighPower/LowPower : high power */
  /* freq:                          :       4 MHz */
  /* VOS: Range 0 / 1 / 2           :     Range 2 */
  /* -------------------------------------------- */

  /* Set in known state */
  /* VOS range 2        */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* -------------------------------------------- */
  /* Step_1                                       */
  /* SMPS: Bypass/HighPower/LowPower : high power */
  /* freq:                          :       4 MHz */
  /* VOS: Range 0 / 1 / 2           :     Range 2 */
  /* -------------------------------------------- */

  /* SMPS fast start always enabled */
  HAL_PWREx_SMPS_EnableFastStart();

  /* Disable Bypass SMPS */
  /* Allows SMPS DC/DC to provide power supply */
  HAL_PWREx_SMPS_DisableBypassMode();

  HAL_Delay(DELAY_FOR_POWER_MEASUREMENTS);

#ifdef ADD_EXTRA_DELAY
  /* Insert 1000 mseconds delay */
  HAL_Delay(1000);
#endif

 config_check_status = check_config_in_use_example(1);
  if (config_check_status != 0)
  {
    while(1);
  }

  /* -------------------------------------------- */
  /* Step_2                                       */
  /* SMPS: Bypass/HighPower/LowPower :  Low power */
  /* freq:                          :       4 MHz */
  /* VOS: Range 0 / 1 / 2           :     Range 2 */
  /* -------------------------------------------- */

  /* SMPS step down converter in low-power mode */
  if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_LOW_POWER) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

#ifdef ADD_EXTRA_DELAY
  /* Insert 1000 mseconds delay */
  HAL_Delay(1000);
#endif

  HAL_Delay(1);
  config_check_status = check_config_in_use_example(2);
  if (config_check_status != 0)
  {
    while(1);
  }

  /* -------------------------------------------- */
  /* Step_3                                       */
  /* SMPS: Bypass/HighPower/LowPower :  Low power */
  /* freq:                          :      24 MHz */
  /* VOS: Range 0 / 1 / 2           :     Range 2 */
  /* -------------------------------------------- */

  /* Best power SMPS configuration @ 24 MHz */
  SystemClock_24MHz();

#ifdef DELAY_FOR_POWER_MEASUREMENTS
  /* Insert 5000 mseconds delay */
  // HAL_Delay(5000);
  HAL_Delay(DELAY_FOR_POWER_MEASUREMENTS);
#endif

  HAL_Delay(1);
  config_check_status = check_config_in_use_example(3);
  if (config_check_status != 0)
  {
    while(1);
  }

  /* This part is looped  */
  while (1)
  {

  /* -------------------------------------------- */
  /* Step_4                                       */
  /* SMPS: Bypass/HighPower/LowPower : High power */
  /* freq:                          :      24 MHz */
  /* VOS: Range 0 / 1 / 2           :     Range 2 */
  /* -------------------------------------------- */

    /* SMPS step down converter in high-power mode */
    if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_HIGH_POWER) != HAL_OK)
    {
      /* Initialization Error */
      while (1);
    }

#ifdef ADD_EXTRA_DELAY
    /* Insert 1000 mseconds delay */
    HAL_Delay(1000);
#endif

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(4);
    if (config_check_status != 0)
    {
      while (1);
    }

    /* -------------------------------------------- */
    /* Step_5                                       */
    /* SMPS: Bypass/HighPower/LowPower : High power */
    /* freq:                          :      24 MHz */
    /* VOS: Range 0 / 1 / 2           :     Range 1 */
    /* -------------------------------------------- */

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
      /* Initialization Error */
      while(1);
    }

#ifdef ADD_EXTRA_DELAY
    /* Insert 1000 mseconds delay */
    HAL_Delay(1000);
#endif

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(5);
    if (config_check_status != 0)
    {
      while(1);
    }

    /* -------------------------------------------- */
    /* Step_6                                       */
    /* SMPS: Bypass/HighPower/LowPower : High power */
    /* freq:                          :      80 MHz */
    /* VOS: Range 0 / 1 / 2           :     Range 1 */
    /* -------------------------------------------- */

    /* Best power SMPS configuration @ 80 MHz */
    SystemClock_80MHz();

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(6);
    if (config_check_status != 0)
    {
      while(1);
    }

#ifdef ADD_EXTRA_DELAY_MEASUREMENTS
    /* Insert DELAY_FOR_POWER_MEASUREMENTS mseconds delay */
    HAL_Delay(DELAY_FOR_POWER_MEASUREMENTS);
#endif

    /* -------------------------------------------- */
    /* Step_7                                       */
    /* SMPS: Bypass/HighPower/LowPower : High power */
    /* freq:                          :      80 MHz */
    /* VOS: Range 0 / 1 / 2           :     Range 0 */
    /* -------------------------------------------- */

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
    {
      /* Initialization Error */
      while(1);
    }

#ifdef ADD_EXTRA_DELAY
    /* Insert 1000 mseconds delay */
    HAL_Delay(1000);
#endif

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(7);
    if (config_check_status != 0)
    {
      while(1);
    }

    /* -------------------------------------------- */
    /* Step_8                                      */
    /* SMPS: Bypass/HighPower/LowPower : High power */
    /* freq:                          :      24 MHz */
    /* VOS: Range 0 / 1 / 2           :     Range 0 */
    /* -------------------------------------------- */

    SystemClock_24MHz();

#ifdef ADD_EXTRA_DELAY
    /* Insert 1000 mseconds delay */
    HAL_Delay(1000);
#endif

    // HAL_Delay(1);
    config_check_status = check_config_in_use_example(8);
    if (config_check_status != 0)
    {
      while(1);
    }

    /* -------------------------------------------- */
    /* Step_9                                      */
    /* SMPS: Bypass/HighPower/LowPower : High power */
    /* freq:                          :     110 MHz */
    /* VOS: Range 0 / 1 / 2           :     Range 0 */
    /* -------------------------------------------- */

    /* Best power SMPS configuration @ 110 MHz */

    SystemClock_110MHz();

#ifdef ADD_EXTRA_DELAY_MEASUREMENTS
    /* Insert DELAY_FOR_POWER_MEASUREMENTS mseconds delay */
    HAL_Delay(DELAY_FOR_POWER_MEASUREMENTS);
#endif

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(9);
    if (config_check_status != 0)
    {
      while(1);
    }
        
    /* Measurements */
    // while(1);
    
    /* -------------------------------------------- */
    /* Step_10                                      */
    /* SMPS: Bypass/HighPower/LowPower : High power */
    /* freq:                          :     24 MHz  */
    /* VOS: Range 0 / 1 / 2           :     Range 0 */
    /* -------------------------------------------- */

    SystemClock_24MHz();

#ifdef ADD_EXTRA_DELAY
    /* Insert 1000 mseconds delay */
    HAL_Delay(1000);
#endif

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(10);
    if (config_check_status != 0)
    {
      while(1);
    }

    /* -------------------------------------------- */
    /* Step_11                                      */
    /* SMPS: Bypass/HighPower/LowPower : High power */
    /* freq:                          :     24 MHz  */
    /* VOS: Range 0 / 1 / 2           :     Range 2 */
    /* -------------------------------------------- */

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
    {
      /* Initialization Error */
      while(1);
    }

#ifdef ADD_EXTRA_DELAY
    /* Insert 1000 mseconds delay */
    HAL_Delay(1000);
#endif

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(11);
    if (config_check_status != 0)
    {
      while(1);
    }

    /* -------------------------------------------- */
    /* Step_12                                      */
    /* SMPS: Bypass/HighPower/LowPower : Low power */
    /* freq:                          :     24 MHz  */
    /* VOS: Range 0 / 1 / 2           :     Range 2 */
    /* -------------------------------------------- */

    /* Best power SMPS configuration @ 24 MHz */

    /* SMPS step down converter in low-power mode */
    if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_LOW_POWER) != HAL_OK)
    {
      /* Initialization Error */
      while(1);
    }

#ifdef ADD_EXTRA_DELAY_MEASUREMENTS
    /* Insert DELAY_FOR_POWER_MEASUREMENTS mseconds delay */
    HAL_Delay(DELAY_FOR_POWER_MEASUREMENTS);
#endif

    HAL_Delay(1);
    config_check_status = check_config_in_use_example(12);
    if (config_check_status != 0)
    {
      while(1);
    }
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 24000000
  *            HCLK(Hz)                       = 24000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 24000000
  *            PLL_M                          =
  *            PLL_N                          =
  *            PLL_R                          =
  *            PLL_P                          =
  *            PLL_Q                          =
  *            Flash Latency(WS)              =
  * @param  None
  * @retval None
  */

void SystemClock_24MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Select MSI as system clock source and set target Flash latency for 24Mhz */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct,FLASH_LATENCY_2) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Update MSI to 24Mhz (RCC_MSIRANGE_9) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_Q                          = 2
  *            PLL_P                          = 7
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_80MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Set MSI to 4Mhz (RCC_MSIRANGE_6), activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 110000000
  *            HCLK(Hz)                       = 110000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 55
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_110MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  check_config_in_use_example
  *         Checks that the power configuration is use is as expected
  * @param  expected configuration
  *
  * @retval 0: configuration as expected
  *         1: Power mode in used not as expected
  *         2: Hclkfreq in use not as expected
  *         3: Power mode in used not as expected
  *            Hclkfreq in use not as expected
  *         4: VOS in use not as expected
  *         5: Power mode in used not as expected
  *            VOS in use not as expected
  *         6: Hclkfreq in use not as expected
  *            VOS in use not as expected
  *         7: Power mode in used not as expected
  *            Hclkfreq in use not as expected
  *            VOS in use not as expected
  */

uint32_t check_config_in_use_example(uint32_t config)
{
  config--;
  uint32_t result = 0;
  power_mode_in_use = HAL_PWREx_SMPS_GetEffectiveMode();
  hclkfreq_in_use = HAL_RCC_GetHCLKFreq();
  vos_in_use = HAL_PWREx_GetVoltageRange();

  if (power_mode_in_use !=  (config_in_use_example[config].power_mode_in_use))
  {
    result = 1;
  }
  if (hclkfreq_in_use !=  (config_in_use_example[config].hclkfreq_in_use))
  {
    result += 2;
  }
  if (vos_in_use !=  (config_in_use_example[config].vos_in_use))
  {
    result += 4;
  }
 return result;
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
