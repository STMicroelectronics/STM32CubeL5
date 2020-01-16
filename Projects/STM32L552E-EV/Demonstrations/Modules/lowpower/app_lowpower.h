/**
  ******************************************************************************
  * @file    app_lowpower.c
  * @author  MCD Application Team
  * @brief   Low power application interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#undef GLOBAL
#ifdef __APP_LOWPOWER_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "k_module.h"

/* external defines ----------------------------------------------------------*/
#define STANDBY                             0x01u
#define STOP                                0x02u
#define SLEEP                               0x03u
#define RUN_SMPS_BYPASS                     0x04u
#define RUN_SMPS_HP                         0x05u
#define RUN_SMPS_LP                         0x06u

/* External variables --------------------------------------------------------*/
GLOBAL const K_ModuleItem_Typedef ModuleLowPower;

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LowPowerIddMeasureDeInit(void);
uint32_t LowPowerIddGetValue(void);
void LowPowerExitDisplay(uint32_t Mode, uint32_t iddValue);

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
