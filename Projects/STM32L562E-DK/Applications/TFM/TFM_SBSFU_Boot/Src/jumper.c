/**
  ******************************************************************************
  * @file    jumper.c
  * @author  MCD Application Team
  * @brief   This file provides the operations for secure application launch
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
#include "main.h"
#include "tfm_low_level_security.h"
#include "tfm_bl2_shared_data.h"
#include "flash_layout.h"
#include "mpu_armv8m_drv.h"
#if defined(TFM_HDP_PROTECT_ENABLE)
#ifdef TFM_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* TFM_DEV_MODE  */
#include "bootutil/bootutil_log.h"
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
  if (BL_ID == (0xd0))
  {
    BOOT_LOG_ERR("Wrong BootLoader version: 0x%1x, HDP (switch TFM_HDP_PROTECT_ENABLE) must be disabled",
                 BL_ID);;
  }
  else if (BL_ID == (0x90))
  {
    RSSLIB_PFUNC->S.CloseExitHDP_BL90(hdp_area, VectorTableAddr);
  }
  else
  {
    RSSLIB_PFUNC->S.CloseExitHDP_BL91(hdp_area, VectorTableAddr);
  }
}

#endif /* TFM_HDP_PROTECT_ENABLE */
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
#if !defined(TFM_HDP_PROTECT_ENABLE)
  static struct arm_vector_table *vt_cpy;
#endif /* !defined(TFM_HDP_PROTECT_ENABLE)*/
#ifdef TFM_BOOT_MPU_PROTECTION
  mpu_config_appli();
#endif /* TFM_BOOT_MPU_PROTECTION */
  /* set the secure vector */
  SCB->VTOR = (uint32_t)vt;
#if  !defined(TFM_HDP_PROTECT_ENABLE)
  vt_cpy = vt;
#endif /* !defined(TFM_HDP_PROTECT_ENABLE) */
  TFM_LL_SECU_ApplyWRP_SRAM2(BOOT_TFM_SHARED_DATA_BASE, BOOT_TFM_SHARED_DATA_SIZE);
  /*  change stack limit  */
  __set_MSPLIM(0);
#if defined(TFM_HDP_PROTECT_ENABLE)
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
  ((void (*)(void))vt_cpy->reset)();
#endif /* TFM_HDP_PROTECT_ENABLE */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
