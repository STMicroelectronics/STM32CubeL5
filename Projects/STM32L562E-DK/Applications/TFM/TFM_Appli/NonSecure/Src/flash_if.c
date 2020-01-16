/**
  ******************************************************************************
  * @file    flash_if.c
  * @author  MCD Application Team
  * @brief   FLASH Interface module.
  *          This file provides set of firmware functions to manage Flash
  *          Interface functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup USER_APP_COMMON Common
  * @{
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"

#include "flash_if.h"
#include "tfm_ns_svc.h"
#include "string.h"
#include <stdio.h>

/* Uncomment the line below if you want some debug logs */
#define FLASH_IF_DBG
#ifdef FLASH_IF_DBG
#define FLASH_IF_TRACE printf
#else
#define FLASH_IF_TRACE(...)
#endif /* FLASH_IF_DBG */
/*  struct handler for Flash in Svc */
#ifdef USE_SVC
typedef enum
{
  FLASH_ERASE,
  FLASH_WRITE,
} FLASH_Access_t;     /*!< Comm status structures definition */


typedef struct
{ /*  write /erase info structure */
  void *pDest; /*erase /write dest */
  const void *pSrc; /*  write content */
  uint32_t uLen; /*  erase , write length */
  FLASH_Access_t cmd;
} FLashIfReq;


/*  handler for call  */
__attribute__ ((naked))
static uint32_t flash_access(void *req)
{
    SVC(SVC_FLASH_ACCESS);
    __ASM("BX LR");
}
static HAL_StatusTypeDef FLASH_If_Write_svc(void *pDestination, const void *pSource, uint32_t uLength);
static HAL_StatusTypeDef FLASH_If_Erase_Size_svc(void *pStart, uint32_t uLength);
static HAL_StatusTypeDef FLASH_If_Init_svc(void);
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NB_PAGE_SECTOR_PER_ERASE  2U    /*!< Nb page erased per erase */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint32_t GetPage(uint32_t uAddr);
static uint32_t GetBank(uint32_t uAddr);
static uint32_t GetBankAddr(uint32_t bank);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Gets the page of a given address
  * @param  uAddr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t uAddr)
{
  uint32_t page = 0U;

  if (uAddr < (FLASH_BASE + (FLASH_BANK_SIZE)))
  {
    /* Bank 1 */
    page = (uAddr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (uAddr - (FLASH_BASE + (FLASH_BANK_SIZE))) / FLASH_PAGE_SIZE;
  }

  return page;
}
/**
  * @brief  Gets the bank of a given address
  * @param  uAddr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t uAddr)
{
  uint32_t bank = 0U;
/*
  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0U)
  {
*/
    /* No Bank swap */
    if (uAddr < (FLASH_BASE + (FLASH_BANK_SIZE)))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
#if 0
  }
  else
  {
    /* Bank swap */
    if (uAddr < (FLASH_BASE + (FLASH_BANK_SIZE)))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }
#endif
  return bank;
}


/**
  * @brief  Gets the address of a bank
  * @param  Bank: Bank ID
  * @retval Address of the bank
  */
static uint32_t GetBankAddr(uint32_t Bank)
{
  if (Bank == FLASH_BANK_2)
  {
    return  FLASH_BASE + (FLASH_BANK_SIZE);
  }
  else
  {
    return FLASH_BASE;
  }
}
#ifdef USE_SVC
/* interface calling a svc handler */
HAL_StatusTypeDef FLASH_If_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
   FLashIfReq req;
   req.cmd= FLASH_WRITE;
   req.pDest=pDestination;
   req.pSrc=pSource;
   req.uLen=uLength;
   return flash_access((void *)&req);
}

HAL_StatusTypeDef FLASH_If_Erase_Size(void *pDestination, uint32_t uLength)
{
   FLashIfReq req;
   req.cmd= FLASH_ERASE;
   req.pDest=pDestination;
   req.uLen=uLength;
   return flash_access((void *)&req);
}

HAL_StatusTypeDef Flash_If_handler(void *pReq)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  FLashIfReq *req=(FLashIfReq *)pReq;
  switch (req->cmd)
  {
  case FLASH_ERASE:
    ret = FLASH_If_Erase_Size_svc(req->pDest, req->uLen);
    break;

  case FLASH_WRITE:
    ret = FLASH_If_Write_svc(req->pDest,req->pSrc,  req->uLen);
    break;
  default: break;
  }
  return ret;
}
#endif




/* Public functions ---------------------------------------------------------*/
/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval HAL Status.
  */
#ifdef USE_SVC
static HAL_StatusTypeDef FLASH_If_Init_svc(void)
#else
static HAL_StatusTypeDef FLASH_If_Init(void)
#endif
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  /* Unlock the Program memory */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    /* Unlock the Program memory */
    if (HAL_FLASH_Lock() == HAL_OK)
    {
      ret = HAL_OK;
    }
#ifdef FLASH_IF_DBG
    else
    {
      FLASH_IF_TRACE("[FLASH_IF] Lock failure\r\n");
    }
#endif /* FLASH_IF_DBG */
  }
#ifdef FLASH_IF_DBG
  else
  {
    FLASH_IF_TRACE("[FLASH_IF] Unlock failure\r\n");
  }
#endif /* FLASH_IF_DBG */
  return ret;
}

/**
  * @brief  This function does an erase of n (depends on Length) pages in user flash area
  * @param  pStart: Start of user flash area
  * @param  uLength: number of bytes.
  * @retval HAL status.
  */
#ifdef USE_SVC
static HAL_StatusTypeDef FLASH_If_Erase_Size_svc(void *pStart, uint32_t uLength)
#else
HAL_StatusTypeDef FLASH_If_Erase_Size(void *pStart, uint32_t uLength)
#endif
{
  uint32_t page_error = 0U;
  uint32_t uStart = (uint32_t)pStart;
  FLASH_EraseInitTypeDef x_erase_init;
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;
  uint32_t first_page = 0U, nb_pages = 0U;
  uint32_t chunk_nb_pages;
  uint32_t erase_command = 0U;
  uint32_t bank_number = 0U;

  /* Initialize Flash */
	#ifdef USE_SVC
  e_ret_status = FLASH_If_Init_svc();
  #else
	e_ret_status = FLASH_If_Init();
	#endif


  if (e_ret_status == HAL_OK)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    if (HAL_FLASH_Unlock() == HAL_OK)
    {
      do
      {
        /* Get the 1st page to erase */
        first_page = GetPage(uStart);
        bank_number = GetBank(uStart);
        if (GetBank(uStart + uLength - 1U) == bank_number)
        {
          /* Get the number of pages to erase from 1st page */
          nb_pages = GetPage(uStart + uLength - 1U) - first_page + 1U;

          /* Fill EraseInit structure*/
          x_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
          x_erase_init.Banks = bank_number;

          /* Erase flash per NB_PAGE_SECTOR_PER_ERASE to avoid watch-dog */
          do
          {
            chunk_nb_pages = (nb_pages >= NB_PAGE_SECTOR_PER_ERASE) ? NB_PAGE_SECTOR_PER_ERASE : nb_pages;
            x_erase_init.Page = first_page;
            x_erase_init.NbPages = chunk_nb_pages;
            first_page += chunk_nb_pages;
            nb_pages -= chunk_nb_pages;
            if (HAL_FLASHEx_Erase(&x_erase_init, &page_error) != HAL_OK)
            {
              HAL_FLASH_GetError();
              e_ret_status = HAL_ERROR;
            }
            /* Refresh Watchdog */
#if 0
            WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);
#endif
          }
          while (nb_pages > 0);
          erase_command = 1U;
        }
        else
        {
          uint32_t startbank2 = GetBankAddr(FLASH_BANK_2);
          nb_pages = GetPage(startbank2 - 1U) - first_page + 1U;
          x_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
          x_erase_init.Banks       = bank_number;
          uLength = uLength  - (startbank2 - uStart);
          uStart = startbank2;

          /* Erase flash per NB_PAGE_SECTOR_PER_ERASE to avoid watch-dog */
          do
          {
            chunk_nb_pages = (nb_pages >= NB_PAGE_SECTOR_PER_ERASE) ? NB_PAGE_SECTOR_PER_ERASE : nb_pages;
            x_erase_init.Page = first_page;
            x_erase_init.NbPages = chunk_nb_pages;
            first_page += chunk_nb_pages;
            nb_pages -= chunk_nb_pages;
            if (HAL_FLASHEx_Erase(&x_erase_init, &page_error) != HAL_OK)
            {
              HAL_FLASH_GetError();
              e_ret_status = HAL_ERROR;
            }
            /* Refresh Watchdog */
#if 0
            WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);
#endif
          }
          while (nb_pages > 0);
        }
      }
      while (erase_command == 0);
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();

    }
    else
    {
      e_ret_status = HAL_ERROR;
    }
  }

  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  pDestination: Start address for target location
  * @param  pSource: pointer on buffer with data to write
  * @param  uLength: Length of data buffer in byte. It has to be 64-bit aligned.
  * @retval HAL Status.
  */
#ifdef USE_SVC
static HAL_StatusTypeDef FLASH_If_Write_svc(void *pDestination, const void *pSource, uint32_t uLength)
#else
HAL_StatusTypeDef FLASH_If_Write(void *pDestination, const void *pSource, uint32_t uLength)
#endif
{
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;
  uint32_t i = 0U;
  uint32_t pdata = (uint32_t)pSource;
  /* Initialize Flash */
	#ifdef USE_SVC
  e_ret_status = FLASH_If_Init_svc();
  #else
	e_ret_status = FLASH_If_Init();
	#endif
  if (e_ret_status == HAL_OK)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      return HAL_ERROR;

    }
    else
    {
      /* DataLength must be a multiple of 64 bit */
      for (i = 0U; i < uLength; i += 8U)
      {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
        be done by word */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)pDestination,  *((uint64_t *)(pdata + i))) == HAL_OK)
        {
          /* Check the written value */
          if (*(uint64_t *)pDestination != *(uint64_t *)(pdata + i))
          {
            /* Flash content doesn't match SRAM content */
            e_ret_status = HAL_ERROR;
            break;
          }
          /* Increment FLASH Destination address */
          pDestination = (void *)((uint32_t)pDestination + 8U);
        }
        else
        {
          /* Error occurred while writing data in Flash memory */
          e_ret_status = HAL_ERROR;
          break;
        }
      }
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();
    }
  }
  return e_ret_status;
}
/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
