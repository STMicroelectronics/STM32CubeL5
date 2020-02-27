/**
  ******************************************************************************
  * @file    stm32l552e_eval_ospi.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l552e_eval_ospi.c driver.
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
#ifndef STM32L552E_EVAL_OSPI_H
#define STM32L552E_EVAL_OSPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l552e_eval_conf.h"
#include "stm32l552e_eval_errno.h"
#include "../Components/mx25lm51245g/mx25lm51245g.h"
#include "../Components/iss66wvh8m8/iss66wvh8m8.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E-EV
  * @{
  */

/** @addtogroup STM32L552E-EV_OSPI
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup STM32L552E-EV_OSPI_Exported_Types STM32L552E-EV OSPI Exported Types
  * @{
  */
typedef enum {
  OSPI_ACCESS_NONE = 0,          /*!<  Instance not initialized,              */
  OSPI_ACCESS_INDIRECT,          /*!<  Instance use indirect mode access      */
  OSPI_ACCESS_MMP                /*!<  Instance use Memory Mapped Mode read   */
} OSPI_Access_t;

#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pOSPI_CallbackTypeDef  pMspInitCb;
  pOSPI_CallbackTypeDef  pMspDeInitCb;
}BSP_OSPI_Cb_t;
#endif /* (USE_HAL_OSPI_REGISTER_CALLBACKS == 1) */

typedef struct
{
  uint32_t MemorySize;
  uint32_t ClockPrescaler;
  uint32_t SampleShifting;
  uint32_t TransferRate;
} MX_OSPI_InitTypeDef;
/**
  * @}
  */

/** @defgroup STM32L552E-EV_OSPI_NOR_Exported_Types STM32L552E-EV OSPI_NOR Exported Types
  * @{
  */
#define BSP_OSPI_NOR_Info_t                MX25LM51245G_Info_t
#define BSP_OSPI_NOR_Interface_t           MX25LM51245G_Interface_t
#define BSP_OSPI_NOR_Transfer_t            MX25LM51245G_Transfer_t
#define BSP_OSPI_NOR_Erase_t               MX25LM51245G_Erase_t

typedef struct
{
  OSPI_Access_t              IsInitialized;  /*!<  Instance access Flash method     */
  BSP_OSPI_NOR_Interface_t   InterfaceMode;  /*!<  Flash Interface mode of Instance */
  BSP_OSPI_NOR_Transfer_t    TransferRate;   /*!<  Flash Transfer mode of Instance  */
} OSPI_NOR_Ctx_t;

typedef struct
{
  BSP_OSPI_NOR_Interface_t   InterfaceMode;      /*!<  Current Flash Interface mode */
  BSP_OSPI_NOR_Transfer_t    TransferRate;       /*!<  Current Flash Transfer rate  */
} BSP_OSPI_NOR_Init_t;
  /**
  * @}
  */
  
/** @defgroup STM32L552E-EV_OSPI_RAM_Exported_Types STM32L552E-EV OSPI_RAM Exported Types
  * @{
  */
#define BSP_OSPI_RAM_BurstLength_t ISS66WVH8M8_BurstLength_t

typedef enum {
  BSP_OSPI_RAM_VARIABLE_LATENCY = HAL_OSPI_VARIABLE_LATENCY,
  BSP_OSPI_RAM_FIXED_LATENCY    = HAL_OSPI_FIXED_LATENCY
} BSP_OSPI_RAM_Latency_t;

typedef enum {
  BSP_OSPI_RAM_HYBRID_BURST = 0,
  BSP_OSPI_RAM_LINEAR_BURST
} BSP_OSPI_RAM_BurstType_t;

typedef struct
{
  OSPI_Access_t               IsInitialized; /*!< Instance access Flash method */
  BSP_OSPI_RAM_Latency_t      LatencyType;   /*!< Latency Type of Instance     */
  BSP_OSPI_RAM_BurstType_t    BurstType;     /*!< Burst Type of Instance       */
  BSP_OSPI_RAM_BurstLength_t  BurstLength;   /*!< Burst Length of Instance     */
} OSPI_RAM_Ctx_t;

typedef struct
{
  BSP_OSPI_RAM_Latency_t      LatencyType;   /*!< Current HyperRAM Latency Type */
  BSP_OSPI_RAM_BurstType_t    BurstType;     /*!< Current HyperRAM Burst Type   */
  BSP_OSPI_RAM_BurstLength_t  BurstLength;   /*!< Current HyperRAM Burst Length */
} BSP_OSPI_RAM_Init_t;
  /**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/** @defgroup STM32L552E-EV_OSPI_Exported_Constants STM32L552E-EV OSPI Exported Constants
  * @{
  */


/* Definition for OSPI clock resources */
#define OSPI_CLK_ENABLE()                 __HAL_RCC_OSPI1_CLK_ENABLE()
#define OSPI_CLK_DISABLE()                __HAL_RCC_OSPI1_CLK_DISABLE()

#define OSPI_CLK_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define OSPI_DQS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define OSPI_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define OSPI_D0_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define OSPI_D1_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define OSPI_D2_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define OSPI_D3_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define OSPI_D4_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define OSPI_D5_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define OSPI_D6_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define OSPI_D7_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()

#define OSPI_FORCE_RESET()                __HAL_RCC_OSPI1_FORCE_RESET()
#define OSPI_RELEASE_RESET()              __HAL_RCC_OSPI1_RELEASE_RESET()

/* Definition for OSPI Pins */
/* OSPI_CLK */
#define OSPI_CLK_PIN                      GPIO_PIN_3
#define OSPI_CLK_GPIO_PORT                GPIOA
#define OSPI_CLK_PIN_AF                   GPIO_AF10_OCTOSPI1
/* OSPI_DQS */
#define OSPI_DQS_PIN                      GPIO_PIN_2
#define OSPI_DQS_GPIO_PORT                GPIOB
#define OSPI_DQS_PIN_AF                   GPIO_AF10_OCTOSPI1
/* OSPI_CS */
#define OSPI_CS_PIN                       GPIO_PIN_2
#define OSPI_CS_GPIO_PORT                 GPIOA
#define OSPI_CS_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D0 */
#define OSPI_D0_PIN                       GPIO_PIN_1
#define OSPI_D0_GPIO_PORT                 GPIOB
#define OSPI_D0_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D1 */
#define OSPI_D1_PIN                       GPIO_PIN_0
#define OSPI_D1_GPIO_PORT                 GPIOB
#define OSPI_D1_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D2 */
#define OSPI_D2_PIN                       GPIO_PIN_7
#define OSPI_D2_GPIO_PORT                 GPIOA
#define OSPI_D2_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D3 */
#define OSPI_D3_PIN                       GPIO_PIN_6
#define OSPI_D3_GPIO_PORT                 GPIOA
#define OSPI_D3_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D4 */
#define OSPI_D4_PIN                       GPIO_PIN_1
#define OSPI_D4_GPIO_PORT                 GPIOC
#define OSPI_D4_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D5 */
#define OSPI_D5_PIN                       GPIO_PIN_2
#define OSPI_D5_GPIO_PORT                 GPIOC
#define OSPI_D5_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D6 */
#define OSPI_D6_PIN                       GPIO_PIN_3
#define OSPI_D6_GPIO_PORT                 GPIOC
#define OSPI_D6_PIN_AF                    GPIO_AF10_OCTOSPI1
/* OSPI_D7 */
#define OSPI_D7_PIN                       GPIO_PIN_0
#define OSPI_D7_GPIO_PORT                 GPIOC
#define OSPI_D7_PIN_AF                    GPIO_AF3_OCTOSPI1

/**
  * @}
  */

/** @defgroup STM32L552E-EV_OSPI_NOR_Exported_Constants STM32L552E-EV OSPI_NOR Exported Constants
  * @{
  */
#define OSPI_NOR_INSTANCES_NUMBER         1U

/* Definition for OSPI modes */
#define BSP_OSPI_NOR_SPI_MODE             (BSP_OSPI_NOR_Interface_t)MX25LM51245G_SPI_MODE      /* 1 Cmd Line, 1 Address Line and 1 Data Line    */
#define BSP_OSPI_NOR_OPI_MODE             (BSP_OSPI_NOR_Interface_t)MX25LM51245G_OPI_MODE      /* 8 Cmd Lines, 8 Address Lines and 8 Data Lines */

/* Definition for OSPI transfer rates */
#define BSP_OSPI_NOR_STR_TRANSFER         (BSP_OSPI_NOR_Transfer_t)MX25LM51245G_STR_TRANSFER   /* Single Transfer Rate */
#define BSP_OSPI_NOR_DTR_TRANSFER         (BSP_OSPI_NOR_Transfer_t)MX25LM51245G_DTR_TRANSFER   /* Double Transfer Rate */

/* OSPI erase types */
#define BSP_OSPI_NOR_ERASE_4K             MX25LM51245G_ERASE_4K
#define BSP_OSPI_NOR_ERASE_64K            MX25LM51245G_ERASE_64K
#define BSP_OSPI_NOR_ERASE_CHIP           MX25LM51245G_ERASE_BULK

/* OSPI block sizes */
#define BSP_OSPI_NOR_BLOCK_4K             MX25LM51245G_SUBSECTOR_4K
#define BSP_OSPI_NOR_BLOCK_64K            MX25LM51245G_SECTOR_64K
/**
  * @}
  */

/** @defgroup STM32L552E-EV_OSPI_RAM_Exported_Constants STM32L552E-EV OSPI_RAM Exported Constants
  * @{
  */
#define OSPI_RAM_INSTANCES_NUMBER         1U

/* OSPI Burst length */
#define BSP_OSPI_RAM_BURST_16_BYTES       (BSP_OSPI_RAM_BurstLength_t)ISS66WVH8M8_BURST_16_BYTES
#define BSP_OSPI_RAM_BURST_32_BYTES       (BSP_OSPI_RAM_BurstLength_t)ISS66WVH8M8_BURST_32_BYTES
#define BSP_OSPI_RAM_BURST_64_BYTES       (BSP_OSPI_RAM_BurstLength_t)ISS66WVH8M8_BURST_64_BYTES
#define BSP_OSPI_RAM_BURST_128_BYTES      (BSP_OSPI_RAM_BurstLength_t)ISS66WVH8M8_BURST_128_BYTES

/* DMA definitions for OSPI RAM DMA transfer */
#define OSPI_RAM_DMAx_CLK_ENABLE          __HAL_RCC_DMA1_CLK_ENABLE
#define OSPI_RAM_DMAx_CLK_DISABLE         __HAL_RCC_DMA1_CLK_DISABLE
#define OSPI_RAM_DMAx_CHANNEL             DMA1_Channel2
#define OSPI_RAM_DMAx_IRQn                DMA1_Channel2_IRQn
#define OPSI_RAM_DMAx_IRQHandler          DMA1_Channel2_IRQHandler
/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/** @defgroup STM32L552E-EV_OSPI_NOR_Exported_Variables STM32L552E-EV OSPI_NOR Exported Variables
  * @{
  */
extern OSPI_HandleTypeDef hospi_nor[OSPI_NOR_INSTANCES_NUMBER];
extern OSPI_NOR_Ctx_t Ospi_Nor_Ctx[OSPI_NOR_INSTANCES_NUMBER];
/**
  * @}
  */

/** @defgroup STM32L552E-EV_OSPI_RAM_Exported_Variables STM32L552E-EV OSPI_RAM Exported Variables
  * @{
  */
extern OSPI_HandleTypeDef hospi_ram[OSPI_RAM_INSTANCES_NUMBER];
extern OSPI_RAM_Ctx_t Ospi_Ram_Ctx[OSPI_RAM_INSTANCES_NUMBER];
/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/
/** @defgroup STM32L552E-EV_OSPI_NOR_Exported_Functions STM32L552E-EV OSPI_NOR Exported Functions
  * @{
  */
int32_t BSP_OSPI_NOR_Init                        (uint32_t Instance, BSP_OSPI_NOR_Init_t *Init);
int32_t BSP_OSPI_NOR_DeInit                      (uint32_t Instance);
#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 1)
int32_t BSP_OSPI_NOR_RegisterMspCallbacks        (uint32_t Instance, BSP_OSPI_Cb_t *CallBacks);
int32_t BSP_OSPI_NOR_RegisterDefaultMspCallbacks (uint32_t Instance);
#endif /* (USE_HAL_OSPI_REGISTER_CALLBACKS == 1) */
int32_t BSP_OSPI_NOR_Read                        (uint32_t Instance, uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
int32_t BSP_OSPI_NOR_Write                       (uint32_t Instance, uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
int32_t BSP_OSPI_NOR_Erase_Block                 (uint32_t Instance, uint32_t BlockAddress, BSP_OSPI_NOR_Erase_t BlockSize);
int32_t BSP_OSPI_NOR_Erase_Chip                  (uint32_t Instance);
int32_t BSP_OSPI_NOR_GetStatus                   (uint32_t Instance);
int32_t BSP_OSPI_NOR_GetInfo                     (uint32_t Instance, BSP_OSPI_NOR_Info_t* pInfo);
int32_t BSP_OSPI_NOR_EnableMemoryMappedMode      (uint32_t Instance);
int32_t BSP_OSPI_NOR_DisableMemoryMappedMode     (uint32_t Instance);
int32_t BSP_OSPI_NOR_ReadID                      (uint32_t Instance, uint8_t *Id);
int32_t BSP_OSPI_NOR_ConfigFlash                 (uint32_t Instance, BSP_OSPI_NOR_Interface_t Mode, BSP_OSPI_NOR_Transfer_t Rate);
int32_t BSP_OSPI_NOR_SuspendErase                (uint32_t Instance);
int32_t BSP_OSPI_NOR_ResumeErase                 (uint32_t Instance);
int32_t BSP_OSPI_NOR_EnterDeepPowerDown          (uint32_t Instance);
int32_t BSP_OSPI_NOR_LeaveDeepPowerDown          (uint32_t Instance);

/* These functions can be modified in case the current settings
   need to be changed for specific application needs */
HAL_StatusTypeDef MX_OSPI_NOR_Init(OSPI_HandleTypeDef *hospi, MX_OSPI_InitTypeDef *Init);

/**
  * @}
  */

/** @defgroup STM32L552E-EV_OSPI_RAM_Exported_Functions STM32L552E-EV OSPI_RAM Exported Functions
  * @{
  */
int32_t BSP_OSPI_RAM_Init                        (uint32_t Instance, BSP_OSPI_RAM_Init_t *Init);
int32_t BSP_OSPI_RAM_DeInit                      (uint32_t Instance);
#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 1)
int32_t BSP_OSPI_RAM_RegisterMspCallbacks        (uint32_t Instance, BSP_OSPI_Cb_t *CallBacks);
int32_t BSP_OSPI_RAM_RegisterDefaultMspCallbacks (uint32_t Instance);
#endif /* (USE_HAL_OSPI_REGISTER_CALLBACKS == 1) */
int32_t BSP_OSPI_RAM_Read                        (uint32_t Instance, uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
int32_t BSP_OSPI_RAM_Read_DMA                    (uint32_t Instance, uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
int32_t BSP_OSPI_RAM_Write                       (uint32_t Instance, uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
int32_t BSP_OSPI_RAM_Write_DMA                   (uint32_t Instance, uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
int32_t BSP_OSPI_RAM_EnableMemoryMappedMode      (uint32_t Instance);
int32_t BSP_OSPI_RAM_DisableMemoryMappedMode     (uint32_t Instance);
int32_t BSP_OSPI_RAM_ConfigHyperRAM              (uint32_t Instance, BSP_OSPI_RAM_Latency_t Latency, BSP_OSPI_RAM_BurstType_t BurstType, BSP_OSPI_RAM_BurstLength_t BurstLength);
int32_t BSP_OSPI_RAM_EnterDeepPowerDown          (uint32_t Instance);
int32_t BSP_OSPI_RAM_LeaveDeepPowerDown          (uint32_t Instance);

void    BSP_OSPI_RAM_DMA_IRQHandler              (uint32_t Instance);
void    BSP_OSPI_RAM_IRQHandler                  (uint32_t Instance);

/* These functions can be modified in case the current settings
   need to be changed for specific application needs */
HAL_StatusTypeDef MX_OSPI_RAM_Init(OSPI_HandleTypeDef *hospi, MX_OSPI_InitTypeDef *Init);
/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32L552E_EVAL_OSPI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
