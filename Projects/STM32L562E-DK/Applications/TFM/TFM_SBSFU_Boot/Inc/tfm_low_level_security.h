/**
  ******************************************************************************
  * @file    tfm_low_level_security.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update security
  *          low level interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TFM_LOW_LEVEL_SECURITY_H
#define TFM_LOW_LEVEL_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bootutil/bootutil_log.h"

/** @addtogroup TFM Secure Secure Boot / Firmware Update
  * @{
  */

/** @addtogroup TFM_LOW_LEVEL
  * @{
  */
/** @defgroup TFM_LOW_LEVEL_SECURITY Security Low Level define
  * @{
  */
#define SRAM2_PAGE_SIZE (0x400)
/**
  * @}
  */
/** @defgroup TFM_LOW_LEVEL_SECURITY Security Low Level Interface
  * @{
  */
/** @defgroup TFM_SECURITY_Configuration Security Configuration
  * @{
  */


/**
  * @}
  */

/** @defgroup TFM_SECURITY_Exported_Constants Exported Constants
  * @{
  */

/** @defgroup TFM_SECURITY_Exported_Constants_BOOL SFU Bool definition
  * @{
  */
typedef enum
{
  TFM_FALSE = 0U,
  TFM_TRUE = !TFM_FALSE
} TFM_BoolTypeDef;

/**
  * @}
  */

/** @defgroup TFM_SECURITY_Exported_Constants_State TFM Functional State definition
  * @{
  */
typedef enum
{
  TFM_DISABLE = 0U,
  TFM_ENABLE = !TFM_DISABLE
} TFM_FunctionalState;

/**
  * @}
  */

/** @defgroup TFM_SECURITY_Exported_Functions Exported Functions
  * @{
  */
void    TFM_LL_SECU_ApplyRunTimeProtections(void);
void    TFM_LL_SECU_CheckStaticProtections(void);
static inline void TFM_LL_SECU_ApplyWRP_SRAM2(uint32_t offset, uint32_t len)
{
	uint32_t start_offset = ((offset - SRAM2_BASE_S)/ SRAM2_PAGE_SIZE);
	uint32_t end_offset = start_offset + (len -1)/SRAM2_PAGE_SIZE;
	uint32_t index;
	__IO uint32_t *pt;
	__IO uint32_t *SRAM2_CFG[2]={&SYSCFG_S->SWPR, &SYSCFG_S->SWPR2};
	uint32_t val[2]={0, 0};

        __HAL_RCC_SYSCFG_CLK_ENABLE();
#if 0
	BOOT_LOG_INF("SRAM2 write protection [0x%x, 0x%x] : %d %d", offset, len, start_offset, end_offset);
#endif
  for (index = start_offset; index <= end_offset; index ++)
  {
		val[(index > 31) ? 1 : 0]|= (1 << ( (index > 31) ? (index -32) : index));
  }
	for(index = 0; index < 2; index ++)
	{
		pt = SRAM2_CFG[index];
#if 0
	  BOOT_LOG_INF("SRAM2 write protection [0x%x] : %x", pt, val[index]);
#endif
		*pt = val[index];
	}
}
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

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* TFM_LOW_LEVEL_SECURITY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
