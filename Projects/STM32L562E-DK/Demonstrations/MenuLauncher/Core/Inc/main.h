/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define BKP_REG_PWR_CFG           (TAMP_NS->BKP28R)
#define BKP_REG_SW_CFG            (TAMP_NS->BKP27R)
#define BKP_REG_SUBDEMO_ADDRESS   (TAMP_NS->BKP26R)
#define BKP_REG_CALIB_DR0         (TAMP_NS->BKP25R)
#define BKP_REG_CALIB_DR1         (TAMP_NS->BKP24R)

#define DEMO_1_BASE_ADDRESS       (0x08018000UL)   /* Measurements Module */
#define DEMO_2_BASE_ADDRESS       (0x08028000UL)   /* AI Module           */
#define DEMO_3_BASE_ADDRESS       (0x08038000UL)   /* BLE Module          */
#define DEMO_4_BASE_ADDRESS       (0x08050000UL)   /* Audio Player Module */
#define DEMO_5_BASE_ADDRESS       (0x08070000UL)   /* Information Module  */

#define MODULES_COUNT             (5)

#if defined(CALIBRATION_TS)
#define CALIBRATION_BASE_ADDRESS  (0x0807F800UL)   /* Calibration Data    */
#endif
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported functions prototypes ---------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
