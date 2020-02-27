/**
  ******************************************************************************
  * @file    stm32l552e_eval_sram.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l552e_eval_sram.c driver.
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
#ifndef STM32L552E_EVAL_SRAM_H
#define STM32L552E_EVAL_SRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l552e_eval_conf.h"
#include "stm32l552e_eval_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E-EV
  * @{
  */

/** @addtogroup STM32L552E-EV_SRAM
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup STM32L552E-EV_SRAM_Exported_Types STM32L552E-EV SRAM Exported Types
  * @{
  */
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSRAM_CallbackTypeDef  pMspInitCb;
  pSRAM_CallbackTypeDef  pMspDeInitCb;
}BSP_SRAM_Cb_t;
#endif /* (USE_HAL_SRAM_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup STM32L552E-EV_SRAM_Exported_Constants STM32L552E-EV SRAM Exported Constants
  * @{
  */

#define SRAM_INSTANCES_NBR    1U

#define SRAM_DEVICE_ADDR      0x60000000UL
#define SRAM_DEVICE_SIZE      0x200000UL  /* SRAM device size : 1024 words of 16-bits */

/* DMA definitions for SRAM DMA transfer */
#define SRAM_DMAx_CLK_ENABLE  __HAL_RCC_DMA1_CLK_ENABLE
#define SRAM_DMAx_CLK_DISABLE __HAL_RCC_DMA1_CLK_DISABLE
#define SRAM_DMAx_CHANNEL     DMA1_Channel1
#define SRAM_DMAx_IRQn        DMA1_Channel1_IRQn
#define SRAM_DMAx_IRQHandler  DMA1_Channel1_IRQHandler

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/

/** @addtogroup STM32L552E-EV_SRAM_Exported_Variables
  * @{
  */
extern SRAM_HandleTypeDef hsram[SRAM_INSTANCES_NBR];
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup STM32L552E-EV_SRAM_Exported_Functions
  * @{
  */
int32_t BSP_SRAM_Init(uint32_t Instance);
int32_t BSP_SRAM_DeInit(uint32_t Instance);
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
int32_t BSP_SRAM_RegisterDefaultMspCallbacks(uint32_t Instance);
int32_t BSP_SRAM_RegisterMspCallbacks(uint32_t Instance, BSP_SRAM_Cb_t *CallBacks);
#endif /* (USE_HAL_SRAM_REGISTER_CALLBACKS == 1) */
void    BSP_SRAM_DMA_IRQHandler(uint32_t Instance);

/* These functions can be modified in case the current settings
   need to be changed for specific application needs */
HAL_StatusTypeDef MX_SRAM_BANK1_Init(SRAM_HandleTypeDef *hSram);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32L552E_EVAL_SRAM_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
