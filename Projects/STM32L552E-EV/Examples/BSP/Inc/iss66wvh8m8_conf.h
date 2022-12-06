/**
  ******************************************************************************
  * @file    ISS66WVH8M8_conf_template.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the ISS66WVH8M8 OSPI memory.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ISS66WVH8M8_CONF_H
#define ISS66WVH8M8_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx.h"
#include "stm32l5xx_hal.h"

/** @addtogroup BSP
  * @{
  */
#define CONF_OSPI_RAM_ODS                         ISS66WVH8M8_CR0_DS_34   /* ISS66WVH8M8 Output Driver Strength         */

#define RW_RECOVERY_TIME                          3 /* 40ns @ 60MHz */
#define DEFAULT_INITIAL_LATENCY                   6
#define OPTIMAL_FIXED_INITIAL_LATENCY             3
#define OPTIMAL_FIXED_INITIAL_LATENCY_REG_VAL     ISS66WVH8M8_CR0_IL_3_CLOCK
#define OPTIMAL_VARIABLE_INITIAL_LATENCY          6
#define OPTIMAL_VARIABLE_INITIAL_LATENCY_REG_VAL  ISS66WVH8M8_CR0_IL_6_CLOCK

#ifdef __cplusplus
}
#endif

#endif /* ISS66WVH8M8_CONF_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
