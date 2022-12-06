/**
  ******************************************************************************
  * @file    main.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   Header for main.c module
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"
#include "stm32l562e_discovery.h"
#include "ALLMEMS1_config.h"

/* Exported macro ------------------------------------------------------------*/
#define MCR_BLUEMS_F2I_1D(in, out_int, out_dec) {out_int = (int32_t)in; out_dec= (int32_t)((in-out_int)*10);};
#define MCR_BLUEMS_F2I_2D(in, out_int, out_dec) {out_int = (int32_t)in; out_dec= (int32_t)((in-out_int)*100);};

/* Exported functions ------------------------------------------------------- */
extern void Error_Handler(void);

extern void Set2GAccelerometerFullScale(void);
extern void Set4GAccelerometerFullScale(void);

extern unsigned char ResetAccellerometerCalibrationInMemory(void);
extern unsigned char ReCallAccellerometerCalibrationFromMemory(uint16_t dataSize, uint32_t *data);
extern unsigned char SaveAccellerometerCalibrationToMemory(uint16_t dataSize, uint32_t *data);

extern uint8_t BufferToWrite[256];
extern int32_t BytesToWrite;

/* Exported defines and variables  ------------------------------------------------------- */

/* Code for MotionCP & MotionGR & MotionSD & MotionTL & MotionVC integration - Start Section */
/* Algorithm frequency for MotionCP, MotionGR, MotionSD, MotionTL and MotionVC [Hz] */
#define ALGO_FREQ_CP_GR_SD_TL_VC        50U
/* Algorithm period for MotionCP, MotionGR, MotionSD, MotionTL and MotionVC [ms] */
#define ALGO_PERIOD_CP_GR_SD_TL_VC      (1000U / ALGO_FREQ_CP_GR_SD_TL_VC)
/* 10kHz/50 as default value for:
  MotionCP@50Hz or
  MotionGR@50Hz or
  MotionSD@50Hz or
  MotionTL@50Hz or
  MotionVC@50Hz */
#define DEFAULT_uhCCR2_Val      (10000U / ALGO_FREQ_CP_GR_SD_TL_VC)
/* Code for MotionCP & MotionGR & MotionSD & MotionTL & MotionVC integration - End Section */

/* Code for MotionAR & MotionID & MotionPE integration - Start Section */
/* Algorithm frequency for MotionAR, MotionID and MotionPE libraries [Hz] */
#define ALGO_FREQ_AR_ID_PE      16U
/* Algorithm period for MotionAR, MotionID and MotionPE libraries [ms] */
#define ALGO_PERIOD_AR_ID_PE    (1000U / ALGO_FREQ_AR_ID_PE)
/* 10kHz/16 as default value for:
  MotionAR@16Hz
  MotionID@16Hz
  MotionPE@16Hz  */
#define DEFAULT_uhCCR3_Val      (10000U / ALGO_FREQ_AR_ID_PE)
/* Code for MotionAR & MotionID & MotionPE integration - End Section */

/* Update frequency for Acc/Gyro sensor [Hz] */
#define FREQ_ACC_GYRO                   20U
/* Update period for Acc/Gyro [ms] */
#define ALGO_PERIOD_ACC_GYRO            (1000U / FREQ_ACC_GYRO)
/* 10kHz/20  For Acc/Gyro@20Hz */
#define DEFAULT_uhCCR4_Val              (10000U / FREQ_ACC_GYRO)

/* Range time without connect before Shut Down Mode starts (20 sec) */
#define RANGE_TIME_WITHOUT_CONNECTED  20000

#endif /* __MAIN_H */
