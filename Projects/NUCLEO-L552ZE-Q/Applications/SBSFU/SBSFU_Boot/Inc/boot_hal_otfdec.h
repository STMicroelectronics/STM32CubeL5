/**
  ******************************************************************************
  * @file    boot_otfdec.h
  * @author  MCD Application Team
  * @brief   Header for otfdec services in boot_hal.c module
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
#ifndef BOOT_HAL_OTFDEC_H
#define BOOT_HAL_OTFDEC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @defgroup BOOT_OTFDEC_Exported_Functions Exported Functions
  * @{
  */
int otfdec_invalidate_key(void);
int otfdec_write_key(const void *data);
int otfdec_is_key_valid(void);
int otfdec_read_key(void *data);
int32_t otfdec_config(void);

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* BOOT_HAL_OTFDEC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
