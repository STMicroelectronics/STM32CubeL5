/**
  ******************************************************************************
  * @file    ALLMEMS1_config.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   FP-SNS-ALLMEMS1 configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ALLMEMS1_CONFIG_H
#define __ALLMEMS1_CONFIG_H

/* Exported define ------------------------------------------------------------*/

/* Define the ALLMEMS1 MAC address, otherwise it will create a Unique MAC */
//#define MAC_ALLMEMS1 0xFF, 0xEE, 0xDD, 0xAA, 0xAA, 0xAA

#ifndef MAC_ALLMEMS1
/* For creating one MAC related to STM32 UID, Otherwise the BLE will use it's random MAC */
//#define MAC_STM32UID_ALLMEMS1
#endif /* MAC_ALLMEMS1 */

/*************** Debug Defines ******************/
/* For enabling the printf on UART */
//#define ALLMEMS1_ENABLE_PRINTF

/* For enabling connection and notification subscriptions debug */
//#define ALLMEMS1_DEBUG_CONNECTION

/* For enabling transmission for notified services */
//#define ALLMEMS1_DEBUG_NOTIFY_TRAMISSION

/*************** Don't Change the following defines *************/

/**************************************/
/*  Remapping instance sensor defines */
/**************************************/
/* Motion Sensor Instance */
#define LSM6DS0_0                0
#define ACCELERO_INSTANCE        LSM6DS0_0
#define GYRO_INSTANCE            LSM6DS0_0

/*************************************/
/********************************/
/*  Remapping APIsensor defines */
/********************************/
#define MOTION_SENSOR_AxesRaw_t  BSP_MOTION_SENSOR_AxesRaw_t

/* Package Name */
#define ALLMEMS1_PACKAGENAME "FP-SNS-ALLMEMS1"

/* Package Version only numbers 0->9 */
#define ALLMEMS1_VERSION_MAJOR '4'
#define ALLMEMS1_VERSION_MINOR '0'
#define ALLMEMS1_VERSION_PATCH '0'

/* Define the ALLMEMS1 Name MUST be 7 char long */
#define NAME_BLUEMS 'M','B','1','3','7','3','C'

#ifdef ALLMEMS1_ENABLE_PRINTF
  #define ALLMEMS1_PRINTF  printf
#else
  #define ALLMEMS1_PRINTF(...)
#endif

/* STM32L562xx Unique ID */
#define STM32_UUID ((uint32_t *)UID_BASE)

/* STM32 MCU_ID */
#define STM32_MCU_ID ((uint32_t *)DBGMCU_BASE)
/* Control Section */

#endif /* __ALLMEMS1_CONFIG_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
