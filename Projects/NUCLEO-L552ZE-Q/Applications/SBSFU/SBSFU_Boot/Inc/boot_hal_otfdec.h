/**
  ******************************************************************************
  * @file    boot_otfdec.h
  * @author  MCD Application Team
  * @brief   Header for otfdec services in boot_hal.c module
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
