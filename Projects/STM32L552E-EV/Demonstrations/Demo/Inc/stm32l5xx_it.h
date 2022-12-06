/**
  ******************************************************************************
  * @file    stm32l5xx_it.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the interrupt handlers.
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
