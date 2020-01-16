/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    GTZC/GTZC_TZSC_MPCBB_TrustZone/Secure/Src/stm32l5xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *          and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */
 
/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_GTZC_CLK_ENABLE();

  /* System interrupt init*/

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral 
  */
  HAL_PWREx_DisableUCPDDeadBattery();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Initializes the HASH MSP.
  *        This function configures the hardware resources used in this example:
  *           - HASH's clock enable
  *           - DMA's clocks enable
  * @param  hhash: HASH handle pointer
  * @retval None
  */
void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash)
{
  static DMA_HandleTypeDef  hdma_hash;

  /* Enable HASH clock */
  __HAL_RCC_HASH_CLK_ENABLE();

  /* Enable DMA clocks */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMAMUX1_CLK_ENABLE();

  /***************** Configure common DMA In parameters ***********************/
  hdma_hash.Init.Request             = DMA_REQUEST_HASH_IN;
  hdma_hash.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_hash.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_hash.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_hash.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_hash.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_hash.Init.Mode                = DMA_NORMAL;
  hdma_hash.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_hash.Instance                 = DMA2_Channel7;

  /* Associate the DMA handle */
  __HAL_LINKDMA(hhash, hdmain, hdma_hash);

  /* Configure the DMA channel */
  HAL_DMA_Init(hhash->hdmain);

  /* Set Channel secure attributes: channel secure, source non-secure (non-secure SRAM), destination secure (HASH) */
  HAL_DMA_ConfigChannelAttributes(hhash->hdmain, DMA_CHANNEL_SEC | DMA_CHANNEL_SRC_NSEC | DMA_CHANNEL_DEST_SEC);

  /* NVIC configuration for DMA Input data interrupt */
  HAL_NVIC_SetPriority(DMA2_Channel7_IRQn, 0x07, 0x00);
  HAL_NVIC_EnableIRQ(DMA2_Channel7_IRQn);
}

/**
  * @brief  DeInitializes the HASH MSP.
  *        This function disables the peripheral.
  * @param  hhash: HASH handle pointer
  * @retval None
  */
void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash)
{
  /* Disable HASH clock */
  __HAL_RCC_HASH_CLK_DISABLE();

  /* Reset HASH peripheral */
  __HAL_RCC_HASH_FORCE_RESET();
  __HAL_RCC_HASH_RELEASE_RESET();
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
