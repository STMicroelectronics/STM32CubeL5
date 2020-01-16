/**
  ******************************************************************************
  * @file    tick.c
  * @author  MCD Application Team
  * @brief   Tick implementation
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
#include "stm32l5xx.h"

/**
  * @brief Provide a tick value in millisecond.
  * @note This function overwrites the one declared as __weak in HAL.
  *       In this implementation, time is counted without using SysTick timer interrupts.
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
  static uint32_t m_uTick = 0U;
  static uint32_t t1 = 0U, tdelta = 0U;
  uint32_t t2;

  t2 =  SysTick->VAL;

  if (t2 <= t1)
  {
    tdelta += t1 - t2;
  }
  else
  {
    tdelta += t1 + SysTick->LOAD - t2;
  }

  if (tdelta > (SystemCoreClock / (1000U)))
  {
    tdelta = 0U;
    m_uTick ++;
  }

  t1 = t2;
  return m_uTick;
}