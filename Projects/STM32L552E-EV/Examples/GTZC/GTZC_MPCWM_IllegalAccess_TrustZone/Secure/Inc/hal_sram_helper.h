/**
  ******************************************************************************
  * @file    GTZC/GTZC_MPCWM_IllegalAccess_TrustZone/Secure/Inc/hal_sram_helper.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the SRAM helper.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_SRAM_HELPER_H
#define HAL_SRAM_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* ******************** FMC SRAM **********************************************/
#define IS61WV102416BLL_SUPPORT

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#ifdef IS61WV102416BLL_SUPPORT
typedef struct
{
  uint32_t AddressSetupTime;             /*!< Defines the minimum address setup time in ns.
                                              @note This parameter is not used with synchronous NOR Flash memories.      */

  uint32_t AddressHoldTime;              /*!< Defines the minimum address hold time in ns.
                                              @note This parameter is not used with synchronous NOR Flash memories.      */

  uint32_t DataSetupTime;                /*!< Defines the minimum data setup time in ns.
                                              @note This parameter is used for SRAMs, ROMs and asynchronous multiplexed
                                              NOR Flash memories.                                                        */

  uint32_t DataHoldTime;                 /*!< Defines the minimum data hold time in ns.
                                              @note This parameter is not used with synchronous NOR Flash memories.      */

  uint32_t PreCharge;                    /*!< Defines the minimum pre-charge time in ns.
                                              @note This parameter is only used for multiplexed NOR Flash memories and FRAM.      */

  uint32_t MaxCLKPeriod;                 /*!< Defines the max period of CLK clock output signal in ns (0 for unknown or asynchrone mode).
                                              @note This parameter is not used for asynchronous NOR Flash, SRAM or ROM
                                              accesses.                                                                  */

  uint32_t DataLatency;                  /*!< Defines the number of memory clock cycles to issue
                                              to the memory before getting the first data.
                                              The parameter value depends on the memory type as shown below:
                                              - It must be set to 0 in case of a CRAM
                                              - It is don't care in asynchronous NOR, SRAM or ROM accesses
                                              - It may assume a value between Min_Data = 2 and Max_Data = 17 in NOR and PSRAM Flash memories
                                                with synchronous burst mode enable                                       */
} NORSRAM_TimingTypeDef;
#endif /* IS61WV102416BLL_SUPPORT */

/* Exported constants --------------------------------------------------------*/
/********************* SRAM base ADDR and NEx definition **********************/
#define FMC_SRAM_BANK_ADDR         ((uint32_t) 0x60000000)
#define FMC_SRAM_BANKx             FMC_NORSRAM_BANK1

#ifdef IS61WV102416BLL_SUPPORT
/********************** SRAM SIZE ************************/
#define SRAM_SIZE_16               (1024*1024)
#define SRAM_SIZE_8                SRAM_SIZE_16*2
#define SRAM_SIZE_32               SRAM_SIZE_16/2
#endif /* IS61WV102416BLL_SUPPORT */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint32_t extSRAM_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_SRAM_HELPER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
