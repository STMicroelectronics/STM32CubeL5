/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_MPU/Inc/main.h
  * @author  MCD Application Team
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MemManage_Recover(void);

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define LED1_GREEN_Pin GPIO_PIN_7
#define LED1_GREEN_GPIO_Port GPIOC
#define LED2_BLUE_Pin GPIO_PIN_7
#define LED2_BLUE_GPIO_Port GPIOB
#define LED3_RED_Pin GPIO_PIN_9
#define LED3_RED_GPIO_Port GPIOA

#endif /* __MAIN_H */
