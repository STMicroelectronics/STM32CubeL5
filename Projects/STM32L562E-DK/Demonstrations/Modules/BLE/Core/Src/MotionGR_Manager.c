/**
  ******************************************************************************
  * @file    MotionGR_Manager.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   This file includes gesture recognition interface functions
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

/* Includes ------------------------------------------------------------------*/
#include "TargetFeatures.h"

/* Code for MotionGR integration - Start Section */

/* Imported Variable -------------------------------------------------------------*/
extern float sensitivity_Mul;

/* exported Variable -------------------------------------------------------------*/
MGR_output_t GestureRecognitionCode = MGR_NOGESTURE;


/* Private defines -----------------------------------------------------------*/

/** @addtogroup  Drv_Sensor      Drv_Sensor
  * @{
  */

/** @addtogroup Drv_MotionGR    Drv_MotionGR
  * @{
  */

/* Exported Functions --------------------------------------------------------*/
/**
* @brief  Run gesture recognition algorithm. This function collects and scale data
* from accelerometer and calls the Gesture Recognition Algo
* @param  MOTION_SENSOR_AxesRaw_t ACC_Value_Raw Acceleration value (x/y/z)
* @retval None
*/
void MotionGR_manager_run(MOTION_SENSOR_AxesRaw_t ACC_Value_Raw)
{
  MGR_input_t iDataIN;

  iDataIN.AccX = ACC_Value_Raw.x * sensitivity_Mul;
  iDataIN.AccY = ACC_Value_Raw.y * sensitivity_Mul;
  iDataIN.AccZ = ACC_Value_Raw.z * sensitivity_Mul;

  MotionGR_Update(&iDataIN, &GestureRecognitionCode);
}

/**
* @brief  Initialises MotionGR algorithm
* @param  None
* @retval None
*/
void MotionGR_manager_init(void)
{
  char LibVersion[36];
  char acc_orientation[3];

  MotionGR_Initialize();
  MotionGR_GetLibVersion(LibVersion);

  acc_orientation[0] ='n';
  acc_orientation[1] ='w';
  acc_orientation[2] ='d';

  MotionGR_SetOrientation_Acc(acc_orientation);

  TargetBoardFeatures.MotionGRIsInitalized=1;
  ALLMEMS1_PRINTF("Initialized %s\r\n", LibVersion);
}

/**
 * @}
 */ /* end of group  Drv_MotionGR        Drv_MotionGR*/

/**
 * @}
 */ /* end of group Drv_Sensor          Drv_Sensor*/

/* Code for MotionGR integration - End Section */
