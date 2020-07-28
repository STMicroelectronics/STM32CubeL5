/**
  ******************************************************************************
  * @file    low_level_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#include "flash_layout.h"
#include "low_level_flash.h"
static struct flash_range erase_vect[] =
{
#if !defined(MCUBOOT_PRIMARY_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
#if  defined(TFM_EXTERNAL_FLASH_ENABLE)
  { FLASH_AREA_OTFDEC_OFFSET, FLASH_AREA_OTFDEC_OFFSET + FLASH_AREA_OTFDEC_SIZE - 1},
  { FLASH_AREA_0_OFFSET, FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE - 1}
#else
  { FLASH_AREA_0_OFFSET, FLASH_AREA_2_OFFSET + FLASH_PARTITION_SIZE - 1}
#endif /* TFM_EXTERNAL_FLASH_ENABLE*/
#else
  { FLASH_AREA_0_OFFSET, FLASH_AREA_0_OFFSET + FLASH_PARTITION_SIZE - 1}
#endif /* MCUBOOT_PRIMARY_ONLY */
};
static struct flash_range write_vect[] =
{
  { FLASH_BL2_NVCNT_AREA_OFFSET, FLASH_BL2_NVCNT_AREA_OFFSET  + FLASH_BL2_NVCNT_AREA_SIZE - 1},
#if !defined(MCUBOOT_PRIMARY_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
#if  defined(TFM_EXTERNAL_FLASH_ENABLE)
  { FLASH_AREA_OTFDEC_OFFSET, FLASH_AREA_OTFDEC_OFFSET + FLASH_AREA_OTFDEC_SIZE - 1},
  { FLASH_AREA_0_OFFSET, FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE - 1}
#else
  { FLASH_AREA_0_OFFSET, FLASH_AREA_2_OFFSET + FLASH_PARTITION_SIZE - 1}
#endif /* TFM_EXTERNAL_FLASH_ENABLE */
#else
  { FLASH_AREA_0_OFFSET, FLASH_AREA_0_OFFSET + FLASH_PARTITION_SIZE - 1}
#endif /* MCUBOOT_PRIMARY_ONLY */
};
static struct flash_range secure_vect[] =
{
  { FLASH_BL2_NVCNT_AREA_OFFSET, FLASH_BL2_NVCNT_AREA_OFFSET  + FLASH_BL2_NVCNT_AREA_SIZE - 1},
#if !defined(MCUBOOT_PRIMARY_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
#if  defined(TFM_EXTERNAL_FLASH_ENABLE)
  { FLASH_AREA_OTFDEC_OFFSET, FLASH_AREA_OTFDEC_OFFSET + FLASH_AREA_OTFDEC_SIZE - 1},
#endif /* TFM_EXTERNAL_FLASH_ENABLE*/
#endif /* MCUBOOT_PRIMARY_ONLY */
  { FLASH_AREA_0_OFFSET, FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE - 1}
};

struct low_level_device FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(struct flash_range), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(struct flash_range), .range = write_vect},
  .secure = { .nb = sizeof(secure_vect) / sizeof(struct flash_range), .range = secure_vect},
  .read_error = 1
};
