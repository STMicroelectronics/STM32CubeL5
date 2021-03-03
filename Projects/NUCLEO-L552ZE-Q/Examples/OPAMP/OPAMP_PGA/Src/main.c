/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OPAMP/OPAMP_PGA/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the OPAMP
  *          peripheral to amplify several signals.
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
/* set to 1 after User push-button interrupt  */
__IO uint32_t UserButtonStatus = 0; 
const uint32_t Sine12bit[32] = {511, 611, 707, 796, 847, 874, 984, 1014, 1023,
                                1014, 984, 937, 874, 796, 707, 611, 511, 411,
                                315, 227, 149, 149, 38, 9, 0, 9, 38, 86, 149, 227,
                                315, 411};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch2;

OPAMP_HandleTypeDef hopamp2;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ICACHE_Init(void);
static void MX_DAC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_OPAMP2_Init(void);
/* USER CODE BEGIN PFP */
static void DAC_Config_LowPower(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief  Configure DAC in low Power mode.
  * @param None
  * @retval None
  */
static void DAC_Config_LowPower(void)
{
  DAC_ChannelConfTypeDef sConfig = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct_LSI = {0};
  
  RCC_OscInitStruct_LSI.OscillatorType = RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct_LSI.LSIDiv         = RCC_LSI_DIV1;
  RCC_OscInitStruct_LSI.LSIState       = RCC_LSI_ON;
  RCC_OscInitStruct_LSI.PLL.PLLState   = RCC_PLL_NONE;
  
  if (HAL_OK != HAL_RCC_OscConfig(&RCC_OscInitStruct_LSI))
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Deinit  DAC */
  if (HAL_OK != HAL_DAC_Init(&hdac1))
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_ENABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;

  /* configure low power mode - sample and hold mode */
  sConfig.DAC_SampleAndHoldConfig.DAC_SampleTime = 20;
  sConfig.DAC_SampleAndHoldConfig.DAC_HoldTime = 10;
  sConfig.DAC_SampleAndHoldConfig.DAC_RefreshTime = 5;

  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Transition is detected */
  UserButtonStatus = 1;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t examplestate = 0; 
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
  MX_DMA_Init();
  MX_ICACHE_Init();
  MX_DAC1_Init();
  MX_TIM2_Init();
  MX_OPAMP2_Init();
  /* USER CODE BEGIN 2 */

  /* Configure and enable the User push-button in EXTI mode used as DMA external request signal */
  if (BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI) != BSP_ERROR_NONE)
  {
    while(1);
  }
  
  /* Enable TIM peripheral counter */
  HAL_TIM_Base_Start(&htim2);
  
  /* Start DAC with DMA used as input signal */
  if(HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t*)Sine12bit, 
                       sizeof (Sine12bit) / sizeof (uint32_t), 
                       DAC_ALIGN_12B_R) != HAL_OK)
  {
    Error_Handler();
  }  
 
  /*--------------------------------------------------------------------------*/
/* Step 0:                                                                    */
/*  	DAC:     normal power mode                                            */
/*  	OPAMP:   normal power mode - gain = 2                                 */
/*  	DMA:     circular mode                                                */
/*  	Cortex:  run mode                                                     */
/*----------------------------------------------------------------------------*/
  
    /* Enable OPAMP */
    if(HAL_OK != HAL_OPAMP_Start(&hopamp2))
    {
      Error_Handler();
    }  
   
    while (UserButtonStatus == 0)
    {
    }
    UserButtonStatus = 0; 

/*----------------------------------------------------------------------------*/
/* Step 1:                                                                    */
/*  	DAC:     normal power mode                                            */
/*  	OPAMP:   normal power mode - gain = 4                                 */
/*  	DMA:     circular mode                                                */
/*  	Cortex:  run mode                                                     */
/*----------------------------------------------------------------------------*/
    /* PGA gain set to 4 */    
    hopamp2.Init.PgaGain = OPAMP_PGA_GAIN_4;
    if (HAL_OPAMP_Init(&hopamp2) != HAL_OK)
    {
      Error_Handler();
    }
    while (UserButtonStatus == 0)
    {
    }
    UserButtonStatus = 0; 

/*----------------------------------------------------------------------------*/
/* Step 2: (2 pushes on Key button)                                           */
/*    DAC:     Low power mode                                                 */
/*    OPAMP:   Low power mode - gain = 4                                      */
/*  	DMA:     circular mode                                                */
/*    Cortex:  run mode                                                       */
/*----------------------------------------------------------------------------*/
    /* STOP DMA as DAC will be re-configured */
    if (HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_2) != HAL_OK)
    {
      Error_Handler();
    }     
    /* DAC now set in low power mode */
    DAC_Config_LowPower();
    /* Re-start DAC  */
    if(HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t*)Sine12bit, 
                       sizeof (Sine12bit) / sizeof (uint32_t), 
                       DAC_ALIGN_12B_R) != HAL_OK)
    {
      Error_Handler();
    }     
    
    /* Power mode set to low power */    
    if (HAL_OK != HAL_OPAMP_DeInit(&hopamp2))
    {
      Error_Handler();
    }
    hopamp2.Init.PowerMode = OPAMP_POWERMODE_LOWPOWER; 
    if (HAL_OPAMP_Init(&hopamp2) != HAL_OK)
    {
      Error_Handler();
    }
    /* Enable OPAMP */
    if(HAL_OK != HAL_OPAMP_Start(&hopamp2))
    {
      Error_Handler();
    }      
    
    while (UserButtonStatus == 0)
    {
    }
    UserButtonStatus = 0; 

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(UserButtonStatus == 1)
    {
      if(examplestate == 0)
      {
/*----------------------------------------------------------------------------*/
/* Step 3: (another push on Key button)                                       */
/*    DAC:     Low power mode                                                 */
/*    OPAMP:   Low power mode - gain = 4                                      */
/*    DMA:     circular mode                                                  */
/*    Cortex:  sleep mode                                                     */ 
/*----------------------------------------------------------------------------*/
        /* Gain change on the fly to 2 */
        hopamp2.Init.PgaGain = OPAMP_PGA_GAIN_2;
        examplestate = 1;
      }
      else
      {
/*----------------------------------------------------------------------------*/
/* Step 4: (another push on Key button)                                       */
/*    DAC:     Low power mode                                                 */
/*    OPAMP:   Low power mode - gain = 2                                      */
/*    DMA:     circular mode                                                  */
/*    Cortex:  sleep mode                                                     */ 
/*----------------------------------------------------------------------------*/
        /* Gain change on the fly to 4 */
        hopamp2.Init.PgaGain = OPAMP_PGA_GAIN_4;   
        examplestate = 0;
      }

      if(HAL_OK != HAL_OPAMP_Init(&hopamp2))
      {
        Error_Handler();
      }
      UserButtonStatus = 0;
    }
    /* Suspend Tick */
    HAL_SuspendTick();
    
    /* Enter SLEEP Mode, wake up is done once Key push button is pressed */  
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);        
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
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */
  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

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
  * @brief OPAMP2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OPAMP2_Init(void)
{

  /* USER CODE BEGIN OPAMP2_Init 0 */

  /* USER CODE END OPAMP2_Init 0 */

  /* USER CODE BEGIN OPAMP2_Init 1 */

  /* USER CODE END OPAMP2_Init 1 */
  hopamp2.Instance = OPAMP2;
  hopamp2.Init.PowerSupplyRange = OPAMP_POWERSUPPLY_LOW;
  hopamp2.Init.Mode = OPAMP_PGA_MODE;
  hopamp2.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC_CH;
  hopamp2.Init.InvertingInput = OPAMP_INVERTINGINPUT_IO0;
  hopamp2.Init.PgaGain = OPAMP_PGA_GAIN_2;
  hopamp2.Init.PowerMode = OPAMP_POWERMODE_NORMALPOWER;
  hopamp2.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP2_Init 2 */

  /* USER CODE END OPAMP2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
  /* User can add his own implementation to report the HAL error return state */
  while(1);
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
