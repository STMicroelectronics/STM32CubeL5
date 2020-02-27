/**
  ******************************************************************************
  * @file    stm32l552e_eval_idd.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage the
  *          Idd measurement driver for STM32L552E-EV board.
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
#include "stm32l552e_eval_idd.h"
#include "stm32l552e_eval_io.h"

#if (USE_BSP_IO_CLASS == 1)

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E-EV
  * @{
  */

/** @defgroup STM32L552E-EV_IDD STM32L552E-EV IDD
  * @brief This file includes the Idd driver for STM32L552E-EV board.
  *        It allows user to measure MCU Idd current on board, especially in
  *        different low power modes.
  * @{
  */



/** @defgroup STM32L552E-EV_IDD_Private_Constant STM32L552E-EV IDD Private Constant
  * @{
  */

/**
  * @brief  Shunt values on evaluation board in milli ohms
  */
#define EVAL_IDD_SHUNT0_VALUE                  1000U     /*!< value in milliohm */
#define EVAL_IDD_SHUNT1_VALUE                  24U       /*!< value in ohm */
#define EVAL_IDD_SHUNT2_VALUE                  620U      /*!< value in ohm */
#define EVAL_IDD_SHUNT3_VALUE                  0U        /*!< value in ohm */
#define EVAL_IDD_SHUNT4_VALUE                  10000U    /*!< value in ohm */

/**
  * @brief  Shunt stabilization delay on evaluation board in milli ohms
  */
#define EVAL_IDD_SHUNT0_STABDELAY              149U       /*!< value in millisec */
#define EVAL_IDD_SHUNT1_STABDELAY              149U       /*!< value in millisec */
#define EVAL_IDD_SHUNT2_STABDELAY              149U       /*!< value in millisec */
#define EVAL_IDD_SHUNT3_STABDELAY              0U         /*!< value in millisec */
#define EVAL_IDD_SHUNT4_STABDELAY              255U       /*!< value in millisec */

/**
  * @brief  IDD Ampli Gain on evaluation board
  */
#define EVAL_IDD_AMPLI_GAIN                    4967U     /*!< value is gain * 100 */

/**
  * @brief  IDD Vdd Min on evaluation board
  */
#define EVAL_IDD_VDD_MIN                       1710U     /*!< value in millivolt */

/**
  * @brief  IDD amp control pin
  */
#define EVAL_IDD_AMP_CONTROL_PIN               AGPIO_PIN_1

/**
  * @}
  */

/** @addtogroup STM32L552E-EV_IDD_Exported_Variables
  * @{
  */
EXTI_HandleTypeDef hidd_exti[IDD_INSTANCES_NBR];
/**
  * @}
  */

/** @defgroup STM32L552E-EV_IDD_Exported_Functions STM32L552E-EV IDD Exported Functions
  * @{
  */
static void IDD1_EXTI_Callback(void);


/**
  * @brief  BSP Idd measurement ended Callback.
  * @param  Instance IDD instance.
  * @retval None.
  */
__weak void BSP_IDD_Callback(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  /* This function should be implemented by the user application. */
}


/**
  * @brief  BSP IDD interrupt handler.
  * @param  Instance IDD instance.
  * @retval None.
  */
void BSP_IDD_IRQHandler(uint32_t Instance)
{
  HAL_EXTI_IRQHandler(&hidd_exti[Instance]);
}

/**
  * @brief  IDD1 EXTI line detection callbacks.
  * @retval None
  */
static void IDD1_EXTI_Callback(void)
{
  BSP_IDD_Callback(0);
}

/**
  * @brief  Configures IDD measurement component.
  * @retval BSP status.
  */
int32_t BSP_IDD_Init(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if(Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
    {
      if(BSP_IOEXPANDER_Init(Instance, IOEXPANDER_IDD_MODE) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        MFXSTM32L152_IDD_Config_t iddconfig;

        /* Configure Idd component with default values */
        iddconfig.AmpliGain = EVAL_IDD_AMPLI_GAIN;
        iddconfig.VddMin = EVAL_IDD_VDD_MIN;
        iddconfig.Shunt0Value = EVAL_IDD_SHUNT0_VALUE;
        iddconfig.Shunt1Value = EVAL_IDD_SHUNT1_VALUE;
        iddconfig.Shunt2Value = EVAL_IDD_SHUNT2_VALUE;
        iddconfig.Shunt3Value = EVAL_IDD_SHUNT3_VALUE;
        iddconfig.Shunt4Value = EVAL_IDD_SHUNT4_VALUE;
        iddconfig.Shunt0StabDelay = EVAL_IDD_SHUNT0_STABDELAY;
        iddconfig.Shunt1StabDelay = EVAL_IDD_SHUNT1_STABDELAY;
        iddconfig.Shunt2StabDelay = EVAL_IDD_SHUNT2_STABDELAY;
        iddconfig.Shunt3StabDelay = EVAL_IDD_SHUNT3_STABDELAY;
        iddconfig.Shunt4StabDelay = EVAL_IDD_SHUNT4_STABDELAY;
        iddconfig.ShuntNbOnBoard = MFXSTM32L152_IDD_SHUNT_NB_4;
        iddconfig.ShuntNbUsed = MFXSTM32L152_IDD_SHUNT_NB_4;
        iddconfig.VrefMeasurement = MFXSTM32L152_IDD_VREF_AUTO_MEASUREMENT_ENABLE;
        iddconfig.Calibration = MFXSTM32L152_IDD_AUTO_CALIBRATION_ENABLE;

        iddconfig.PreDelayUnit = MFXSTM32L152_IDD_PREDELAY_20_MS;
        iddconfig.PreDelayValue = 0x7FU; /* 127 * 20 = 2540 ms (max value) */

        iddconfig.DeltaDelayUnit = MFXSTM32L152_IDD_DELTADELAY_0_5_MS;
        iddconfig.DeltaDelayValue = 4U;
        iddconfig.MeasureNb = 250; /* Acquisition duration is 250 * 4 * 0.5 = 500 ms */

        status = Idd_Drv[Instance]->Config(&Io_CompObj[Instance], &iddconfig);

        if (status >= 0)
        {
          Io_Ctx[Instance].Functions |= IOEXPANDER_IDD_MODE;
          BSP_IOEXPANDER_ITConfig(Instance);

          if (HAL_EXTI_GetHandle(&hidd_exti[Instance], IOEXPANDER_IRQOUT_EXTI_LINE) == HAL_OK)
          {
            /* Interrupt already enabled on BSP_IOEXPANDER_Init, just register callback */
            if (HAL_EXTI_RegisterCallback(&hidd_exti[Instance], HAL_EXTI_RISING_CB_ID, IDD1_EXTI_Callback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
          }
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Unconfigures IDD measurement component.
  * @retval BSP status.
  */
int32_t BSP_IDD_DeInit(uint32_t Instance)
{
  int32_t status;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    Io_Ctx[Instance].Functions &= ~(IOEXPANDER_IDD_MODE);
    status = BSP_IOEXPANDER_DeInit(Instance);
  }

  return status;
}

/**
  * @brief  Reset Idd measurement component.
  * @retval BSP status
  */
int32_t BSP_IDD_Reset(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Idd_Drv[Instance]->Reset(&Io_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Turn Idd measurement component in low power (standby/sleep) mode
  * @retval BSP status
  */
int32_t BSP_IDD_EnterLowPower(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Idd_Drv[Instance]->LowPower(&Io_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Wake up Idd measurement component.
  * @retval BSP status
  */
int32_t BSP_IDD_ExitLowPower(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Idd_Drv[Instance]->WakeUp(&Io_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}


/**
  * @brief  Start Measurement campaign
  * @retval BSP status
  */
int32_t BSP_IDD_StartMeasurement(uint32_t Instance)
{
  int32_t status;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    BSP_IO_Init_t io;

    io.Pin = EVAL_IDD_AMP_CONTROL_PIN;
    io.Mode = IO_MODE_OUTPUT_PP;
    io.Pull = GPIO_PULLUP;
    status = BSP_IO_Init(Instance, &io);
    if (status >= 0)
    {
      status = BSP_IO_WritePin(Instance, EVAL_IDD_AMP_CONTROL_PIN, IO_PIN_RESET);

      if (status >= 0)
      {
        if (Idd_Drv[Instance]->Start(&Io_CompObj[Instance]) < 0)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Configure Idd component
  * @param  IddConfig: structure of idd parameters
  * @retval BSP status
  */
int32_t BSP_IDD_Config(uint32_t Instance, BSP_IDD_Config_t * IddConfig)
{
  int32_t status;

  if ((Instance >= IDD_INSTANCES_NBR) || (IddConfig == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    MFXSTM32L152_IDD_Config_t MfxIddConfig;

    /* Configure Idd component with default values */
    MfxIddConfig.AmpliGain = EVAL_IDD_AMPLI_GAIN;
    MfxIddConfig.VddMin = EVAL_IDD_VDD_MIN;
    MfxIddConfig.Shunt0Value = EVAL_IDD_SHUNT0_VALUE;
    MfxIddConfig.Shunt1Value = EVAL_IDD_SHUNT1_VALUE;
    MfxIddConfig.Shunt2Value = EVAL_IDD_SHUNT2_VALUE;
    MfxIddConfig.Shunt3Value = EVAL_IDD_SHUNT3_VALUE;
    MfxIddConfig.Shunt4Value = EVAL_IDD_SHUNT4_VALUE;
    MfxIddConfig.Shunt0StabDelay = EVAL_IDD_SHUNT0_STABDELAY;
    MfxIddConfig.Shunt1StabDelay = EVAL_IDD_SHUNT1_STABDELAY;
    MfxIddConfig.Shunt2StabDelay = EVAL_IDD_SHUNT2_STABDELAY;
    MfxIddConfig.Shunt3StabDelay = EVAL_IDD_SHUNT3_STABDELAY;
    MfxIddConfig.Shunt4StabDelay = EVAL_IDD_SHUNT4_STABDELAY;
    MfxIddConfig.ShuntNbOnBoard = MFXSTM32L152_IDD_SHUNT_NB_4;
    MfxIddConfig.ShuntNbUsed = MFXSTM32L152_IDD_SHUNT_NB_4;
    MfxIddConfig.VrefMeasurement = MFXSTM32L152_IDD_VREF_AUTO_MEASUREMENT_ENABLE;
    MfxIddConfig.Calibration = MFXSTM32L152_IDD_AUTO_CALIBRATION_ENABLE;

    status = BSP_ERROR_NONE;

    /* Calculate predelay */
    if (IddConfig->PreDelay <= 63)
    {
      MfxIddConfig.PreDelayUnit = MFXSTM32L152_IDD_PREDELAY_0_5_MS;
      MfxIddConfig.PreDelayValue = (uint8_t) (IddConfig->PreDelay) * 2U;
    }
    else if (IddConfig->PreDelay <= (int32_t) BSP_IDD_PRE_DELAY_MAX)
    {
      MfxIddConfig.PreDelayUnit = MFXSTM32L152_IDD_PREDELAY_20_MS;
      MfxIddConfig.PreDelayValue = (uint8_t) ((uint32_t) IddConfig->PreDelay / 20U);
    }
    else
    {
      status = BSP_ERROR_WRONG_PARAM;
    }

    /* Calculate acquisition duration */
    if (IddConfig->AcquisitionDuration <= 250)
    {
      MfxIddConfig.DeltaDelayUnit = MFXSTM32L152_IDD_DELTADELAY_0_5_MS;
      MfxIddConfig.DeltaDelayValue = 1;
      MfxIddConfig.MeasureNb = (uint8_t) IddConfig->AcquisitionDuration + 1U;
    }
    else if (IddConfig->AcquisitionDuration <= 2500)
    {
      MfxIddConfig.DeltaDelayUnit = MFXSTM32L152_IDD_DELTADELAY_0_5_MS;
      MfxIddConfig.DeltaDelayValue = 19;
      MfxIddConfig.MeasureNb = (uint8_t) (((uint32_t) IddConfig->AcquisitionDuration / 10U) + 1U);
    }
    else if (IddConfig->AcquisitionDuration <= (int32_t) BSP_IDD_ACQUISITION_DURATION_MAX)
    {
      MfxIddConfig.DeltaDelayUnit = MFXSTM32L152_IDD_DELTADELAY_20_MS;
      MfxIddConfig.DeltaDelayValue = 4;
      MfxIddConfig.MeasureNb = (uint8_t) (((uint32_t) IddConfig->AcquisitionDuration / 100U) + 1U);
    }
    else
    {
      status = BSP_ERROR_WRONG_PARAM;
    }

    if (status == BSP_ERROR_NONE)
    {
      if (Idd_Drv[Instance]->Config(&Io_CompObj[Instance], &MfxIddConfig) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  return status;
}

/**
  * @brief  Get Idd current value.
  * @param  IddValue: Pointer on u32 to store Idd. Value unit is 10 nA.
  * @retval BSP status
  */
int32_t BSP_IDD_GetValue(uint32_t Instance, uint32_t *IddValue)
{
  int32_t status;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    BSP_IO_Init_t io;

    io.Pin = EVAL_IDD_AMP_CONTROL_PIN;
    io.Mode = IO_MODE_INPUT;
    io.Pull = GPIO_NOPULL;

    /* De-activate the OPAMP used ny the MFX to measure the current consumption */
    status = BSP_IO_Init(0, &io);

    if (status >= 0)
    {
      if (Idd_Drv[Instance]->GetValue(&Io_CompObj[Instance], IddValue) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  return status;
}

/**
  * @brief  Enable Idd interrupt that warn end of measurement
  * @retval BSP status
  */
int32_t BSP_IDD_EnableIT(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Idd_Drv[Instance]->EnableIT(&Io_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Clear Idd interrupt that warn end of measurement
  * @retval BSP status
  */
int32_t BSP_IDD_ClearIT(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Idd_Drv[Instance]->ClearIT(&Io_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get Idd interrupt status
  * @retval status
  */
int32_t BSP_IDD_GetITStatus(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Idd_Drv[Instance]->ITStatus(&Io_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Disable Idd interrupt that warn end of measurement
  * @retval BSP status
  */
int32_t BSP_IDD_DisableIT(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Io_Ctx[Instance].Functions & IOEXPANDER_IDD_MODE) != IOEXPANDER_IDD_MODE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Idd_Drv[Instance]->DisableIT(&Io_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
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

