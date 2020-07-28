/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"
#include "stm32l562e_discovery.h"
#include "stm32l562e_discovery_lcd.h"
#include "stm32l562e_discovery_ts.h"
#include "stm32l562e_discovery_sd.h"
#include "stm32l562e_discovery_ospi.h"
#include "stm32l562e_discovery_audio.h"
#include "stm32l562e_discovery_motion_sensors.h"
#include "stm32_lcd.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern const unsigned char stlogo[];
extern __IO FlagStatus UserButtonPressed;
extern __IO FlagStatus TouchDetected;

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
void Led_demo(void);
void LCD_demo(void);
void Touchscreen_demo(void);
void SD_demo(void);
void OSPI_NOR_demo(void);
void AudioPlay_demo(void);
void AudioRecord_demo(void);
void MotionSensor_demo(void);

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
