/**
  ******************************************************************************
  * @file    boot_hal_imagevalid.h
  * @author  MCD Application Team
  * @brief   Header for image double validation services in boot_hal.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#ifndef BOOT_HAL_IMAGEVALID_H
#define BOOT_HAL_IMAGEVALID_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
#include "flash_layout.h"
extern uint8_t ImageValidEnable;
extern uint8_t ImageValidIndex;
extern uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER];
#define IMAGE_VALID   (uint8_t)(0x55)
#define IMAGE_INVALID (uint8_t)(0x82)
#endif

#ifdef __cplusplus
}
#endif

#endif /* BOOT_HAL_IMAGEVALID_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
