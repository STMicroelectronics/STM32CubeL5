/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
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
/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup OTFDEC_Ciphering_TrustZone
  * @{
  */

extern uint8_t  Plain[];

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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
  * @brief  Secure on-the-fly decipher data from external memory
  * @param  ReadAddress       OTFDEC region address to read from
  * @param  pOutputData       Pointer to output data buffer
  * @param  OutputDataMaxSize Maximum size of output data in bytes
  * @retval SUCCESS or ERROR
  */
CMSE_NS_ENTRY ErrorStatus SECURE_ONTHEFLY_DECIPHER_DATA(uint32_t ReadAddress, uint32_t *pOutputData, uint32_t OutputDataMaxSize)
{
  ErrorStatus  ret = ERROR;
  uint32_t i;
  uint32_t * ptr_r = (uint32_t *)(ReadAddress);
  uint32_t * ptr_w = pOutputData;

  /* Check read address */
  if (ReadAddress == START_ADRESS_OTFDEC1_REGION1)
  {
    /* Check output address range in non-secure) */
    if (cmse_check_address_range(pOutputData, OutputDataMaxSize * sizeof(uint8_t), CMSE_NONSECURE))
    {
      /* On-the-fly decoding of ciphered data stored in external memory */
      for (i=0; i < ((strlen((char const *)Plain) / 4) + ((strlen((char const *)Plain)%4) + 3)/4); i++)
      {
        *ptr_w++ = *ptr_r++;
      }

      ret = SUCCESS;
    }
  }

  return ret;
}

/**
  * @}
  */

/**
  * @}
  */
/* USER CODE END Non_Secure_CallLib */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
