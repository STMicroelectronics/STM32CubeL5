/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/RCC/RCC_OutputSystemClockOnMCO/Inc/main.h
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_ll_icache.h"
#include "stm32l5xx_ll_pwr.h"
#include "stm32l5xx_ll_crs.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_system.h"
#include "stm32l5xx_ll_exti.h"
#include "stm32l5xx_ll_cortex.h"
#include "stm32l5xx_ll_utils.h"
#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/**
  * @brief LED1
  */

#define LED1_PIN                           LL_GPIO_PIN_7
#define LED1_GPIO_PORT                     GPIOC
#define LED1_GPIO_CLK_ENABLE()             LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC)

/**
  * @brief Key push-button
  */
#define USER_BUTTON_PIN                         LL_GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT                   GPIOC
#define USER_BUTTON_GPIO_CLK_ENABLE()           LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC)
#define USER_BUTTON_EXTI_LINE                   LL_EXTI_EXTI_LINE13
#define USER_BUTTON_EXTI_IRQn                   EXTI13_IRQn
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_SYSCFG_SET_EXTI()           LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);
#define USER_BUTTON_IRQHANDLER                  EXTI13_IRQHandler
#define USER_BUTTON_PULL_MODE                   LL_GPIO_PULL_DOWN

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER button IRQ Handler treatment. */
void UserButton_Callback(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin LL_GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define USER_BUTTON_EXTI_IRQn EXTI13_IRQn
#define LED1_Pin LL_GPIO_PIN_7
#define LED1_GPIO_Port GPIOC
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 1 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
