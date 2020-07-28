/**
  ******************************************************************************
  * @file    stm32l562e_discovery_ts.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the Touch
  *          Screen on STM32L562E-DISCOVERY board.
  @verbatim
  1. How To use this driver:
  --------------------------
   - This driver is used to drive the touch screen module of the STM32L562E-DISCOVERY
     discovery board.
     The touch screen driver IC is a FT6X06.

  2. Driver description:
  ---------------------
    + Initialization steps:
       o Initialize the TS using the BSP_TS_Init() function. You can select
         display orientation with "Orientation" parameter of TS_Init_t structure
         (portrait, landscape, portrait with 180 degrees rotation or landscape
         with 180 degrees rotation). The LCD size properties (width and height)
         are also parameters of TS_Init_t and depend on the orientation selected.

    + Touch screen use
       o Call BSP_TS_EnableIT() (BSP_TS_DisableIT()) to enable (disable) touch
         screen interrupt. BSP_TS_Callback() is called when TS interrupt occurs.
       o Call BSP_TS_GetState() to get the current touch status (detection and
         coordinates).
       o Call BSP_TS_Set_Orientation() to change the current orientation.
         Call BSP_TS_Get_Orientation() to get the current orientation.
       o Call BSP_TS_GetCapabilities() to get the FT6X06 capabilities.
       o Call BSP_TS_Get_MultiTouchState() to get the current touches and events.
       o FT6X06 doesn't support gesture feature.
         BSP_TS_GestureConfig() and BSP_TS_GetGestureId() functions will return 
         BSP_ERROR_FEATURE_NOT_SUPPORTED.
 
    + De-initialization steps:
       o De-initialize the touch screen using the BSP_TS_DeInit() function.

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
#include "stm32l562e_discovery_ts.h"
#include "stm32l562e_discovery_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L562E-DK
  * @{
  */

/** @defgroup STM32L562E-DK_TS STM32L562E-DK TS
  * @{
  */

/** @defgroup STM32L562E-DK_TS_Private_Defines STM32L562E-DK TS Private Defines
  * @{
  */
#define TS_IT_GPIO_PORT           GPIOF
#define TS_IT_GPIO_PIN            GPIO_PIN_1
#define TS_IT_GPIO_CLOCK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
/**
  * @}
  */

/** @defgroup STM32L562E-DK_TS_Private_Variables STM32L562E-DK TS Private Variables
  * @{
  */
static FunctionalState TsPollingMode = ENABLE;
/**
  * @}
  */

/** @defgroup STM32L562E-DK_TS_Private_TypesDefinitions STM32L562E-DK TS Private TypesDefinitions
  * @{
  */
typedef void (* BSP_EXTI_LineCallback)(void);
/**
  * @}
  */

/** @addtogroup STM32L562E-DK_TS_Exported_Variables
  * @{
  */
void     *Ts_CompObj[TS_INSTANCES_NBR] = {0};
TS_Drv_t *Ts_Drv[TS_INSTANCES_NBR]     = {0};
TS_Ctx_t  Ts_Ctx[TS_INSTANCES_NBR]     = {0};

EXTI_HandleTypeDef hts_exti[TS_INSTANCES_NBR];
IRQn_Type          Ts_IRQn[TS_INSTANCES_NBR] = {EXTI1_IRQn};
/**
  * @}
  */

/** @defgroup STM32L562E-DK_TS_Private_FunctionPrototypes STM32L562E-DK TS Private Function Prototypes
  * @{
  */
static int32_t FT6X06_Probe(uint32_t Instance);
static void    TS_EXTI_Callback(void);
/**
  * @}
  */

/** @addtogroup STM32L562E-DK_TS_Exported_Functions
  * @{
  */
/**
  * @brief  Initialize the TS.
  * @param  Instance TS Instance.
  * @param  TS_Init  Pointer to TS initialization structure.
  * @retval BSP status.
  */
int32_t BSP_TS_Init(uint32_t Instance, TS_Init_t *TS_Init)
{
  int32_t               status = BSP_ERROR_NONE;
  GPIO_InitTypeDef      GPIO_Init;
  uint32_t              TS_EXTI_LINE[TS_INSTANCES_NBR] = {EXTI_LINE_1};
  BSP_EXTI_LineCallback TsCallback[TS_INSTANCES_NBR]   = {TS_EXTI_Callback};

  if (TS_Init == NULL)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance >= TS_INSTANCES_NBR) || (TS_Init->Orientation > TS_ORIENTATION_LANDSCAPE_ROT180))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Probe the TS driver */
    if (FT6X06_Probe(Instance) != BSP_ERROR_NONE)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      TS_Capabilities_t Capabilities;
      /* Store parameters on TS context */
      Ts_Ctx[Instance].Width       = TS_Init->Width;
      Ts_Ctx[Instance].Height      = TS_Init->Height;
      Ts_Ctx[Instance].Orientation = TS_Init->Orientation;
      Ts_Ctx[Instance].Accuracy    = TS_Init->Accuracy;
      /* Get capabilities to retrieve maximum values of X and Y */
      if (Ts_Drv[Instance]->GetCapabilities(Ts_CompObj[Instance], &Capabilities) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        /* Store maximum X and Y on context */
        Ts_Ctx[Instance].MaxX = Capabilities.MaxXl;
        Ts_Ctx[Instance].MaxY = Capabilities.MaxYl;
        /* Initialize previous position in order to always detect first touch */
        Ts_Ctx[Instance].PreviousX = TS_Init->Width + TS_Init->Accuracy + 1U;
        Ts_Ctx[Instance].PreviousY = TS_Init->Height + TS_Init->Accuracy + 1U;

        /* Configure TS interrupt GPIO */
        TS_IT_GPIO_CLOCK_ENABLE();
        GPIO_Init.Pin   = TS_IT_GPIO_PIN;
        GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_Init.Mode  = GPIO_MODE_IT_FALLING;
        GPIO_Init.Pull  = GPIO_PULLUP;
        HAL_GPIO_Init(TS_IT_GPIO_PORT, &GPIO_Init);
        if (HAL_EXTI_GetHandle(&hts_exti[Instance], TS_EXTI_LINE[Instance]) == HAL_OK)
        {
          if (HAL_EXTI_RegisterCallback(&hts_exti[Instance], HAL_EXTI_FALLING_CB_ID, TsCallback[Instance]) != HAL_OK)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }
        }
        else
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        /* Enable interrupt trigger mode on TS driver */
        if (Ts_Drv[Instance]->EnableIT(Ts_CompObj[Instance]) < 0)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  De-Initialize the TS.
  * @param  Instance TS Instance.
  * @retval BSP status.
  */
int32_t BSP_TS_DeInit(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= TS_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* De-Init the TS driver */
    if (Ts_Drv[Instance]->DeInit(Ts_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* De-Init TS interrupt GPIO */
      HAL_GPIO_DeInit(TS_IT_GPIO_PORT, TS_IT_GPIO_PIN);
      /* Reset polling mode status */
      TsPollingMode = ENABLE;
    }
  }

  return status;
}

/**
  * @brief  Enable the TS interrupt.
  * @param  Instance TS Instance.
  * @retval BSP status.
  */
int32_t BSP_TS_EnableIT(uint32_t Instance)
{
  int32_t               status = BSP_ERROR_NONE;
  uint32_t              TS_IT_PRIO[TS_INSTANCES_NBR]     = {BSP_TS_IT_PRIORITY};

  if (Instance >= TS_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Update polling mode status */
    TsPollingMode = DISABLE;

    /* Activate TS interrupt */
    /* Enable and set TS EXTI interrupt to the lowest priority */
    HAL_NVIC_SetPriority(Ts_IRQn[Instance], TS_IT_PRIO[Instance], 0x00);
    HAL_NVIC_EnableIRQ(Ts_IRQn[Instance]);
  }

  return status;
}

/**
  * @brief  Disable the TS interrupt.
  * @param  Instance TS Instance.
  * @retval BSP status.
  */
int32_t BSP_TS_DisableIT(uint32_t Instance)
{
  int32_t          status = BSP_ERROR_NONE;

  if (Instance >= TS_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
      /* Disable TS EXTI interrupt */
      HAL_NVIC_DisableIRQ(Ts_IRQn[Instance]);

      /* Update polling mode status */
      TsPollingMode = ENABLE;
  }

  return status;
}

/**
  * @brief  Set the TS orientation.
  * @param  Instance TS Instance.
  * @param  Orientation TS orientation.
  * @retval BSP status.
  */
int32_t BSP_TS_Set_Orientation(uint32_t Instance, uint32_t Orientation)
{
  int32_t  status = BSP_ERROR_NONE;
  uint32_t tmp;

  if ((Instance >= TS_INSTANCES_NBR) || (Orientation > TS_ORIENTATION_LANDSCAPE_ROT180))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Update TS context if orientation is changed from/to portrait to/from landscape */
    if ((((Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE) || (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE_ROT180)) &&
         ((Orientation == TS_ORIENTATION_PORTRAIT) || (Orientation == TS_ORIENTATION_PORTRAIT_ROT180))) ||
        (((Ts_Ctx[Instance].Orientation == TS_ORIENTATION_PORTRAIT) || (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_PORTRAIT_ROT180)) &&
         ((Orientation == TS_ORIENTATION_LANDSCAPE) || (Orientation == TS_ORIENTATION_LANDSCAPE_ROT180))))
    {
      /* Invert width and height */
      tmp = Ts_Ctx[Instance].Width;
      Ts_Ctx[Instance].Width  = Ts_Ctx[Instance].Height;
      Ts_Ctx[Instance].Height = tmp;
      /* Invert MaxX and MaxY */
      tmp = Ts_Ctx[Instance].MaxX;
      Ts_Ctx[Instance].MaxX = Ts_Ctx[Instance].MaxY;
      Ts_Ctx[Instance].MaxY = tmp;
    }      
    /* Store orientation on TS context */
    Ts_Ctx[Instance].Orientation = Orientation;
    /* Reset previous position */
    Ts_Ctx[Instance].PreviousX = Ts_Ctx[Instance].Width + Ts_Ctx[Instance].Accuracy + 1U;
    Ts_Ctx[Instance].PreviousY = Ts_Ctx[Instance].Height + Ts_Ctx[Instance].Accuracy + 1U;
  }

  return status;
}

/**
  * @brief  Get the TS orientation.
  * @param  Instance TS Instance.
  * @param  Orientation Pointer to TS orientation.
  * @retval BSP status.
  */
int32_t BSP_TS_Get_Orientation(uint32_t Instance, uint32_t *Orientation)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= TS_INSTANCES_NBR) || (Orientation == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Get orientation from TS context */
    *Orientation = Ts_Ctx[Instance].Orientation;
  }

  return status;
}

/**
  * @brief  Get position of a single touch.
  * @param  Instance TS Instance.
  * @param  TS_State Pointer to single touch structure.
  * @retval BSP status.
  */
int32_t BSP_TS_GetState(uint32_t Instance, TS_State_t *TS_State)
{
  int32_t  status = BSP_ERROR_NONE;
  uint32_t xDiff, yDiff;

  if ((Instance >= TS_INSTANCES_NBR) || (TS_State == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    FT6X06_State_t State;
    uint8_t        CallDriver = 1U;

    /* In polling mode */
    if (TsPollingMode == ENABLE)
    {
      if (HAL_EXTI_GetPending(&hts_exti[Instance], EXTI_TRIGGER_FALLING) == 0x0U)
      {
        /* No need to call driver, no new touch available */
        CallDriver = 0U;
        TS_State->TouchDetected = 0U;
      }
      else
      {
        /* New touch available, clear pending flag */
        HAL_EXTI_ClearPending(&hts_exti[Instance], EXTI_TRIGGER_FALLING);
      }
    }

    if (CallDriver == 1U)
    {
      /* Get the TS state */
      if (Ts_Drv[Instance]->GetState(Ts_CompObj[Instance], &State) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        if (State.TouchDetected != 0U)
        {
          /* Compute the oriented touch X and Y */
          if (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE)
          {
            TS_State->TouchX = State.TouchX;
            TS_State->TouchY = State.TouchY;
          }
          else if (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_PORTRAIT_ROT180)
          {
            TS_State->TouchX = Ts_Ctx[Instance].MaxY - State.TouchY;
            TS_State->TouchY = State.TouchX;
          }
          else if (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE_ROT180)
          {
            TS_State->TouchX = Ts_Ctx[Instance].MaxX - State.TouchX;
            TS_State->TouchY = Ts_Ctx[Instance].MaxY - State.TouchY;
          }
          else /* PORTRAIT */
          {
            TS_State->TouchX = State.TouchY;
            TS_State->TouchY = Ts_Ctx[Instance].MaxX - State.TouchX;
          }
          /* Check accuracy */
          xDiff = (TS_State->TouchX >= Ts_Ctx[Instance].PreviousX) ?
            (TS_State->TouchX - Ts_Ctx[Instance].PreviousX) :
            (Ts_Ctx[Instance].PreviousX - TS_State->TouchX);
            yDiff = (TS_State->TouchY >= Ts_Ctx[Instance].PreviousY) ?
              (TS_State->TouchY - Ts_Ctx[Instance].PreviousY) :
              (Ts_Ctx[Instance].PreviousY - TS_State->TouchY);
              if ((xDiff <= Ts_Ctx[Instance].Accuracy) && (yDiff <= Ts_Ctx[Instance].Accuracy))
              {
                /* Touch not detected */
                TS_State->TouchDetected = 0U;
              }
              else
              {
                /* New touch detected */
                TS_State->TouchDetected = 1U;
                Ts_Ctx[Instance].PreviousX = TS_State->TouchX;
                Ts_Ctx[Instance].PreviousY = TS_State->TouchY;
              }
        }
        else
        {
          TS_State->TouchDetected = 0U;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Get positions of multiple touch.
  * @param  Instance TS Instance.
  * @param  TS_State Pointer to multiple touch structure.
  * @retval BSP status.
  */
int32_t BSP_TS_Get_MultiTouchState(uint32_t Instance, TS_MultiTouch_State_t *TS_State)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= TS_INSTANCES_NBR) || (TS_State == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    FT6X06_MultiTouch_State_t State;
    uint8_t                   CallDriver = 1U;

    /* In polling mode */
    if (TsPollingMode == ENABLE)
    {
      if (HAL_EXTI_GetPending(&hts_exti[Instance], EXTI_TRIGGER_FALLING) == 0x0U)
      {
        /* No need to call driver, no new touch available */
        CallDriver = 0U;
        TS_State->TouchDetected = 0U;
      }
      else
      {
        /* New touch available, clear pending flag */
        HAL_EXTI_ClearPending(&hts_exti[Instance], EXTI_TRIGGER_FALLING);
      }
    }

    if (CallDriver == 1U)
    {
      /* Get the TS state */
      if (Ts_Drv[Instance]->GetMultiTouchState(Ts_CompObj[Instance], &State) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        if (State.TouchDetected != 0U)
        {
          /* Compute the oriented touch X and Y */
          if (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE)
          {
            TS_State->TouchX[0] = State.TouchX[0];
            TS_State->TouchY[0] = State.TouchY[0];
            TS_State->TouchX[1] = State.TouchX[1];
            TS_State->TouchY[1] = State.TouchY[1];
          }
          else if (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_PORTRAIT_ROT180)
          {
            TS_State->TouchX[0] = Ts_Ctx[Instance].MaxY - State.TouchY[0];
            TS_State->TouchY[0] = State.TouchX[0];
            TS_State->TouchX[1] = Ts_Ctx[Instance].MaxY - State.TouchY[1];
            TS_State->TouchY[1] = State.TouchX[1];
          }
          else if (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE_ROT180)
          {
            TS_State->TouchX[0] = Ts_Ctx[Instance].MaxX - State.TouchX[0];
            TS_State->TouchY[0] = Ts_Ctx[Instance].MaxY - State.TouchY[0];
            TS_State->TouchX[1] = Ts_Ctx[Instance].MaxX - State.TouchX[1];
            TS_State->TouchY[1] = Ts_Ctx[Instance].MaxY - State.TouchY[1];
          }
          else /* PORTRAIT */
          {
            TS_State->TouchX[0] = State.TouchY[0];
            TS_State->TouchY[0] = Ts_Ctx[Instance].MaxX - State.TouchX[0];
            TS_State->TouchX[1] = State.TouchY[1];
            TS_State->TouchY[1] = Ts_Ctx[Instance].MaxX - State.TouchX[1];
          }
          /* Get touch number, weight, event and area */
          TS_State->TouchDetected  = State.TouchDetected;
          TS_State->TouchWeight[0] = State.TouchWeight[0];
          TS_State->TouchWeight[1] = State.TouchWeight[1];
          TS_State->TouchEvent[0]  = State.TouchEvent[0];
          TS_State->TouchEvent[1]  = State.TouchEvent[1];
          TS_State->TouchArea[0]   = State.TouchArea[0];
          TS_State->TouchArea[1]   = State.TouchArea[1];
        }
        else
        {
          TS_State->TouchDetected = 0U;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Configure gesture on TS.
  * @param  Instance TS Instance.
  * @param  GestureConfig Pointer to gesture configuration structure.
  * @retval BSP status.
  */
int32_t BSP_TS_GestureConfig(uint32_t Instance, TS_Gesture_Config_t *GestureConfig)
{
  int32_t status;

  if ((Instance >= TS_INSTANCES_NBR) || (GestureConfig == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Feature not supported */
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  Get gesture.
  * @param  Instance TS Instance.
  * @param  GestureId Pointer to gesture.
  * @retval BSP status.
  */
int32_t BSP_TS_GetGestureId(uint32_t Instance, uint32_t *GestureId)
{
  int32_t status;

  if ((Instance >= TS_INSTANCES_NBR) || (GestureId == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Feature not supported */
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  Get the TS capabilities.
  * @param  Instance TS Instance.
  * @param  Capabilities Pointer to TS capabilities structure.
  * @retval BSP status.
  */
int32_t BSP_TS_GetCapabilities(uint32_t Instance, TS_Capabilities_t *Capabilities)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= TS_INSTANCES_NBR) || (Capabilities == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Get the TS driver capabilities */
    if (Ts_Drv[Instance]->GetCapabilities(Ts_CompObj[Instance], Capabilities) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Update maximum X and Y according orientation */
      if ((Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE) || (Ts_Ctx[Instance].Orientation == TS_ORIENTATION_LANDSCAPE_ROT180))
      {
        uint32_t tmp;
        tmp = Capabilities->MaxXl;
        Capabilities->MaxXl = Capabilities->MaxYl;
        Capabilities->MaxYl = tmp;
      }
    }
  }

  return status;
}

/**
  * @brief  TS Callback.
  * @param  Instance TS Instance.
  * @retval None.
  */
__weak void BSP_TS_Callback(uint32_t Instance)
{  
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on TS touch detection */  
}

/**
  * @brief  BSP TS interrupt handler.
  * @param  Instance TS Instance.
  * @retval None.
  */
void BSP_TS_IRQHandler(uint32_t Instance)
{
  HAL_EXTI_IRQHandler(&hts_exti[Instance]);
}
/**
  * @}
  */

/** @defgroup STM32L562E-DK_TS_Private_Functions STM32L562E-DK TS Private Functions
  * @{
  */ 
/**
  * @brief  Probe the FT6X06 TS driver.
  * @param  Instance TS Instance.
  * @retval BSP status.
  */
static int32_t FT6X06_Probe(uint32_t Instance)
{
  int32_t                status;
  FT6X06_IO_t            IOCtx;
  uint32_t               ft6x06_id;
  static FT6X06_Object_t FT6X06Obj;

  /* Configure the TS driver */
  IOCtx.Address     = TS_I2C_ADDRESS;
  IOCtx.Init        = BSP_I2C1_Init;
  IOCtx.DeInit      = BSP_I2C1_DeInit;
  IOCtx.ReadReg     = BSP_I2C1_ReadReg;
  IOCtx.WriteReg    = BSP_I2C1_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if (FT6X06_RegisterBusIO(&FT6X06Obj, &IOCtx) != FT6X06_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else if (FT6X06_ReadID(&FT6X06Obj, &ft6x06_id) != FT6X06_OK)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  } 
  else if (ft6x06_id != FT6X06_ID)
  {
    status = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    Ts_CompObj[Instance] = &FT6X06Obj;
    Ts_Drv[Instance]   = (TS_Drv_t *) &FT6X06_TS_Driver;
    if (Ts_Drv[Instance]->Init(Ts_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      status = BSP_ERROR_NONE;
    }
  }

  return status;
} 

/**
  * @brief  TS EXTI callback.
  * @retval None.
  */
static void TS_EXTI_Callback(void)
{
  BSP_TS_Callback(0);

  /* Clear interrupt on TS driver */
  if (Ts_Drv[0]->ClearIT(Ts_CompObj[0]) < 0)
  {
    /* Nothing to do */
  }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
