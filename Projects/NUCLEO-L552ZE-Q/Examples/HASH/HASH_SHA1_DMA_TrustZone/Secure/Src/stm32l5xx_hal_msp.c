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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
extern DMA_HandleTypeDef hdma_hash_in;

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

  /* Peripheral interrupt init */
  /* GTZC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(GTZC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(GTZC_IRQn);

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  HAL_PWREx_DisableUCPDDeadBattery();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief HASH MSP Initialization
* This function configures the hardware resources used in this example
* @param hhash: HASH handle pointer
* @retval None
*/
void HAL_HASH_MspInit(HASH_HandleTypeDef* hhash)
{
  /* USER CODE BEGIN HASH_MspInit 0 */

  /* USER CODE END HASH_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_HASH_CLK_ENABLE();

    /* HASH DMA Init */
    /* HASH_IN Init */
    hdma_hash_in.Instance = DMA2_Channel7;
    hdma_hash_in.Init.Request = DMA_REQUEST_HASH_IN;
    hdma_hash_in.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_hash_in.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_hash_in.Init.MemInc = DMA_MINC_ENABLE;
    hdma_hash_in.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_hash_in.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_hash_in.Init.Mode = DMA_NORMAL;
    hdma_hash_in.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_hash_in) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMA_ConfigChannelAttributes(&hdma_hash_in, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMA_ConfigChannelAttributes(&hdma_hash_in, DMA_CHANNEL_SEC) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMA_ConfigChannelAttributes(&hdma_hash_in, DMA_CHANNEL_SRC_NSEC) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMA_ConfigChannelAttributes(&hdma_hash_in, DMA_CHANNEL_DEST_SEC) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hhash,hdmain,hdma_hash_in);

  /* USER CODE BEGIN HASH_MspInit 1 */

  /* USER CODE END HASH_MspInit 1 */

}

/**
* @brief HASH MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hhash: HASH handle pointer
* @retval None
*/
void HAL_HASH_MspDeInit(HASH_HandleTypeDef* hhash)
{
  /* USER CODE BEGIN HASH_MspDeInit 0 */

  /* USER CODE END HASH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_HASH_CLK_DISABLE();

    /* HASH DMA DeInit */
    HAL_DMA_DeInit(hhash->hdmain);
  /* USER CODE BEGIN HASH_MspDeInit 1 */

  /* USER CODE END HASH_MspDeInit 1 */

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
