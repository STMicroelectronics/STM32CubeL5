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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Configure_COMP(void);
void Activate_COMP(void);
void LED_Init(void);
void LED_On(void);
void LED_Off(void);
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
  /* USER CODE BEGIN 2 */
  /* Initialize LED_GREEN */
  LED_Init();

  /* Configure comparator */
  Configure_COMP();
  
  /* Activate comparator */
  Activate_COMP();
  
  /* Lock comparator instance */
  //LL_COMP_Lock(COMP1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Note: LED state depending on COMP status is set into COMP IRQ handler, */
    /*        refer to function "ComparatorTrigger_Callback()".               */
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

/* USER CODE BEGIN 4 */
/**
  * @brief  Configure comparator (COMP instance: COMP1), GPIO and EXTI line
  *         used by comparator.
  * @note   This function configures the comparator but does not enable it,
  *         in order to optimize power consumption (comparator enabled only
  *         when needed).
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void Configure_COMP(void)
{
  __IO uint32_t wait_loop_index = 0;
  
  /*## Configuration of GPIO used by comparator ##############################*/
  
  /* Configure GPIO connected to the selected comparator input plus */
  /* Enable GPIO clock */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  
  /* Configure GPIO in analog mode to be used as COMP input */
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_ANALOG);
  
  /*## Configuration of NVIC #################################################*/
  /* Configure NVIC to enable COMP1 interruptions */
  NVIC_SetPriority(COMP_IRQn, 0);
  NVIC_EnableIRQ(COMP_IRQn);
  
  /*## Configuration of comparator ###########################################*/
  
  /* Enable COMP clock */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  
  /*## Configuration of comparator common instance ###########################*/
  
  /* Set comparator common instance window mode */
  /* Note: Window mode is common to 2 COMP instances, and therefore must      */
  /*       with a COMP common instance (COMP12_COMMON, ...)                   */
  //LL_COMP_SetCommonWindowMode(__LL_COMP_COMMON_INSTANCE(COMP1), LL_COMP_WINDOWMODE_DISABLE);
  
  /*## Configuration of comparator instance ##################################*/
  
  /* Set comparator instance operating mode to adjust power and speed */
  LL_COMP_SetPowerMode(COMP1, LL_COMP_POWERMODE_ULTRALOWPOWER);
  
  /* Set comparator inputs */
  LL_COMP_ConfigInputs(COMP1, LL_COMP_INPUT_MINUS_VREFINT, LL_COMP_INPUT_PLUS_IO2);
  
  /* Note: Call of the functions below are commented because they are         */
  /*       useless in this example:                                           */
  /*       settings corresponding to default configuration from reset state.  */
  
  /* Set comparator instance hysteresis mode of the input minus */
  //LL_COMP_SetInputHysteresis(COMP1, LL_COMP_HYSTERESIS_NONE);
  
  /* Set comparator instance output polarity */
  //LL_COMP_SetOutputPolarity(COMP1, LL_COMP_OUTPUTPOL_NONINVERTED);
  
  /* Set comparator instance blanking source */
  //LL_COMP_SetOutputBlankingSource(COMP1, LL_COMP_BLANKINGSRC_NONE);
  
  /* Delay for COMP voltage scaler stabilization time.                        */
  /* Compute number of CPU cycles to wait for, from delay in us.              */
  /* Note: Variable divided by 2 to compensate partially                      */
  /*       CPU processing cycles (depends on compilation optimization).       */
  /* Note: If system core clock frequency is below 200kHz, wait time          */
  /*       is only a few CPU processing cycles.                               */
  wait_loop_index = ((LL_COMP_DELAY_VOLTAGE_SCALER_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
  while(wait_loop_index != 0)
  {
    wait_loop_index--;
  }
  
  /*## Configuration of EXTI line used by comparator #########################*/
  /* Set EXTI line trigger edge */
  LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_21);
  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_21);
  
  /* Set EXTI line interruption */
  LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_21);
}

/**
  * @brief  Perform comparator activation procedure
  *         (COMP instance: COMP1).
  * @note   Operations:
  *         - Enable comparator instance
  *         - Wait for comparator startup time
  *           (required to reach propagation delay specification)
  * @param  None
  * @retval None
  */
void Activate_COMP(void)
{
  __IO uint32_t wait_loop_index = 0;
  
  /* Enable comparator */
  LL_COMP_Enable(COMP1);
  
  /* Delay for comparator startup time.                                       */
  /* Compute number of CPU cycles to wait for, from delay in us.              */
  /* Note: Variable divided by 2 to compensate partially                      */
  /*       CPU processing cycles (depends on compilation optimization).       */
  /* Note: If system core clock frequency is below 200kHz, wait time          */
  /*       is only a few CPU processing cycles.                               */
  wait_loop_index = ((LL_COMP_DELAY_STARTUP_US * (SystemCoreClock / (100000 * 2))) / 10);
  while(wait_loop_index != 0)
  {
    wait_loop_index--;
  }
  
}

/**
  * @brief  Initialize LED_GREEN.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  /* Enable the LED_GREEN Clock */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  /* Configure IO in output push-pull mode to drive external LED_GREEN */
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  //LL_GPIO_SetPinOutputTypeGPIOC, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
  //LL_GPIO_SetPinSpeedGPIOC, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_LOW);
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPullGPIOC, LL_GPIO_PIN_7, LL_GPIO_PULL_NO);
}

/**
  * @brief  Turn-on LED_GREEN.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED_GREEN on */
  LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_7);
}

/**
  * @brief  Turn-off LED_GREEN.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LED_GREEN off */
  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_7);
}

/**
  * @brief  Comparator interruption callback
  *         This function is executed when the comparator threshold
  *         is triggered.
  * @retval None
  */
void ComparatorTrigger_Callback(void)
{
  /* Set LED state in function of comparator output level */
  if (LL_COMP_ReadOutputLevel(COMP1) == LL_COMP_OUTPUT_LEVEL_HIGH)
  {
    LED_On();
  }
  else
  {
    LED_Off();
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
  /* Infinite loop */
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
