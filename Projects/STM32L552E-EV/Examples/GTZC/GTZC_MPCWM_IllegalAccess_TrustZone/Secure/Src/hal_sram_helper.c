/**
  ******************************************************************************
  * @file    GTZC/GTZC_MPCWM_IllegalAccess_TrustZone/Secure/Src/hal_sram_helper.c
  * @author  MCD Application Team
  * @brief   Helper C file to configure the external SRAM
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

#include "main.h"
#include "hal_sram_helper.h"

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup GTZC_MPCWM_TZIC
  * @{
  */

#if defined (HAL_SRAM_MODULE_ENABLED)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SRAM_HandleTypeDef    hsram;
NORSRAM_TimingTypeDef IS61WV102416BLL_Timing = { 8, /* AddressSetupTime  */
                                                 0,  /* AddressHoldTime   */
                                                 6,  /* DataSetupTime     */
                                                 0,  /* DataHoldTime      */
                                                 0,  /* PreCharge         */
                                                 0,  /* MaxCLKPeriod      */
                                                 0   /* DataLatency       */
                                               };

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  external SRAM configuration
  * @retval None
  */
uint32_t extSRAM_Config( void )
{
  FMC_NORSRAM_TimingTypeDef SRAM_Timing;

  /* SRAM device configuration */
  hsram.Instance  = FMC_NORSRAM_DEVICE;
  hsram.Extended  = FMC_NORSRAM_EXTENDED_DEVICE;

  hsram.Init.NSBank             = FMC_SRAM_BANKx;
  hsram.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram.Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;
  hsram.Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram.Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  hsram.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  hsram.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;
  hsram.Init.WriteBurst         = FMC_WRITE_BURST_DISABLE;
  hsram.Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram.Init.WriteFifo          = FMC_WRITE_FIFO_DISABLE;
  hsram.Init.NBLSetupTime       = 0;
  hsram.Init.PageSize           = FMC_PAGE_SIZE_NONE;

  /* Timings for SRAM IS61WV102416BLL-10MLI */
  SRAM_Timing.AccessMode            = FMC_ACCESS_MODE_A;
  SRAM_Timing.AddressSetupTime      = 2;
  SRAM_Timing.AddressHoldTime       = 1;  /* Min value, Don't care on SRAM Access mode A */
  SRAM_Timing.DataSetupTime         = 1;
  SRAM_Timing.DataHoldTime          = 0;
  SRAM_Timing.BusTurnAroundDuration = 1;
  SRAM_Timing.CLKDivision           = 2;  /* Min value, Don't care on SRAM Access mode A */
  SRAM_Timing.DataLatency           = 2;  /* Min value, Don't care on SRAM Access mode A */

  /* Init SRAM controller */
  if(HAL_SRAM_Init(&hsram, &SRAM_Timing, &SRAM_Timing) != HAL_OK)
  {
    return 1;
  }

  return 0;
}

/**
  * @brief  FMC_SRAM BSP Init
  * @param  hsram : FMC_SRAM handle
  * @retval None
  */
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
  GPIO_InitTypeDef GPIO_Init_Structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  /* IOSV bit MUST be set to access GPIO port G[2:15] */
  __HAL_RCC_PWR_CLK_ENABLE();
  SET_BIT(PWR->CR2, PWR_CR2_IOSV);

  /* Common GPIO configuration */
  GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init_Structure.Pull      = GPIO_PULLUP;
  GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_Init_Structure.Alternate = GPIO_AF12_FMC;

  /*## Address Bus #######*/
  /* GPIOD configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

  /* GPIOE configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

  /* GPIOF configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4 |\
                            GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);

  /* GPIOG configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |\
                            GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_14;
  HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

  /*## Data Bus #######*/
  /* GPIOD configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_0  | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |\
                            GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

  /* GPIOE configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |\
                            GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

  /*## NBL0 and NBL1 #######*/
  /* GPIOE configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

  /*## NE1 to NE4 #######*/
  /* GPIOD configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

  /* GPIOG configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

  /*## NL, NOE, NWAIT, NWE #######*/
  /* GPIOB configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_Init_Structure);

  /* GPIOD configuration */
  GPIO_Init_Structure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);
}

/**
  * @brief  SRAM BSP DeInit
  * @param  hsram : FMC_SRAM handle
  * @retval None
  */
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram)
{
}

#endif /* HAL_SRAM_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
