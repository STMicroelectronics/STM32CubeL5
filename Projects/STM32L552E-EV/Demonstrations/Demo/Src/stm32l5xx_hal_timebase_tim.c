/**
  ******************************************************************************
  * @file    stm32l5xx_hal_timebase_tim.c
  * @author  MCD Application Team
  * @brief   Template for HAL time base based on the peripheral hardware TIM6.
  *
  *          This file override the native HAL time base functions (defined as weak)
  *          the TIM time base:
  *           + Initializes the TIM6 peripheral to generate a Period elapsed Event each 1ms
  *           + HAL_IncTick is called inside HAL_TIM_PeriodElapsedCallback ie each 1ms
  *
  @verbatim
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"
#include "stm32l552e_eval.h"
#include "app_ucpd.h"
#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#if defined(_GUI_INTERFACE)
#include "gui_api.h"
#endif /* _GUI_INTERFACE */

/** @addtogroup STM32L5xx_HAL_Driver
  * @{
  */

/** @addtogroup HAL_TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef        TimHandle;
/* Private function prototypes -----------------------------------------------*/
void TIM6_IRQHandler(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the TIM6 as a time base source.
  *         The time source is configured  to have 1ms time base with a dedicated
  *         Tick interrupt priority.
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
  * @param  TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
  uint32_t              uwPrescalerValue = 0U;
  uint32_t              pFLatency;

  /* Configure the TIM6 IRQ priority */
  HAL_NVIC_SetPriority(TIM6_IRQn, TickPriority, 0U);

  /* Enable the TIM6 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM6_IRQn);

  /* Enable TIM6 clock */
  __HAL_RCC_TIM6_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;

  /* Compute TIM6 clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1)
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
  }

  /* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);

  /* Initialize TIM6 */
  TimHandle.Instance = TIM6;

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  TimHandle.Init.Period = (1000000U / 1000U) - 1U;
  TimHandle.Init.Prescaler = uwPrescalerValue;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if(HAL_TIM_Base_Init(&TimHandle) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    return HAL_TIM_Base_Start_IT(&TimHandle);
  }

  /* Return function status */
  return HAL_ERROR;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM6 update interrupt.
  * @retval None
  */
void HAL_SuspendTick(void)
{
  /* Disable TIM6 update interrupt */
  __HAL_TIM_DISABLE_IT(&TimHandle, TIM_IT_UPDATE);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by enabling TIM6 update interrupt.
  * @retval None
  */
void HAL_ResumeTick(void)
{
  /* Enable TIM6 update interrupt */
  __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM6_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  HAL_IncTick();
  if (UcpdDemoRunning == 1)
  {
    USBPD_DPM_TimerCounter();
#if defined(_GUI_INTERFACE)
    GUI_TimerCounter();
#endif /* _GUI_INTERFACE */
  }
}

/**
  * @brief  This function handles TIM6 interrupt request.
  * @retval None
  */
void TIM6_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);
}

/**
  * @}
  */

/**
  * @}
  */
