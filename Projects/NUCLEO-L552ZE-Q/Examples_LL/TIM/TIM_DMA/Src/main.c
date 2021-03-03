/**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_DMA/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use DMA with TIM1 Update request to
  *          transfer Data from memory to TIM1 Capture Compare Register 3 (CCR3)
  *          using the STM32L5xx TIM LL API.
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

/** @addtogroup TIM_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CC_VALUE_NB       3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Capture Compare buffer */
static uint32_t aCCValue[CC_VALUE_NB] = {0};

/* TIM1 Clock */
static uint32_t TimOutClock = 1;

/* Private function prototypes -----------------------------------------------*/
__STATIC_INLINE void     SystemClock_Config(void);
__STATIC_INLINE void     Configure_DMA(void);
__STATIC_INLINE void     Configure_TIM(void);
__STATIC_INLINE void     LED_Init(void);
__STATIC_INLINE void     LED_Blinking(uint32_t Period);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the system clock to 110 MHz */
  SystemClock_Config();
  
  /* For better performances, enable the instruction cache in 1-way direct mapped mode */
  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();

  /* Initialize LED2 */
  LED_Init();

  /* Configure DMA transfer */  
  Configure_DMA();
  
  /* Configure timer instance */
  Configure_TIM();

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  This function enables the peripheral clock for the DMA,
  *         configures the DMA transfer, configures the NVIC for DMA and
  *         enables the DMA.
  * @param  None
  * @retval None
  */
__STATIC_INLINE void  Configure_DMA(void)
{  
  /******************************************************/
  /* Configure NVIC for DMA transfer related interrupts */
  /******************************************************/
  NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);

  /*****************************/
  /* Peripheral clock enabling */
  /*****************************/
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);

  /******************************/
  /* DMA transfer Configuration */
  /******************************/
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                                                LL_DMA_PRIORITY_HIGH              |
                                                LL_DMA_MODE_CIRCULAR              |
                                                LL_DMA_PERIPH_NOINCREMENT         |
                                                LL_DMA_MEMORY_INCREMENT           |
                                                LL_DMA_PDATAALIGN_WORD            |
                                                LL_DMA_MDATAALIGN_WORD);
  
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_5, LL_DMAMUX_REQ_TIM1_UP);
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, (uint32_t)&aCCValue, (uint32_t)&TIM1->CCR1, LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, CC_VALUE_NB);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);
    
  /***************************/
  /* Enable the DMA transfer */
  /***************************/
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
}


/**
  * @brief  This function configures TIM1 channel 1 to generate a PWM edge
  *         aligned signal with a frequency equal to 17.57 KHz and a variable
  *         duty cycle that is changed by the DMA after a specific number of 
  *         update DMA requests. The number of this repetitive requests is
  *         defined by the TIM1 repetition counter, each 4 update requests, the
  *         TIM1 Channel 1 Duty Cycle changes to the next new value defined by
  *         the aCCValue.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @retval None
  */
__STATIC_INLINE void  Configure_TIM(void)
{  
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);

  /* GPIO TIM1_CH1 configuration */
  LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_9, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_9, LL_GPIO_AF_1);

  /******************************************************/
  /* Configure the NVIC to handle TIM1 update interrupt */
  /******************************************************/
  NVIC_SetPriority(TIM1_UP_IRQn, 0);
  NVIC_EnableIRQ(TIM1_UP_IRQn);
  
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the peripheral clock of TIM1 */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
  
  /* Set the TIM1 auto-reload register to get a PWM frequency at 17.57 KHz */
  /* Note that the timer pre-scaler isn't used, therefore the timer counter   */
  /* clock frequency is equal to the timer frequency.                        */
    /* In this example TIM1 input clock (TIM1CLK) frequency is set to APB2 clock*/
  /*  (PCLK2), since APB2 pre-scaler is equal to 1.                                     */
  /*    TIM1CLK = PCLK2                                                       */
  /*    PCLK2 = HCLK                                                          */
  /*    => TIM1CLK = HCLK = SystemCoreClock (110 Mhz)                           */
  
  /* TIM1CLK = SystemCoreClock / (APB prescaler & multiplier)              */
  TimOutClock = SystemCoreClock/1;
  LL_TIM_SetAutoReload(TIM1, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_COUNTERMODE_UP, 17570));
  
  /* Set the repetition counter in order to generate one update event every 4 */
  /* counter cycles.                                                          */
  LL_TIM_SetRepetitionCounter(TIM1, 4-1);
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output channel 1 in PWM1 mode */
  LL_TIM_OC_SetMode(TIM1,  LL_TIM_CHANNEL_CH1,  LL_TIM_OCMODE_PWM1);

  /* TIM1 channel 1 configuration:    */
  LL_TIM_OC_ConfigOutput(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH | LL_TIM_OCIDLESTATE_HIGH);

  /* Compute compare value to generate a duty cycle at 75% */
  aCCValue[0] = (uint32_t)(((uint32_t) 75 * (LL_TIM_GetAutoReload(TIM1) - 1)) / 100);
  /* Compute compare value to generate a duty cycle at 50% */
  aCCValue[1] = (uint32_t)(((uint32_t) 50 * (LL_TIM_GetAutoReload(TIM1) - 1)) / 100);
  /* Compute compare value to generate a duty cycle at 25% */
  aCCValue[2] = (uint32_t)(((uint32_t) 25 * (LL_TIM_GetAutoReload(TIM1) - 1)) / 100);

  /* Set PWM duty cycle  for TIM1 channel 1*/
  LL_TIM_OC_SetCompareCH1(TIM1, aCCValue[0]);
  
  /* Enable register preload for TIM1 channel 1 */
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
  
  /****************************/
  /* TIM1 DMA requests set-up */
  /****************************/
  /* Enable DMA request on update event */
  LL_TIM_EnableDMAReq_UPDATE(TIM1);

  /**************************/
  /* TIM1 interrupts set-up */
  /**************************/
  /* Enable the update interrupt */
  LL_TIM_EnableIT_UPDATE(TIM1);
  
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable TIM1 channel 1 */
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
  
  /* Enable TIM1 outputs */
  LL_TIM_EnableAllOutputs(TIM1);
  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM1);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM1);
}

/**
  * @brief  Initialize LED2.
  * @param  None
  * @retval None
  */
__STATIC_INLINE void LED_Init(void)
{
  /* Enable the LED2 Clock */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

  /* Configure IO in output push-pull mode to drive external LED2 */
  LL_GPIO_SetPinMode(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_SPEED_FREQ_LOW);
  LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
}

/**
  * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
__STATIC_INLINE void LED_Blinking(uint32_t Period)
{
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);  
    LL_mDelay(Period);
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
  * @brief  Timer update interrupt processing
  * @param  None
  * @retval None
  */
void TimerUpdate_Callback(void)
{
  static uint32_t UpdateEventCnt = 0;

  /* At every update event the CCR1 register is updated with a new value */
  /* which is DMA transferred from aCCValue[].                           */
  /* Note that the update event (UEV) is generated after upcounting is   */
  /* repeated for the number of times programmed in the repetition       */
  /* counter register (TIM1_RCR) + 1                                     */
  if (LL_TIM_OC_GetCompareCH1(TIM1) != aCCValue[UpdateEventCnt])
  {
    LED_Blinking(LED_BLINK_ERROR);
  }
  else
  {
    UpdateEventCnt = (UpdateEventCnt+1) % CC_VALUE_NB;
  }
}

/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated after DMA transfer
  * @retval None
  */
void TransferComplete_Callback()
{
  /* Once the DMA transfer is completed the CCR1 value must match */
  /* the value of the last element of aCCValue[].                 */
  if (LL_TIM_OC_GetCompareCH1(TIM1) != aCCValue[CC_VALUE_NB-1])
  {
    LED_Blinking(LED_BLINK_ERROR);
  }
}

/**
  * @brief  DMA transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
void TransferError_Callback()
{
  LED_Blinking(LED_BLINK_ERROR);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

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
