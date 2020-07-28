/**
  ******************************************************************************
  * @file    fw_update_app.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Firmware Update module
  *          functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#ifndef FW_UPDATE_APP_H
#define FW_UPDATE_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup FW_UPDATE Firmware Update Example
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/** @addtogroup  FW_UPDATE_Exported_Functions
  * @{
  */

void FW_UPDATE_Run(void);

/**
  * @}
  */
typedef struct
{
  uint32_t  MaxSizeInBytes;        /*!< The maximum allowed size for the FwImage in User Flash (in Bytes) */
  uint32_t  DownloadAddr;          /*!< The download address for the FwImage in UserFlash */
  uint32_t  ImageOffsetInBytes;    /*!< Image write starts at this offset */
  uint32_t  Secure;                /*!< when different from 0, access is secure */
} SFU_FwImageFlashTypeDef;

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* FW_UPDATE_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

