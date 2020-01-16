/**
  ******************************************************************************
  * @file    stm32l5xx_it.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the interrupt handlers.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L5xx_IT_H
#define STM32L5xx_IT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SecureFault_Handler(void);
#if !defined(_RTOS)
void SVC_Handler(void);
#endif /* _RTOS */
void DebugMon_Handler(void);
#if !defined(_RTOS)
void PendSV_Handler(void);
void SysTick_Handler(void);
#endif /* _RTOS */
void RTC_IRQHandler(void);
void UCPD1_IRQHandler(void);
#if defined(_TRACE) || defined(_GUI_INTERFACE)
void TRACER_EMB_TX_DMA_IRQHANDLER(void);
void TRACER_EMB_USART_IRQHANDLER(void);
void TRACER_EMB_USART_IRQHANDLER(void);
#endif /*_TRACE || _GUI_INTERFACE */
void EXTI0_IRQHandler(void);
void EXTI13_IRQHandler(void);
void EXTI1_IRQHandler(void);
void DMA2_Channel1_IRQHandler(void);
void DMA2_Channel2_IRQHandler(void);



#ifdef __cplusplus
}
#endif

#endif /* STM32L5xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
