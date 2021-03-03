/**
  ******************************************************************************
  * @file    Examples_LL/LPTIM/LPTIM_PulseCounter/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use the LPTIM in counter mode
  *          using the STM32L5xx LPTIM LL API.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L5xx_LL_Examples
  * @{
  */

/** @addtogroup LPTIM_PulseCounter
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void     SystemClock_Config(void);
void     Enable_LSI(void);
void     Configure_LPTIMCounter(void);
void     EnterStop1Mode(void);
void     LED_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the system clock */
  SystemClock_Config();
  
  /* For better performances, enable the instruction cache in 1-way direct mapped mode */
  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  LL_PWR_DisableUCPDDeadBattery();

  /* Initialize LED1 */
  LED_Init();

  /* Enable the LSI Clock */
  Enable_LSI();

  /* Configures LPTIM2 in counter mode */
  Configure_LPTIMCounter();
  
  /* Enter STOP 1 mode */
  EnterStop1Mode();

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Enable Internal Low Speed Clock (LSI)
  * @param  None
  * @retval Status
  */
void Enable_LSI(void)
{
  /* Enable LSI Oscillator */
  LL_RCC_LSI_Enable();
  
  while(LL_RCC_LSI_IsReady() != 1) 
  {
  }
}

/**
  * @brief  Configures the LPTIM2 instance in counter mode. 
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void Configure_LPTIMCounter(void)
{
  /***************************************/
  /* Select LSI as LPTIM2 clock source */
  /***************************************/
  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM2_CLKSOURCE_LSI);
  
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
  
  /* GPIO LPTIM2_IN1 configuration */
  LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_12, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOD, LL_GPIO_PIN_12, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_8_15(GPIOD, LL_GPIO_PIN_12, LL_GPIO_AF_14);

  /*************************************************/
  /* Configure the NVIC to handle LPTIM2 interrupt */
  /*************************************************/
  NVIC_SetPriority(LPTIM2_IRQn, 0);
  NVIC_EnableIRQ(LPTIM2_IRQn);

  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPTIM2);

  /****************************/
  /* LPTIM2 interrupts set-up */
  /****************************/
  /* Enable the Autoreload match Interrupt */
  LL_LPTIM_EnableIT_ARRM(LPTIM2);
  
  /*****************************/
  /* LPTIM2 configuration      */
  /*****************************/
  /* Set the source of the clock used by the LPTIM instance */
  //LL_LPTIM_SetClockSource(LPTIM2, LL_LPTIM_CLK_SOURCE_INTERNAL);
  
  /* Set actual prescaler division ratio */
  //LL_LPTIM_SetPrescaler(LPTIM2, LL_LPTIM_PRESCALER_DIV1);
  
  /* Configure the LPTIM instance output */
  //LL_LPTIM_ConfigOutput(LPTIM2, LL_LPTIM_OUTPUT_WAVEFORM_PWM, LL_LPTIM_OUTPUT_POLARITY_REGULAR);
  
  /* The counter is incremented following each valid clock pulse on the LPTIM external Input1 */
  LL_LPTIM_SetCounterMode(LPTIM2, LL_LPTIM_COUNTER_MODE_EXTERNAL);
  
  /*****************************/
  /* Enable the LPTIM2 counter */
  /*****************************/
  LL_LPTIM_Enable(LPTIM2);
  
  /****************************/
  /* Set the Autoreload value */
  /****************************/
  LL_LPTIM_SetAutoReload(LPTIM2, 1000);
  
  /************************/
  /* Start LPTIM2 counter */
  /************************/
  /* Start the LPTIM counter in continuous mode */
  LL_LPTIM_StartCounter(LPTIM2, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
}

/**
  * @brief  Function to configure and enter in STOP 1 Mode.
  * @param  None
  * @retval None
  */
void EnterStop1Mode(void)
{
  /* Set low-power mode "Stop" */
  LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep();

  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief  Initialize LED1.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  /* Enable the LED1 Clock */
  LED1_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED1 */
  LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  //LL_GPIO_SetPinOutputType(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
  //LL_GPIO_SetPinSpeed(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_PULL_NO);
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
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 5
  *            Voltage range                  = 0
  * @retval None
  */
void SystemClock_Config(void)
{
  /* Enable voltage range 0 mode for frequency above 80 Mhz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }

  /* MSI already enabled at reset */

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 55, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Sysclk activation on the main PLL */
  /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Insure 1µs transition state at intermediate medium speed clock based on DWT*/
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < 100);

  /* AHB prescaler 1 */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 110MHz */
  /* This frequency can be calculated through LL RCC macro */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ(MSI_VALUE,
                                  LL_RCC_PLLM_DIV_1, 55, LL_RCC_PLLR_DIV_2)*/
  LL_Init1msTick(110000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(110000000);
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  LPTimer Autoreload match interrupt processing
  * @param  None
  * @retval None
  */
void LPTimerAutoreloadMatch_Callback(void)
{
  LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);  
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
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */



/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
