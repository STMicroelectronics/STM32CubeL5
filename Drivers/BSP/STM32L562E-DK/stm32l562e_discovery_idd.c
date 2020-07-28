/**
  ******************************************************************************
  * @file    stm32l562e_discovery_idd.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage the
  *          Idd measurement driver for STM32L562E-DK board.
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
#include "stm32l562e_discovery.h"
#include "stm32l562e_discovery_idd.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L562E-DK
  * @{
  */

/** @defgroup STM32L562E-DK_IDD STM32L562E-DK IDD
  * @brief This file includes the Idd driver for STM32L562E-DK board.
  *        It allows user to measure MCU Idd current on board, especially in
  *        different low power modes.
  * @{
  */

/** @defgroup STM32L562E-DK_IDD_Private_Defines STM32L562E-DK IDD Private Defines
  * @{
  */

/**
  * @brief  PowerShield HW constant.
  * Refer to UM2269 Table 1
  */
#define PS_TIMEOUT                     1000UL
#define PS_CMD_LENGTH_MAX              20U
#define PS_SHELL_FEEDBACK_LENGTH_MAX   100U
#define PS_NOT_READY_RETRY_DELAY_MS    3000UL
#define PS_SHELL_BUFFER_EMPTY_DELAY_MS 30UL
#define PS_SHELL_CHAR_UNIT_MILLI       ((uint8_t)'m')
#define CHAR_END_OF_STRING             ((uint8_t)'\0')

#define PS1_INSTANCE                   LPUART1
#define PS1_CLK_ENABLE()               __HAL_RCC_LPUART1_CLK_ENABLE()
#define PS1_CLK_DISABLE()              __HAL_RCC_LPUART1_CLK_DISABLE()
#define PS1_TX_GPIO_PORT               GPIOB
#define PS1_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define PS1_TX_PIN                     GPIO_PIN_11
#define PS1_TX_AF                      GPIO_AF8_LPUART1
#define PS1_RX_GPIO_PORT               GPIOB
#define PS1_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define PS1_RX_PIN                     GPIO_PIN_10
#define PS1_RX_AF                      GPIO_AF8_LPUART1

#define PS1_IRQOUT_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()
#define PS1_IRQOUT_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOC_CLK_DISABLE()
#define PS1_IRQOUT_GPIO_PIN            GPIO_PIN_13
#define PS1_IRQOUT_GPIO_PORT           GPIOC
#define PS1_IRQOUT_EXTI_IRQn           EXTI13_IRQn
#define PS1_IRQOUT_EXTI_LINE           EXTI_LINE_13
/**
  * @}
  */

/** @defgroup STM32L562E-DK_IDD_Private_Constants STM32L562E-DK IDD Private Constants
  * @{
  */
/**
  * @brief  Command send to PowerShield.
  * Refer to UM2269 Table 1
  */
#define PS1_CMD_none_string "" /* No command, used to refresh shell prompt */
#define PS1_CMD_htc_string "htc"
#define PS1_CMD_output_energy_string "output energy"
#define PS1_CMD_freq_string "freq "
#define PS1_CMD_acqtime_string "acqtime "
#define PS1_CMD_trigdelay_string "trigdelay "
#define PS1_CMD_voltage_string "volt "
#define PS1_CMD_voltage_no_calibration_string " nocal"
#define PS1_CMD_start_string "start"
#define PS1_CMD_enableit_string "itend en"
#define PS1_CMD_disableit_string "itend dis"
#define PS1_CMD_getlast_string "getlast n"
#define PS1_CMD_psrst_string "psrst"
#define PS1_GET_ack_string "ack"
#define PS1_GET_end_string "end"
#define PS1_shell_command_end_characters_string "\r\n"
#define PS1_shell_prompt_string "PowerShield > "
#define PS1_shell_acknowledge_string "ack "
#define PS1_shell_error_string "error"
#define PS1_shell_error_description_string "Error detail:"


static uint8_t PS1_CMD_htc[] = PS1_CMD_htc_string;
//static uint8_t PS1_GET_ack[] = PS1_GET_ack_string;
//static uint8_t PS1_GET_end[] = PS1_GET_end_string;
static uint8_t PS1_shell_command_end_characters[] = PS1_shell_command_end_characters_string;
//static uint8_t PS1_shell_error[] = PS1_shell_error_string;
/**
  * @}
  */

/** @defgroup STM32L562E-DK_IDD_Exported_Variables STM32L562E-DK IDD Exported Variables
  * @{
  */
IDD_Ctx_t Idd_Ctx[IDD_INSTANCES_NBR] = {0};
/**
  * @}
  */

/** @defgroup STM32L562E-DK_IDD_Private_FunctionPrototypes STM32L562E-DK IDD Private Function Prototypes
  * @{
  */
static void IDD1_EXTI_Callback(void);
static uint32_t asciiToUint(uint8_t * buf);
static uint32_t uintToAscii(uint32_t uintNumber, uint8_t * buf);
static int32_t waitForAckData(uint8_t *cmd, uint32_t cmd_length, uint8_t *returned_data, uint32_t *returned_data_length);
static int32_t SendCmdData(uint8_t *cmd, uint32_t cmd_length, uint8_t *returned_data, uint32_t *returned_data_length);
static int32_t SendCmdNoData(uint8_t *cmd, uint32_t cmd_length);
static void UART_MspInit(UART_HandleTypeDef *huart);
static void UART_MspDeInit(UART_HandleTypeDef *huart);
/**
  * @}
  */

/** @defgroup STM32L562E-DK_IDD_Exported_Functions STM32L562E-DK IDD Exported Functions
  * @{
  */

/**
  * @brief  BSP Idd measurement ended Callback
  * @param  Instance IDD Instance.
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
  HAL_EXTI_IRQHandler(&Idd_Ctx[Instance].hidd_exti);
}

/**
  * @brief  Initialize IDD measurement component
  * @param  Instance IDD instance.
  * @retval BSP status.
  */
int32_t BSP_IDD_Init(uint32_t Instance)
{
  uint8_t PS1_CMD_output_energy[] = PS1_CMD_output_energy_string;

  int32_t status = BSP_ERROR_NONE;

  if(Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state != IDD_IDLE)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    UART_MspInit(&Idd_Ctx[Instance].UartHandle);
#else
    /* Register MspInit/MspDeInit callbacks */
    if (HAL_UART_RegisterCallback(&Idd_Ctx[Instance].UartHandle, HAL_UART_MSPINIT_CB_ID, UART_MspInit) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_UART_RegisterCallback(&Idd_Ctx[Instance].UartHandle, HAL_UART_MSPDEINIT_CB_ID, UART_MspDeInit) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
#endif
    {
      /* COM configuration */
      Idd_Ctx[Instance].UartHandle.Instance        = PS1_INSTANCE;
      Idd_Ctx[Instance].UartHandle.Init.BaudRate   = 115200;
      Idd_Ctx[Instance].UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
      Idd_Ctx[Instance].UartHandle.Init.StopBits   = UART_STOPBITS_1;
      Idd_Ctx[Instance].UartHandle.Init.Parity     = UART_PARITY_NONE;
      Idd_Ctx[Instance].UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
      Idd_Ctx[Instance].UartHandle.Init.Mode       = UART_MODE_TX_RX;

      /* Configure default measure */
      Idd_Ctx[Instance].IddConfig.PreDelay = 1000;
      Idd_Ctx[Instance].IddConfig.AcquisitionDuration = 1000;
      Idd_Ctx[Instance].IddConfig.Voltage = 3300;

      /* State INIT */
      Idd_Ctx[Instance].state = IDD_INIT;

      /* Init Uart */
      if (HAL_UART_Init(&Idd_Ctx[Instance].UartHandle) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        /* Check if power monitor is alive */
        if (SendCmdNoData((uint8_t*) "", sizeof("")) != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        /* Check if power monitor is ready */
        else if (SendCmdNoData((uint8_t*) PS1_CMD_htc, sizeof(PS1_CMD_htc)) != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        /* Configure energy mode */
        else if (SendCmdNoData((uint8_t*) PS1_CMD_output_energy, sizeof(PS1_CMD_output_energy)) != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        /* acqtime and trigdelay */
        else if (BSP_IDD_Config(Instance, &Idd_Ctx[Instance].IddConfig) != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        else
        {
          /* Initialize Powershield interrupt */
          GPIO_InitTypeDef  gpio_init_structure;

          PS1_IRQOUT_GPIO_CLK_ENABLE();

          gpio_init_structure.Pin   = PS1_IRQOUT_GPIO_PIN;
          gpio_init_structure.Pull  = GPIO_PULLDOWN;
          gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
          gpio_init_structure.Mode  = GPIO_MODE_IT_RISING;
          HAL_GPIO_Init(PS1_IRQOUT_GPIO_PORT, &gpio_init_structure);

          if (HAL_EXTI_GetHandle(&Idd_Ctx[Instance].hidd_exti, PS1_IRQOUT_EXTI_LINE) == HAL_OK)
          {
            /* Interrupt already enabled on BSP_PS_Init, just register callback */
            if (HAL_EXTI_RegisterCallback(&Idd_Ctx[Instance].hidd_exti, HAL_EXTI_RISING_CB_ID, IDD1_EXTI_Callback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
            else
            {
              /* Enable and set GPIO EXTI Interrupt */
              HAL_NVIC_SetPriority(PS1_IRQOUT_EXTI_IRQn, BSP_IDD_IT_PRIORITY, 0x00);
              HAL_NVIC_EnableIRQ(PS1_IRQOUT_EXTI_IRQn);
            }
          }
        }
      }
    }
  }

  /* Clean context */
  if (status != BSP_ERROR_NONE)
  {
    BSP_IDD_DeInit(Instance);
  }

  return status;
}

/**
  * @brief  DeInitialize IDD measurement component
  * @param  Instance IDD instance.
  * @retval BSP status.
  */
int32_t BSP_IDD_DeInit(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    UART_MspDeInit(&Idd_Ctx[Instance].UartHandle);
    if (HAL_UART_DeInit(&Idd_Ctx[Instance].UartHandle) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      Idd_Ctx[Instance].state = IDD_IDLE;
    }
  }

  return status;
}

/**
  * @brief  Reset Idd measurement component
  * @param  Instance IDD instance.
  * @retval BSP status
  */
int32_t BSP_IDD_Reset(uint32_t Instance)
{
  uint8_t PS1_CMD_psrst[] = PS1_CMD_psrst_string;
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else if (SendCmdNoData((uint8_t*) PS1_CMD_psrst, sizeof(PS1_CMD_psrst)) != BSP_ERROR_NONE)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Nothing to do: status already BSP_ERROR_NONE */
  }

  return status;
}

/**
  * @brief  Turn Idd measurement component in low power (standby/sleep) mode
  * @param  Instance IDD instance.
  * @retval BSP status
  */
int32_t BSP_IDD_EnterLowPower(uint32_t Instance)
{
  int32_t status;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Not Supported by the Power Shield */
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  Wake up Idd measurement component
  * @param  Instance IDD instance.
  * @retval BSP status
  */
int32_t BSP_IDD_ExitLowPower(uint32_t Instance)
{
  int32_t status;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Not Supported by the Power Shield */
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return status;
}


/**
  * @brief  Start Measurement campaign
  * @param  Instance IDD instance.
  * @retval BSP status
  */
int32_t BSP_IDD_StartMeasurement(uint32_t Instance)
{
  uint8_t PS1_CMD_start[] = PS1_CMD_start_string;
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else if (SendCmdNoData((uint8_t*) PS1_CMD_start, sizeof(PS1_CMD_start)) != BSP_ERROR_NONE)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Nothing to do: status already BSP_ERROR_NONE */
  }

  return status;
}

/**
  * @brief  Configure Idd component
  * @param  Instance IDD instance.
  * @param  IddConfig structure of idd parameters
  * @retval BSP status
  */
int32_t BSP_IDD_Config(uint32_t Instance, BSP_IDD_Config_t *IddConfig)
{
  uint8_t PS1_CMD_freq[] = PS1_CMD_freq_string;
  uint8_t PS1_CMD_acqtime[] = PS1_CMD_acqtime_string;
  uint8_t PS1_CMD_trigdelay[] = PS1_CMD_trigdelay_string;
  uint8_t PS1_CMD_voltage[] = PS1_CMD_voltage_string;
  uint8_t PS1_CMD_voltage_no_calibration[] = PS1_CMD_voltage_no_calibration_string;

  uint8_t buf[PS_CMD_LENGTH_MAX];
  uint32_t buf_length;
  int32_t status = BSP_ERROR_NONE;
  uint32_t size;
  uint32_t i;

  if ((Instance >= IDD_INSTANCES_NBR) || (IddConfig == (BSP_IDD_Config_t *)NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Set acquisition duration */
    for (i = 0; i < (sizeof(PS1_CMD_acqtime) - 1U); i++)
    {
      buf[i] = PS1_CMD_acqtime[i];
    }

    size = uintToAscii((uint32_t)IddConfig->AcquisitionDuration, buf + sizeof(PS1_CMD_acqtime) - 1U);
    buf_length = (uint32_t)(sizeof(PS1_CMD_acqtime) + size - 1U);

    buf[buf_length] = PS_SHELL_CHAR_UNIT_MILLI;
    buf_length++;

    if (SendCmdNoData((uint8_t*) buf, buf_length) != BSP_ERROR_NONE)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      for (i = 0; i < (sizeof(PS1_CMD_freq) - 1U); i++)
      {
        buf[i] = PS1_CMD_freq[i];
      }

      /* Set acquisition frequency close to acquisition time value to integrate energy on few values
         (integration dispatched on from 1 to 10 values) */
      /* Note: Raw acquisition frequency is higher than this value: in acquisition mode "energy",
         Powershield raw acquisition frequency is 100kSmps/sec and integration time
         corresponds to parameter "freq".
      */
      if(IddConfig->AcquisitionDuration >= 1000UL)
      {
        Idd_Ctx[Instance].Integration_freq = 1UL;
      }
      else if(IddConfig->AcquisitionDuration >= 100UL)
      {
        Idd_Ctx[Instance].Integration_freq = 10UL;
      }
      else if(IddConfig->AcquisitionDuration >= 10UL)
      {
        Idd_Ctx[Instance].Integration_freq = 100UL;
      }
      else
      {
        Idd_Ctx[Instance].Integration_freq = 1000UL;
      }

      size = uintToAscii(Idd_Ctx[Instance].Integration_freq, buf + sizeof(PS1_CMD_freq) - 1U);
      buf_length = sizeof(PS1_CMD_freq) + size - 1U;

      if (SendCmdNoData((uint8_t*) buf, buf_length) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        /* Set trigger delay */
        for (i = 0; i < (sizeof(PS1_CMD_trigdelay) - 1U); i++)
        {
          buf[i] = PS1_CMD_trigdelay[i];
        }

        size = uintToAscii((uint32_t)IddConfig->PreDelay, buf + sizeof(PS1_CMD_trigdelay) - 1U);
        buf_length = sizeof(PS1_CMD_trigdelay) + size - 1U;

        buf[buf_length] = PS_SHELL_CHAR_UNIT_MILLI;
        buf_length++;

        if (SendCmdNoData((uint8_t*) buf, buf_length) != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        else
        {
          /* Set voltage level */
          for (i = 0; i < (sizeof(PS1_CMD_voltage) - 1U); i++)
          {
            buf[i] = PS1_CMD_voltage[i];
          }

          size = uintToAscii((uint32_t)IddConfig->Voltage, buf + sizeof(PS1_CMD_voltage) - 1U);
          buf_length = sizeof(PS1_CMD_voltage) + size - 1U;

          buf[buf_length] = PS_SHELL_CHAR_UNIT_MILLI;
          buf_length++;

          /* Add option of no calibration to not disconnect power supply */
          /* (Default case is calibration performed at each voltage change, with calibration requiring target disconnection) */
          for (i = 0; i < (sizeof(PS1_CMD_voltage_no_calibration) - 1U); i++)
          {
            buf[buf_length + i] = PS1_CMD_voltage_no_calibration[i];
          }
          buf_length += sizeof(PS1_CMD_voltage_no_calibration) - 1U;

          if (SendCmdNoData((uint8_t*) buf, buf_length) != BSP_ERROR_NONE)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }

          /* Wait for voltage stabilization */
          HAL_Delay(120);
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Get Idd current value.
  * @param  Instance IDD instance.
  * @param  IddValue_nA Pointer on u32 to store Idd (unit: nA)
  * @retval BSP status
  */
int32_t BSP_IDD_GetValue(uint32_t Instance, uint32_t *IddValue_nA)
{
  uint8_t PS1_CMD_getlast[] = PS1_CMD_getlast_string;
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    GPIO_InitTypeDef GPIO_Init;

    /* Wait for Powershield shell buffer emptying after acquisition */
    HAL_Delay(PS_SHELL_BUFFER_EMPTY_DELAY_MS);

    PS1_TX_GPIO_CLK_ENABLE();
    PS1_RX_GPIO_CLK_ENABLE();
    PS1_CLK_ENABLE();

    /* Configure COM Tx as alternate function */
    GPIO_Init.Pin       = PS1_TX_PIN;
    GPIO_Init.Mode      = GPIO_MODE_AF_OD;
    GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init.Pull      = GPIO_PULLUP;
    GPIO_Init.Alternate = PS1_TX_AF;
    HAL_GPIO_Init(PS1_TX_GPIO_PORT, &GPIO_Init);

    /* Configure COM Rx as alternate function */
    GPIO_Init.Pin       = PS1_RX_PIN;
    GPIO_Init.Alternate = PS1_RX_AF;
    HAL_GPIO_Init(PS1_RX_GPIO_PORT, &GPIO_Init);

    /* COM configuration */
    Idd_Ctx[Instance].UartHandle.Instance        = PS1_INSTANCE;
    Idd_Ctx[Instance].UartHandle.Init.BaudRate   = 115200;
    Idd_Ctx[Instance].UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    Idd_Ctx[Instance].UartHandle.Init.StopBits   = UART_STOPBITS_1;
    Idd_Ctx[Instance].UartHandle.Init.Parity     = UART_PARITY_NONE;
    Idd_Ctx[Instance].UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    Idd_Ctx[Instance].UartHandle.Init.Mode       = UART_MODE_TX_RX;

    if (HAL_UART_Init(&Idd_Ctx[Instance].UartHandle) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }

    /* Get last measurement data */
    uint8_t data[PS_SHELL_FEEDBACK_LENGTH_MAX];
    uint32_t data_length;
    if (SendCmdData((uint8_t*) PS1_CMD_getlast, sizeof(PS1_CMD_getlast), data, &data_length) != BSP_ERROR_NONE)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      /* Convert energy to current */
      /* Note: Units: Energy retrieved in nJ, voltage in mV, integration frequency in Hz, current computed in nA */
      *IddValue_nA = (uint32_t)((((uint64_t)asciiToUint(data)) * Idd_Ctx[Instance].Integration_freq * 1000UL) / (Idd_Ctx[Instance].IddConfig.Voltage));
    }
  }

  return status;
}

/**
  * @brief  Enable Idd interrupt that indicates end of measurement
  * @param  Instance IDD instance.
  * @retval BSP status
  */
int32_t BSP_IDD_EnableIT(uint32_t Instance)
{
  uint8_t PS1_CMD_enableit[] = PS1_CMD_enableit_string;
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else if (SendCmdNoData((uint8_t*) PS1_CMD_enableit, sizeof(PS1_CMD_enableit)) != BSP_ERROR_NONE)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Nothing to do: status already BSP_ERROR_NONE */
  }

  return status;
}

/**
  * @brief  Clear Idd interrupt that indicates end of measurement
  * @param  Instance IDD instance.
  * @retval BSP status
  */
int32_t BSP_IDD_ClearIT(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Auto Clear, nothing to do: status already BSP_ERROR_NONE */
  }

  return status;
}

/**
  * @brief  Get Idd interrupt status
  * @param  Instance IDD instance.
  * @retval status
  */
int32_t BSP_IDD_GetITStatus(uint32_t Instance)
{
  int32_t status;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Not Supported by the Power Shield */
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  Disable Idd interrupt that indicates end of measurement
  * @param  Instance IDD instance.
  * @retval BSP status
  */
int32_t BSP_IDD_DisableIT(uint32_t Instance)
{
  uint8_t PS1_CMD_disableit[] = PS1_CMD_disableit_string;
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= IDD_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Idd_Ctx[Instance].state == IDD_IDLE)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else if (SendCmdNoData((uint8_t*) PS1_CMD_disableit, sizeof(PS1_CMD_disableit)) != BSP_ERROR_NONE)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Nothing to do: status already BSP_ERROR_NONE */
  }

  return status;
}

/**
  * @}
  */

/** @defgroup STM32L562E-DK_IDD_Private_Functions STM32L562E-DK IDD Private Functions
  * @{
  */

/**
  * @brief  IDD1 EXTI line detection callbacks
  * @retval None
  */
static void IDD1_EXTI_Callback(void)
{
  BSP_IDD_Callback(0);
}

/**
  * @brief  Wait for acknowledge and retrieve feedbak data from power shield
  * @note   If command returned no data, then "returned_data_length" is equal to 0.
  * @note   To not return any data, set pointer "returned_data_length" to value NULL.
  * @param cmd Command sent to PowerShield
  * @param cmd_length Command length sent to PowerShield
  * @param returned_data Data retrieved from PowerShield. Maximum length is PS_SHELL_FEEDBACK_LENGTH_MAX.
  * @param returned_data_length Data retrieved from PowerShield
  * @retval BSP status
  */
static int32_t waitForAckData(uint8_t *cmd, uint32_t cmd_length, uint8_t *returned_data, uint32_t *returned_data_length)
{
  uint8_t PS1_shell_prompt[] = PS1_shell_prompt_string;
  uint8_t PS1_shell_acknowledge[] = PS1_shell_acknowledge_string;
  uint8_t PS1_shell_error_description[] = PS1_shell_error_description_string;
  uint8_t value;

  static uint32_t index, string_cmp_start_index, diff, count;
  HAL_StatusTypeDef halStatus;
  int32_t status = BSP_ERROR_NONE;
  static uint8_t receive_buffer[sizeof(PS1_shell_command_end_characters) + sizeof(PS1_shell_prompt) + PS_SHELL_FEEDBACK_LENGTH_MAX] = {CHAR_END_OF_STRING};

  /* Initialize variables */
  diff = 0;
  count = 0;

  /* Get the first character */
  halStatus = HAL_UART_Receive(&Idd_Ctx[0].UartHandle, &value, 1, PS_TIMEOUT);

  if(halStatus == HAL_OK)
  {
    receive_buffer[count] = value;
    count++;

    /* Receive PowerShield feedback message till end of char or 5ms */
    do
    {
      halStatus = HAL_UART_Receive(&Idd_Ctx[0].UartHandle, &value, 1, 5);
      if(halStatus == HAL_OK)
      {
        receive_buffer[count] = value;
        count++;
      }
    }
    while((value != 0) && (count <= sizeof(receive_buffer)) && (halStatus == HAL_OK));

    if(halStatus != HAL_ERROR)
    {
      /* Verify message integrity: shell command end characters */
      string_cmp_start_index = 0;
      for (index = 0; index < (sizeof(PS1_shell_command_end_characters) - 1U); index++)
      {
        if(receive_buffer[index] != PS1_shell_command_end_characters[index])
        {
          diff++;
        }
      }

      /* Verify message integrity: shell prompt */
      string_cmp_start_index += (sizeof(PS1_shell_command_end_characters) - 1U);
      for (index = 0; index < (sizeof(PS1_shell_prompt) - 1U); index++)
      {
        if(receive_buffer[string_cmp_start_index + index] != PS1_shell_prompt[index])
        {
          diff++;
        }
      }


      if (cmd_length != 2) /* No check for empty prompt command */
      {
        /* Verify message integrity: shell acknowledge string */
        /* Note: In case of error, error string (refer to "PS1_shell_error") is sent
                 followed by error description */
        string_cmp_start_index += (sizeof(PS1_shell_prompt) - 1U);
        for (index = 0; index < (sizeof(PS1_shell_acknowledge) - 1U); index++)
        {
          if(receive_buffer[string_cmp_start_index + index] != PS1_shell_acknowledge[index])
          {
            diff++;
          }
        }

        /* Verify message integrity: echo of command string */
        /* Note: In case of error, error string (refer to "PS1_shell_error") is sent
                 followed by error description */
        string_cmp_start_index += (sizeof(PS1_shell_acknowledge) - 1U);
        for (index = 0; index < (cmd_length - 2U); index++)
        {
          if(receive_buffer[string_cmp_start_index + index] != cmd[index])
          {
            diff++;
          }
        }
      }

      /* Retrieve data if requested */
      if(returned_data != NULL)
      {
        /* Retrieve data if command integrity is validated */
        if (diff == 0U)
        {
          /* Set index from command end */
          string_cmp_start_index += index;

          /* Check if command returned a data */
          if(receive_buffer[string_cmp_start_index] == PS1_shell_command_end_characters[0])
          {
            /* No data to be returned */

            /* Add end of string character */
            returned_data[0] = CHAR_END_OF_STRING;

            /* Set string length */
            *returned_data_length = 0;
          }
          else
          {
            /* Retrieve data */
            string_cmp_start_index++;
            for (index = 0; index < (sizeof(receive_buffer) - string_cmp_start_index); index++)
            {
              if(receive_buffer[string_cmp_start_index + index] == PS1_shell_command_end_characters[0])
              {
                break;
              }
              returned_data[index] = receive_buffer[string_cmp_start_index + index];
            }

            /* Add end of string character */
            returned_data[index] = CHAR_END_OF_STRING;

            /* Set string length */
            *returned_data_length = index;
          }
        }
        else
        {
          /* Command error: return error message */
          status = BSP_ERROR_COMPONENT_FAILURE;

          /* Parse for presence of error message */
          string_cmp_start_index = 0;
          for (index = 0; index < (sizeof(receive_buffer) - sizeof(PS1_shell_error_description) - 2U); index++)
          {
            if(receive_buffer[index] == PS1_shell_error_description[0])
            {
              if(receive_buffer[index + 1U] == PS1_shell_error_description[1])
              {
                if(receive_buffer[index + sizeof(PS1_shell_error_description) - 2U] == PS1_shell_error_description[sizeof(PS1_shell_error_description) - 2U])
                {
                  string_cmp_start_index = index + sizeof(PS1_shell_error_description) - 1U;
                  break;
                }
              }
            }
          }

          if(string_cmp_start_index != 0U)
          {
            /* case of error message to be returned */
            /* Set index after space character */
            string_cmp_start_index++;
            /* Retrieve error message */
            for (index = 0; index < (sizeof(receive_buffer) - (string_cmp_start_index + 1U)); index++)
            {
              if(receive_buffer[string_cmp_start_index + index] == PS1_shell_command_end_characters[0])
              {
                break;
              }
              returned_data[index] = receive_buffer[string_cmp_start_index + index];
            }
            *returned_data_length = index;
          }
        }
      }
      else
      {
        /* Command integrity is not validated */
        if (diff != 0U)
        {
          /* Command error (no error message to be returned since "returned_data" is null) */
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
    }
    else
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }

  return status;
}

/**
  * @brief  Convert ASCII to Unsigned Integer
  * @param  buf Pointer to string to convert
  * @retval Unsigned integer
  */
static uint32_t asciiToUint(uint8_t * buf) {
  uint32_t c, uintNumber = 0;

  for (c = 0; buf[c] != CHAR_END_OF_STRING; c++) {
    uintNumber = (uintNumber * 10U) + buf[c] - (uint32_t)'0';
  }

  return uintNumber;
}

/**
  * @brief  Convert Unsigned Integer to ASCII
  * @param  uintNumber Unsigned integer to convert
  * @param  buf Pointer to string to convert to
  * @retval Size of ASCII string
  */
static uint32_t uintToAscii(uint32_t uintNumber, uint8_t * buf){
  uint8_t const digit[] = "0123456789";
  uint8_t* p = buf;
  uint32_t size = 0, shifter = uintNumber, number = uintNumber;

  do{
    ++p;
    shifter = shifter/10U;
    size++;
  }while(shifter != 0x0U);
  *p = CHAR_END_OF_STRING;

  do{
    p--;
    *p = digit[(uint32_t)(number%10U)];
    number = number/10U;
  }while(number != 0x0U);

  return size;
}

/**
  * @brief  Send command to the Power Shield and retrieve feedback data
  * @note   If command returned no data, then "returned_data_length" is equal to 0.
  * @param cmd Command sent to PowerShield
  * @param cmd_length Command length sent to PowerShield
  * @param returned_data Data retrieved from PowerShield. Maximum length is PS_SHELL_FEEDBACK_LENGTH_MAX.
  * @param returned_data_length Data retrieved from PowerShield
  * @retval BSP status
  */
static int32_t SendCmdData(uint8_t *cmd, uint32_t cmd_length, uint8_t *returned_data, uint32_t *returned_data_length)
{
  uint32_t index;
  uint32_t buf_length;
  uint8_t buf[PS_CMD_LENGTH_MAX] = {0};
  int32_t status = BSP_ERROR_NONE;
  uint8_t specific_command_htc = 0; /* Specific actions for command "host take control" */

  /* Detect specific commands */
  index = (sizeof(PS1_CMD_htc) - 2U);
  while(cmd[index] == PS1_CMD_htc[index])
  {
    if(index > 0U)
    {
      index --;
    }
    else
    {
      specific_command_htc = 1;
      break;
    }
  }

  /* Format buffer */
  buf_length = cmd_length;
  /* Remove end of string character */
  if(cmd[cmd_length - 1U] == 0U)
  {
    buf_length--;
  }
  for (index = 0; index < buf_length; index++)
  {
    buf[index] = cmd[index];
  }
  for (index = 0; index < (sizeof(PS1_shell_command_end_characters) - 1U); index++)
  {
    buf[buf_length + index] = PS1_shell_command_end_characters[index];
  }
  buf_length += (sizeof(PS1_shell_command_end_characters) - 1U);

  /* Transmit buffer */
  if (HAL_UART_Transmit(&Idd_Ctx[0].UartHandle, buf, (uint16_t)buf_length, PS_TIMEOUT) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  /* Check feedback message */
  else if (waitForAckData(cmd, buf_length, returned_data, returned_data_length) != BSP_ERROR_NONE)
  {
    if(specific_command_htc != 0U)
    {
      /* Case of powershield not yet ready: wait and retry */
      HAL_Delay(PS_NOT_READY_RETRY_DELAY_MS);
      if (HAL_UART_Transmit(&Idd_Ctx[0].UartHandle, buf,  (uint16_t) buf_length, PS_TIMEOUT) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (waitForAckData(cmd, buf_length, returned_data, returned_data_length) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        /* Nothing to do */
      }
    }
    else
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
  }
  else
  {
    /* Nothing to do: status already BSP_ERROR_NONE */
  }

  return status;
}

/**
  * @brief  Send command to the Power Shield without any feedback data to retrieve
  * @param cmd Command sent to PowerShield
  * @param cmd_length Command length sent to PowerShield
  * @retval BSP status
  */
static int32_t SendCmdNoData(uint8_t *cmd, uint32_t cmd_length)
{
  return SendCmdData(cmd, cmd_length, NULL, NULL);
}


/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this BSP:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
static void UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_Init;

    PS1_TX_GPIO_CLK_ENABLE();
    PS1_RX_GPIO_CLK_ENABLE();
    PS1_CLK_ENABLE();

    /* Configure COM Tx as alternate function */
    GPIO_Init.Pin       = PS1_TX_PIN;
    GPIO_Init.Mode      = GPIO_MODE_AF_OD;
    GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init.Pull      = GPIO_PULLUP;
    GPIO_Init.Alternate = PS1_TX_AF;
    HAL_GPIO_Init(PS1_TX_GPIO_PORT, &GPIO_Init);

    /* Configure COM Rx as alternate function */
    GPIO_Init.Pin       = PS1_RX_PIN;
    GPIO_Init.Alternate = PS1_RX_AF;
    HAL_GPIO_Init(PS1_RX_GPIO_PORT, &GPIO_Init);
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this BSP:
  *          - Revert GPIOs to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
static void UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /* Revert GPIOs to their default state */
  HAL_GPIO_DeInit(PS1_RX_GPIO_PORT, PS1_RX_PIN);
  HAL_GPIO_DeInit(PS1_TX_GPIO_PORT, PS1_TX_PIN);
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

