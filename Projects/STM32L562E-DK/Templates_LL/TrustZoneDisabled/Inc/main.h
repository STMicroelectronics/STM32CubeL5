/**
  ******************************************************************************
  * @file    Templates_LL/Legacy/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
/* LL drivers common to all LL examples */
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_system.h"
#include "stm32l5xx_ll_utils.h"
#include "stm32l5xx_ll_pwr.h"
#include "stm32l5xx_ll_exti.h"
#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_icache.h"
/* LL drivers specific to LL examples IPs */
#include "stm32l5xx_ll_adc.h"
#include "stm32l5xx_ll_comp.h"
#include "stm32l5xx_ll_cortex.h"
#include "stm32l5xx_ll_crc.h"
#include "stm32l5xx_ll_crs.h"
#include "stm32l5xx_ll_dac.h"
#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_dmamux.h"
#include "stm32l5xx_ll_i2c.h"
#include "stm32l5xx_ll_iwdg.h"
#include "stm32l5xx_ll_lptim.h"
#include "stm32l5xx_ll_lpuart.h"
#include "stm32l5xx_ll_opamp.h"
#include "stm32l5xx_ll_rng.h"
#include "stm32l5xx_ll_rtc.h"
#include "stm32l5xx_ll_spi.h"
#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_ucpd.h"
#include "stm32l5xx_ll_usart.h"
#include "stm32l5xx_ll_wwdg.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
/**
  * @brief LED9
  */
#define LED9_PIN                           LL_GPIO_PIN_3
#define LED9_GPIO_PORT                     GPIOD
#define LED9_GPIO_CLK_ENABLE()             LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)

/**
  * @brief Key push-button
  */
#define USER_BUTTON_PIN                         LL_GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT                   GPIOC
#define USER_BUTTON_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)
#define USER_BUTTON_EXTI_LINE                   LL_EXTI_LINE_13
#define USER_BUTTON_EXTI_IRQn                   EXTI13_IRQn
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_13)
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_13)
#define USER_BUTTON_IRQHANDLER                  EXTI13_IRQHandler
/* ==============   BOARD SPECIFIC CONFIGURATION CODE END      ============== */

/**
  * @brief Toggle periods for various blinking modes
  */
#define LED_BLINK_FAST  200
#define LED_BLINK_SLOW  500
#define LED_BLINK_ERROR 1000

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* MAIN_H */
