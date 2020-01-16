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
#include <string.h>     /* for memcmp() */

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates_TZ
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

void *pSecureToNonSecureTransferCompleteCallback = NULL; /* Pointer to callback in Non-secure */
void *pNonSecureToSecureTransferCompleteCallback = NULL; /* Pointer to callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Secure service to provide secure RAM data by dedicated DMA channel
  *         under interrupt and with callback notification to non-secure
  * @param  pBufferDst  pointer to non-secure destination buffer
  * @param  Size        requested size of data
  * @param  func   pointer to non-secure callback function on transfer end
  * @retval SUCCESS or ERROR
  */
CMSE_NS_ENTRY ErrorStatus SECURE_DMA_Secure_To_NonSecure(uint32_t *pBufferDst,
                                                         uint32_t Size,
                                                         void *func)
{
  ErrorStatus  ret = ERROR;

  /* Check output address range in non-secure */
  if (cmse_check_address_range(pBufferDst, Size * sizeof(uint32_t), CMSE_NONSECURE))
  {
    /* Save callback function */
    pSecureToNonSecureTransferCompleteCallback = func;

    /* Start DMA transfer of secure RAM data to non-secure area */
    /* For this use DMA1 channel 2 configured as follows:       */
    /*   - secure channel                                       */
    /*   - source address in secure                             */
    /*   - destination address in non-secure                    */
    if (HAL_DMA_Start_IT(&hdma_memtomem_dma1_channel2,
                         (uint32_t)&aDST_SEC_RAM_Buffer,
                         (uint32_t)pBufferDst,
                         BUFFER_SIZE) == HAL_OK)
    {
      /* Transfer started */
      ret = SUCCESS;
    }
  }

  return ret;
}

/**
  * @brief  Secure service to get non-secure data by dedicated DMA channel
  *         under interrupt and with callback notification to non-secure
  * @param  pBufferSrc  pointer to non-secure source buffer
  * @param  Size        requested size of data
  * @param  func   pointer to non-secure callback function on transfer end
  * @retval SUCCESS or ERROR
  */
CMSE_NS_ENTRY ErrorStatus SECURE_DMA_NonSecure_To_Secure(uint32_t *pBufferSrc,
                                                         uint32_t Size,
                                                         void *func)
{
  ErrorStatus  ret = ERROR;

  /* Check input address range in non-secure */
  if (cmse_check_address_range(pBufferSrc, Size * sizeof(uint32_t), CMSE_NONSECURE))
  {
    /* Save callback function */
    pNonSecureToSecureTransferCompleteCallback = func;

    /* Start DMA transfer of non-secure RAM data to secure area */
    /* For this use DMA1 channel 3 configured as follows:       */
    /*   - secure channel                                       */
    /*   - source address in non-secure                         */
    /*   - destination address in secure                        */
    if (HAL_DMA_Start_IT(&hdma_memtomem_dma1_channel3,
                         (uint32_t)pBufferSrc,
                         (uint32_t)&aDST_SEC_RAM_COPY_Buffer,
                         BUFFER_SIZE) == HAL_OK)
    {
      /* Transfer started */
      ret = SUCCESS;
    }
  }

  return ret;
}

/**
  * @brief  Secure service to compare the last data buffer copy from versus the
  *         secure reference buffer
  * @retval SUCCESS or ERROR
  */
CMSE_NS_ENTRY ErrorStatus SECURE_DATA_Last_Buffer_Compare(void)
{
  ErrorStatus  ret = ERROR;

  /* Compare the secure destination verus the source */
  if (memcmp((uint8_t *)aDST_SEC_RAM_COPY_Buffer,
             (uint8_t *)aSRC_SEC_ROM_Buffer,
             sizeof(uint32_t) * BUFFER_SIZE) == 0)
  {
    ret = SUCCESS;
  }

  return ret;
}
/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure callback function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_RegisterCallback(uint32_t CallbackId, void *func)
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

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
void SecureToNonSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel2)
{
  funcptr_NS callback_NS; // non-secure callback function pointer

  if(pSecureToNonSecureTransferCompleteCallback != (void *)NULL)
  {
   /* return function pointer with cleared LSB */
   callback_NS = (funcptr_NS)cmse_nsfptr_create(pSecureToNonSecureTransferCompleteCallback);

   callback_NS();
  }
  else
  {
    Error_Handler();  /* Something went wrong */
  }
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
void SecureToNonSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel2)
{
  /* Error detected by secure application */
  Error_Handler();
}

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
void NonSecureToSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel3)
{
  funcptr_NS callback_NS; // non-secure callback function pointer

  if(pNonSecureToSecureTransferCompleteCallback != (void *)NULL)
  {
   /* return function pointer with cleared LSB */
   callback_NS = (funcptr_NS)cmse_nsfptr_create(pNonSecureToSecureTransferCompleteCallback);

   callback_NS();
  }
  else
  {
    Error_Handler();  /* Something went wrong */
  }
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
void NonSecureToSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel3)
{
  /* Error detected by secure application */
  Error_Handler();
}

/**
  * @}
  */

/**
  * @}
  */
/* USER CODE END Non_Secure_CallLib */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
