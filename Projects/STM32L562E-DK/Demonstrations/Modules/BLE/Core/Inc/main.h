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

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
