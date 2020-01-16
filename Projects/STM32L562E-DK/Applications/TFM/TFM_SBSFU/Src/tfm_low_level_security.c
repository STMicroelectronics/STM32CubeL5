/**
  ******************************************************************************
  * @file    tfm_low_level_security.c
  * @author  MCD Application Team
  * @brief   Security Low Level Interface module
  *          This file provides set of firmware functions to manage security
  *          low level interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "region_defs.h"
#include "tfm_low_level_security.h"
#ifdef TFM_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* TFM_DEV_MODE  */
#include "bootutil/bootutil_log.h"

/* DUAL BANK page size */
#define PAGE_SIZE 0x800
#define PAGE_MAX_NUMBER_IN_BANK 127
/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup NCNT_Private_Functions EEPROM Private Functions
  * @{
  */
#ifdef TFM_OB_RDP_LEVEL_VALUE
static void rdp_level(uint32_t rdplevel);
#endif /* TFM_OB_RDP_LEVEL_VALUE */

/**
  * @}
  */

/** @defgroup TFM_SECURITY_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Apply the runtime security  protections to
  *
  * @param  None
  * @note   By default, the best security protections are applied
  * @retval None
  */
void TFM_LL_SECU_ApplyRunTimeProtections(void)
{
#ifdef TFM_FLASH_PRIVONLY_ENABLE
  BOOT_LOG_INF("FLASH DRIVER ACCESS set to PRIVELEGED ONLY");
  HAL_FLASHEx_ConfigPrivMode(FLASH_PRIV_DENIED);
#endif /*  TFM_FLASH_PRIVONLY_ENABLE */
}

/**
  * @brief  Check if the Static security  protections to
  *         all the Sections in Flash:  WRP, SECURE FLASH, SECURE USER FLASH.
  *         those protections not impacted by a Reset. They are set using the Option Bytes
  *         When the device is locked (RDP Level2), these protections cannot be changed anymore
  * @param  None
  * @note   By default, the best security protections are applied to the different
  *         flash sections in order to maximize the security level for the specific MCU.
  * @retval None
  */
FLASH_OBProgramInitTypeDef flash_option_bytes_bank1;
FLASH_OBProgramInitTypeDef flash_option_bytes_bank2;

void TFM_LL_SECU_CheckStaticProtections(void)
{
#ifdef TFM_ENABLE_SET_OB
  HAL_StatusTypeDef ret;
#endif  /* TFM_ENABLE_SET_OB  */
  uint32_t start;
  uint32_t end;
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();


  /* get A and  bank1 OB  */
  memset(&flash_option_bytes_bank1, 0, sizeof(flash_option_bytes_bank1));
  memset(&flash_option_bytes_bank2, 0, sizeof(flash_option_bytes_bank2));
  flash_option_bytes_bank1.WRPArea = OB_WRPAREA_BANK1_AREAA;
  flash_option_bytes_bank1.WMSecConfig = OB_WMSEC_AREA1;
  flash_option_bytes_bank1.BootAddrConfig = OB_BOOTADDR_SEC0;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank1);

  /* get B and  bank2 OB  */
  flash_option_bytes_bank2.WRPArea = OB_WRPAREA_BANK2_AREAA;
  flash_option_bytes_bank2.WMSecConfig = OB_WMSEC_AREA2;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank2);


  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

#ifdef TFM_ENABLE_SET_OB
  /*  clean the option configuration */
  flash_option_bytes_bank1.OptionType = 0;
  flash_option_bytes_bank2.OptionType = 0;
  flash_option_bytes_bank2.WRPArea = 0;
  flash_option_bytes_bank1.WRPArea = 0;
#endif /*   TFM_ENABLE_SET_OB */

  /* check if dual bank is set */
  if (!(flash_option_bytes_bank1.USERConfig & FLASH_OPTR_DB256K))
  {
    BOOT_LOG_ERR("Error while checking dual bank configuration");
    Error_Handler();
  }


#ifdef TFM_OB_BOOT_SEC_ENABLE
  {
    BOOT_LOG_INF("BootAddr 0x%x", flash_option_bytes_bank1.BootAddr);
    if (flash_option_bytes_bank1.BootAddr != (uint32_t)(BL2_CODE_START + FLASH_AREA_BL2_OFFSET))
    {
      BOOT_LOG_ERR("Error while checking SEC BOOT Address");
      Error_Handler();
    }
  }
#endif /* TFM_OB_BOOT_SEC_ENABLE   */

#ifdef TFM_OB_SEC_PROTECT_ENABLE
  start = FLASH_AREA_PERSO_OFFSET / PAGE_SIZE;
  end = (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE - 1) / PAGE_SIZE;
  if (end > PAGE_MAX_NUMBER_IN_BANK)
  {
    end = PAGE_MAX_NUMBER_IN_BANK;
  }
  BOOT_LOG_INF("BANK 1 secure flash [%d, %d] : OB [%d, %d]",
               start, end, flash_option_bytes_bank1.WMSecStartPage, flash_option_bytes_bank1.WMSecEndPage);
  if ((flash_option_bytes_bank1.WMSecStartPage > flash_option_bytes_bank1.WMSecEndPage)
      || (start != flash_option_bytes_bank1.WMSecStartPage)
      || (end != flash_option_bytes_bank1.WMSecEndPage))
  {
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Error while checking secure flash protection");
    Error_Handler();
#else
    BOOT_LOG_ERR("Error while checking secure flash protection :set wmsec1");
    flash_option_bytes_bank1.WMSecStartPage = start;
    flash_option_bytes_bank1.WMSecEndPage = end;
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_WMSEC;
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_AREA1 | OB_WMSEC_SECURE_AREA_CONFIG;

#endif /* TFM_ENABLE_SET_OB */
  }
  start = 0;
  end = (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE - 1) / PAGE_SIZE;
  if (end > PAGE_MAX_NUMBER_IN_BANK)
  {
    end = end - (PAGE_MAX_NUMBER_IN_BANK + 1);
    BOOT_LOG_INF("BANK 2 secure flash [%d, %d] : OB [%d, %d]", start, end, flash_option_bytes_bank2.WMSecStartPage,
                 flash_option_bytes_bank2.WMSecEndPage);
    if ((flash_option_bytes_bank2.WMSecStartPage > flash_option_bytes_bank2.WMSecEndPage)
        || (start < flash_option_bytes_bank2.WMSecStartPage)
        || (end > flash_option_bytes_bank2.WMSecEndPage))
    {
#ifndef TFM_ENABLE_SET_OB
      BOOT_LOG_ERR("Error while checking secure flash protection");
      Error_Handler();
#else
      BOOT_LOG_ERR("Error while checking secure flash protection : set wmsec2");
      flash_option_bytes_bank2.WMSecStartPage = start;
      flash_option_bytes_bank2.WMSecEndPage = end;
      flash_option_bytes_bank2.OptionType = OPTIONBYTE_WMSEC;
      flash_option_bytes_bank2.WMSecConfig |= OB_WMSEC_AREA2 | OB_WMSEC_SECURE_AREA_CONFIG ;

#endif /* TFM_ENABLE_SET_OB  */
    }
  }
#endif /* TFM_OB_SEC_PROTECT_ENABLE */
#ifdef  TFM_WRP_PROTECT_ENABLE

  start = FLASH_AREA_PERSO_OFFSET / PAGE_SIZE;
  end = (FLASH_AREA_PERSO_OFFSET + FLASH_AREA_BL2_SIZE - 1) / PAGE_SIZE;
  BOOT_LOG_INF("BANK 1 flash write protection [%d, %d] : OB[%d, %d]", start, end,
               flash_option_bytes_bank1.WRPStartOffset,
               flash_option_bytes_bank1.WRPEndOffset);
  if ((flash_option_bytes_bank1.WRPStartOffset > flash_option_bytes_bank1.WRPEndOffset)
      || (start != flash_option_bytes_bank1.WRPStartOffset)
      || (end != flash_option_bytes_bank1.WRPEndOffset))
  {
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Error while checking write protection ");
    Error_Handler();
#else
    flash_option_bytes_bank1.WRPStartOffset = start;
    flash_option_bytes_bank1.WRPEndOffset = end;
    flash_option_bytes_bank1.WRPArea |= OB_WRPAREA_BANK1_AREAA;

    BOOT_LOG_ERR("Error while checking write protection : set wrp1");
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_WRP;

#endif /* TFM_ENABLE_SET_OB */
  }
#endif /* TFM_WRP_PROTECT_ENABLE */
#ifdef  TFM_SECURE_USER_PROTECT_ENABLE

  start = FLASH_AREA_PERSO_OFFSET / PAGE_SIZE;
  end = (FLASH_BL2_HDP_END) / PAGE_SIZE;
  BOOT_LOG_INF("BANK 1 secure user flash [%d, %d] :OB [%d, %d]",
               start,
               end,
               flash_option_bytes_bank1.WMSecStartPage,
               flash_option_bytes_bank1.WMHDPEndPage);

  if (
    (flash_option_bytes_bank1.WMSecStartPage > flash_option_bytes_bank1.WMHDPEndPage)
    || (start < flash_option_bytes_bank1.WMSecStartPage)
    || (end > flash_option_bytes_bank1.WMHDPEndPage)
    || (flash_option_bytes_bank1.WMSecConfig & OB_WMSEC_HDP_AREA_DISABLE))
  {
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Error while checking secure user flash protection");
    Error_Handler();
#else
    BOOT_LOG_ERR("Error while checking secure user flash protection : set hdp1");
    flash_option_bytes_bank1.WMSecStartPage = start;
    flash_option_bytes_bank1.WMHDPEndPage = end;
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_WMSEC;
    /*  clean disable */
    flash_option_bytes_bank1.WMSecConfig &= ~OB_WMSEC_HDP_AREA_DISABLE;
    /* enable */
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_CONFIG ;
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_ENABLE;
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_ENABLE;
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_AREA1;
#endif  /*  TFM_ENABLE_SET_OB */
  }
#else /* TFM_SECURE_USER_PROTECT_ENABLE */
  flash_option_bytes_bank1.WMSecConfig &= ~(OB_WMSEC_HDP_AREA_CONFIG | OB_WMSEC_HDP_AREA_ENABLE);
  flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_DISABLE;
#endif /* TFM_SECURE_USER_PROTECT_ENABLE */
#ifdef TFM_SECURE_USER_SRAM2_ERASE_AT_RESET
  if (flash_option_bytes_bank1.USERConfig & FLASH_OPTR_SRAM2_RST)
  {
    BOOT_LOG_ERR("Error while checking OB for SRAM2 ERASE at Reset");
    Error_Handler();
  }
#endif /*TFM_SECURE_USER_SRAM2_RST */
#ifdef TFM_ENABLE_SET_OB
  if ((flash_option_bytes_bank1.OptionType) || (flash_option_bytes_bank2.OptionType))
  {
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();


    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();
    /* verify RDP  level 0 */
    if (flash_option_bytes_bank1.OptionType & OPTIONBYTE_RDP)
    {
      BOOT_LOG_ERR("Error while checking OB RDP to programm");
      Error_Handler();
    }
    if (flash_option_bytes_bank2.OptionType & OPTIONBYTE_RDP)
    {
      BOOT_LOG_ERR("Error while checking OB RDP to programm");
      Error_Handler();
    }
    if (flash_option_bytes_bank2.WRPArea)
    {
      BOOT_LOG_ERR("Error while checking bank 2 OB WRP AREA  to programm");
      Error_Handler();
    }

    if (flash_option_bytes_bank1.WRPArea & ~OB_WRPAREA_BANK1_AREAA)
    {
      BOOT_LOG_ERR("Error while checking bank 1 OB WRP AREA  to programm");
      Error_Handler();
    }
    if (flash_option_bytes_bank1.OptionType)
    {


      ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank1);
      if (ret != HAL_OK)
      {
        BOOT_LOG_ERR("Error while setting OB Bank1 config");
        Error_Handler();
      }
    }
    if (flash_option_bytes_bank2.OptionType)
    {

      ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank2);
      if (ret != HAL_OK)
      {
        BOOT_LOG_ERR("Error while setting OB Bank1 config");
        Error_Handler();
      }
    }

    ret = HAL_FLASH_OB_Launch();
    if (ret != HAL_OK)
    {
      BOOT_LOG_ERR("Error while execution OB_Launch");
      Error_Handler();
    }
    /*  reset the board to apply protection */
    HAL_NVIC_SystemReset();
  }
#endif /*   TFM_ENABLE_SET_OB */

#ifdef TFM_OB_BOOT_LOCK
  {
    BOOT_LOG_INF("BootLock 0x%x", flash_option_bytes_bank1.BootLock);
    if (flash_option_bytes_bank1.BootLock != TFM_OB_BOOT_LOCK)
    {
      BOOT_LOG_ERR("Error while checking SEC BOOT LOCK");
      Error_Handler();
    }
  }
#endif /* TFM_OB_BOOT_LOCK_ENABLE   */

#ifdef TFM_OB_RDP_LEVEL_VALUE
  BOOT_LOG_INF("RDPLevel 0x%x (0x%x)", flash_option_bytes_bank1.RDPLevel, TFM_OB_RDP_LEVEL_VALUE);
  if (flash_option_bytes_bank1.RDPLevel != TFM_OB_RDP_LEVEL_VALUE)
  {
    BOOT_LOG_ERR("Error while checking RDP level");
    /* perform a regression to 0 */

    rdp_level(TFM_OB_RDP_LEVEL_VALUE);

    Error_Handler();
  }
#endif /* TFM_OB_RDP_LEVEL_VALUE */
}

FLASH_OBProgramInitTypeDef flash_option_bytes_bank = {0};


#ifdef TFM_OB_RDP_LEVEL_VALUE
static void rdp_level(uint32_t rdplevel)
{
#ifdef TFM_ENABLE_SET_OB
  HAL_StatusTypeDef ret;
  flash_option_bytes_bank.OptionType = OPTIONBYTE_RDP;

  flash_option_bytes_bank.RDPLevel = rdplevel;
  BOOT_LOG_INF("Programming RDP to %x", rdplevel);
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Unlock the Options Bytes *************************************************/
  HAL_FLASH_OB_Unlock();

  HAL_FLASH_OB_Unlock();
  ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank);
  if (ret != HAL_OK)
  {
    BOOT_LOG_ERR("Error while setting OB Bank config retry");
    ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank);
    if (ret != HAL_OK)
    {
      BOOT_LOG_ERR("Error while setting OB Bank config");
      Error_Handler();
    }
  }
  ret = HAL_FLASH_OB_Launch();
  if (ret != HAL_OK)
  {
    BOOT_LOG_ERR("Error while execution OB_Launch");
    Error_Handler();
  }
  /*  reset the board to apply protection */
  HAL_NVIC_SystemReset();
#endif /* TFM_ENABLE_SET_OB */
}
#endif /* TFM_OB_RDP_LEVEL_VALUE */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
