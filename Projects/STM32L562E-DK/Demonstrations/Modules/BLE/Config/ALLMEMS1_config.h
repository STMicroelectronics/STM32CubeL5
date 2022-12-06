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
