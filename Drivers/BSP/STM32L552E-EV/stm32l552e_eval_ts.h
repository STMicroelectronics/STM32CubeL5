/**
  ******************************************************************************
  * @file    stm32l552e_eval_ts.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l552e_eval_ts.c driver.
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
#ifndef STM32L552E_EVAL_TS_H
#define STM32L552E_EVAL_TS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l552e_eval_conf.h"
#include "stm32l552e_eval_errno.h"
#include "../Components/Common/ts.h"
#include "../Components/stmpe811/stmpe811.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E-EV
  * @{
  */

/** @addtogroup STM32L552E-EV_TS
  * @{
  */

/** @defgroup STM32L552E-EV_TS_Exported_Constants STM32L552E-EV TS Exported Constants
  * @{
  */
/* TS instances */
#define TS_INSTANCES_NBR 1U

/* TS orientations */
#define TS_ORIENTATION_PORTRAIT          0U
#define TS_ORIENTATION_LANDSCAPE         1U
#define TS_ORIENTATION_PORTRAIT_ROT180   2U
#define TS_ORIENTATION_LANDSCAPE_ROT180  3U

/* TS I2C address */
#define TS_I2C_ADDRESS                   0x82U
/**
  * @}
  */


/** @defgroup STM32L552E-EV_TS_Exported_Types STM32L552E-EV TS Exported Types
  * @{
  */
typedef struct
{
  uint32_t   Width;                  /* Screen width */
  uint32_t   Height;                 /* Screen height */
  uint32_t   Orientation;            /* Touch screen orientation */
  uint32_t   Accuracy;               /* Expressed in pixel and means the x or y difference vs old 
                                        position to consider the new values valid */
} TS_Init_t;

typedef struct 
{       
  uint8_t   MultiTouch;      
  uint8_t   Gesture;
  uint8_t   MaxTouch;
  uint32_t  MaxXl;
  uint32_t  MaxYl;  
} TS_Capabilities_t;

typedef struct
{
  uint32_t  TouchDetected;
  uint32_t  TouchX;
  uint32_t  TouchY;
} TS_State_t;

typedef struct
{
  uint32_t  TouchDetected;
  uint32_t  TouchX[2];
  uint32_t  TouchY[2];
  uint32_t  TouchWeight[2];
  uint32_t  TouchEvent[2];
  uint32_t  TouchArea[2];
} TS_MultiTouch_State_t;

typedef struct
{ 
  uint32_t  Radian;
  uint32_t  OffsetLeftRight;
  uint32_t  OffsetUpDown;
  uint32_t  DistanceLeftRight;
  uint32_t  DistanceUpDown;
  uint32_t  DistanceZoom;  
} TS_Gesture_Config_t;

typedef struct
{
  uint32_t   Width;
  uint32_t   Height;
  uint32_t   Orientation;
  uint32_t   Accuracy;
  uint32_t   MaxX;
  uint32_t   MaxY;
  uint32_t   PreviousX;
  uint32_t   PreviousY;
} TS_Ctx_t;
/**
  * @}
  */

/** @defgroup STM32L552E-EV_TS_Exported_Variables STM32L552E-EV TS Exported Variables
  * @{
  */
extern void     *Ts_CompObj[TS_INSTANCES_NBR];
extern TS_Drv_t *Ts_Drv[TS_INSTANCES_NBR];
extern TS_Ctx_t  Ts_Ctx[TS_INSTANCES_NBR];

extern EXTI_HandleTypeDef hts_exti[TS_INSTANCES_NBR];
extern IRQn_Type          Ts_IRQn[TS_INSTANCES_NBR];
/**
  * @}
  */

/** @defgroup STM32L552E-EV_TS_Exported_Functions STM32L552E-EV TS Exported Functions
  * @{
  */
int32_t  BSP_TS_Init(uint32_t Instance, TS_Init_t *TS_Init);
int32_t  BSP_TS_DeInit(uint32_t Instance);
int32_t  BSP_TS_EnableIT(uint32_t Instance);
int32_t  BSP_TS_DisableIT(uint32_t Instance);
int32_t  BSP_TS_Set_Orientation(uint32_t Instance, uint32_t Orientation);
int32_t  BSP_TS_Get_Orientation(uint32_t Instance, uint32_t *Orientation);
int32_t  BSP_TS_GetState(uint32_t Instance, TS_State_t *TS_State);
int32_t  BSP_TS_Get_MultiTouchState(uint32_t Instance, TS_MultiTouch_State_t *TS_State);
int32_t  BSP_TS_GestureConfig(uint32_t Instance, TS_Gesture_Config_t *GestureConfig);
int32_t  BSP_TS_GetGestureId(uint32_t Instance, uint32_t *GestureId);
int32_t  BSP_TS_GetCapabilities(uint32_t Instance, TS_Capabilities_t *Capabilities);
void     BSP_TS_Callback(uint32_t Instance);
void     BSP_TS_IRQHandler(uint32_t Instance);
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

#endif /* STM32L552E_EVAL_TS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
