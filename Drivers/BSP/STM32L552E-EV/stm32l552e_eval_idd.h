 /**
  ******************************************************************************
  * @file    stm32l552e_eval_idd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l552e_eval_idd.c driver.
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
#ifndef STM32L552E_EVAL_IDD_H
#define STM32L552E_EVAL_IDD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l552e_eval_conf.h"
#include "stm32l552e_eval_errno.h"
#include "../Components/Common/idd.h"
#include "../Components/mfxstm32l152/mfxstm32l152.h"

#if (USE_BSP_IO_CLASS == 1)

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E-EV
  * @{
  */

/** @addtogroup STM32L552E-EV_IDD
  * @{
  */

/** @defgroup STM32L552E-EV_IDD_Exported_Defines STM32L552E-EV IDD Exported Defines
  * @{
  */
#define IDD_INSTANCES_NBR 1U

#define BSP_IDD_PRE_DELAY_MIN                  0U      /*!< Minimum value in millisecond before starting acquisition. */
#define BSP_IDD_PRE_DELAY_MAX                  2540U   /*!< Maximum value in millisecond before starting acquisition. This is the default configuration after BSP_IDD_Init. */
#define BSP_IDD_ACQUISITION_DURATION_MIN       0U      /*!< Minmum duration in millisecond of the acquisation. */
#define BSP_IDD_ACQUISITION_DURATION_MAX       25000U  /*!< Maximum duration in millisecond of the acquisation. This is the default configuration after BSP_IDD_Init */
/**
  * @}
  */

/** @defgroup STM32L552E-EV_IDD_Exported_Types STM32L552E-EV IDD Exported Types
  * @{
  */
typedef struct
{
  int32_t PreDelay;            /*!< Specifies Pre delay value in millisecond.
                                    Limitation, BSP_IDD_PRE_DELAY_MIN <= PreDelay <= BSP_IDD_PRE_DELAY_MAX */
  int32_t AcquisitionDuration; /*!< Specifies the acquisition duration value in millisecond of one measurement
                                    Limitation, BSP_IDD_ACQUISITION_DURATION_MIN <= AcquisitionDuration <= BSP_IDD_ACQUISITION_DURATION_MAX */
}BSP_IDD_Config_t;
/**
  * @}
  */

/** @defgroup STM32L552E-EV_IDD_Exported_Variables STM32L552E-EV IDD Exported Variables
  * @{
  */
extern EXTI_HandleTypeDef hidd_exti[IDD_INSTANCES_NBR];
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup STM32L552E-EV_IDD_Exported_Functions STM32L552E-EV IDD Exported Functions
  * @{
  */
int32_t   BSP_IDD_Init(uint32_t Instance);
int32_t   BSP_IDD_DeInit(uint32_t Instance);
int32_t   BSP_IDD_Reset(uint32_t Instance);
int32_t   BSP_IDD_EnterLowPower(uint32_t Instance);
int32_t   BSP_IDD_ExitLowPower(uint32_t Instance);
int32_t   BSP_IDD_StartMeasurement(uint32_t Instance);
int32_t   BSP_IDD_Config(uint32_t Instance, BSP_IDD_Config_t * IddConfig);
int32_t   BSP_IDD_GetValue(uint32_t Instance, uint32_t *IddValue);
int32_t   BSP_IDD_EnableIT(uint32_t Instance);
int32_t   BSP_IDD_ClearIT(uint32_t Instance);
int32_t   BSP_IDD_GetITStatus(uint32_t Instance);
int32_t   BSP_IDD_DisableIT(uint32_t Instance);

void      BSP_IDD_Callback(uint32_t Instance);
void      BSP_IDD_IRQHandler(uint32_t Instance);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* #if (USE_BSP_IO_CLASS == 1) */

#ifdef __cplusplus
}
#endif

#endif /* STM32L552E_EVAL_IDD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
