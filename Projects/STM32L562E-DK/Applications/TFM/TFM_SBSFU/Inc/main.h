/**
  ******************************************************************************
  * @file    Templates/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"
#include "bl2.h"
#define RTC_CLOCK_SOURCE_LSE


#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif /* RTC_CLOCK_SOURCE_LSE */



/* Static protection checking  Flag */
#define TFM_WRP_PROTECT_ENABLE  /*!< Write Protection  */
#define TFM_SECURE_USER_PROTECT_ENABLE /*!< HDP protection   */

/* by Default HDP is activated in jumper function compiled with optimized flag
to ensure that the fetch of branch instruction is done before HDP is activated*/
/* to activate HDP with RSS service use with this flag **/
#define TFM_USE_RSS_SERVICE
#ifdef TFM_USE_RSS_SERVICE
/* according to RSS library present activate the following flags */
#define STM32L5_CUT2_0
#endif /* TFM_USE_RSS_SERVICE */

#define TFM_OB_SEC_PROTECT_ENABLE /*!< Secure Area for Flash  */
#define TFM_OB_RDP_LEVEL_VALUE OB_RDP_LEVEL_1 /*!< RDP level */
#define TFM_OB_BOOT_SEC_ENABLE /*!< Secure Boot address  */
#define TFM_SECURE_USER_SRAM2_ERASE_AT_RESET /*!< SRAM2 clear at Reset  */
#ifndef TFM_DEV_MODE
#define TFM_OB_BOOT_LOCK 1 /*!< BOOT Lock expected value  */
#endif
/* run time protection */

#define TFM_FLASH_PRIVONLY_ENABLE  /*!< Flash Command in Privileged only  */
#define TFM_WRP_BL2_SHARED_DATA    /*!< TFM_SBSFU set BL2 shared Area  as write protected before jumping in TFM_Appli secure    */
#define TFM_BOOT_MPU_PROTECTION    /*!< TFM_SBSFU uses MPU to prevent execution outside of TFM_boot code  */


/* flag  use in debug only*/
#ifdef TFM_DEV_MODE
#define TFM_OB_BOOT_LOCK 0 /*!< BOOT Lock expected value  */
#define TFM_ENABLE_SET_OB /*!< Option bytes are set by TFM_boot when not correctly set  */
#define TFM_ERROR_HANDLER_NON_SECURE /*!< Error handler is in Non Secure , this allows regression without jumping   */
//#define TFM_HUK_RANDOM_GENERATION
#endif
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  TFM_SUCCESS = 0U,
  TFM_FAILED
} TFM_ErrorStatus;



/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
void mpu_config_appli(void);
#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
