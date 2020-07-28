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
#include "region_defs.h"
#include "low_level_flash.h"
static struct flash_range access_vect[] =
{
#if !defined(MCUBOOT_PRIMARY_ONLY)
  { S_IMAGE_SECONDARY_PARTITION_OFFSET, S_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_PARTITION_SIZE - 1}
#else
  { NS_IMAGE_PRIMARY_PARTITION_OFFSET, NS_IMAGE_PRIMARY_PARTITION_OFFSET+ FLASH_NS_PARTITION_SIZE - 1}
#endif /* !defined(MCUBOOT_PRIMARY_ONLY) */
};

struct low_level_device FLASH0_DEV =
{
  .erase = { .nb = sizeof(access_vect) / sizeof(struct flash_range), .range = access_vect},
  .write = { .nb = sizeof(access_vect) / sizeof(struct flash_range), .range = access_vect},
};
