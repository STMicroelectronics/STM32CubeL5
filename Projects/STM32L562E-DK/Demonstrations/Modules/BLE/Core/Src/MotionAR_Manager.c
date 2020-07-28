/**
  ******************************************************************************
  * @file    MotionAR_Manager.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   This file includes activity recognition interface functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <limits.h>
#include "TargetFeatures.h"

/* Code for MotionAR integration - Start Section */

/* Imported Variable -------------------------------------------------------------*/
extern float sensitivity_Mul;

/* exported Variable -------------------------------------------------------------*/
MAR_output_t ActivityCode = MAR_NOACTIVITY;


/* Private defines -----------------------------------------------------------*/

/** @addtogroup  Drv_Sensor      Drv_Sensor
  * @{
  */

/** @addtogroup Drv_MotionAR    Drv_MotionAR
  * @{
  */

/* Exported Functions --------------------------------------------------------*/
/**
* @brief  Run activity recognition algorithm. This function collects and scale data
* from accelerometer and calls the Activity Recognition Algo
* @param  SensorAxesRaw_t ACC_Value_Raw Acceleration value (x/y/z)
* @retval None
*/
void MotionAR_manager_run(MOTION_SENSOR_AxesRaw_t ACC_Value_Raw, uint32_t TimeStamp)
{
  MAR_input_t iDataIN;

  iDataIN.acc_x = ACC_Value_Raw.x * sensitivity_Mul;
  iDataIN.acc_y = ACC_Value_Raw.y * sensitivity_Mul;
  iDataIN.acc_z = ACC_Value_Raw.z * sensitivity_Mul;

  MotionAR_Update(&iDataIN, &ActivityCode, (long int)(TimeStamp & LONG_MAX));
}

/**
* @brief  Initialises MotionAR algorithm
* @param  None
* @retval None
*/
void MotionAR_manager_init(void)
{
  char LibVersion[36];
  char acc_orientation[3];

  MotionAR_Initialize();
  MotionAR_GetLibVersion(LibVersion);

  acc_orientation[0] ='n';
  acc_orientation[1] ='w';
  acc_orientation[2] ='d';

  MotionAR_SetOrientation_Acc(acc_orientation);

  TargetBoardFeatures.MotionARIsInitalized=1;
  ALLMEMS1_PRINTF("Initialized %s\r\n", LibVersion);
}

/**
 * @}
 */ /* end of group  Drv_MotionAR        Drv_MotionAR*/

/**
 * @}
 */ /* end of group Drv_Sensor          Drv_Sensor*/

/* Code for MotionAR integration - End Section */
/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
