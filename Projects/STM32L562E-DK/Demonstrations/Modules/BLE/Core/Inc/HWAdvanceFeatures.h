/**
  ******************************************************************************
  * @file    HWAdvanceFeatures.h 
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   DS3/DSM HW Advance Features API
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
#ifndef _HW_ADVANCE_FEATURES_H_
#define _HW_ADVANCE_FEATURES_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "TargetFeatures.h"

#include <stdlib.h>
#include "main.h"


/* Exported types ------------------------------------------------------- */
typedef enum
{
  ACC_NOT_USED     = 0x00,
  ACC_6D_OR_TOP    = 0x01,
  ACC_6D_OR_LEFT   = 0x02,
  ACC_6D_OR_BOTTOM = 0x03,
  ACC_6D_OR_RIGTH  = 0x04,
  ACC_6D_OR_UP     = 0x05,
  ACC_6D_OR_DOWN   = 0x06,
  ACC_TILT         = 0x08,
  ACC_FREE_FALL    = 0x10,
  ACC_SINGLE_TAP   = 0x20,
  ACC_DOUBLE_TAP   = 0x40,
  ACC_WAKE_UP      = 0x80
} AccEventType;

/* Exported functions ------------------------------------------------------- */
extern void InitHWFeatures(void);

extern void DisableHWFeatures(void);

extern void EnableHWPedometer (void);
extern void DisableHWPedometer(void);
extern void ResetHWPedometer  (void);
extern uint16_t GetStepHWPedometer(void);

extern void EnableHWFreeFall (void);
extern void DisableHWFreeFall(void);

extern void EnableHWDoubleTap (void);
extern void DisableHWDoubleTap(void);

extern void EnableHWSingleTap (void);
extern void DisableHWSingleTap(void);

extern void EnableHWWakeUp (void);
extern void DisableHWWakeUp(void);

extern void EnableHWTilt (void);
extern void DisableHWTilt(void);

extern void EnableHWOrientation6D (void);
extern void DisableHWOrientation6D(void);
extern AccEventType GetHWOrientation6D(void);

extern void EnableHWMultipleEvents(void);
extern void DisableHWMultipleEvents(void);

/* Exported variables */
extern uint32_t HWAdvanceFeaturesStatus;

/* Exported defines */
#define W2ST_HWF_PEDOMETER        (1   )
#define W2ST_HWF_FREE_FALL        (1<<1)
#define W2ST_HWF_DOUBLE_TAP       (1<<2)
#define W2ST_HWF_SINGLE_TAP       (1<<3)
#define W2ST_HWF_WAKE_UP          (1<<4)
#define W2ST_HWF_TILT             (1<<5)
#define W2ST_HWF_6DORIENTATION    (1<<6)
#define W2ST_HWF_MULTIPLE_EVENTS  (1<<7)


#define W2ST_CHECK_HW_FEATURE(Feature) ((HWAdvanceFeaturesStatus&(Feature)) ? 1 : 0)
#define W2ST_ON_HW_FEATURE(Feature)    (HWAdvanceFeaturesStatus|=(Feature))
#define W2ST_OFF_HW_FEATURE(Feature)   (HWAdvanceFeaturesStatus&=(~Feature))


#ifdef __cplusplus
}
#endif

#endif /* _HW_ADVANCE_FEATURES_H_ */
