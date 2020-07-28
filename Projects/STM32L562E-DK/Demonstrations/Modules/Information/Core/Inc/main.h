/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

#define DEMO_1_BASE_ADDRESS       (0x08020000U)
#define DEMO_2_BASE_ADDRESS       (0x08040000U)
#define DEMO_3_BASE_ADDRESS       (0x08060000U)
#define DEMO_4_BASE_ADDRESS       (0x08000000U)

#define MODULES_COUNT             (4)
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
#define USB_OTGFS_DP_Pin GPIO_PIN_12
#define USB_OTGFS_DP_GPIO_Port GPIOA
#define USB_OTGFS_DM_Pin GPIO_PIN_11
#define USB_OTGFS_DM_GPIO_Port GPIOA
#define USB_OTGFS_ID_Pin GPIO_PIN_10
#define USB_OTGFS_ID_GPIO_Port GPIOA
#define USB_OTGFS_VBUS_Pin GPIO_PIN_9
#define USB_OTGFS_VBUS_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
