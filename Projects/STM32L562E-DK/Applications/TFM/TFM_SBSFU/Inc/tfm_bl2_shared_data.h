/**
  ******************************************************************************
  * @file    tfm_bl2_shared_data.h
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
#ifndef TFM_BL2_SHARED_DATA_H
#define TFM_BL2_SHARED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/** @addtogroup TFM_BL2_SHARED_DATA
 * * @{
  */
/** @defgroup TFM_BL2_SHARED_DATA define
  * @{
  */
/**
  * @}
  */
/** @defgroupTFM_BL2_SHARED_DATA  Interface
  * @{
  */
/** @defgroup TFM_SECURITY_Configuration Security Configuration
  * @{
  */


/**
  * @}
  */

/** @defgroup TFM_BL2_SHARED_DATA__Exported_Constants Exported Constants
  * @{
  */

/** @defgroup TFM_BL2_SHARED_DATA__Constants_BOOL SFU Bool definition
  * @{
  */

/**
  * @}
  */

/** @defgroup TFM_BL2_SHARED_DATA_Constants_State TFM Functional State definition
  * @{
  */

/**
  * @}
  */

/** @defgroup TFM_BL2_SHARED_DATA_Exported_Functions Exported Functions
  * @{
  */
void TFM_BL2_CopySharedData(void);
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

#endif /* TFM_BL2_SHARED_DATA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
