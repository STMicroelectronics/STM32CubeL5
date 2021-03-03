/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWR/PWR_RUN_SMPS/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32L5xx PWR HAL API to enter
  *          SMPS modes in Run mode.
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
typedef struct
{
uint32_t power_mode_in_use;
uint32_t hclkfreq_in_use;
uint32_t vos_in_use;
} config_in_useTypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// #define ADD_EXTRA_DELAY
#define ADD_EXTRA_DELAY_MEASUREMENTS
#define DELAY_FOR_POWER_MEASUREMENTS 20000 /* in ms */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
void SystemClock_110MHz(void);
void SystemClock_80MHz(void);
void SystemClock_24MHz(void);

uint32_t check_config_in_use_example(uint32_t config);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* Enable Instruction cache in 1-way direct (default 2-ways set associative cache) */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);

  if(HAL_ICACHE_Enable() != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
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
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();

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
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE7 PE8 PE9
                           PE10 PE11 PE12 PE13
                           PE14 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC14 PC15 PC0
                           PC1 PC2 PC3 PC6
                           PC7 PC8 PC9 PC10
                           PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0
                          |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PF0 PF1 PF2 PF3
                           PF4 PF5 PF6 PF7
                           PF8 PF9 PF10 PF11
                           PF12 PF13 PF14 PF15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PH0 PH1 PH3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 PA9 PA10 PA11
                           PA12 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB13 PB14 PB15
                           PB4 PB5 PB6 PB7
                           PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PG0 PG1 PG2 PG3
                           PG4 PG5 PG6 PG7
                           PG8 PG9 PG10 PG12
                           PG13 PG14 PG15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11
                           PD12 PD13 PD14 PD15
                           PD0 PD1 PD2 PD3
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PA13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
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
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
