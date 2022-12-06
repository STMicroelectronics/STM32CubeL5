/**
  ******************************************************************************
  * @file    TargetFeatures.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   Specification of the HW Features for each target platform
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
#ifndef _TARGET_FEATURES_H_
#define _TARGET_FEATURES_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "stm32l5xx_hal.h"
#include "stm32l562e_discovery.h"
#include "stm32l562e_discovery_lcd.h"
#include "stm32l562e_discovery_motion_sensors.h"
#include "stm32l562e_discovery_motion_sensors_ex.h"
#include "stm32l562e_discovery_ospi.h"
#include "stm32l562e_discovery_ts.h"
#include "stm32_lcd.h"

#include "ALLMEMS1_config.h"
#include "MetaDataManager.h"

/* Code for MotionAR integration - Start Section */
#include "MotionAR_Manager.h"
#include "motion_ar.h"
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
#include "MotionCP_Manager.h"
#include "motion_cp.h"
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
#include "MotionGR_Manager.h"
#include "motion_gr.h"
/* Code for MotionGR integration - End Section */

/* Exported defines ------------------------------------------------------- */

/* Exported macros ------------------------------------------------------- */

/* Exported types ------------------------------------------------------- */

/**
 * @brief  Target's Features data structure definition
 */
typedef struct
{
  uint8_t LedStatus;

  /* Code for MotionAR integration - Start Section */
  uint32_t MotionARIsInitalized;
  /* Code for MotionAR integration - End Section */

  /* Code for MotionCP integration - Start Section */
  uint32_t MotionCPIsInitalized;
  /* Code for MotionCP integration - End Section */

  /* Code for MotionGR integration - Start Section */
  uint32_t MotionGRIsInitalized;
  /* Code for MotionGR integration - End Section */

} TargetFeatures_t;

/* Exported variables ------------------------------------------------------- */
extern TargetFeatures_t TargetBoardFeatures;

/* Exported functions ------------------------------------------------------- */
extern void InitTargetPlatform(void);

extern void LedOnTargetPlatform(void);
extern void LedOffTargetPlatform(void);
extern void LedToggleTargetPlatform(void);

#ifdef __cplusplus
}
#endif

#endif /* _TARGET_FEATURES_H_ */
