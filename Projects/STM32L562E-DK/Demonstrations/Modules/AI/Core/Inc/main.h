/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"
#include "stm32l562e_discovery.h"
#include "stm32l562e_discovery_lcd.h"
#include "stm32l562e_discovery_ts.h"
#include "stm32l562e_discovery_ospi.h"
#include "stm32_lcd.h"
#include "app_hcr.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//#define DEBUG
#if defined(DEBUG)
#define DBG_TRACE(__STRING__)   printf((__STRING__));
#else
#define DBG_TRACE(__STRING__)
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern __IO FlagStatus UserButtonPressed;
extern __IO FlagStatus TouchDetected;

extern TS_Init_t TsInit;

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);

#endif /* MAIN_H */
