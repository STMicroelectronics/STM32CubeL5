/**
  ******************************************************************************
  * @file    stm32l562e_discovery_lcd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l562e_discovery_lcd.c driver.
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
#ifndef STM32L562E_DISCOVERY_LCD_H
#define STM32L562E_DISCOVERY_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l562e_discovery_conf.h"
#include "stm32l562e_discovery_errno.h"
#include "../Components/Common/lcd.h"
#include "../Components/st7789h2/st7789h2.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L562E-DK
  * @{
  */

/** @addtogroup STM32L562E-DK_LCD
  * @{
  */

/** @defgroup STM32L562E-DK_LCD_Exported_Constants STM32L562E-DK LCD Exported Constants
  * @{
  */
/* LCD instances */
#define LCD_INSTANCES_NBR 1U

/* LCD orientations */
#define LCD_ORIENTATION_PORTRAIT          0U
#define LCD_ORIENTATION_LANDSCAPE         1U
#define LCD_ORIENTATION_PORTRAIT_ROT180   2U
#define LCD_ORIENTATION_LANDSCAPE_ROT180  3U

/* LCD colors */
#define LCD_COLOR_BLUE          0x001FU
#define LCD_COLOR_GREEN         0x07E0U
#define LCD_COLOR_RED           0xF800U
#define LCD_COLOR_CYAN          0x07FFU
#define LCD_COLOR_MAGENTA       0xF81FU
#define LCD_COLOR_YELLOW        0xFFE0U
#define LCD_COLOR_LIGHTBLUE     0x841FU
#define LCD_COLOR_LIGHTGREEN    0x87F0U
#define LCD_COLOR_LIGHTRED      0xFC10U
#define LCD_COLOR_LIGHTCYAN     0x87FFU
#define LCD_COLOR_LIGHTMAGENTA  0xFC1FU
#define LCD_COLOR_LIGHTYELLOW   0xFFF0U
#define LCD_COLOR_DARKBLUE      0x0010U
#define LCD_COLOR_DARKGREEN     0x0400U
#define LCD_COLOR_DARKRED       0x8000U
#define LCD_COLOR_DARKCYAN      0x0410U
#define LCD_COLOR_DARKMAGENTA   0x8010U
#define LCD_COLOR_DARKYELLOW    0x8400U
#define LCD_COLOR_WHITE         0xFFFFU
#define LCD_COLOR_LIGHTGRAY     0xD69AU
#define LCD_COLOR_GRAY          0x8410U
#define LCD_COLOR_DARKGRAY      0x4208U
#define LCD_COLOR_BLACK         0x0000U
#define LCD_COLOR_BROWN         0xA145U
#define LCD_COLOR_ORANGE        0xFD20U
/**
  * @}
  */


/** @defgroup STM32L562E-DK_LCD_Exported_Types STM32L562E-DK LCD Exported Types
  * @{
  */
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSRAM_CallbackTypeDef  pMspFmcInitCb;
  pSRAM_CallbackTypeDef  pMspFmcDeInitCb;
} BSP_LCD_Cb_t;
#endif /* (USE_HAL_SRAM_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @defgroup STM32L562E-DK_LCD_Exported_Variables STM32L562E-DK LCD Exported Variables
  * @{
  */
extern SRAM_HandleTypeDef hlcd_sram[LCD_INSTANCES_NBR];

extern void      *Lcd_CompObj[LCD_INSTANCES_NBR];
extern LCD_Drv_t *Lcd_Drv[LCD_INSTANCES_NBR];
/**
  * @}
  */

/** @defgroup STM32L562E-DK_LCD_Exported_Functions STM32L562E-DK LCD Exported Functions
  * @{
  */
int32_t  BSP_LCD_Init(uint32_t Instance, uint32_t Orientation);
int32_t  BSP_LCD_DeInit(uint32_t Instance);
int32_t  BSP_LCD_DisplayOn(uint32_t Instance);
int32_t  BSP_LCD_DisplayOff(uint32_t Instance);
int32_t  BSP_LCD_SetBrightness(uint32_t Instance, uint32_t Brightness);
int32_t  BSP_LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness);
int32_t  BSP_LCD_GetXSize(uint32_t Instance, uint32_t *Xsize);
int32_t  BSP_LCD_GetYSize(uint32_t Instance, uint32_t *Ysize);
int32_t  BSP_LCD_SetActiveLayer(uint32_t Instance, uint32_t LayerIndex);
int32_t  BSP_LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp);
int32_t  BSP_LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
int32_t  BSP_LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t  BSP_LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t  BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
int32_t  BSP_LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
int32_t  BSP_LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Color);
int32_t  BSP_LCD_GetFormat(uint32_t Instance, uint32_t *Format);

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
int32_t  BSP_LCD_RegisterDefaultMspCallbacks(uint32_t Instance);
int32_t  BSP_LCD_RegisterMspCallbacks(uint32_t Instance, BSP_LCD_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_FMC_BANK1_Init(SRAM_HandleTypeDef *hSram);
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

#endif /* STM32L562E_DISCOVERY_LCD_H */
