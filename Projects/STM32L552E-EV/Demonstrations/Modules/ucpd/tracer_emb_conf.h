/**
  ******************************************************************************
  * @file    tracer_emb_conf.h
  * @author  MCD Application Team
  * @brief   This file contains the Trace HW related defines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef TRACER_EMB_CONF_H
#define TRACER_EMB_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_usart.h"
#if defined(LPUART1)
#include "stm32l5xx_ll_lpuart.h"
#endif /* LPUART1 */
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_dma.h"
/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
      Definitions for TRACE feature
-------------------------------------------------------------------------------*/
#define TRACER_EMB_BAUDRATE                          921600UL

#define TRACER_EMB_BUFFER_SIZE                       1024UL

#define TRACER_EMB_DMA_MODE                          1UL
#define TRACER_EMB_IT_MODE                           0UL

/* -----------------------------------------------------------------------------
      Definitions for TRACE Hw information
-------------------------------------------------------------------------------*/

#if defined(USE_STM32L5XX_NUCLEO_144)

#define TRACER_EMB_IS_INSTANCE_LPUART_TYPE           1UL /* set to 1UL if LPUART is used instead of USART */
#define TRACER_EMB_USART_INSTANCE                    LPUART1

#define TRACER_EMB_TX_GPIO                           GPIOG
#define TRACER_EMB_TX_PIN                            LL_GPIO_PIN_7
#define TRACER_EMB_TX_AF                             LL_GPIO_AF_8
#define TRACER_EMB_TX_GPIO_ENABLE_CLOCK()            do {                                                                  \
                                                       __HAL_RCC_PWR_CLK_ENABLE();      /* Enable VddIO2 for GPIOG */      \
                                                       HAL_PWREx_EnableVddIO2();                                           \
                                                       LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);                \
                                                       } while(0)
#define TRACER_EMB_TX_GPIO_DISABLE_CLOCK()           LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOG)
#define TRACER_EMB_RX_GPIO                           GPIOG
#define TRACER_EMB_RX_PIN                            LL_GPIO_PIN_8
#define TRACER_EMB_RX_AF                             LL_GPIO_AF_8
#define TRACER_EMB_RX_GPIO_ENABLE_CLOCK()            do {                                                                  \
                                                       __HAL_RCC_PWR_CLK_ENABLE();      /* Enable VddIO2 for GPIOG */      \
                                                       HAL_PWREx_EnableVddIO2();                                           \
                                                       LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);                \
                                                       } while(0)
#define TRACER_EMB_RX_GPIO_DISABLE_CLOCK()           LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOG)
#define TRACER_EMB_ENABLE_CLK_USART()                LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1)
#define TRACER_EMB_DISABLE_CLK_USART()               LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_LPUART1)
#define TRACER_EMB_SET_CLK_SOURCE_USART()            LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_SYSCLK);
#define TRACER_EMB_USART_IRQ                         LPUART1_IRQn
#define TRACER_EMB_USART_IRQHANDLER                  LPUART1_IRQHandler
#define TRACER_EMB_TX_AF_FUNCTION                    LL_GPIO_SetAFPin_0_7
#define TRACER_EMB_RX_AF_FUNCTION                    LL_GPIO_SetAFPin_8_15
#define TRACER_EMB_DMA_INSTANCE                      DMA1
#define TRACER_EMB_ENABLE_CLK_DMA()                  do {                                                                  \
                                                       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);                 \
                                                       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);              \
                                                       } while(0)
#define TRACER_EMB_TX_DMA_REQUEST                    LL_DMAMUX_REQ_LPUART1_TX
#define TRACER_EMB_TX_DMA_CHANNEL                    LL_DMA_CHANNEL_6
#define TRACER_EMB_ENABLECHANNEL                     LL_DMA_EnableChannel
#define TRACER_EMB_DISABLECHANNEL                    LL_DMA_DisableChannel
#define TRACER_EMB_TX_DMA_IRQ                        DMA1_Channel6_IRQn
#define TRACER_EMB_TX_DMA_IRQHANDLER                 DMA1_Channel6_IRQHandler
#define TRACER_EMB_TX_DMA_ACTIVE_FLAG                LL_DMA_IsActiveFlag_TC6
#define TRACER_EMB_TX_DMA_CLEAR_FLAG                 LL_DMA_ClearFlag_GI6

#elif defined(USE_STM32L552E_EVAL)

#define TRACER_EMB_IS_INSTANCE_LPUART_TYPE           0UL /* set to 1UL if LPUART is used instead of USART */
#define TRACER_EMB_USART_INSTANCE                    USART3

#define TRACER_EMB_TX_GPIO                           GPIOB
#define TRACER_EMB_TX_PIN                            LL_GPIO_PIN_10
#define TRACER_EMB_TX_AF                             LL_GPIO_AF_7
#define TRACER_EMB_TX_GPIO_ENABLE_CLOCK()            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define TRACER_EMB_TX_GPIO_DISABLE_CLOCK()           LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define TRACER_EMB_RX_GPIO                           GPIOB
#define TRACER_EMB_RX_PIN                            LL_GPIO_PIN_11
#define TRACER_EMB_RX_AF                             LL_GPIO_AF_7
#define TRACER_EMB_RX_GPIO_ENABLE_CLOCK()            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define TRACER_EMB_RX_GPIO_DISABLE_CLOCK()           LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define TRACER_EMB_ENABLE_CLK_USART()                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define TRACER_EMB_DISABLE_CLK_USART()               LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART3)
#define TRACER_EMB_SET_CLK_SOURCE_USART()            LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_SYSCLK);
#define TRACER_EMB_USART_IRQ                         USART3_IRQn
#define TRACER_EMB_USART_IRQHANDLER                  USART3_IRQHandler
#define TRACER_EMB_TX_AF_FUNCTION                    LL_GPIO_SetAFPin_8_15
#define TRACER_EMB_RX_AF_FUNCTION                    LL_GPIO_SetAFPin_8_15
#define TRACER_EMB_DMA_INSTANCE                      DMA1
#define TRACER_EMB_ENABLE_CLK_DMA()                  do {                                                                  \
                                                       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);                 \
                                                       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);              \
                                                       } while(0)
#define TRACER_EMB_TX_DMA_REQUEST                    LL_DMAMUX_REQ_USART3_TX
#define TRACER_EMB_TX_DMA_CHANNEL                    LL_DMA_CHANNEL_6
#define TRACER_EMB_ENABLECHANNEL                     LL_DMA_EnableChannel
#define TRACER_EMB_DISABLECHANNEL                    LL_DMA_DisableChannel
#define TRACER_EMB_TX_DMA_IRQ                        DMA1_Channel6_IRQn
#define TRACER_EMB_TX_DMA_IRQHANDLER                 DMA1_Channel6_IRQHandler
#define TRACER_EMB_TX_DMA_ACTIVE_FLAG                LL_DMA_IsActiveFlag_TC6
#define TRACER_EMB_TX_DMA_CLEAR_FLAG                 LL_DMA_ClearFlag_GI6

#else

#error "Please define Tracer configuration for your board"

#endif /* USE_STM32L5XX_NUCLEO_144 */

#ifdef __cplusplus
}
#endif

#endif /* USBPD_DEVICE_CONF_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

