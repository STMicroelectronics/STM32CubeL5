/**
  ******************************************************************************
  * @file    tfm_low_level_security_rss.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update security
  *          low level interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TFM_LOW_LEVEL_SECURITY_RSS_H
#define TFM_LOW_LEVEL_SECURITY_RSS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/** @addtogroup TFM_LOW_LEVEL_SECURITY_RSS
  * @{
  */

/** @defgroup TFM_SECURITY_SECURITY_RSS structure 
  * @{
  */
uint32_t RSSLIB_Sec_CloseExitHDP( uint32_t HdpAreaId, uint32_t VectorTableAddr );

typedef uint32_t ( *RSSLIB_sec_CloseExitHDP_t)( uint32_t hdp_area, uint32_t VectorTableAddr );

typedef struct
{
  uint32_t Reserved1[8]; /* RSS area specific*/
  RSSLIB_sec_CloseExitHDP_t RSSLIB_sec_CloseExitHDP_BL90;
  uint32_t Reserved2; /* RSS area specific*/
  RSSLIB_sec_CloseExitHDP_t RSSLIB_sec_CloseExitHDP_BL91;
}RSSLIB_STM32xxApiTable_t;
typedef struct
{
  uint32_t Reserved[16]; /* RSS area specific*/
  RSSLIB_STM32xxApiTable_t  RSSLIBApiTable; /*Pointer to the RSSlib api table within RSSLIB sysflash area*/
} STM32xx_Descriptor_t;

/**
  * @}
  */
  
/** @defgroup TFM_LOW_LEVEL_SECURITY_RSS Security Low Level RSS define
  * @{
  */
#define STM32L5_SYSFLASH_DESCRIPTOR_ADDR (0x0BF97F00U)
#define STM32L5_BOOTLOADER_VERSION_ADDR  (0x0BF97FFEU)
#define BOOTLOADER_ID (*(uint8_t*)STM32L5_BOOTLOADER_VERSION_ADDR)
 
#define RSSLIB_sec_CloseExitHDP_BL91 \
  ((RSSLIB_sec_CloseExitHDP_t )(((STM32xx_Descriptor_t *)STM32L5_SYSFLASH_DESCRIPTOR_ADDR)\
                               ->RSSLIBApiTable.RSSLIB_sec_CloseExitHDP_BL91))
#define RSSLIB_sec_CloseExitHDP_BL90 \
  ((RSSLIB_sec_CloseExitHDP_t )(((STM32xx_Descriptor_t *)STM32L5_SYSFLASH_DESCRIPTOR_ADDR)\
                               ->RSSLIBApiTable.RSSLIB_sec_CloseExitHDP_BL90))
#define BOOTLOADER_ID (*(uint8_t*)STM32L5_BOOTLOADER_VERSION_ADDR)


#define RSSLIB_HDP_AREA_Pos (0U)
#define RSSLIB_HDP_AREA_Msk ( 0x3U << RSSLIB_HDP_AREA_Pos )
#define RSSLIB_HDP_AREA1_Pos (0U)
#define RSSLIB_HDP_AREA1_Msk (0x1U << RSSLIB_HDP_AREA1_Pos )
#define RSSLIB_HDP_AREA2_Pos (1U)
#define RSSLIB_HDP_AREA2_Msk (0x1U << RSSLIB_HDP_AREA2_Pos )
/**
  * @}
  */


/**
  * @}
  */




#ifdef __cplusplus
}
#endif

#endif /* TFM_LOW_LEVEL_SECURITY_RSS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
