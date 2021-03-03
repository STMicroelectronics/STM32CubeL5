/**
  ******************************************************************************
  * @file    tfm_low_level_security.c
  * @author  MCD Application Team
  * @brief   security protection implementation for for secure boot on STM32L5xx
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "boot_hal_cfg.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
#include "mcuboot_config/mcuboot_config.h"
#include "tfm_low_level_security.h"
#ifdef TFM_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* TFM_DEV_MODE  */
#include "bootutil/bootutil_log.h"

/** @defgroup TFM_SECURITY_Private_Defines  Private Defines
  * @{
  */
/* DUAL BANK page size */
#define PAGE_SIZE 0x800
#define PAGE_MAX_NUMBER_IN_BANK 127
/* SBSFU_Boot Vector Address  */
#define SBSFU_BOOT_VTOR_ADDR ((uint32_t)(BL2_CODE_START))
/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup TFM_SECURITY_Private_Functions  Private Functions
  * @{
  */
#ifdef TFM_OB_RDP_LEVEL_VALUE
static void rdp_level(uint32_t rdplevel, uint32_t current_rdplevel);
#endif /* TFM_OB_RDP_LEVEL_VALUE */
static void mpc_init_cfg(void);
static void sau_and_idau_cfg(void);
static void mpu_init_cfg(void);
static void mpu_appli_cfg(void);
static void enable_hdp_protection(void);
#ifdef MCUBOOT_EXT_LOADER
static void mpu_loader_cfg(void);
static void sau_loader_cfg(void);
#if defined(MCUBOOT_EXT_LOADER) && defined(MCUBOOT_PRIMARY_ONLY) 
static void secure_internal_flash(uint32_t offset_start, uint32_t offset_end);
#endif /* defined(MCUBOOT_EXT_LOADER) && defined(MCUBOOT_PRIMARY_ONLY) */
#endif /* MCUBOOT_EXT_LOADER */

/**
  * @}
  */

/** @defgroup TFM_SECURITY_Exported_Functions Exported Functions
  * @{
  */
#if defined(MCUBOOT_EXT_LOADER) && defined(MCUBOOT_PRIMARY_ONLY)
void TFM_LL_SECU_SetLoaderCodeSecure(void)
{
	secure_internal_flash(FLASH_AREA_LOADER_OFFSET, FLASH_AREA_LOADER_OFFSET+LOADER_IMAGE_S_CODE_SIZE-1);
}
/* Place code in a specific section */
#endif /* defined(MCUBOOT_EXT_LOADER) && defined(MCUBOOT_PRIMARY_ONLY) */

#if defined(MCUBOOT_EXT_LOADER)
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */
/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void TFM_LL_SECU_UpdateLoaderRunTimeProtections(void)
{
  /* Enable HDP protection to hide sensible boot material */
  enable_hdp_protection();  
  /* Set MPU to enable execution of secure /non secure  loader */
  mpu_loader_cfg();
  /* reconfigure SAU to allow non secure execution */
  sau_loader_cfg();
}
/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */
#endif /*MCUBOOT_EXT_LOADER*/
/**
  * @brief  Apply the runtime security  protections to
  *
  * @param  None
  * @note   By default, the best security protections are applied
  * @retval None
  */
void TFM_LL_SECU_ApplyRunTimeProtections(void)
{
  /* Unsecure bottom of SRAM1 for error_handler */
  mpc_init_cfg();

  /* Set MPU to forbidd execution outside of not muteable code  */
  /* Initialize not secure MPU to forbidd execution on Flash /SRAM */
  mpu_init_cfg();

  /* Enable SAU to gain access to flash area non secure for write/read */
  sau_and_idau_cfg();
#ifdef TFM_FLASH_PRIVONLY_ENABLE
  HAL_FLASHEx_ConfigPrivMode(FLASH_PRIV_DENIED);
#endif /*  TFM_FLASH_PRIVONLY_ENABLE */
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */

/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void TFM_LL_SECU_UpdateRunTimeProtections(void)
{
  /* Enable HDP protection to hide sensible boot material */
  enable_hdp_protection();

  /* Set MPU to enable execution of Secure and Non Secure active slots */
  mpu_appli_cfg();
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

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
void TFM_LL_SECU_CheckStaticProtections(void)
{
  static FLASH_OBProgramInitTypeDef flash_option_bytes_bank1 = {0};
  static FLASH_OBProgramInitTypeDef flash_option_bytes_bank2 = {0};
#ifdef TFM_NSBOOT_CHECK_ENABLE
  static FLASH_OBProgramInitTypeDef flash_option_bytes_nsboot0 = {0};
  static FLASH_OBProgramInitTypeDef flash_option_bytes_nsboot1 = {0};
#endif /* TFM_NSBOOT_CHECK_ENABLE */
#ifdef TFM_ENABLE_SET_OB
  HAL_StatusTypeDef ret = HAL_ERROR;
#endif  /* TFM_ENABLE_SET_OB  */
  uint32_t start;
  uint32_t end;

#ifdef TFM_NSBOOT_CHECK_ENABLE
  /* Get NSBOOTADD0 and NSBOOTADD1 value */
  flash_option_bytes_nsboot0.BootAddrConfig = OB_BOOTADDR_NS0;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_nsboot0);
  flash_option_bytes_nsboot1.BootAddrConfig = OB_BOOTADDR_NS1;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_nsboot1);
#endif /* TFM_NSBOOT_CHECK_ENABLE */

  /* Get bank1 areaA OB  */
  flash_option_bytes_bank1.WRPArea = OB_WRPAREA_BANK1_AREAA;
  flash_option_bytes_bank1.WMSecConfig = OB_WMSEC_AREA1;
  flash_option_bytes_bank1.BootAddrConfig = OB_BOOTADDR_SEC0;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank1);

  /* Get bank2 areaB OB  */
  flash_option_bytes_bank2.WRPArea = OB_WRPAREA_BANK2_AREAA;
  flash_option_bytes_bank2.WMSecConfig = OB_WMSEC_AREA2;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank2);

#ifdef TFM_ENABLE_SET_OB
  /* Clean the option configuration */
  flash_option_bytes_bank1.OptionType = 0;
  flash_option_bytes_bank2.OptionType = 0;
  flash_option_bytes_bank2.WRPArea = 0;
  flash_option_bytes_bank1.WRPArea = 0;
#endif /*   TFM_ENABLE_SET_OB */

  /* Check TZEN = 1 , we are in secure */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_TZEN) != FLASH_OPTR_TZEN)
  {
    BOOT_LOG_ERR("Unexpected value for TZEN value");
    Error_Handler();
  }

  /* Check if dual bank is set */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_DBANK) != FLASH_OPTR_DBANK)
  {
    BOOT_LOG_ERR("Unexpected value for dual bank configuration");
    Error_Handler();
  }

  /* Check if swap bank is reset */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_SWAP_BANK) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for swap bank configuration");
    Error_Handler();
  }

  /* Check secure boot address */
  if (flash_option_bytes_bank1.BootAddr != SBSFU_BOOT_VTOR_ADDR)
  {
    BOOT_LOG_INF("BootAddr 0x%x", flash_option_bytes_bank1.BootAddr);
    BOOT_LOG_ERR("Unexpected value for SEC BOOT Address");
    Error_Handler();
  }

#ifdef TFM_NSBOOT_CHECK_ENABLE
  /* Check non-secure boot addresses */
  if ((flash_option_bytes_nsboot0.BootAddr != SBSFU_BOOT_VTOR_ADDR)
      || (flash_option_bytes_nsboot1.BootAddr != SBSFU_BOOT_VTOR_ADDR))
  {
    BOOT_LOG_ERR("Unexpected value for NS BOOT Address");
    Error_Handler();
  }
#endif /* TFM_NSBOOT_CHECK_ENABLE */

  /* Check bank1 secure flash protection */
  start = FLASH_BL2_NVCNT_AREA_OFFSET / PAGE_SIZE;
  end = (S_IMAGE_PRIMARY_PARTITION_OFFSET  + FLASH_S_PARTITION_SIZE - 1) / PAGE_SIZE;
  if (end > PAGE_MAX_NUMBER_IN_BANK)
  {
    end = PAGE_MAX_NUMBER_IN_BANK;
  }
  if ((start != flash_option_bytes_bank1.WMSecStartPage)
      || (end != flash_option_bytes_bank1.WMSecEndPage))
  {
    BOOT_LOG_INF("BANK 1 secure flash [%d, %d] : OB [%d, %d]",
                 start, end, flash_option_bytes_bank1.WMSecStartPage, flash_option_bytes_bank1.WMSecEndPage);
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for secure flash protection");
    Error_Handler();
#else
    BOOT_LOG_ERR("Unexpected value for secure flash protection: set wmsec1");
    flash_option_bytes_bank1.WMSecStartPage = start;
    flash_option_bytes_bank1.WMSecEndPage = end;
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_WMSEC;
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_AREA1 | OB_WMSEC_SECURE_AREA_CONFIG;
#endif /* TFM_ENABLE_SET_OB */
  }

  /* Check bank2 secure flash protection */
  start = 0;
  end = (S_IMAGE_PRIMARY_PARTITION_OFFSET  + FLASH_S_PARTITION_SIZE - 1) / PAGE_SIZE;
  if (end > PAGE_MAX_NUMBER_IN_BANK)
  {
    end = end - (PAGE_MAX_NUMBER_IN_BANK + 1);
    if ((start != flash_option_bytes_bank2.WMSecStartPage)
        || (end != flash_option_bytes_bank2.WMSecEndPage))
    {
    BOOT_LOG_INF("BANK 2 secure flash [%d, %d] : OB [%d, %d]", start, end, flash_option_bytes_bank2.WMSecStartPage,
                   flash_option_bytes_bank2.WMSecEndPage);
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for secure flash protection");
    Error_Handler();
#else
    BOOT_LOG_ERR("Unexpected value for secure flash protection : set wmsec2");
    flash_option_bytes_bank2.WMSecStartPage = start;
    flash_option_bytes_bank2.WMSecEndPage = end;
    flash_option_bytes_bank2.OptionType = OPTIONBYTE_WMSEC;
    flash_option_bytes_bank2.WMSecConfig |= OB_WMSEC_AREA2 | OB_WMSEC_SECURE_AREA_CONFIG ;
#endif /* TFM_ENABLE_SET_OB  */
    }
  }
  /* the bank 2 must be fully unsecure */
  else if (flash_option_bytes_bank2.WMSecEndPage >= flash_option_bytes_bank2.WMSecStartPage)
  {
    BOOT_LOG_INF("BANK 2 secure flash [%d, %d] : OB [%d, %d]", 127, 0, flash_option_bytes_bank2.WMSecStartPage,
                 flash_option_bytes_bank2.WMSecEndPage);
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for secure flash protection");
    Error_Handler();
#else
    /* bank is not unsecured , modify option bytes */
    flash_option_bytes_bank2.WMSecStartPage = 127;
    flash_option_bytes_bank2.WMSecEndPage = 0;
    flash_option_bytes_bank2.OptionType = OPTIONBYTE_WMSEC;
    flash_option_bytes_bank2.WMSecConfig |= OB_WMSEC_AREA2 | OB_WMSEC_SECURE_AREA_CONFIG ;
#endif /* TFM_ENABLE_SET_OB */
  }

#ifdef  TFM_WRP_PROTECT_ENABLE
  /* Check flash write protection */
  start = FLASH_AREA_PERSO_OFFSET / PAGE_SIZE;
  end = (FLASH_AREA_PERSO_OFFSET + FLASH_AREA_PERSO_SIZE + FLASH_AREA_BL2_SIZE +
         FLASH_AREA_BL2_NOHDP_SIZE - 1) / PAGE_SIZE;
  if ((flash_option_bytes_bank1.WRPStartOffset > flash_option_bytes_bank1.WRPEndOffset)
      || (start != flash_option_bytes_bank1.WRPStartOffset)
      || (end != flash_option_bytes_bank1.WRPEndOffset))
  {
    BOOT_LOG_INF("BANK 1 flash write protection [%d, %d] : OB [%d, %d]", start, end,
                 flash_option_bytes_bank1.WRPStartOffset,
                 flash_option_bytes_bank1.WRPEndOffset);
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for write protection ");
    Error_Handler();
#else
    flash_option_bytes_bank1.WRPStartOffset = start;
    flash_option_bytes_bank1.WRPEndOffset = end;
    flash_option_bytes_bank1.WRPArea |= OB_WRPAREA_BANK1_AREAA;

    BOOT_LOG_ERR("Unexpected value for write protection : set wrp1");
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_WRP;
#endif /* TFM_ENABLE_SET_OB */
  }
#ifdef MCUBOOT_EXT_LOADER
  /* Check flash write protection for local loader */
  start = FLASH_AREA_LOADER_BANK_OFFSET / PAGE_SIZE;
  end = (FLASH_B_SIZE - 1) / PAGE_SIZE;
  if ((flash_option_bytes_bank2.WRPStartOffset > flash_option_bytes_bank2.WRPEndOffset)
      || (start != flash_option_bytes_bank2.WRPStartOffset)
      || (end != flash_option_bytes_bank2.WRPEndOffset))
  {
    BOOT_LOG_INF("BANK 2 flash write protection [%d, %d] : OB [%d, %d]", start, end,
                 flash_option_bytes_bank2.WRPStartOffset,
                 flash_option_bytes_bank2.WRPEndOffset);
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for write protection ");
    Error_Handler();
#else
    flash_option_bytes_bank2.WRPStartOffset = start;
    flash_option_bytes_bank2.WRPEndOffset = end;
    flash_option_bytes_bank2.WRPArea |= OB_WRPAREA_BANK2_AREAA;

    BOOT_LOG_ERR("Unexpected value for write protection : set wrp2");
    flash_option_bytes_bank2.OptionType |= OPTIONBYTE_WRP;
#endif /* TFM_ENABLE_SET_OB */
  }
#endif /* MCUBOOT_EXT_LOADER */
#endif /* TFM_WRP_PROTECT_ENABLE */

#ifdef  TFM_HDP_PROTECT_ENABLE
  /* Check secure user flash protection (HDP) */
  start = FLASH_BL2_NVCNT_AREA_OFFSET / PAGE_SIZE;
  end = (FLASH_BL2_HDP_END) / PAGE_SIZE;
  if (
    (flash_option_bytes_bank1.WMSecStartPage > flash_option_bytes_bank1.WMHDPEndPage)
    || (start < flash_option_bytes_bank1.WMSecStartPage)
    || (end > flash_option_bytes_bank1.WMHDPEndPage)
    || (flash_option_bytes_bank1.WMSecConfig & OB_WMSEC_HDP_AREA_DISABLE))
  {
    BOOT_LOG_INF("BANK 1 secure user flash [%d, %d] : OB [%d, %d]",
                 start,
                 end,
                 flash_option_bytes_bank1.WMSecStartPage,
                 flash_option_bytes_bank1.WMHDPEndPage);
#ifndef TFM_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for secure user flash protection");
    Error_Handler();
#else
    BOOT_LOG_ERR("Unexpected value for secure user flash protection : set hdp1");
    flash_option_bytes_bank1.WMSecStartPage = start;
    flash_option_bytes_bank1.WMHDPEndPage = end;
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_WMSEC;
    /*  clean disable */
    flash_option_bytes_bank1.WMSecConfig &= ~OB_WMSEC_HDP_AREA_DISABLE;
    /* enable */
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_CONFIG ;
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_ENABLE;
    flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_AREA1;
#endif  /*  TFM_ENABLE_SET_OB */
  }
#else /* TFM_HDP_PROTECT_ENABLE */
  flash_option_bytes_bank1.WMSecConfig &= ~(OB_WMSEC_HDP_AREA_CONFIG | OB_WMSEC_HDP_AREA_ENABLE);
  flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_DISABLE;
#endif /* TFM_HDP_PROTECT_ENABLE */

#ifdef TFM_SECURE_USER_SRAM2_ERASE_AT_RESET
  /* Check SRAM2 ERASE on reset */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_SRAM2_RST) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for SRAM2 ERASE at Reset");
    Error_Handler();
  }
#endif /*TFM_SECURE_USER_SRAM2_ERASE_AT_RESET */

#ifdef TFM_ENABLE_SET_OB
  /* Configure Options Bytes */
  if ((flash_option_bytes_bank1.OptionType != 0) || (flash_option_bytes_bank2.OptionType != 0))
  {
    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Unlock the Options Bytes */
    HAL_FLASH_OB_Unlock();

    /* Verify Options Bytes to configure */
    if ((flash_option_bytes_bank1.OptionType & OPTIONBYTE_RDP) != 0)
    {
      BOOT_LOG_ERR("Unexpected value for OB RDP to program");
      Error_Handler();
    }
    if ((flash_option_bytes_bank2.OptionType & OPTIONBYTE_RDP) != 0)
    {
      BOOT_LOG_ERR("Unexpected value for OB RDP to program");
      Error_Handler();
    }
#ifdef MCUBOOT_EXT_LOADER
    if ((flash_option_bytes_bank2.WRPArea & ~OB_WRPAREA_BANK2_AREAA) != 0)
#else
	if ((flash_option_bytes_bank2.WRPArea & ~OB_WRPAREA_BANK2_AREAA) != 0)
#endif
	{
      BOOT_LOG_ERR("Unexpected value for bank 2 OB WRP AREA to program");
      Error_Handler();
    }
    if ((flash_option_bytes_bank1.WRPArea & ~OB_WRPAREA_BANK1_AREAA) != 0)
    {
      BOOT_LOG_ERR("Unexpected value for bank 1 OB WRP AREA to program");
      Error_Handler();
    }
    if ((flash_option_bytes_bank1.OptionType) != 0)
    {
      /* Program the Options Bytes */
      ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank1);
      if (ret != HAL_OK)
      {
        BOOT_LOG_ERR("Error while setting OB Bank1 config");
        Error_Handler();
      }
    }
    if ((flash_option_bytes_bank2.OptionType) != 0)
    {
      /* Program the Options Bytes */
      ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank2);
      if (ret != HAL_OK)
      {
        BOOT_LOG_ERR("Error while setting OB Bank1 config");
        Error_Handler();
      }
    }

    /* Launch the Options Bytes (reset the board, should not return) */
    ret = HAL_FLASH_OB_Launch();
    if (ret != HAL_OK)
    {
      BOOT_LOG_ERR("Error while execution OB_Launch");
      Error_Handler();
    }

    /* Code should not be reached, reset the board */
    HAL_NVIC_SystemReset();
  }
#endif /* TFM_ENABLE_SET_OB */

#ifdef TFM_OB_BOOT_LOCK
  /* Check Boot lock protection */
  if (flash_option_bytes_bank1.BootLock != TFM_OB_BOOT_LOCK)
  {
    BOOT_LOG_INF("BootLock 0x%x", flash_option_bytes_bank1.BootLock);
    BOOT_LOG_ERR("Unexpected value for SEC BOOT LOCK");
    Error_Handler();
  }
#endif /* TFM_OB_BOOT_LOCK */

#ifdef TFM_OB_RDP_LEVEL_VALUE
  /* Check RDL level */
  if (flash_option_bytes_bank1.RDPLevel != TFM_OB_RDP_LEVEL_VALUE)
  {
    BOOT_LOG_INF("RDPLevel 0x%x (0x%x)", flash_option_bytes_bank1.RDPLevel, TFM_OB_RDP_LEVEL_VALUE);
    BOOT_LOG_ERR("Unexpected value for RDP level");
    rdp_level(TFM_OB_RDP_LEVEL_VALUE, flash_option_bytes_bank1.RDPLevel);
    Error_Handler();
  }
#endif /* TFM_OB_RDP_LEVEL_VALUE */
}

#ifdef TFM_OB_RDP_LEVEL_VALUE
static void rdp_level(uint32_t rdplevel, uint32_t current_rdplevel)
{
#ifdef TFM_ENABLE_SET_OB
  static FLASH_OBProgramInitTypeDef flash_option_bytes_bank = {0};
  HAL_StatusTypeDef ret = HAL_ERROR;

  if ((rdplevel == OB_RDP_LEVEL_0) ||
      ((rdplevel == OB_RDP_LEVEL_0_5) && (current_rdplevel == OB_RDP_LEVEL_1)) ||
      (rdplevel == OB_RDP_LEVEL_2))
  {
    Error_Handler();
  }

  flash_option_bytes_bank.OptionType = OPTIONBYTE_RDP;
  flash_option_bytes_bank.RDPLevel = rdplevel;
  BOOT_LOG_INF("Programming RDP to %x", rdplevel);
  BOOT_LOG_INF("Unplug/Plug jumper JP5 (IDD)");

  /* Unlock the Flash to enable the flash control register access */
  HAL_FLASH_Unlock();

  /* Unlock the Options Bytes */
  HAL_FLASH_OB_Unlock();

  /* Program the Options Bytes */
  ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank);
  if (ret != HAL_OK)
  {
    BOOT_LOG_ERR("Error while setting RDP level");
    Error_Handler();
  }

  /* Launch the Options Bytes (reset the board, should not return) */
  ret = HAL_FLASH_OB_Launch();
  if (ret != HAL_OK)
  {
    BOOT_LOG_ERR("Error while execution OB_Launch");
    Error_Handler();
  }

  /* Code should not be reached, reset the board */
  HAL_NVIC_SystemReset();
#endif /* TFM_ENABLE_SET_OB */
}
#endif /* TFM_OB_RDP_LEVEL_VALUE */
#if defined(MCUBOOT_EXT_LOADER) && defined(MCUBOOT_PRIMARY_ONLY) 
static void secure_internal_flash(uint32_t offset_start, uint32_t offset_end)
{
	volatile uint32_t *SecBB[8]= {&FLASH_S->SECBB1R1, &FLASH_S->SECBB1R2, &FLASH_S->SECBB1R3, &FLASH_S->SECBB1R4,
      &FLASH_S->SECBB2R1, &FLASH_S->SECBB2R2, &FLASH_S->SECBB2R3, &FLASH_S->SECBB2R4};
    volatile uint32_t *ptr;
    uint32_t regwrite=0x0, index;
    uint32_t block_start = offset_start;
    uint32_t block_end =  offset_end;
    block_start = block_start / PAGE_SIZE;
    block_end = (block_end / PAGE_SIZE) ;
    /*  Check alignment to avoid further problem  */
    if (offset_start & (PAGE_SIZE-1))
		Error_Handler();
    /*  1f is for 32 bits */
    for (index = block_start & ~0x1f; index < (8<<5) ; index++)
    { /* clean register on index aligned */
      if (!(index & 0x1f)){
         regwrite=0x0;
      }
      if ((index >= block_start) && (index <= block_end))
        regwrite = regwrite | ( 1 << (index & 0x1f));
      /* write register when 32 sub block are set or last block to set  */
      if ((index & 0x1f ) == 0x1f) {
        ptr = (uint32_t *)SecBB[index>>5];
        *ptr =  regwrite;
       }
    }
}
#endif /* defined(MCUBOOT_EXT_LOADER) && defined(MCUBOOT_PRIMARY_ONLY)  */
/**
  * @brief  Memory Config Init
  * @param  None
  * @retval None
  */
static void  mpc_init_cfg(void)
{
#ifdef TFM_ERROR_HANDLER_NON_SECURE
  /*  unsecure only one block in SRAM1 */
  GTZC_MPCBB1_S->VCTR[0] = 0xfffffffe;
#endif /*  TFM_ERROR_HANDLER_NON_SECURE */

}

/**
  * @brief  Sau idau init
  * @param  None
  * @retval None
  */
static void sau_and_idau_cfg(void)
{
  /* Disable SAU */
  TZ_SAU_Disable();
#ifdef TFM_ERROR_HANDLER_NON_SECURE
  /* Configures SAU regions non-secure to gain access to SRAM1 non secure   */
  SAU->RNR  = 0;
  SAU->RBAR = (SRAM1_BASE_NS & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((SRAM1_BASE_NS + NS_NO_INIT_DATA_SIZE - 1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#endif /* TFM_ERROR_HANDLER_NON_SECURE */
  /* Allow non secure access to Flash non Secure peripheral for regression */
  /* Secure HAL flash driver uses the non secure flash address to perform access on non secure flash area */
  SAU->RNR  = 1;
  SAU->RBAR = ((uint32_t)FLASH_NS & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = (((uint32_t)FLASH_NS + 0xffff) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
  /* Allow non secure Flash base access for Area 1/2/3 */
  SAU->RNR  = 2;
  SAU->RBAR = ((uint32_t)FLASH_BASE_NS + NS_IMAGE_PRIMARY_PARTITION_OFFSET) & SAU_RBAR_BADDR_Msk;
#if !defined(MCUBOOT_PRIMARY_ONLY)
  SAU->RLAR = (((uint32_t)FLASH_BASE_NS + S_IMAGE_SECONDARY_PARTITION_OFFSET
                + FLASH_PARTITION_SIZE-1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#else
  SAU->RLAR = (((uint32_t)FLASH_BASE_NS + NS_IMAGE_PRIMARY_PARTITION_OFFSET
                + FLASH_NS_PARTITION_SIZE-1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#endif /* MCUBOOT_PRIMARY_ONLY */
#if defined(MCUBOOT_EXT_LOADER)
  /* create a region to access to loader code for hash check */
  SAU->RNR  = 3;
  SAU->RBAR = LOADER_NS_ROM_ALIAS(FLASH_AREA_LOADER_OFFSET) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = LOADER_NS_ROM_ALIAS(FLASH_TOTAL_SIZE-1) & SAU_RBAR_BADDR_Msk;
#endif
  /* Force memory writes before continuing */
  __DSB();
  /* Flush and refill pipeline with updated permissions */
  __ISB();
  /* Enable SAU */
  TZ_SAU_Enable();
}

#ifdef MCUBOOT_EXT_LOADER
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */
/**
  * @brief  Sau idau update for executing loader
  * @param  None
  * @retval None
  */
static void sau_loader_cfg(void)
{
  /* Disable SAU */
  TZ_SAU_Disable();
  /* allow access to non secure loader area ram*/
  SAU->RNR  = 0;
  SAU->RBAR = ((uint32_t)LOADER_NS_DATA_START) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = (((uint32_t)LOADER_NS_DATA_LIMIT) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
  /* loader non secure need access to periph */
  SAU->RNR  = 1;
  SAU->RBAR = ((uint32_t)PERIPH_BASE_NS & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = (((uint32_t)PERIPH_BASE_NS + 0xFFFFFFF) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
  /* allow access to non secure loader area code */
#ifdef MCUBOOT_EXT_LOADER
  SAU->RNR  = 3;
  SAU->RBAR = ((uint32_t)LOADER_NS_CODE_START) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = (((uint32_t)LOADER_NS_CODE_LIMIT) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#endif /*   MCUBOOT_EXT_LOADER */
  
#if defined(MCUBOOT_PRIMARY_ONLY) 
  /* allow access to non secure calleable area */
  SAU->RNR  = 4;
  SAU->RBAR = ((uint32_t)LOADER_CMSE_VENEER_REGION_START) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = (((uint32_t)LOADER_CMSE_VENEER_REGION_LIMIT) & SAU_RLAR_LADDR_Msk) |  SAU_RLAR_NSC_Msk |SAU_RLAR_ENABLE_Msk;
#endif
 
  /* Force memory writes before continuing */
  __DSB();
  /* Flush and refill pipeline with updated permissions */
  __ISB();
  /* Enable SAU */
  TZ_SAU_Enable();
/* FIX ME to INDENT better */
#if 1
   /* set up floating point */
/*
// <e>Setup behaviour of Floating Point Unit
*/
#define TZ_FPU_NS_USAGE 1

/*
// <o>Floating Point Unit usage
//     <0=> Secure state only
//     <3=> Secure and Non-Secure state
//   <i> Value for SCB->NSACR register bits CP10, CP11
*/
#define SCB_NSACR_CP10_11_VAL       3

/*
// <o>Treat floating-point registers as Secure
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit TS
*/
#define FPU_FPCCR_TS_VAL            0

/*
// <o>Clear on return (CLRONRET) accessibility
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for FPU->FPCCR register bit CLRONRETS
*/
#define FPU_FPCCR_CLRONRETS_VAL     0

/*
// <o>Clear floating-point caller saved registers on exception return
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit CLRONRET
*/
#define FPU_FPCCR_CLRONRET_VAL      1
    SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk)) |
                   ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

    FPU->FPCCR = (FPU->FPCCR & ~(FPU_FPCCR_TS_Msk | FPU_FPCCR_CLRONRETS_Msk | FPU_FPCCR_CLRONRET_Msk)) |
                   ((FPU_FPCCR_TS_VAL        << FPU_FPCCR_TS_Pos       ) & FPU_FPCCR_TS_Msk       ) |
                   ((FPU_FPCCR_CLRONRETS_VAL << FPU_FPCCR_CLRONRETS_Pos) & FPU_FPCCR_CLRONRETS_Msk) |
                   ((FPU_FPCCR_CLRONRET_VAL  << FPU_FPCCR_CLRONRET_Pos ) & FPU_FPCCR_CLRONRET_Msk );
  /* Lock SAU config */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  SYSCFG->CSLCKR |= SYSCFG_CSLCKR_LOCKSAU;
#endif
}
/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */
#endif /* MCUBOOT_EXT_LOADER */
/**
  * @brief  mpu init
  * @param  None
  * @retval None
  */
static void mpu_init_cfg(void)
{
#ifdef TFM_BOOT_MPU_PROTECTION
  struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
  struct mpu_armv8m_dev_t dev_mpu_ns = { MPU_BASE_NS};
  struct mpu_armv8m_region_cfg_t region_cfg;
  /* Secure MPU */
  /* background region is enabled , secure execution on unsecure flash is not possible*/
  /* but non secure execution on unsecure flash is possible , non secure mpu is used to protect execution */
  /* since SAU is enabled later to gain access to non secure flash */
  /* Forbid execuction outside of flash write protected area  */
  /* descriptor 0 is set execute readonly before jumping in Secure application */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET ;
  region_cfg.region_limit = FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  region_cfg.region_nr = 1;
  region_cfg.region_base = FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE ;
#ifndef MCUBOOT_PRIMARY_ONLY
  region_cfg.region_limit = FLASH_BASE_S + NS_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 1;
#else
   region_cfg.region_limit = FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE + FLASH_NS_PARTITION_SIZE - 1;
#endif /* MCUBOOT_PRIMARY_ONLY */
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }

  region_cfg.region_nr = 2;
  region_cfg.region_base = FLASH_BASE_S;
  region_cfg.region_limit = FLASH_BASE_S + FLASH_AREA_BL2_OFFSET - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  region_cfg.region_nr = 3;
  region_cfg.region_base = FLASH_BASE_S + FLASH_AREA_BL2_NOHDP_OFFSET + FLASH_AREA_BL2_NOHDP_SIZE;
  region_cfg.region_limit = FLASH_BASE_S + FLASH_AREA_0_OFFSET - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* Forbid execuction on full SRAM area */
  region_cfg.region_nr = 4;
  region_cfg.region_base = SRAM1_BASE_S ;
  region_cfg.region_limit = SRAM1_BASE_S + TOTAL_RAM_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* forbid secure peripheral execution */
  region_cfg.region_nr = 5;
  region_cfg.region_base = PERIPH_BASE_S;
  region_cfg.region_limit = PERIPH_BASE_S + 0xFFFFFFF;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* enable secure MPU */
  mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE,
                    HARDFAULT_NMI_ENABLE);
  /* forbid execution on non secure FLASH /RAM in case of jump in non secure */
  /* Non Secure MPU  background all access in priviligied */
  /* reduced execution to all flash during control */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_NS + NS_IMAGE_PRIMARY_PARTITION_OFFSET ;
  region_cfg.region_limit = FLASH_BASE_NS + NS_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
#if !defined(MCUBOOT_PRIMARY_ONLY)
  region_cfg.region_nr = 1;
  region_cfg.region_base = FLASH_BASE_NS + S_IMAGE_SECONDARY_PARTITION_OFFSET;
  region_cfg.region_limit = FLASH_BASE_NS + NS_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
#endif /* MCUBOOT_PRIMARY_ONLY */
  /* Forbid execuction on full SRAM area */
  region_cfg.region_nr = 2;
#ifdef TFM_ERROR_HANDLER_NON_SECURE
  region_cfg.region_base = SRAM1_BASE_NS + (~MPU_RBAR_BASE_Msk) + 1;
#else
  region_cfg.region_base = SRAM1_BASE_NS ;
#endif /*   TFM_ERROR_HANDLER_NON_SECURE */
  region_cfg.region_limit = SRAM1_BASE_NS + TOTAL_RAM_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* forbid secure peripheral execution */
  region_cfg.region_nr = 3;
  region_cfg.region_base = PERIPH_BASE_NS;
  region_cfg.region_limit = PERIPH_BASE_NS + 0xFFFFFFF;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* enable non secure MPU */
  mpu_armv8m_enable(&dev_mpu_ns, PRIVILEGED_DEFAULT_ENABLE,
                    HARDFAULT_NMI_ENABLE);
#endif /* TFM_BOOT_MPU_PROTECTION */
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */

static void mpu_appli_cfg(void)
{
#ifdef TFM_BOOT_MPU_PROTECTION
  /* static variables are used to ensure rodata placement in the specific section */
  static struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
  static struct mpu_armv8m_dev_t dev_mpu_ns = { MPU_BASE_NS};
  struct mpu_armv8m_region_cfg_t region_cfg;
  /* region 0 is now enable for execution */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET ;
  region_cfg.region_limit = FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* region 0 is now enable for execution */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_NS + NS_IMAGE_PRIMARY_PARTITION_OFFSET;
  region_cfg.region_limit = FLASH_BASE_NS + NS_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
#endif /* TFM_BOOT_MPU_PROTECTION */
}
/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

#ifdef MCUBOOT_EXT_LOADER
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */
static void mpu_loader_cfg(void)
{
#ifdef TFM_BOOT_MPU_PROTECTION 
#if defined(MCUBOOT_PRIMARY_ONLY)
  struct mpu_armv8m_region_cfg_t region_cfg;
  /* static variables are used to ensure rodata placement in the specific section */
  static struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
  /* secure region 2 is configured to allow read /write to NS_RAM */
  mpu_armv8m_region_disable(&dev_mpu_s,2);
  region_cfg.region_nr = 7;
  region_cfg.region_base = SRAM1_BASE_NS;
  region_cfg.region_limit = SRAM1_BASE_NS + TOTAL_RAM_SIZE - 1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
#endif /* MCUBOOT_PRIMARY_ONLY */

#endif /* TFM_BOOT_MPU_PROTECTION  */
  /* Lock Secure MPU config */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  SYSCFG->CSLCKR |= SYSCFG_CSLCKR_LOCKSMPU;  

   /* Lock Non Secure MPU config */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  SYSCFG->CSLCKR |= SYSCFG_CNSLCKR_LOCKNSMPU;
}
/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */
#endif /* MCUBOOT_EXT_LOADER */


/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */

/**
  * @brief  Enable HDP protection
  * @param  None
  * @retval None
  */
static void enable_hdp_protection(void)
{
#ifdef TFM_HDP_PROTECT_ENABLE
  do
  {
    /* Activate HDP protection */
    SET_BIT(FLASH->SECHDPCR, FLASH_SECHDPCR_HDP1_ACCDIS);
  } while ((FLASH->SECHDPCR & FLASH_SECHDPCR_HDP1_ACCDIS) != FLASH_SECHDPCR_HDP1_ACCDIS);

  if ((FLASH->SECHDPCR & FLASH_SECHDPCR_HDP1_ACCDIS) != FLASH_SECHDPCR_HDP1_ACCDIS)
  {
    /* Security issue : execution stopped ! */
    Error_Handler();
  }
#endif /* TFM_HDP_PROTECT_ENABLE */
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
