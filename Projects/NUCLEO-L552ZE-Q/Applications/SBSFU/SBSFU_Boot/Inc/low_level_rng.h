/**
  ******************************************************************************
  * @file    low_level_rng.h
  * @author  MCD Application Team
  * @brief   Header for low_level_rng.c module
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
#ifndef LOW_LEVEL_RNG_H
#define LOW_LEVEL_RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mbedtls/entropy.h"

/* Exported functions --------------------------------------------------------*/
void RNG_DeInit(void);
void RNG_Init(void);
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);
void RNG_GetBytes(uint8_t *output, size_t length, size_t *output_length);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_RNG_H */