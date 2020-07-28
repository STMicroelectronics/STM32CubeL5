/**
  ******************************************************************************
  * @file    stm32l562e_discovery.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32L562E-DK bus.
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

/** @addtogroup STM32L562E-DK
  * @{
  */

/** @addtogroup STM32L562E-DK_BUS
  * @{
  */

/** @defgroup STM32L562E-DK_BUS_Exported_Types STM32L562E-DK BUS Exported Types
  * @{
  */
#if defined(HAL_I2C_MODULE_ENABLED)
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  pI2C_CallbackTypeDef  pMspI2cInitCb;
  pI2C_CallbackTypeDef  pMspI2cDeInitCb;
} BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */
#endif /* HAL_I2C_MODULE_ENABLED */

#if defined(HAL_SPI_MODULE_ENABLED)
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSPI_CallbackTypeDef  pMspSpiInitCb;
  pSPI_CallbackTypeDef  pMspSpiDeInitCb;
} BSP_SPI_Cb_t;
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */
#endif /* HAL_SPI_MODULE_ENABLED */
/**
  * @}
  */

/** @defgroup STM32L562E-DK_BUS_Exported_Constants STM32L562E-DK BUS Exported Constants
  * @{
  */
#if defined(HAL_I2C_MODULE_ENABLED)
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
#endif /* HAL_I2C_MODULE_ENABLED */

#if defined(HAL_SPI_MODULE_ENABLED)
#define BUS_SPI1                        SPI1
#define BUS_SPI1_CLK_ENABLE()           __HAL_RCC_SPI1_CLK_ENABLE()
#define BUS_SPI1_CLK_DISABLE()          __HAL_RCC_SPI1_CLK_DISABLE()
#define BUS_SPI1_FORCE_RESET()          __HAL_RCC_SPI1_FORCE_RESET()
#define BUS_SPI1_RELEASE_RESET()        __HAL_RCC_SPI1_RELEASE_RESET()
#define BUS_SPI1_SCK_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_SPI1_SCK_GPIO_PIN           GPIO_PIN_2
#define BUS_SPI1_SCK_GPIO_PORT          GPIOG
#define BUS_SPI1_SCK_GPIO_AF            GPIO_AF5_SPI1
#define BUS_SPI1_MISO_GPIO_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_SPI1_MISO_GPIO_PIN          GPIO_PIN_3
#define BUS_SPI1_MISO_GPIO_PORT         GPIOG
#define BUS_SPI1_MISO_GPIO_AF           GPIO_AF5_SPI1
#define BUS_SPI1_MOSI_GPIO_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_SPI1_MOSI_GPIO_PIN          GPIO_PIN_4
#define BUS_SPI1_MOSI_GPIO_PORT         GPIOG
#define BUS_SPI1_MOSI_GPIO_AF           GPIO_AF5_SPI1

#define BUS_SPI1_TIMEOUT                10000UL
#endif /* HAL_SPI_MODULE_ENABLED */
/**
  * @}
  */

/** @addtogroup STM32L562E-DK_BUS_Exported_Variables STM32L562E-DK BUS Exported Variables
  * @{
  */
#if defined(HAL_I2C_MODULE_ENABLED)
extern I2C_HandleTypeDef hbus_i2c1;
#endif /* HAL_I2C_MODULE_ENABLED */

#if defined(HAL_SPI_MODULE_ENABLED)
extern SPI_HandleTypeDef hbus_spi1;
#endif /* HAL_SPI_MODULE_ENABLED */
/**
  * @}
  */

/** @addtogroup STM32L562E-DK_BUS_Exported_Functions STM32L562E-DK BUS Exported Functions
  * @{
  */
int32_t BSP_GetTick(void);

#if defined(HAL_I2C_MODULE_ENABLED)
int32_t BSP_I2C1_Init(void);
int32_t BSP_I2C1_DeInit(void);
int32_t BSP_I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_IsReady(uint16_t DevAddr, uint32_t Trials);

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C1_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C1_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);
#endif /* HAL_I2C_MODULE_ENABLED */

#if defined(HAL_SPI_MODULE_ENABLED)
int32_t BSP_SPI1_Init(void);
int32_t BSP_SPI1_DeInit(void);
int32_t BSP_SPI1_Send(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI1_Recv(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
int32_t BSP_SPI1_RegisterDefaultMspCallbacks(void);
int32_t BSP_SPI1_RegisterMspCallbacks(BSP_SPI_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef *hSpi, uint32_t baudrate_presc);
#endif /* HAL_SPI_MODULE_ENABLED */
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
