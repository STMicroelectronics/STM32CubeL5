 /**
  ******************************************************************************
  * @file    MotionCP_Manager.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   Header for MotionCP_Manager.c
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
#ifndef _MOTIONCP_MANAGER_H_
#define _MOTIONCP_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "motion_cp.h"


/** @defgroup Drv_Sensor      Drv_Sensor
* @{
*/

/** @defgroup Drv_MotionCP            Drv_MotionCP
* @brief    This file includes Carry Position Recognition interface functions 
* @{
*/ 

/* Exported Functions Prototypes ---------------------------------------------*/
extern void MotionCP_manager_init(void);
extern void MotionCP_manager_run(MOTION_SENSOR_AxesRaw_t ACC_Value_Raw);


/**
 * @}
 */  /* end of group Drv_MotionCP */

/**
 * @}
 */  /* end of group Drv_Sensor */

#ifdef __cplusplus
}
#endif

#endif //_MOTIONCP_MANAGER_H_

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
