/**
******************************************************************************
* @file    stm32l562e_discovery_motion_sensors_ex.c
* @author  MEMS Software Solutions Team
* @brief   This file provides a set of extended functions needed to manage the motion sensors
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
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
#include "stm32l562e_discovery_motion_sensors_ex.h"

/**
 * @brief  Enable 6D Orientation
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @note   This function sets the LSM6DSO accelerometer ODR to 416Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_6D_Orientation(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Enable_6D_Orientation(Motion_Sensor_CompObj[Instance], (LSM6DSO_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable 6D Orientation
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_6D_Orientation(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_6D_Orientation(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the 6D orientation threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_6D_Orientation_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_6D_Orientation_Threshold(Motion_Sensor_CompObj[Instance], Threshold) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the free fall detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @note   This function sets the LSM6DSO accelerometer ODR to 416Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Free_Fall_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Enable_Free_Fall_Detection(Motion_Sensor_CompObj[Instance], (LSM6DSO_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the free fall detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Free_Fall_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_Free_Fall_Detection(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the free fall detection threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Free_Fall_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Free_Fall_Threshold(Motion_Sensor_CompObj[Instance], Threshold) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the free fall detection duration
 * @param  Instance the device instance
 * @param  Duration the duration to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Free_Fall_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Free_Fall_Duration(Motion_Sensor_CompObj[Instance], Duration) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the pedometer detection
 * @param  Instance the device instance
 * @note   This function sets the LSM6DSO accelerometer ODR to 26Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Pedometer(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  if (IntPin != BSP_MOTION_SENSOR_INT1_PIN)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Enable_Pedometer(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the pedometer detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Pedometer(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_Pedometer(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Reset step counter (available only for LSM6DSO sensor)
 * @param  Instance the device instance
 * @note   This function sets the LSM6DSO accelerometer ODR to 26Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Reset_Step_Counter(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Step_Counter_Reset(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the single tap detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @note   This function sets the LSM6DSO accelerometer ODR to 416Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Single_Tap_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Enable_Single_Tap_Detection(Motion_Sensor_CompObj[Instance], (LSM6DSO_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the single tap detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Single_Tap_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_Single_Tap_Detection(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the double tap detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @note   This function sets the LSM6DSO accelerometer ODR to 416Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Double_Tap_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Enable_Double_Tap_Detection(Motion_Sensor_CompObj[Instance], (LSM6DSO_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the double tap detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Double_Tap_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_Double_Tap_Detection(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the tap threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Tap_Threshold(Motion_Sensor_CompObj[Instance], Threshold) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the tap shock time
 * @param  Instance the device instance
 * @param  Time the tap shock time to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Shock_Time(uint32_t Instance, uint8_t Time)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Tap_Shock_Time(Motion_Sensor_CompObj[Instance], Time) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the tap quiet time
 * @param  Instance the device instance
 * @param  Time the tap quiet time to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Quiet_Time(uint32_t Instance, uint8_t Time)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Tap_Quiet_Time(Motion_Sensor_CompObj[Instance], Time) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the tap duration time
 * @param  Instance the device instance
 * @param  Time the tap duration time to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Duration_Time(uint32_t Instance, uint8_t Time)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Tap_Duration_Time(Motion_Sensor_CompObj[Instance], Time) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the tilt detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @note   This function sets the LSM6DSO accelerometer ODR to 26Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Tilt_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Enable_Tilt_Detection(Motion_Sensor_CompObj[Instance], (LSM6DSO_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the tilt detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Tilt_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_Tilt_Detection(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the wake up detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @note   This function sets the LSM6DSO accelerometer ODR to 416Hz and the LSM6DSO accelerometer full scale to 2g
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Wake_Up_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Enable_Wake_Up_Detection(Motion_Sensor_CompObj[Instance], (LSM6DSO_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the wake up detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Wake_Up_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_Wake_Up_Detection(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the wake up detection threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Wake_Up_Threshold(Motion_Sensor_CompObj[Instance], Threshold) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the wake up detection duration
 * @param  Instance the device instance
 * @param  Duration the duration to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Wake_Up_Duration(Motion_Sensor_CompObj[Instance], Duration) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the inactivity detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Inactivity_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the inactivity detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Inactivity_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Disable_Inactivity_Detection(Motion_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the sleep duration
 * @param  Instance the device instance
 * @param  Duration the duration to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Sleep_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Set_Sleep_Duration(Motion_Sensor_CompObj[Instance], Duration) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the status of all hardware events
 * @param  Instance the device instance
 * @param  Status the pointer to the status of all hardware events
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_Event_Status(uint32_t Instance, BSP_MOTION_SENSOR_Event_Status_t *Status)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      if (LSM6DSO_ACC_Get_Event_Status(Motion_Sensor_CompObj[Instance], (LSM6DSO_Event_Status_t *)(void *)Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the status of data ready bit
 * @param  Instance the device instance
 * @param  Function Motion sensor function. Could be:
 *         - MOTION_ACCELERO or MOTION_GYRO for instance LSM6DSO_0
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if ((Function & MOTION_ACCELERO) == MOTION_ACCELERO)
      {
        if (LSM6DSO_ACC_Get_DRDY_Status(Motion_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else if ((Function & MOTION_GYRO) == MOTION_GYRO)
      {
        if (LSM6DSO_GYRO_Get_DRDY_Status(Motion_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation XL
 * @param  Instance the device instance
 * @param  xl the pointer to the 6D orientation XL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XL(uint32_t Instance, uint8_t *xl)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Get_6D_Orientation_XL(Motion_Sensor_CompObj[Instance], xl) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation XH
 * @param  Instance the device instance
 * @param  xh the pointer to the 6D orientation XH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XH(uint32_t Instance, uint8_t *xh)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Get_6D_Orientation_XH(Motion_Sensor_CompObj[Instance], xh) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation YL
 * @param  Instance the device instance
 * @param  yl the pointer to the 6D orientation YL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YL(uint32_t Instance, uint8_t *yl)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Get_6D_Orientation_YL(Motion_Sensor_CompObj[Instance], yl) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation YH
 * @param  Instance the device instance
 * @param  yh the pointer to the 6D orientation YH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YH(uint32_t Instance, uint8_t *yh)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Get_6D_Orientation_YH(Motion_Sensor_CompObj[Instance], yh) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation ZL
 * @param  Instance the device instance
 * @param  zl the pointer to the 6D orientation ZL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZL(uint32_t Instance, uint8_t *zl)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Get_6D_Orientation_ZL(Motion_Sensor_CompObj[Instance], zl) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation ZH
 * @param  Instance the device instance
 * @param  zh the pointer to the 6D orientation ZH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZH(uint32_t Instance, uint8_t *zh)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Get_6D_Orientation_ZH(Motion_Sensor_CompObj[Instance], zh) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get step count
 * @param  Instance the device instance
 * @param  StepCount number of steps
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_Step_Count(uint32_t Instance, uint16_t *StepCount)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_ACC_Get_Step_Count(Motion_Sensor_CompObj[Instance], StepCount) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data pointer where the value is written to
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_Read_Reg(Motion_Sensor_CompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data value to be written
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_Write_Reg(Motion_Sensor_CompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get number of unread FIFO samples
 * @param  Instance the device instance
 * @param  NumSamples number of unread FIFO samples
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Num_Samples(uint32_t Instance, uint16_t *NumSamples)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_FIFO_Get_Num_Samples(Motion_Sensor_CompObj[Instance], NumSamples) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get FIFO full status
 * @param  Instance the device instance
 * @param  Status FIFO full status
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Full_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_FIFO_Get_Full_Status(Motion_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set FIFO full interrupt on INT1 pin
 * @param  Instance the device instance
 * @param  Status FIFO full interrupt on INT1 pin
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_INT1_FIFO_Full(uint32_t Instance, uint8_t Status)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_FIFO_Set_INT1_FIFO_Full(Motion_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set FIFO watermark level
 * @param  Instance the device instance
 * @param  Watermark FIFO watermark level
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_Watermark_Level(uint32_t Instance, uint16_t Watermark)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_FIFO_Set_Watermark_Level(Motion_Sensor_CompObj[Instance], Watermark) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set FIFO stop on watermark
 * @param  Instance the device instance
 * @param  Status FIFO stop on watermark status
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_Stop_On_Fth(uint32_t Instance, uint8_t Status)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_FIFO_Set_Stop_On_Fth(Motion_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set FIFO mode
 * @param  Instance the device instance
 * @param  Mode FIFO mode
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_Mode(uint32_t Instance, uint8_t Mode)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if (LSM6DSO_FIFO_Set_Mode(Motion_Sensor_CompObj[Instance], Mode) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set accelero self-test
 * @param  Instance the device instance
 * @param  Function Motion sensor function. Could be:
 *         - MOTION_GYRO or MOTION_ACCELERO for instance LSM6DSO_0
 * @param  Data FIFO single axis data
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_SelfTest(uint32_t Instance, uint32_t Function, uint8_t Status)
{
  int32_t ret;

  switch (Instance)
  {
    case LSM6DSO_0:
      if ((Function & MOTION_ACCELERO) == MOTION_ACCELERO)
      {
        if (LSM6DSO_ACC_Set_SelfTest(Motion_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else if ((Function & MOTION_GYRO) == MOTION_GYRO)
      {
        if (LSM6DSO_GYRO_Set_SelfTest(Motion_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else
      {
        ret = BSP_ERROR_WRONG_PARAM;
      }
      break;

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
