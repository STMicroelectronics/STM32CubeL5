/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DMA/DMA_MemToMem_TrutZone/Secure/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for secure main.c module
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l5xx_nucleo.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/
#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL (*funcptr)(void);
#endif

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define BUFFER_SIZE  32U

extern DMA_HandleTypeDef hdma_memtomem_dma1_channel2;
extern DMA_HandleTypeDef hdma_memtomem_dma1_channel3;

extern const uint32_t aSRC_SEC_ROM_Buffer[];
extern uint32_t aDST_SEC_RAM_Buffer[];
extern uint32_t aDST_SEC_RAM_COPY_Buffer[];

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SecureToNonSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel2);
void SecureToNonSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel2);
void NonSecureToSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel3);
void NonSecureToSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel23);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
