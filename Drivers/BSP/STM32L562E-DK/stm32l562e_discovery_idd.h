 /**
  ******************************************************************************
  * @file    stm32l562e_discovery_idd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l562e_discovery_idd.c driver.
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
#ifndef STM32L562E_DISCOVERY_IDD_H
#define STM32L562E_DISCOVERY_IDD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l562e_discovery_conf.h"
#include "stm32l562e_discovery_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L562E-DK
  * @{
  */

/** @addtogroup STM32L562E-DK_IDD
  * @{
  */

/** @defgroup STM32L562E-DK_IDD_Exported_Defines  STM32L562E-DK IDD Exported Defines
  * @{
  */
#define IDD_INSTANCES_NBR 1U


#define IDD_PRE_DELAY_MIN                  5UL      /*!< Minimum value in millisecond before starting acquisition (unit: ms). */
#define IDD_PRE_DELAY_MAX                  30000UL  /*!< Maximum value in millisecond before starting acquisition. This is the default configuration after BSP_IDD_Init (unit: ms). */
#define IDD_ACQUISITION_DURATION_MIN       1UL      /*!< Minimum duration of the acquisation (unit: ms). */
#define IDD_ACQUISITION_DURATION_MAX       10000UL  /*!< Maximum duration of the acquisation. This is the default configuration after BSP_IDD_Init (unit: ms). */
#define IDD_VOLTAGE_MIN                    1800UL   /*!< Minimum power supply voltage level of the MCU Vdd (unit: mV). */
#define IDD_VOLTAGE_MAX                    3300UL   /*!< Maximum power supply voltage level of the MCU Vdd . This is the default configuration after BSP_IDD_Init (unit: mV). */
/**
  * @}
  */

/** @defgroup STM32L562E-DK_IDD_Exported_Types STM32L562E-DK IDD Exported Types
  * @{
  */
typedef struct
{
  uint32_t PreDelay;            /*!< Specifies Pre delay value in millisecond.
                                    Limitation, IDD_PRE_DELAY_MIN <= PreDelay <= IDD_PRE_DELAY_MAX */
  uint32_t AcquisitionDuration; /*!< Specifies the acquisition duration value in millisecond of one measurement
                                    Limitation, IDD_ACQUISITION_DURATION_MIN <= AcquisitionDuration <= IDD_ACQUISITION_DURATION_MAX */
  uint32_t Voltage;             /*!< Specifies the power supply voltage level of the MCU Vdd
                                    Limitation, IDD_VOLTAGE_MIN <= Voltage <= IDD_VOLTAGE_MAX */
} BSP_IDD_Config_t;

typedef enum
{
  IDD_IDLE    = 0x00,
  IDD_INIT    = 0x01,
} IDD_StateTypeDef;

typedef struct
{
  EXTI_HandleTypeDef  hidd_exti;
  IDD_StateTypeDef    state;
  UART_HandleTypeDef  UartHandle;
  BSP_IDD_Config_t    IddConfig;
  uint32_t            Integration_freq;       /* Energy integration values frequency */
} IDD_Ctx_t;
/**
  * @}
  */

/** @addtogroup STM32L562E-DK_IDD_Exported_Variables
  * @{
  */
extern IDD_Ctx_t Idd_Ctx[IDD_INSTANCES_NBR];
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup STM32L562E-DK_IDD_Exported_Functions  STM32L562E-DK IDD Exported Functions
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

#ifdef __cplusplus
}
#endif

#endif /* STM32L562E_DISCOVERY_IDD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
