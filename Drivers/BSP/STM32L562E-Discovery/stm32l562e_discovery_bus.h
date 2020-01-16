/**
  ******************************************************************************
  * @file    stm32l562e_discovery.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32L562E_DISCOVERY bus.
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
#ifndef STM32L562E_DISCOVERY_BUS_H
#define STM32L562E_DISCOVERY_BUS_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32l562e_discovery_conf.h"
#include "stm32l562e_discovery_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L562E_DISCOVERY
  * @{
  */

/** @addtogroup STM32L562E_DISCOVERY_BUS
  * @{
  */

/** @defgroup STM32L562E_DISCOVERY_BUS_Exported_Types STM32L562E_DISCOVERY BUS Exported Types
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  pI2C_CallbackTypeDef  pMspI2cInitCb;
  pI2C_CallbackTypeDef  pMspI2cDeInitCb;
} BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @defgroup STM32L562E_DISCOVERY_BUS_Exported_Constants STM32L562E_DISCOVERY BUS Exported Constants
  * @{
  */
#define BUS_I2C1                        I2C1
#define BUS_I2C1_CLK_ENABLE()           __HAL_RCC_I2C1_CLK_ENABLE()
#define BUS_I2C1_CLK_DISABLE()          __HAL_RCC_I2C1_CLK_DISABLE()
#define BUS_I2C1_FORCE_RESET()          __HAL_RCC_I2C1_FORCE_RESET()
#define BUS_I2C1_RELEASE_RESET()        __HAL_RCC_I2C1_RELEASE_RESET()
#define BUS_I2C1_SCL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C1_SCL_GPIO_PIN           GPIO_PIN_6
#define BUS_I2C1_SCL_GPIO_PORT          GPIOB
#define BUS_I2C1_SCL_GPIO_AF            GPIO_AF4_I2C1
#define BUS_I2C1_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C1_SDA_GPIO_PIN           GPIO_PIN_7
#define BUS_I2C1_SDA_GPIO_PORT          GPIOB
#define BUS_I2C1_SDA_GPIO_AF            GPIO_AF4_I2C1

#define BUS_I2C1_TIMEOUT                10000UL
/**
  * @}
  */

/** @addtogroup STM32L562E_DISCOVERY_BUS_Exported_Variables STM32L562E_DISCOVERY BUS Exported Variables
  * @{
  */
extern I2C_HandleTypeDef hbus_i2c1;
/**
  * @}
  */

/** @addtogroup STM32L562E_DISCOVERY_BUS_Exported_Functions STM32L562E_DISCOVERY BUS Exported Functions
  * @{
  */
int32_t BSP_I2C1_Init(void);
int32_t BSP_I2C1_DeInit(void);
int32_t BSP_I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_IsReady(uint16_t DevAddr, uint32_t Trials);
int32_t BSP_GetTick(void);

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C1_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C1_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);
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

#endif /* STM32L562E_DISCOVERY_BUS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
