/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
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

/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
#include <stdio.h>

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates_TZ
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*Variable used for Block-Based Security Program procedure*/
extern FLASH_BBAttributesTypeDef BBSecInitStruct;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
  if(func != NULL)
  {
    switch(CallbackId)
    {
      case SECURE_FAULT_CB_ID:           /* SecureFault IT */
        pSecureFaultCallback = func;
        break;
      case GTZC_ERROR_CB_ID:             /* GTZC IT */
        pSecureErrorCallback = func;
        break;
      default:
        /* unknown */
        break;
    }
  }
}

/**
  * @brief  Secure registration of non-secure callback.
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_FLASH_BlockBasedDeactivation(void)
{
  BBSecInitStruct.Bank                  = FLASH_BANK_2;
  BBSecInitStruct.BBAttributesType      = FLASH_BB_SEC;
  BBSecInitStruct.BBAttributes_array[0] = 0;
  BBSecInitStruct.BBAttributes_array[1] = 0;
  BBSecInitStruct.BBAttributes_array[2] = 0;
  BBSecInitStruct.BBAttributes_array[3] = 0;
  
  if (HAL_FLASHEx_ConfigBBAttributes(&BBSecInitStruct) != HAL_OK)
  {
    /*
      Error occurred while configuring block-based attributes.
      User can add here some code to deal with this error.
    */
    Error_Handler();
  }
}

/**
  * @}
  */

/**
  * @}
  */
/* USER CODE END Non_Secure_CallLib */

