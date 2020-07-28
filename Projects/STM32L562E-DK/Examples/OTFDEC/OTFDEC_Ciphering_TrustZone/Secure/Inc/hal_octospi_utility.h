/**
  ******************************************************************************
  * @file    OTFDEC/OTFDEC_Ciphering_TrustZone/Secure/Inc/hal_octospi_utility.h
  * @author  MCD Application Team
  * @brief   Header for helper hal_octospi_utility.c to initialize externalFlash.
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
#ifndef HAL_OCTOSPI_UTILITY_H
#define HAL_OCTOSPI_UTILITY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define OSPI_PAGE_SIZE    256
#define OSPI_SECTOR_SIZE  4096

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint32_t OSPI_Config(void);
uint32_t OSPI_Write(uint32_t * pBuffer, uint32_t FlashAddress, uint32_t Size);
uint32_t OSPI_MemoryMap(void);
extern void Error_Handler(void);

#endif /* HAL_OCTOSPI_UTILITY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
