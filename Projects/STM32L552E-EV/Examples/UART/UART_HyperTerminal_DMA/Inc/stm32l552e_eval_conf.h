/**
  ******************************************************************************
  * @file    stm32l552e_eval_conf.h
  * @author  MCD Application Team
  * @brief   STM32L552E-EV board configuration file.
  *          This file should be copied to the application folder and renamed
  *          to stm32l552e_eval_conf.h .
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
#ifndef STM32L552E_EVAL_CONF_H
#define STM32L552E_EVAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E_EVAL
  * @{
  */

/** @defgroup STM32L552E_EVAL_CONFIG STM32L552E_EVAL CONFIG
  * @{
  */

/** @defgroup STM32L552E_EVAL_CONFIG_Exported_Constants Exported Constants
  * @{
  */

/* Usage of COM feature */
#define USE_BSP_COM_FEATURE 0U
#define USE_COM_LOG         0U

/* Usage of POT feature */
#define USE_BSP_POT_FEATURE 0U

/* Usage of IO expander */
#define USE_BSP_IO_CLASS    0U

/* USBPD BSP PWR TRACE define */
#define USE_BSP_PWR_TRACE   0U

/* Button interrupt priorities */
#define BSP_BUTTON_WAKEUP_IT_PRIORITY 0x07UL  /* Default is lowest priority level */
#define BSP_BUTTON_TAMPER_IT_PRIORITY 0x07UL  /* Default is lowest priority level */

#if (USE_BSP_IO_CLASS == 1)
/* IOExpander interrupt priority */
#define BSP_IOEXPANDER_IT_PRIORITY    0x07UL  /* Default is lowest priority level */
#endif

/* Touch screen interrupt priority */
#define BSP_TS_IT_PRIORITY            0x07UL  /* Default is lowest priority level */

/* Audio interrupt priorities */
#define BSP_AUDIO_IN_IT_PRIORITY      0x07UL  /* Default is lowest priority level */
#define BSP_AUDIO_OUT_IT_PRIORITY     0x07UL  /* Default is lowest priority level */

/* SD card interrupt priority */
#define BSP_SD_IT_PRIORITY            0x07UL  /* Default is lowest priority level */

/* SRAM DMA interrupt priority */
#define BSP_SRAM_DMA_IT_PRIORITY      0x07UL  /* Default is lowest priority level */

/* OSPI RAM interrupt priority */
#define BSP_OSPI_RAM_IT_PRIORITY      0x07UL  /* Default is lowest priority level */
#define BSP_OSPI_RAM_DMA_IT_PRIORITY  0x07UL  /* Default is lowest priority level */

/* Bus frequencies */
#define BUS_I2C1_FREQUENCY            100000UL /* Frequency of I2C1 = 100 KHz */

/* Default AUDIO IN internal buffer size in 32-bit words per micro */
#define BSP_AUDIO_IN_DEFAULT_BUFFER_SIZE  2048UL   /* 2048*4 = 8Kbytes */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32L552E_EVAL_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
