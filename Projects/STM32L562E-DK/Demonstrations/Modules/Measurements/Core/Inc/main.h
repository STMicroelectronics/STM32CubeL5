/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"
#include "stm32l562e_discovery.h"
#include "stm32l562e_discovery_lcd.h"
#include "stm32l562e_discovery_ts.h"
#include "stm32l562e_discovery_idd.h"
#include "stm32_lcd.h"
#include "iddmeasure.h"
/* Exported types ------------------------------------------------------------*/

/** @defgroup MMI_MAIN_Exported_Types
* @{
*/
/**
* @brief  MMI Init Mode.
*/
typedef enum
{
  MMI_INIT_POWER_ON = 0, /* Hard reset */
  MMI_INIT_RESET, /* Reset */
  MMI_INIT_STANDBY, /* Return from standy mode */
  MMI_INIT_SHUTDOWN, /* Return from shutdown mode */
  MMI_INIT_AFTER_IDD_MEASURE, /* Reset after a IDD measure */
}MMI_init_mode_t;
/**
* @}
*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern __IO FlagStatus UserButtonPressed;
extern __IO FlagStatus TouchDetected;

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
void IDD_demo(void);

#endif /* MAIN_H */
