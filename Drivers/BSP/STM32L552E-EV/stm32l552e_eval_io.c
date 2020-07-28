/**
  ******************************************************************************
  * @file    stm32l552e_eval_io.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the IO pins
  *          on STM32L552E-EV evaluation board.
  @verbatim
  How To use this driver:
  -----------------------
   - This driver is used to drive the IO module of the STM32L552E-EV evaluation
     board.
   - The MFXSTM32L152 IO expander device component driver must be included with this
     driver in order to run the IO functionalities commanded by the IO expander (MFX)
     device mounted on the evaluation board.

  Driver description:
  -------------------
  + Initialization steps:
     o Initialize the IO Component and the IO Pin using the BSP_IO_Init() function. This
       function includes the MSP layer hardware resources initialization and the
       communication layer configuration to start the IO functionalities use.

  + IO functionalities use
     o If an IO pin is used in interrupt mode, the function BSP_IO_GetIT() is
       needed to get the interrupt status. To clear the IT pending bits, you should
       call the function BSP_IO_ClearIT() with specifying the IO pending bit to clear.
     o The IT is handled using the corresponding external interrupt IRQ handler,
       the user IT callback treatment is implemented on the same external interrupt
       callback.
     o To get/set an IO pin combination state you can use the functions
       BSP_IO_ReadPin()/BSP_IO_WritePin() or the function BSP_IO_TogglePin() to toggle the pin
       state.
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
#include "stm32l552e_eval_io.h"
#include "stm32l552e_eval_bus.h"

#if (USE_BSP_IO_CLASS == 1)

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E-EV
  * @{
  */

/** @defgroup STM32L552E-EV_IO STM32L552E-EV IO
  * @{
  */

/** @addtogroup STM32L552E-EV_IO_Exported_Variables
  * @{
  */
IO_Ctx_t                  Io_Ctx[IOEXPANDER_INSTANCES_NBR] = {0};
MFXSTM32L152_Object_t     Io_CompObj[IOEXPANDER_INSTANCES_NBR] = {0};
IDD_Drv_t                *Idd_Drv[IOEXPANDER_INSTANCES_NBR] = {NULL};
IO_Drv_t                 *Io_Drv[IOEXPANDER_INSTANCES_NBR] = {NULL};
/**
  * @}
  */

/** @defgroup STM32L552E-EV_IO_Private_Functions_Prototypes STM32L552E-EV IO Private Functions Prototypes
  * @{
  */
static int32_t MFXSTM32L152_Probe(uint32_t Instance);
/**
  * @}
  */

/** @addtogroup STM32L552E-EV_IO_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the IO peripheral according to the specified parameters in the BSP_IO_Init_t.
  * @param  Instance IOE instance
  * @param  Init     pointer to a BSP_IO_Init_t structure that contains
  *         the configuration information for the specified IO pin.
  * @retval BSP status
  */
int32_t BSP_IO_Init(uint32_t Instance, BSP_IO_Init_t *Init)
{
  int32_t status = BSP_ERROR_NONE;

  if((Instance >= IOEXPANDER_INSTANCES_NBR) || (Init == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((Io_Ctx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
    {
      if(BSP_IOEXPANDER_Init(Instance, IOEXPANDER_IO_MODE) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if (Io_Drv[Instance]->Start(&Io_CompObj[Instance], IO_PIN_ALL) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        Io_Ctx[Instance].Functions |= IOEXPANDER_IO_MODE;
      }
    }

    if(status == BSP_ERROR_NONE)
    {
      /* If IT mode is selected, configures MFX low level interrupt */
      if(Init->Mode >= IO_MODE_IT_RISING_EDGE)
      {
        BSP_IOEXPANDER_ITConfig(Instance);
      }

      /* Initializes IO pin */
      if(Io_Drv[Instance]->Init(&Io_CompObj[Instance], Init) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  return status;
}

/**
  * @brief  DeInitializes the IO peripheral
  * @param  Instance IOE instance
  * @retval BSP status
  */
int32_t BSP_IO_DeInit(uint32_t Instance)
{
  int32_t status;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    Io_Ctx[Instance].Functions &= ~(IOEXPANDER_IO_MODE);
    status = BSP_IOEXPANDER_DeInit(Instance);
  }

  return status;
}

/**
  * @brief  Set the selected pins state.
  * @param  Instance IOExpander instance.
  * @param  Pin Selected pins to write.
  *         This parameter can be any combination of the IO pins.
  * @param  PinState New pins state to write.
  * @retval BSP status.
  */
int32_t BSP_IO_WritePin(uint32_t Instance, uint32_t Pin, uint32_t PinState)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Set the Pin state */
    if (Io_Drv[Instance]->WritePin(&Io_CompObj[Instance], Pin, PinState) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  return status;
}

/**
  * @brief  Get the selected pin current state.
  * @param  Instance IOExpander instance.
  * @param  Pin  Selected pin to read.
  * @note   This function can be used to read only one pin.
  * @retval The current pin state or BSP error status if value is negative.
  */
int32_t BSP_IO_ReadPin(uint32_t Instance, uint32_t Pin)
{
  int32_t status;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    status = Io_Drv[Instance]->ReadPin(&Io_CompObj[Instance], Pin);
    if (status < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else if (status == 0)
    {
      status = (int32_t)IO_PIN_RESET;
    }
    else
    {
      status = (int32_t)IO_PIN_SET;
    }
  }
  return status;
}

/**
  * @brief  Toggle the selected pin state.
  * @param  Instance IOExpander instance.
  * @param  Pin Selected pin to toggle.
  * @note   This function can be used to toggle only one pin.
  * @retval BSP status.
  */
int32_t BSP_IO_TogglePin(uint32_t Instance, uint32_t Pin)
{
  int32_t status = BSP_ERROR_NONE;
  int32_t pinState;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Toggle the current pin state */
    pinState = Io_Drv[Instance]->ReadPin(&Io_CompObj[Instance], Pin);
    if (pinState < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      if (pinState == 0) /* Reset state */
      {
        if (Io_Drv[Instance]->WritePin(&Io_CompObj[Instance], Pin, IO_PIN_SET) < 0)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
      else /* Set state */
      {
        if (Io_Drv[Instance]->WritePin(&Io_CompObj[Instance], Pin, IO_PIN_RESET) < 0)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
    }
  }
  return status;
}

/**
  * @brief  Get the selected pin IT status.
  * @param  Instance IOExpander instance.
  * @param  Pin Selected pin.
  * @note   This function can be used to get IT status on only one pin.
  * @retval The current pin IT status or BSP error status if value is negative.
  */
int32_t BSP_IO_GetIT(uint32_t Instance, uint32_t Pin)
{
  int32_t status;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    status = Io_Drv[Instance]->ITStatus(&Io_CompObj[Instance], Pin);
    if (status < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else if (status == 0)
    {
      status = (int32_t)IO_PIN_IT_RESET;
    }
    else
    {
      status = (int32_t)IO_PIN_IT_SET;
    }
  }
  return status;
}

/**
  * @brief  Clear IO IT pending bits.
  * @param  Instance IOExpander instance.
  * @param  Pins Selected pins to clear.
  *         This parameter can be any combination of the IO pins.
  * @retval BSP status.
  */
int32_t BSP_IO_ClearIT(uint32_t Instance, uint32_t Pins)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Clear only the selected list of IO IT pending bits */
    if (Io_Drv[Instance]->ClearIT(&Io_CompObj[Instance], Pins) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  return status;
}

/**
  * @}
  */

/** @addtogroup STM32L552E-EV_IOEXPANDER_Private_Functions
  * @{
  */

/**
  * @brief  Initializes and start the IOExpander component
  * @param  Instance IOE instance
  * @param  Functions to be initialized. Could be IOEXPANDER_IO_MODE or IOEXPANDER_IDD_MODE
  * @note   IOEXPANDER_TS_MODE are not used on STM32L552e-EVAL board
  * @retval BSP status
  */
int32_t BSP_IOEXPANDER_Init(uint32_t Instance, uint32_t Function)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= IOEXPANDER_INSTANCES_NBR) || ((Function != IOEXPANDER_IO_MODE) && (Function != IOEXPANDER_IDD_MODE)))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Io_Ctx[Instance].IsInitialized == 0U)
    {
      if (MFXSTM32L152_Probe(Instance) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_NO_INIT;
      }
      else
      {
        Io_Ctx[Instance].IsInitialized = 1;
      }
    }

    if (Io_Ctx[Instance].IsInitialized == 1U)
    {
      if (Function == IOEXPANDER_IO_MODE)
      {
        Io_Drv[Instance] = (IO_Drv_t *) &MFXSTM32L152_IO_Driver;
      }
      else if(Function == IOEXPANDER_IDD_MODE)
      {
        Idd_Drv[Instance] = (IDD_Drv_t *) &MFXSTM32L152_IDD_Driver;
      }
      else
      {
        status = BSP_ERROR_WRONG_PARAM;
      }
    }
  }

  return status;
}

/**
  * @brief  De-Initializes the IOExpander component
  * @param  Instance IOE instance
  * @note   The de-init is common for all IOE functions. It is executed only if all
  *         functions are deactivated.
  * @retval BSP status
  */
int32_t BSP_IOEXPANDER_DeInit(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Io_Ctx[Instance].IsInitialized == 1U)
    {
      /* DeInit MFX if no more Functions on are going */
      if (Io_Ctx[Instance].Functions == 0U)
      {
        if (Io_Drv[Instance]->DeInit(&Io_CompObj[Instance]) < 0)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          Io_Ctx[Instance].IsInitialized = 0;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Configure IOExpander low level interrupt.
  * @param  Instance IOExpander instance.
  * @retval None.
  */
void BSP_IOEXPANDER_ITConfig(uint32_t Instance)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  /* Enable the GPIO EXTI clock */
  IOEXPANDER_IRQOUT_GPIO_CLK_ENABLE();

  gpio_init_structure.Pin   = IOEXPANDER_IRQOUT_GPIO_PIN;
  gpio_init_structure.Pull  = GPIO_PULLDOWN;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Mode  = GPIO_MODE_IT_RISING;
  HAL_GPIO_Init(IOEXPANDER_IRQOUT_GPIO_PORT, &gpio_init_structure);

  /* Enable and set GPIO EXTI Interrupt */
  HAL_NVIC_SetPriority(IOEXPANDER_IRQOUT_EXTI_IRQn, BSP_IOEXPANDER_IT_PRIORITY, 0x00);
  HAL_NVIC_EnableIRQ(IOEXPANDER_IRQOUT_EXTI_IRQn);
}

/**
  * @}
  */

/** @defgroup STM32L552E-EV_IO_Private_Functions STM32L552E-EV IO Private Functions
  * @{
  */

/**
  * @brief  Register Bus IOs if component ID is OK
  * @param  Instance IOExpander instance.
  * @retval BSP status.
  */
static int32_t MFXSTM32L152_Probe(uint32_t Instance)
{
  int32_t               status = BSP_ERROR_NONE;
  MFXSTM32L152_IO_t     IOCtx;
  uint32_t              mfxstm32l152_id;

  /* Configure the audio driver */
  IOCtx.Init        = BSP_I2C1_Init;
  IOCtx.DeInit      = BSP_I2C1_DeInit;
  IOCtx.ReadReg     = BSP_I2C1_ReadReg;
  IOCtx.WriteReg    = BSP_I2C1_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;
  IOCtx.Address     = IO_I2C_ADDRESS;

  if (MFXSTM32L152_RegisterBusIO(&Io_CompObj[Instance], &IOCtx) != MFXSTM32L152_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else if (MFXSTM32L152_ReadID(&Io_CompObj[Instance], &mfxstm32l152_id) != MFXSTM32L152_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    if ((mfxstm32l152_id == MFXSTM32L152_ID) || (mfxstm32l152_id == MFXSTM32L152_ID_2))
    {
      if (MFXSTM32L152_Init(&Io_CompObj[Instance]) != MFXSTM32L152_OK)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    else
    {
      status = BSP_ERROR_UNKNOWN_COMPONENT;
    }
  }

  return status;
}

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

#endif /* #if (USE_BSP_IO_CLASS == 1) */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
