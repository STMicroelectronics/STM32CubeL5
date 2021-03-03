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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
#include "Driver_Flash.h"
#include "region_defs.h"
/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */
#if defined(MCUBOOT_PRIMARY_ONLY)
extern ARM_DRIVER_FLASH LOADER_FLASH_DEV_NAME;
/** @addtogroup Templates
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Secure Flash Program Non Secure Calleable
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY int32_t SECURE_Flash_ProgramData(int32_t addr,const void *data, uint32_t cnt)
{
 /* Data outside of non seure data are not flashed */
  if (((uint32_t)data < LOADER_NS_DATA_START ) || (((uint32_t)data + cnt)> LOADER_NS_DATA_LIMIT))
	return -1;
  return LOADER_FLASH_DEV_NAME.ProgramData(addr, data, cnt);
}

/**
  * @brief  Secure Flash Erase Sector Non Secure Calleable
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY int32_t SECURE_Flash_EraseSector(uint32_t addr)
{
   return LOADER_FLASH_DEV_NAME.EraseSector(addr);
}
#else
CMSE_NS_ENTRY void dummy(void)
{
	return;
}
#endif /* MCUBOOT_PRIMARY_ONLY */
/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
