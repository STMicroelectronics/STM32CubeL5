/**
  ******************************************************************************
  * @file    TFM_SBSFU/Src/jumper.c
  * @author  MCD Application Team
  * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#ifdef TFM_WRP_BL2_SHARED_DATA
#include "tfm_low_level_security.h"
#include "tfm_bl2_shared_data.h"
#include "flash_layout.h"
#endif /* TFM_WRP_BL2_SHARED_DATA */

#include "mpu_armv8m_drv.h"
#if defined(TFM_SECURE_USER_PROTECT_ENABLE) && defined(TFM_USE_RSS_SERVICE)
#include "tfm_low_level_security_rss.h"
/* Private function prototypes -----------------------------------------------*/
/** @defgroup jumper Private Functions
  * @{
  */

static void launch_application(uint32_t hdp_area, uint32_t VectorTableAddr);

/**
  * @}
  */

static void launch_application(uint32_t hdp_area, uint32_t VectorTableAddr)
{
  if (BOOTLOADER_ID == 0x91)
  {
    RSSLIB_sec_CloseExitHDP_BL91(hdp_area, VectorTableAddr);
  }
  else if (BOOTLOADER_ID == 0x90)
  {
    RSSLIB_sec_CloseExitHDP_BL90(hdp_area, VectorTableAddr);
  }
  else
  {
    BOOT_LOG_ERR("Wrong BootLoader version: 0x%1x, HDP (switch TFM_SECURE_USER_PROTECT_ENABLE) must be disabled",
                 BOOTLOADER_ID);
  }
}

#endif /* TFM_SECURE_USER_PROTECT_ENABLE && TFM_USE_RSS_SERVICE*/
/**
  * @brief  jumper;
  * @param  arm_vector_table *vt
  * @retval None
  */
void jumper(struct arm_vector_table *vt)
{
  /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP then the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
#if !defined(TFM_USE_RSS_SERVICE) || !defined(TFM_SECURE_USER_PROTECT_ENABLE)
  static struct arm_vector_table *vt_cpy;
#endif /* !defined(TFM_USE_RSS_SERVICE) || !defined(TFM_SECURE_USER_PROTECT_ENABLE)*/
#ifdef TFM_BOOT_MPU_PROTECTION
  mpu_config_appli();
#endif /* TFM_BOOT_MPU_PROTECTION */
#if !defined(TFM_USE_RSS_SERVICE) || !defined(TFM_SECURE_USER_PROTECT_ENABLE)
  vt_cpy = vt;
#endif /* defined(TFM_USE_RSS_SERVICE) || !defined(TFM_SECURE_USER_PROTECT_ENABLE) */
#ifdef TFM_WRP_BL2_SHARED_DATA
  TFM_LL_SECU_ApplyWRP_SRAM2(BOOT_TFM_SHARED_DATA_BASE, BOOT_TFM_SHARED_DATA_SIZE);
#endif /*  TFM_WRP_BL2_SHARED_DATA */

  /*  change stack limit  */
  __set_MSPLIM(0);
#if defined(TFM_SECURE_USER_PROTECT_ENABLE) && defined(TFM_USE_RSS_SERVICE)
  launch_application(RSSLIB_HDP_AREA1_Msk, (uint32_t) vt);
  while (1);
#else
  /* Restore the Main Stack Pointer Limit register's reset value
  * before passing execution to runtime firmware to make the
  * bootloader transparent to it.
  */
  __set_MSP(vt_cpy->msp);
  __DSB();
  __ISB();
#ifdef TFM_SECURE_USER_PROTECT_ENABLE
  /*  The source code is compile with optimization to
      ensure that the instruction performing the jump to reset has been done
    before HDP get activated */
  /*  after HDP is enabled no breakpooint can be set  */
  FLASH_S->SECHDPCR |= 0x1;
#endif /*  TFM_SECURE_USER_PROTECT_ENABLE */
  ((void (*)(void))vt_cpy->reset)();
#endif /* TFM_SECURE_USER_PROTECT_ENABLE && TFM_USE_RSS_SERVICE */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
