/**
  ******************************************************************************
  * @file    stm32l562e_discovery_sd.c
  * @author  MCD Application Team
  * @brief   This file includes the uSD card driver.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  (#) This driver is used to drive the micro SD external card mounted on STM32L562E-DK
     discovery board.

  (#) This driver does not need a specific component driver for the micro SD device
     to be included with.

  (#) Initialization steps:
       (++) Initialize the micro SD card using the BSP_SD_Init() function. This
            function includes the MSP layer hardware resources initialization (BSP_SD_MspInit())
            and the SDMMC1 interface configuration to interface with the external micro SD. It
            also includes the micro SD initialization sequence.
       (++) To check the SD card presence you can use the function BSP_SD_IsDetected() which
            returns the detection status.
       (++) If SD presence detection interrupt mode is desired, you must configure the
            SD detection interrupt mode by calling the functions BSP_SD_ITConfig().
            The interrupt is generated as an external interrupt whenever the
            micro SD card is plugged/unplugged in/from the discovery board.
       (++) The function BSP_SD_GetCardInfo() is used to get the micro SD card information
            which is stored in the structure "HAL_SD_CardInfoTypedef".

  (#) Micro SD card operations
       (++) The micro SD card can be accessed with read/write block(s) operations once
            it is reay for access. The access can be performed whether using the polling
            mode by calling the functions BSP_SD_ReadBlocks()/BSP_SD_WriteBlocks(),
            or by DMA transfer using the functions BSP_SD_ReadBlocks_DMA()/BSP_SD_WriteBlocks_DMA().
       (++) The DMA transfer complete is used with interrupt mode. Once the SD transfer
            is complete, the DMA Tx/Rx transfer complete are handled using the
            BSP_SD_WriteCpltCallback()/BSP_SD_ReadCpltCallback() user callback functions implemented
            by the user at application level.
       (++) The SD erase block(s) is performed using the function BSP_SD_Erase() with specifying
            the number of blocks to erase.
       (++) The SD runtime status is returned when calling the function BSP_SD_GetStatus().
   [..]
  @endverbatim
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
#include "stm32l562e_discovery_sd.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L562E-DK
  * @{
  */

/** @defgroup STM32L562E-DK_SD STM32L562E-DK SD
  * @{
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup STM32L562E-DK_SD_Private_TypesDefinitions STM32L562E-DK SD Private Types Definitions
  * @{
  */
typedef void (* BSP_SD_EXTI_Callback)(void);
/**
  * @}
  */

/** @defgroup STM32L562E-DK_SD_Private_Functions_Prototypes STM32L562E-DK SD Private Functions Prototypes
  * @{
  */
static void SDMMC1_DETECT_Callback(void);

static void SD_MspInit(SD_HandleTypeDef *hsd);
static void SD_MspDeInit(SD_HandleTypeDef *hsd);
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
static void SD_AbortCallback(SD_HandleTypeDef *hsd);
static void SD_TxCpltCallback(SD_HandleTypeDef *hsd);
static void SD_RxCpltCallback(SD_HandleTypeDef *hsd);
#endif /* (USE_HAL_SD_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup STM32L562E-DK_SD_Private_Variables STM32L562E-DK SD Private Variables
  * @{
  */
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
/* Is Msp Callbacks registered */
static uint32_t Sd_IsMspCallbacksValid[SD_INSTANCES_NBR] = {0};
#endif
/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/** @addtogroup STM32L562E-DK_SD_Exported_Variables
  * @{
  */
SD_HandleTypeDef hsd_sdmmc[SD_INSTANCES_NBR];

EXTI_HandleTypeDef hsd_exti[SD_INSTANCES_NBR];

uint32_t Sd_PinDetect[SD_INSTANCES_NBR]  = {SD_DETECT_PIN};
GPIO_TypeDef* Sd_PortDetect[SD_INSTANCES_NBR] = {SD_DETECT_GPIO_PORT};
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @addtogroup STM32L562E-DK_SD_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the SD card device.
  * @param  Instance SD Instance
  * @retval BSP status
  */
int32_t BSP_SD_Init(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef gpioinitstruct = {0};

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Configure SD pin detect */
    __SD_DETECT_GPIO_CLK_ENABLE();

    gpioinitstruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    gpioinitstruct.Pull  = GPIO_PULLUP;
    gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioinitstruct.Pin   = Sd_PinDetect[Instance];
    HAL_GPIO_Init(Sd_PortDetect[Instance], &gpioinitstruct);

    /* Check if SD card is present */
    if((uint32_t)BSP_SD_IsDetected(Instance) != SD_PRESENT)
    {
      ret = BSP_ERROR_UNKNOWN_COMPONENT;
    }
    else
    {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 0)
      /* Msp SD initialization */
      SD_MspInit(&hsd_sdmmc[Instance]);
#else
      /* Register the SD MSP Callbacks */
      if(Sd_IsMspCallbacksValid[Instance] == 0UL)
      {
        if(BSP_SD_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }
      }
#endif /* (USE_HAL_SD_REGISTER_CALLBACKS == 0) */

      /* HAL SD initialization operation */
      if(MX_SDMMC1_SD_Init(&hsd_sdmmc[Instance]) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        /* Register SD TC, HT and Abort callbacks */
        if(HAL_SD_RegisterCallback(&hsd_sdmmc[Instance], HAL_SD_TX_CPLT_CB_ID, SD_TxCpltCallback) != HAL_OK)
        {
          ret = BSP_ERROR_PERIPH_FAILURE;
        }
        else if(HAL_SD_RegisterCallback(&hsd_sdmmc[Instance], HAL_SD_RX_CPLT_CB_ID, SD_RxCpltCallback) != HAL_OK)
        {
          ret = BSP_ERROR_PERIPH_FAILURE;
        }
        else if(HAL_SD_RegisterCallback(&hsd_sdmmc[Instance], HAL_SD_ABORT_CB_ID, SD_AbortCallback) != HAL_OK)
        {
          ret = BSP_ERROR_PERIPH_FAILURE;
        }
        else
        {
          /* Do nothing */
        }
#endif /* (USE_HAL_SD_REGISTER_CALLBACKS == 1) */
      }
    }
  }

  return  ret;
}

/**
  * @brief  DeInitializes the SD card device.
  * @param  Instance SD Instance
  * @retval BSP status
  */
int32_t BSP_SD_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef gpioinitstruct = {0};

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* HAL SD de-initialization */
    if(HAL_SD_DeInit(&hsd_sdmmc[Instance]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 0)
      /* Msp SD de-initialization */
      SD_MspDeInit(&hsd_sdmmc[Instance]);
#endif

      /* SD detect pin de-initialization */
      gpioinitstruct.Mode      = GPIO_MODE_ANALOG;
      gpioinitstruct.Pull      = GPIO_NOPULL;
      gpioinitstruct.Speed     = GPIO_SPEED_FREQ_LOW;
      gpioinitstruct.Alternate = 0;
      gpioinitstruct.Pin       = Sd_PinDetect[Instance];
      HAL_GPIO_Init(Sd_PortDetect[Instance], &gpioinitstruct);
    }
  }
  return ret;
}

/**
  * @brief  Initializes the SDMMC1 peripheral.
  * @param  hsd SD handle
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_SDMMC1_SD_Init(SD_HandleTypeDef *hsd)
{
  HAL_StatusTypeDef ret = HAL_OK;

  /* uSD device interface configuration */
  hsd->Instance                 = SDMMC1;
  hsd->Init.ClockEdge           = SDMMC_CLOCK_EDGE_FALLING;
  hsd->Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd->Init.BusWide             = SDMMC_BUS_WIDE_4B;
  hsd->Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
  hsd->Init.ClockDiv            = SDMMC_NSpeed_CLK_DIV;

  /* HAL SD initialization */
  if(HAL_SD_Init(hsd) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
/**
  * @brief BSP SD default Msp Callback registering
  * @param Instance     SD Instance
  * @retval BSP status
  */
int32_t BSP_SD_RegisterDefaultMspCallbacks(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_SD_RegisterCallback(&hsd_sdmmc[Instance], HAL_SD_MSP_INIT_CB_ID, SD_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SD_RegisterCallback(&hsd_sdmmc[Instance], HAL_SD_MSP_DEINIT_CB_ID, SD_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      Sd_IsMspCallbacksValid[Instance] = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP SD Msp Callback registering
  * @param Instance     SD Instance
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_SD_RegisterMspCallbacks(uint32_t Instance, BSP_SD_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_SD_RegisterCallback(&hsd_sdmmc[Instance], HAL_SD_MSP_INIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SD_RegisterCallback(&hsd_sdmmc[Instance], HAL_SD_MSP_DEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      Sd_IsMspCallbacksValid[Instance] = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_SD_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Configures Interrupt mode for SD detection pin.
  * @param  Instance SD Instance
  * @retval BSP status
  */
int32_t BSP_SD_DetectITConfig(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef gpioinitstruct = {0};
  uint32_t LineDetect[SD_INSTANCES_NBR] = {SD_DETECT_EXTI_LINE};
  BSP_SD_EXTI_Callback SdDetectCallback[SD_INSTANCES_NBR] = {SDMMC1_DETECT_Callback};
  IRQn_Type SdDetectIrq[SD_INSTANCES_NBR] = {SD_DETECT_IRQn};

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Enable GPIO clock */
    __SD_DETECT_GPIO_CLK_ENABLE();

    /* Configure Interrupt mode for SD detection pin */
    gpioinitstruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    gpioinitstruct.Pull  = GPIO_PULLUP;
    gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioinitstruct.Pin   = Sd_PinDetect[Instance];
    HAL_GPIO_Init(Sd_PortDetect[Instance], &gpioinitstruct);

    if (HAL_EXTI_GetHandle(&hsd_exti[Instance], LineDetect[Instance]) == HAL_OK)
    {
      if (HAL_EXTI_RegisterCallback(&hsd_exti[Instance], HAL_EXTI_COMMON_CB_ID, SdDetectCallback[Instance]) == HAL_OK)
      {
        /* NVIC configuration for SD detection interrupts */
        HAL_NVIC_SetPriority(SdDetectIrq[Instance], BSP_SD_IT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(SdDetectIrq[Instance]);
      }
      else
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  BSP SD Callback.
  * @param  Instance SD Instance
  * @param  Status   Pin status
  * @retval None.
  */
__weak void BSP_SD_DetectCallback(uint32_t Instance, uint32_t Status)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
  UNUSED(Status);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on SD detect pin is triggered. */
}

/**
  * @brief  Detects if SD card is correctly plugged in the memory slot or not.
  * @param  Instance SD Instance
  * @retval Returns if SD is detected or not
  */
int32_t BSP_SD_IsDetected(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check SD card detect pin */
    if(HAL_GPIO_ReadPin(Sd_PortDetect[Instance], (uint16_t) Sd_PinDetect[Instance]) != GPIO_PIN_RESET)
    {
      ret = (int32_t) SD_NOT_PRESENT;
    }
    else
    {
      ret = (int32_t) SD_PRESENT;
    }
  }

  return ret;
}

/**
  * @brief  BSP SD detect interrupt handler.
  * @param  Instance SD Instance.
  * @retval None.
  */
void BSP_SD_DETECT_IRQHandler(uint32_t Instance)
{
  HAL_EXTI_IRQHandler(&hsd_exti[Instance]);
}

/**
  * @brief  BSP SD interrupt handler.
  * @param  Instance SD Instance.
  * @retval None.
  */
void BSP_SD_IRQHandler(uint32_t Instance)
{
  HAL_SD_IRQHandler(&hsd_sdmmc[Instance]);
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  Instance  SD Instance
  * @param  pData     Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx  Block index from where data is to be read
  * @param  BlocksNbr Number of SD blocks to read
  * @retval BSP status
  */
int32_t BSP_SD_ReadBlocks(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  uint32_t timeout = SD_READ_TIMEOUT*BlocksNbr;
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_ReadBlocks(&hsd_sdmmc[Instance], (uint8_t *)pData, BlockIdx, BlocksNbr, timeout) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be written
  * @param  BlocksNbr  Number of SD blocks to write
  * @retval BSP status
  */
int32_t BSP_SD_WriteBlocks(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  uint32_t timeout = SD_WRITE_TIMEOUT*BlocksNbr;
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_WriteBlocks(&hsd_sdmmc[Instance], (uint8_t *)pData, BlockIdx, BlocksNbr, timeout) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be read
  * @param  BlocksNbr  Number of SD blocks to read
  * @retval BSP status
  */
int32_t BSP_SD_ReadBlocks_DMA(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_ReadBlocks_DMA(&hsd_sdmmc[Instance], (uint8_t *)pData, BlockIdx, BlocksNbr) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be written
  * @param  BlocksNbr  Number of SD blocks to write
  * @retval BSP status
  */
int32_t BSP_SD_WriteBlocks_DMA(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_WriteBlocks_DMA(&hsd_sdmmc[Instance], (uint8_t *)pData, BlockIdx, BlocksNbr) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in IT mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be read
  * @param  BlocksNbr  Number of SD blocks to read
  * @retval SD status
  */
int32_t BSP_SD_ReadBlocks_IT(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_ReadBlocks_IT(&hsd_sdmmc[Instance], (uint8_t *)pData, BlockIdx, BlocksNbr) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in IT mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be written
  * @param  BlocksNbr  Number of SD blocks to write
  * @retval SD status
  */
int32_t BSP_SD_WriteBlocks_IT(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_WriteBlocks_IT(&hsd_sdmmc[Instance], (uint8_t *)pData, BlockIdx, BlocksNbr) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Erases the specified memory area of the given SD card.
  * @param  Instance   SD Instance
  * @param  BlockIdx   Block index from where data is to be
  * @param  BlocksNbr  Number of SD blocks to erase
  * @retval BSP status
  */
int32_t BSP_SD_Erase(uint32_t Instance, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_Erase(&hsd_sdmmc[Instance], BlockIdx, BlockIdx + BlocksNbr) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Gets the current SD card data status.
  * @param  Instance  SD Instance
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  SD_TRANSFER_OK: No data transfer is acting
  *            @arg  SD_TRANSFER_BUSY: Data transfer is acting
  */
int32_t BSP_SD_GetCardState(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = (int32_t) ((HAL_SD_GetCardState(&hsd_sdmmc[Instance]) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
  }

  return ret;
}

/**
  * @brief  Get SD information about specific SD card.
  * @param  Instance  SD Instance
  * @param  CardInfo  Pointer to HAL_SD_CardInfoTypedef structure
  * @retval BSP status
  */
int32_t BSP_SD_GetCardInfo(uint32_t Instance, BSP_SD_CardInfo *CardInfo)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_SD_GetCardInfo(&hsd_sdmmc[Instance], CardInfo) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief BSP SD Abort callback
  * @param  Instance     SD Instance
  * @retval None
  */
__weak void BSP_SD_AbortCallback(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief BSP Tx Transfer completed callback
  * @param  Instance     SD Instance
  * @retval None
  */
__weak void BSP_SD_WriteCpltCallback(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief BSP Rx Transfer completed callback
  * @param  Instance     SD Instance
  * @retval None
  */
__weak void BSP_SD_ReadCpltCallback(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}
/**
  * @}
  */

/** @defgroup STM32L562E-DK_SD_Private_Functions STM32L562E-DK SD Private Functions
  * @{
  */
/**
  * @brief  SD detection callback.
  * @retval None
  */
static void SDMMC1_DETECT_Callback(void)
{
  if(HAL_GPIO_ReadPin(Sd_PortDetect[0], (uint16_t) Sd_PinDetect[0]) != GPIO_PIN_RESET)
  {
    BSP_SD_DetectCallback(0, SD_NOT_PRESENT);
  }
  else
  {
    BSP_SD_DetectCallback(0, SD_PRESENT);
  }
}

/**
  * @brief  Initializes the SD MSP.
  * @param  hsd  SD handle
  * @retval None
  */
static void SD_MspInit(SD_HandleTypeDef *hsd)
{
  HAL_StatusTypeDef ret = HAL_OK;
  GPIO_InitTypeDef gpioinitstruct = {0};
  RCC_OscInitTypeDef       RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);

  /* Check whether HSI48 is enabled or not */
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  if(RCC_OscInitStruct.HSI48State != RCC_HSI48_ON)
  {
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State     = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  }

  if (ret == HAL_OK)
  {
    /* Configure the discovery SDMMC1 clock source. The clock is derived from the HSI48 */
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SDMMC1;
    RCC_PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_HSI48;
    ret = HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

    if (ret == HAL_OK)
    {
      /* Enable SDMMC1 clock */
      __HAL_RCC_SDMMC1_CLK_ENABLE();

      /* Enable GPIOs clock */
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOD_CLK_ENABLE();

      /* Common GPIO configuration */
      gpioinitstruct.Mode      = GPIO_MODE_AF_PP;
      gpioinitstruct.Pull      = GPIO_PULLUP;
      gpioinitstruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
      gpioinitstruct.Alternate = GPIO_AF12_SDMMC1;

      /* GPIOC configuration */
      gpioinitstruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
      HAL_GPIO_Init(GPIOC, &gpioinitstruct);

      /* GPIOD configuration */
      gpioinitstruct.Pin = GPIO_PIN_2;
      HAL_GPIO_Init(GPIOD, &gpioinitstruct);

      /* NVIC configuration for SDMMC1 interrupts */
      HAL_NVIC_SetPriority(SDMMCx_IRQn, BSP_SD_IT_PRIORITY, 0);
      HAL_NVIC_EnableIRQ(SDMMCx_IRQn);
    }
  }
}

/**
  * @brief  De-Initializes the SD MSP.
  * @param  hsd  SD handle
  * @retval None
  */
static void SD_MspDeInit(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);

  /* NVIC configuration for SDMMC1 interrupts */
  HAL_NVIC_DisableIRQ(SDMMCx_IRQn);

  /* Disable SDMMC1 clock */
  __HAL_RCC_SDMMC1_CLK_DISABLE();

  /* Disable common GPIO configuration */
  /* GPIOC configuration */
  HAL_GPIO_DeInit(GPIOC, (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12));

  /* GPIOD configuration */
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);
}

#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
/**
  * @brief SD Abort callbacks
  * @param hsd  SD handle
  * @retval None
  */
static void SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  uint32_t instance = 0;

  do
  {
    if (hsd == &hsd_sdmmc[instance])
    {
      BSP_SD_AbortCallback(instance);
    }
    instance ++;
  }while (instance < SD_INSTANCES_NBR);
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd  SD handle
  * @retval None
  */
static void SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  uint32_t instance = 0;

  do
  {
    if (hsd == &hsd_sdmmc[instance])
    {
      BSP_SD_WriteCpltCallback(instance);
    }
    instance ++;
  }while (instance < SD_INSTANCES_NBR);
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd  SD handle
  * @retval None
  */
static void SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  uint32_t instance = 0;

  do
  {
    if (hsd == &hsd_sdmmc[instance])
    {
      BSP_SD_ReadCpltCallback(instance);
    }
    instance ++;
  }while (instance < SD_INSTANCES_NBR);
}

#else /* (USE_HAL_SD_REGISTER_CALLBACKS == 1) */

/**
  * @brief SD Abort callbacks
  * @param hsd  SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  uint32_t instance = 0;

  do
  {
    if (hsd == &hsd_sdmmc[instance])
    {
      BSP_SD_AbortCallback(instance);
    }
    instance ++;
  }while (instance < SD_INSTANCES_NBR);
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd  SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  uint32_t instance = 0;

  do
  {
    if (hsd == &hsd_sdmmc[instance])
    {
      BSP_SD_WriteCpltCallback(instance);
    }
    instance ++;
  }while (instance < SD_INSTANCES_NBR);
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd  SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  uint32_t instance = 0;

  do
  {
    if (hsd == &hsd_sdmmc[instance])
    {
      BSP_SD_ReadCpltCallback(instance);
    }
    instance ++;
  }while (instance < SD_INSTANCES_NBR);
}
#endif /* (USE_HAL_SD_REGISTER_CALLBACKS == 1) */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
