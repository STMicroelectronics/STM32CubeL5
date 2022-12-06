/**
  ******************************************************************************
  * @file    boot_hal_cfg.h
  * @author  MCD Application Team
  * @brief   File fixing configuration flag specific for STM32L5xx platform
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOT_HAL_CFG_H
#define BOOT_HAL_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"
#define RTC_CLOCK_SOURCE_LSE


#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif /* RTC_CLOCK_SOURCE_LSE */

/* ICache */
#define TFM_ICACHE_ENABLE /*!< Instruction cache enable */

/* Static protection checking  Flag */
#define TFM_WRP_PROTECT_ENABLE  /*!< Write Protection  */
#define TFM_HDP_PROTECT_ENABLE /*!< HDP protection   */
#define TFM_OB_RDP_LEVEL_VALUE OB_RDP_LEVEL_1 /*!< RDP level */
#define TFM_SECURE_USER_SRAM2_ERASE_AT_RESET /*!< SRAM2 clear at Reset  */
#ifdef TFM_DEV_MODE
#define TFM_OB_BOOT_LOCK 0 /*!< BOOT Lock expected value  */
#else
#define TFM_OB_BOOT_LOCK 1 /*!< BOOT Lock expected value  */
#define TFM_NSBOOT_CHECK_ENABLE  /*!<  NSBOOTADD0 and NSBOOTADD1 must be set to SBSFU_Boot Vector  */
#endif /* TFM_DEV_MODE */
/* run time protection */
#define TFM_FLASH_PRIVONLY_ENABLE  /*!< Flash Command in Privileged only  */
#define TFM_BOOT_MPU_PROTECTION    /*!< SBSFU_Boot uses MPU to prevent execution outside of SBSFU_Boot code  */

/* flag  use in dev mode */
#ifdef TFM_DEV_MODE
#define TFM_ENABLE_SET_OB /*!< Option bytes are set by SBSFU_Boot when not correctly set  */
#define TFM_ERROR_HANDLER_NON_SECURE /*!< Error handler is in Non Secure , this allows regression without jumping   */
#endif /* TFM_DEV_MODE */
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  TFM_SUCCESS = 0U,
  TFM_FAILED
} TFM_ErrorStatus;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
#endif /* BOOT_HAL_CFG_H */
