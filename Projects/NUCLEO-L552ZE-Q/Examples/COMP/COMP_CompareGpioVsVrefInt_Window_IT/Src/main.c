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
#define STATE_OVER_THRESHOLD    0x00000001
#define STATE_WITHIN_THRESHOLD  0x00000002
#define STATE_UNDER_THRESHOLD   0x00000003
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
COMP_HandleTypeDef hcomp1;
COMP_HandleTypeDef hcomp2;

/* USER CODE BEGIN PV */
/* Variable to store input state compared to threshold window */
__IO uint32_t State = 0;

/* Variable to indicate that MCU entered in Stop mode */
__IO uint32_t EnterInStopMode = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_GPIO_Init(void);
static void MX_COMP1_Init(void);
static void MX_COMP2_Init(void);
/* USER CODE BEGIN PFP */
static void InputVoltageLevel_Check(void);
static void StopSequence_Config(void);
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
  /* Initialize LEDs */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_GPIO_Init();
  MX_COMP1_Init();
  MX_COMP2_Init();
  /* USER CODE BEGIN 2 */
  /* Start COMP1 */
  if(HAL_COMP_Start(&hcomp1) != HAL_OK)
  {
    Error_Handler(); 
  }
  /* Start COMP2 */
  if(HAL_COMP_Start(&hcomp2) != HAL_OK)
  {
    Error_Handler(); 
  }
  
  /* Check input voltage level: within the thresholds, above the upper threshold
     or under the lower threshold */
  InputVoltageLevel_Check();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (State == STATE_OVER_THRESHOLD)
    {
      /* Restoration done only in case of MCU was in stop mode */
      if (EnterInStopMode == 1)
      {
        /* Restore config: clock, GPIO... */
        SystemClock_Config();
        
        /* Restore GPIO configuration */
        BSP_LED_Init(LED_GREEN);
        BSP_LED_Init(LED_RED);
        
        EnterInStopMode = 0;
      }
      
      while(State == STATE_OVER_THRESHOLD)
      {
        /* add your code here */
        
        /* Toggle LED_GREEN */
        BSP_LED_Toggle(LED_GREEN);
        HAL_Delay(500);
      }
    }
    else if (State == STATE_WITHIN_THRESHOLD)
    {
      /* Turn off LED_GREEN */
      BSP_LED_Off(LED_GREEN);
      
      /* Input voltage is within the thresholds: higher and lower thresholds */
      /* Enter STOP1 mode with regulator in low power */
      StopSequence_Config();
    }
    else /* (State == STATE_UNDER_THRESHOLD) */
    {
      /* Restoration done only in case of MCU was in stop mode */
      if (EnterInStopMode == 1)
      {
        /* Restore config: clock, GPIO... */
        SystemClock_Config();

        /* Restore GPIO configuration */
        BSP_LED_Init(LED_GREEN);
        BSP_LED_Init(LED_RED);

        EnterInStopMode = 0;
      }      
      while(State == STATE_UNDER_THRESHOLD)
      {
        /* Toggle LED_GREEN */
        BSP_LED_Toggle(LED_GREEN);
        HAL_Delay(1000);
      }
    }
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
  * @brief COMP1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_COMP1_Init(void)
{

  /* USER CODE BEGIN COMP1_Init 0 */

  /* USER CODE END COMP1_Init 0 */

  /* USER CODE BEGIN COMP1_Init 1 */

  /* USER CODE END COMP1_Init 1 */
  hcomp1.Instance = COMP1;
  hcomp1.Init.InputMinus = COMP_INPUT_MINUS_VREFINT;
  hcomp1.Init.InputPlus = COMP_INPUT_PLUS_IO2;
  hcomp1.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
  hcomp1.Init.Hysteresis = COMP_HYSTERESIS_LOW;
  hcomp1.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
  hcomp1.Init.Mode = COMP_POWERMODE_MEDIUMSPEED;
  hcomp1.Init.WindowMode = COMP_WINDOWMODE_DISABLE;
  hcomp1.Init.TriggerMode = COMP_TRIGGERMODE_IT_RISING_FALLING;
  if (HAL_COMP_Init(&hcomp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN COMP1_Init 2 */

  /* USER CODE END COMP1_Init 2 */

}

/**
  * @brief COMP2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_COMP2_Init(void)
{

  /* USER CODE BEGIN COMP2_Init 0 */

  /* USER CODE END COMP2_Init 0 */

  /* USER CODE BEGIN COMP2_Init 1 */

  /* USER CODE END COMP2_Init 1 */
  hcomp2.Instance = COMP2;
  hcomp2.Init.InputMinus = COMP_INPUT_MINUS_1_4VREFINT;
  hcomp2.Init.InputPlus = COMP_INPUT_PLUS_IO1;
  hcomp2.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
  hcomp2.Init.Hysteresis = COMP_HYSTERESIS_LOW;
  hcomp2.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
  hcomp2.Init.Mode = COMP_POWERMODE_MEDIUMSPEED;
  hcomp2.Init.WindowMode = COMP_WINDOWMODE_COMP1_INPUT_PLUS_COMMON;
  hcomp2.Init.TriggerMode = COMP_TRIGGERMODE_IT_RISING_FALLING;
  if (HAL_COMP_Init(&hcomp2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN COMP2_Init 2 */

  /* USER CODE END COMP2_Init 2 */

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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
/**
  * @brief  check input voltage level: within the thresholds, above the upper 
  *         threshold or under the lower threshold
  * @param  None
  * @retval None
  */
static void InputVoltageLevel_Check(void)
{
  /*  Check if COMP1 and COMP2 output level is high */
  if (((HAL_COMP_GetOutputLevel(&hcomp1)) == COMP_OUTPUT_LEVEL_HIGH) 
   && ((HAL_COMP_GetOutputLevel(&hcomp2)) == COMP_OUTPUT_LEVEL_HIGH))
  {
    /* A rising edge is detected so the input voltage is higher than VREFINT */
    State = STATE_OVER_THRESHOLD;
  }
  else if (((HAL_COMP_GetOutputLevel(&hcomp1)) == COMP_OUTPUT_LEVEL_LOW)
       && ((HAL_COMP_GetOutputLevel(&hcomp2)) == COMP_OUTPUT_LEVEL_HIGH))
  {
    /* A falling edge is detected so the input voltage is lower than VREFINT */
    State = STATE_WITHIN_THRESHOLD;
  }
  else if (((HAL_COMP_GetOutputLevel(&hcomp1)) == COMP_OUTPUT_LEVEL_LOW)
       && ((HAL_COMP_GetOutputLevel(&hcomp2)) == COMP_OUTPUT_LEVEL_LOW))
  {
    State = STATE_UNDER_THRESHOLD;
  }
}

/**
  * @brief  Prepare the system to enter STOP1 mode.
  * @param  None
  * @retval None
  */

static void StopSequence_Config(void)
{
  /* Request to enter STOP1 mode */
  EnterInStopMode = 1;
  HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
}

/**
  * @brief  Comparator interrupt callback.
  * @param  hcomp: COMP handle
  * @retval None
  */
void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
  /* Check input voltage level: within the thresholds, */
  /* above the upper threshold or under the lower threshold */
  InputVoltageLevel_Check();
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
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
