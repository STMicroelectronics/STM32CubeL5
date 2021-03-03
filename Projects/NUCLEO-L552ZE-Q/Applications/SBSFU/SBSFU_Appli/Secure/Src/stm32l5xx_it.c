/**
  ******************************************************************************
  * @file    SBSFU/SBSFU_Appli/Secure/Src/stm32l5xx_it.c
  * @author  MCD Application Team
  * @brief   Secure Interrupt Service Routines.
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


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l5xx_it.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define SECURE_IO_TOGGLE_DELAY           1000U   /* Toggle every second */



/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/


/* External variables --------------------------------------------------------*/
extern funcptr_NS pSecureFaultCallback;
extern funcptr_NS pSecureErrorCallback;


/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{

}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Secure fault.
  */
void SecureFault_Handler(void)
{

  funcptr_NS callback_NS; /* non-secure callback function pointer */

  if (pSecureFaultCallback != (funcptr_NS)NULL)
  {
    /* return function pointer with cleared LSB */
    callback_NS = (funcptr_NS)cmse_nsfptr_create(pSecureFaultCallback);

    callback_NS();
  }
  else
  {

    while (1)
    {

    }

  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  while (1)
  {
  }

}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  while (1)
  {
  }

}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{

  HAL_IncTick();

  HAL_SYSTICK_Callback();

}

/******************************************************************************/
/* STM32L5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l5xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles Global TrustZone controller global interrupt.
  */
void GTZC_IRQHandler(void)
{

  funcptr_NS callback_NS; /* non-secure callback function pointer */

  HAL_GTZC_IRQHandler();

  if (pSecureErrorCallback != (funcptr_NS)NULL)
  {
    /* return function pointer with cleared LSB */
    callback_NS = (funcptr_NS)cmse_nsfptr_create(pSecureErrorCallback);

    callback_NS();
  }
  else
  {
    /* Something went wrong in test case */
    while (1);
  }

}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
