/**
  ******************************************************************************
  * @file    stm32l552e_eval.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage:
  *          - LEDs, push-buttons, potentiometer, joystick and COM ports available
  *            on STM32L552E-EV board (MB1372) from STMicroelectronics
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
#include "stm32l552e_eval.h"

#if (USE_BSP_COM_FEATURE == 1)
#if (USE_COM_LOG == 1)
#include <stdio.h>
#endif
#endif

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32L552E-EV STM32L552E-EV
  * @{
  */

/** @defgroup STM32L552E-EV_COMMON STM32L552E-EV COMMON
  * @brief This file provides set of firmware functions to manage Leds and push-buttons
  *        potentiometer and COM ports available on STM32L552E-EV board from STMicroelectronics.
  * @{
  */

/** @defgroup STM32L552E-EV_COMMON_Private_Defines STM32L552E-EV COMMON Private Defines
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
#if (USE_COM_LOG == 1)
#define COM_POLL_TIMEOUT     1000
#endif
#endif

#if (USE_BSP_POT_FEATURE == 1)
#define POT_ADC_POLL_TIMEOUT 1000
#endif
/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Private_Macros STM32L552E-EV COMMON Private Macros
  * @{
  */
#if (USE_BSP_POT_FEATURE == 1)
#define POT_CONVERT2PERC(x) (((int32_t)(x) * 100) / 0xFFF)
#endif
/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Private_TypesDefinitions STM32L552E-EV COMMON Private Types Definitions
  * @{
  */
typedef void (* BSP_EXTI_LineCallback)(void);
/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Private_FunctionPrototypes STM32L552E-EV COMMON Private Function Prototypes
  * @{
  */
static void BUTTON_WAKEUP_EXTI_Callback(void);
static void BUTTON_TAMPER_EXTI_Callback(void);
#if (USE_BSP_IO_CLASS == 1)
static void JOY1_EXTI_Callback(void);
#endif

#if (USE_BSP_COM_FEATURE == 1)
static void UART_MspInit(UART_HandleTypeDef *huart);
static void UART_MspDeInit(UART_HandleTypeDef *huart);
#endif

#if (USE_BSP_POT_FEATURE == 1)
static void ADC_MspInit(ADC_HandleTypeDef *hadc);
static void ADC_MspDeInit(ADC_HandleTypeDef *hadc);
#endif
/**
  * @}
  */

/** @addtogroup STM32L552E-EV_COMMON_Exported_Variables
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
UART_HandleTypeDef hcom_uart[COMn];
#endif

#if (USE_BSP_POT_FEATURE == 1)
ADC_HandleTypeDef hpot_adc[POTn];
#endif
/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Private_Variables STM32L552E-EV COMMON Private Variables
  * @{
  */
#if (USE_BSP_IO_CLASS == 1)
static uint16_t      LED_PIN[LEDn]  = {LED4_PIN, LED5_PIN, LED6_PIN, LED7_PIN};
static GPIO_TypeDef *LED_PORT[LEDn] = {LED4_GPIO_PORT, LED5_GPIO_PORT, 0, 0};
#else
static uint16_t      LED_PIN[LEDn]  = {LED4_PIN, LED5_PIN};
static GPIO_TypeDef *LED_PORT[LEDn] = {LED4_GPIO_PORT, LED5_GPIO_PORT};
#endif

static uint16_t           BUTTON_PIN[BUTTONn]  = {BUTTON_WAKEUP_PIN, BUTTON_TAMPER_PIN};
static GPIO_TypeDef      *BUTTON_PORT[BUTTONn] = {BUTTON_WAKEUP_GPIO_PORT, BUTTON_TAMPER_GPIO_PORT};
static IRQn_Type          BUTTON_IRQn[BUTTONn] = {BUTTON_WAKEUP_EXTI_IRQn, BUTTON_TAMPER_EXTI_IRQn};
static EXTI_HandleTypeDef hpb_exti[BUTTONn];

#if (USE_BSP_COM_FEATURE == 1)
static USART_TypeDef     *COM_UART[COMn] = {COM1_UART, COM2_UART};
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t           IsComMspCbValid[COMn] = {0U, 0U};
#endif
#if (USE_COM_LOG == 1)
static COM_TypeDef        COM_ActiveLogPort = COM1;
#endif
#endif /* (USE_BSP_COM_FEATURE == 1) */

#if (USE_BSP_POT_FEATURE == 1)
static ADC_TypeDef      *POT_ADC[POTn] = {POT1_ADC};
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
static uint32_t          IsPotMspCbValid[POTn] = {0U};
#endif
#endif /* (USE_BSP_POT_FEATURE == 1) */

#if (USE_BSP_IO_CLASS == 1)
static uint32_t           CurrentJoyPins[JOYn]  = {0};
static uint32_t           JOY_SEL_PIN[JOYn]     = {JOY1_SEL_PIN};
static uint32_t           JOY_DOWN_PIN[JOYn]    = {JOY1_DOWN_PIN};
static uint32_t           JOY_LEFT_PIN[JOYn]    = {JOY1_LEFT_PIN};
static uint32_t           JOY_RIGHT_PIN[JOYn]   = {JOY1_RIGHT_PIN};
static uint32_t           JOY_UP_PIN[JOYn]      = {JOY1_UP_PIN};
static uint32_t           JOY_ALL_PIN[JOYn]     = {JOY1_ALL_PIN};
static EXTI_HandleTypeDef hjoy_exti[JOYn];
#endif
/**
  * @}
  */

/** @addtogroup STM32L552E-EV_COMMON_Exported_Functions
  * @{
  */

/**
  * @brief  This method returns the STM32L552E EVAL BSP Driver revision
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
int32_t BSP_GetVersion(void)
{
  return ((int32_t)STM32L552E_EVAL_BSP_VERSION);
}

/**
  * @brief  This method returns the board name
  * @retval pointer to the board name string
  */
const uint8_t* BSP_GetBoardName(void)
{
  return (const uint8_t*)STM32L552E_EVAL_BSP_BOARD_NAME;
}

/**
  * @brief  This method returns the board ID
  * @retval pointer to the board ID string
  */
const uint8_t* BSP_GetBoardID(void)
{
  return (const uint8_t*)STM32L552E_EVAL_BSP_BOARD_ID;
}

/** @addtogroup STM32L552E-EV_COMMON_LED_Functions
  * @{
  */

/**
  * @brief  Configure LED GPIO.
  * @param  Led Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg  LED4
  *     @arg  LED5
  *     @arg  LED6 only if IO expander is used
  *     @arg  LED7 only if IO expander is used
  * @retval BSP error code
  */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  int32_t          status = BSP_ERROR_NONE;
  GPIO_InitTypeDef GPIO_Init;
#if (USE_BSP_IO_CLASS == 1)
  BSP_IO_Init_t    IO_Init;
#endif

  if ((Led == LED4) || (Led == LED5))
  {
    /* Enable the GPIO_LED Clock */
    if (Led == LED4)
    {
      LED4_GPIO_CLK_ENABLE();
    }
    else /* Led = LED5 */
    {
      LED5_GPIO_CLK_ENABLE();
    }

    /* configure the GPIO_LED pin */
    GPIO_Init.Pin   = LED_PIN[Led];
    GPIO_Init.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull  = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(LED_PORT[Led], &GPIO_Init);

    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
  }
#if (USE_BSP_IO_CLASS == 1)
  else /* Led = LED6 or Led = LED7 */
  {
    /* Initialize the BSP IO driver and configure the Led pin */
    IO_Init.Pin  = LED_PIN[Led];
    IO_Init.Mode = IO_MODE_OUTPUT_PP;
    IO_Init.Pull = IO_PULLUP;
    status = BSP_IO_Init(0, &IO_Init);
    if (status == BSP_ERROR_NONE)
    {
      status = BSP_IO_WritePin(0, LED_PIN[Led], IO_PIN_SET);
    }
  }
#endif /* USE_BSP_IO_CLASS */

  return status;
}

/**
  * @brief  DeInitialize LEDs.
  * @param  Led LED to be de-init.
  *   This parameter can be one of the following values:
  *     @arg  LED4
  *     @arg  LED5
  *     @arg  LED6 only if IO expander is used
  *     @arg  LED7 only if IO expander is used
  * @note BSP_LED_DeInit() does not disable the GPIO clock
  * @retval BSP error code
  */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
  int32_t          status = BSP_ERROR_NONE;
  GPIO_InitTypeDef GPIO_Init;
#if (USE_BSP_IO_CLASS == 1)
  BSP_IO_Init_t    IO_Init;
#endif

  if ((Led == LED4) || (Led == LED5))
  {
    /* DeInit the GPIO_LED pin */
    GPIO_Init.Pin = LED_PIN[Led];

    /* Turn off LED */
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
    HAL_GPIO_DeInit(LED_PORT[Led], GPIO_Init.Pin);
  }
#if (USE_BSP_IO_CLASS == 1)
  else /* Led = LED6 or Led = LED7 */
  {
    /* Just turn off the LED and reset IO pin */
    status = BSP_IO_WritePin(0, LED_PIN[Led], IO_PIN_SET);
    if (status == BSP_ERROR_NONE)
    {
      IO_Init.Pin  = LED_PIN[Led];
      IO_Init.Mode = IO_MODE_OFF;
      IO_Init.Pull = IO_NOPULL;
      status = BSP_IO_Init(0, &IO_Init);
    }
  }
#endif /* USE_BSP_IO_CLASS */

  return status;
}

/**
  * @brief  Turn selected LED On.
  * @param  Led Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg  LED4
  *     @arg  LED5
  *     @arg  LED6 only if IO expander is used
  *     @arg  LED7 only if IO expander is used
  * @retval BSP error code
  */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Led == LED4) || (Led == LED5))
  {
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
  }
#if (USE_BSP_IO_CLASS == 1)
  else
  {
    status = BSP_IO_WritePin(0, LED_PIN[Led], IO_PIN_RESET);
  }
#endif /* USE_BSP_IO_CLASS */

  return status;
}

/**
  * @brief  Turn selected LED Off.
  * @param  Led Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg  LED4
  *     @arg  LED5
  *     @arg  LED6 only if IO expander is used
  *     @arg  LED7 only if IO expander is used
  * @retval BSP error code
  */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Led == LED4) || (Led == LED5))
  {
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
  }
#if (USE_BSP_IO_CLASS == 1)
  else
  {
    status = BSP_IO_WritePin(0, LED_PIN[Led], IO_PIN_SET);
  }
#endif /* USE_BSP_IO_CLASS */

  return status;
}

/**
  * @brief  Toggle the selected LED.
  * @param  Led Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg  LED4
  *     @arg  LED5
  *     @arg  LED6 only if IO expander is used
  *     @arg  LED7 only if IO expander is used
  * @retval BSP error code
  */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Led == LED4) || (Led == LED5))
  {
    HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
  }
#if (USE_BSP_IO_CLASS == 1)
  else
  {
    status = BSP_IO_TogglePin(0, LED_PIN[Led]);
  }
#endif /* USE_BSP_IO_CLASS */

  return status;
}

/**
  * @brief  Get the state of selected LED.
  * @param  Led Specifies the Led.
  *   This parameter can be one of following parameters:
  *     @arg  LED4
  *     @arg  LED5
  *     @arg  LED6 only if IO expander is used
  *     @arg  LED7 only if IO expander is used
  * @retval 0 means off, 1 means on, negative value means error.
  */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  int32_t status;
#if (USE_BSP_IO_CLASS == 1)
  int32_t tmp;
#endif

#if (USE_BSP_IO_CLASS == 1)
  if ((Led == LED4) || (Led == LED5))
  {
#endif
    status = (HAL_GPIO_ReadPin(LED_PORT[Led], LED_PIN[Led]) == GPIO_PIN_RESET) ? 1 : 0;
#if (USE_BSP_IO_CLASS == 1)
  }
  else
  {
    tmp = BSP_IO_ReadPin(0, LED_PIN[Led]);
    if (tmp < 0)
    {
      /* Error ocuured */
      status = tmp;
    }
    else
    {
      status = ((uint32_t) tmp == IO_PIN_RESET) ? 1 : 0;
    }
  }
#endif /* USE_BSP_IO_CLASS */

  return status;
}

/**
  * @}
  */

/** @addtogroup STM32L552E-EV_COMMON_BUTTON_Functions
  * @{
  */

/**
  * @brief  Configure Button GPIO and EXTI Line.
  * @param  Button Specifies the Button to be configured.
  *   This parameter should be:
  *     @arg BUTTON_WAKEUP
  *     @arg BUTTON_TAMPER
  * @param  ButtonMode Specifies Button mode.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability
  * @retval BSP error code
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  int32_t               status = BSP_ERROR_NONE;
  GPIO_InitTypeDef      GPIO_Init;
  uint32_t              BSP_BUTTON_IT_PRIO[BUTTONn] = {BSP_BUTTON_WAKEUP_IT_PRIORITY, BSP_BUTTON_TAMPER_IT_PRIORITY};
  uint32_t              BUTTON_EXTI_LINE[BUTTONn]   = {BUTTON_WAKEUP_EXTI_LINE, BUTTON_TAMPER_EXTI_LINE};
  BSP_EXTI_LineCallback ButtonCallback[BUTTONn]     = {BUTTON_WAKEUP_EXTI_Callback, BUTTON_TAMPER_EXTI_Callback};

  /* Enable the BUTTON clock */
  if (Button == BUTTON_WAKEUP)
  {
    BUTTON_WAKEUP_GPIO_CLK_ENABLE();
  }
  else /* BUTTON_TAMPER */
  {
    BUTTON_TAMPER_GPIO_CLK_ENABLE();
  }

  GPIO_Init.Pin   = BUTTON_PIN[Button];
  GPIO_Init.Pull  = GPIO_NOPULL;
  GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;

  if (ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    GPIO_Init.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_Init);
  }

  if (ButtonMode == BUTTON_MODE_EXTI)
  {
    /* Configure Button pin as input with External interrupt */
    GPIO_Init.Mode = GPIO_MODE_IT_RISING;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_Init);

    if (HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]) == HAL_OK)
    {
      if (HAL_EXTI_RegisterCallback(&hpb_exti[Button], HAL_EXTI_RISING_CB_ID, ButtonCallback[Button]) == HAL_OK)
      {
        /* Enable and set Button EXTI Interrupt to the lowest priority */
        HAL_NVIC_SetPriority(BUTTON_IRQn[Button], BSP_BUTTON_IT_PRIO[Button], 0x00);
        HAL_NVIC_EnableIRQ(BUTTON_IRQn[Button]);
      }
      else
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    else
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  DeInitialize Push Button.
  * @param  Button Button to be configured
  *   This parameter should be:
  *     @arg BUTTON_WAKEUP
  *     @arg BUTTON_TAMPER
  * @note BSP_PB_DeInit() does not disable the GPIO clock
  * @retval BSP error code
  */
int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef GPIO_Init;

  GPIO_Init.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ(BUTTON_IRQn[Button]);
  HAL_GPIO_DeInit(BUTTON_PORT[Button], GPIO_Init.Pin);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Return the selected Button state.
  * @param  Button Specifies the Button to be checked.
  *   This parameter should be:
  *     @arg BUTTON_WAKEUP
  *     @arg BUTTON_TAMPER
  * @retval 0 means released, 1 means pressed.
  */
int32_t BSP_PB_GetState(Button_TypeDef Button)
{
  int32_t status;

  status = (HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]) == GPIO_PIN_SET) ? 1 : 0;

  return status;
}

/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered. */
}

/**
  * @brief  BSP PB interrupt handler.
  * @param  Button Button where interrupt occurs.
  *   This parameter should be:
  *     @arg BUTTON_WAKEUP
  *     @arg BUTTON_TAMPER
  * @retval None.
  */
void BSP_PB_IRQHandler(Button_TypeDef Button)
{
  HAL_EXTI_IRQHandler(&hpb_exti[Button]);
}

/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @addtogroup STM32L552E-EV_COMMON_COM_Functions
  * @{
  */

/**
  * @brief  Configure COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1 or COM2
  * @param  COM_Init Pointer to a COM_InitTypeDef structure that contains the
  *                configuration information for the specified COM peripheral.
  * @retval BSP error code
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init)
{
  int32_t status = BSP_ERROR_NONE;

  if (COM_Init == NULL)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Initialize COM instance */
    hcom_uart[COM].Instance = COM_UART[COM];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    UART_MspInit(&hcom_uart[COM]);
#else
    if (IsComMspCbValid[COM] == 0U)
    {
      /* Register the MSP callbacks */
      if (BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    if (status == BSP_ERROR_NONE)
    {
      if (COM == COM1)
      {
        if (MX_USART3_Init(&hcom_uart[COM], COM_Init) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }
      else /* COM = COM2 */
      {
        if (MX_LPUART1_Init(&hcom_uart[COM], COM_Init) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  DeInitialize COM port.
  * @param  COM COM port to be de_init.
  *          This parameter can be COM1 or COM2
  * @retval BSP error code
  */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t status = BSP_ERROR_NONE;

  /* COM de-init */
  hcom_uart[COM].Instance = COM_UART[COM];
  if (HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
  else
  {
    UART_MspDeInit(&hcom_uart[COM]);
  }
#endif

  return status;
}

#if (USE_COM_LOG == 1)
/**
  * @brief  Select the active COM port.
  * @param  COM COM port to be activated.
  *          This parameter can be COM1 or COM2
  * @retval BSP error code
  */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if (COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

#if defined(__ARMCC_VERSION) || defined(__ICCARM__)
int fputc(int ch, __attribute__((unused))FILE *f)
#elif __GNUC__
int __io_putchar(int ch)
#endif
{
  (void) HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *) &ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#endif /* (USE_COM_LOG == 1) */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register default COM msp callbacks.
  * @param  COM COM port.
  *          This parameter can be COM1 or COM2
  * @retval BSP status.
  */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t status = BSP_ERROR_NONE;

  /* Register MspInit/MspDeInit callbacks */
  if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, UART_MspInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  } 
  else if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, UART_MspDeInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsComMspCbValid[COM] = 1U;
  }
  /* Return BSP status */
  return status;
}

/**
  * @brief  Register BSP COM msp callbacks.
  * @param  COM COM port.
  *          This parameter can be COM1 or COM2
  * @param  CallBacks Pointer to MspInit/MspDeInit callback functions.
  * @retval BSP status
  */
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *CallBacks)
{
  int32_t status = BSP_ERROR_NONE;

  if (CallBacks == NULL)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit callbacks */
    if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }
  /* Return BSP status */
  return status; 
}
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Initialize LPUART1.
  * @param  huart UART handle.
  * @param  MXInit UART initialization structure.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_LPUART1_Init(UART_HandleTypeDef* huart, MX_UART_InitTypeDef *MXInit)
{
  /* UART configuration */
  huart->Init.BaudRate       = MXInit->BaudRate;
  huart->Init.WordLength     = (uint32_t) MXInit->WordLength;
  huart->Init.StopBits       = (uint32_t) MXInit->StopBits;
  huart->Init.Parity         = (uint32_t) MXInit->Parity;
  huart->Init.Mode           = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl      = (uint32_t) MXInit->HwFlowCtl;
  huart->Init.OverSampling   = UART_OVERSAMPLING_8;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;

  return HAL_UART_Init(huart);
}

/**
  * @brief  Initialize USART3.
  * @param  huart UART handle.
  * @param  MXInit UART initialization structure.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_USART3_Init(UART_HandleTypeDef* huart, MX_UART_InitTypeDef *MXInit)
{
  /* UART configuration */
  huart->Init.BaudRate       = MXInit->BaudRate;
  huart->Init.WordLength     = (uint32_t) MXInit->WordLength;
  huart->Init.StopBits       = (uint32_t) MXInit->StopBits;
  huart->Init.Parity         = (uint32_t) MXInit->Parity;
  huart->Init.Mode           = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl      = (uint32_t) MXInit->HwFlowCtl;
  huart->Init.OverSampling   = UART_OVERSAMPLING_8;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;

  return HAL_UART_Init(huart);
}

/**
  * @}
  */
#endif /* (USE_BSP_COM_FEATURE == 1) */

#if (USE_BSP_POT_FEATURE == 1)
/** @addtogroup STM32L552E-EV_COMMON_POT_Functions
  * @{
  */

/**
  * @brief  Configure Potentiometer.
  * @param  POT Potentiometer to be configured.
  *          This parameter can be POT1
  * @retval BSP error code
  */
int32_t BSP_POT_Init(POT_TypeDef POT)
{
  int32_t status = BSP_ERROR_NONE;

  /* Initialize POT instance */
  hpot_adc[POT].Instance = POT_ADC[POT];

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 0)
  ADC_MspInit(&hpot_adc[POT]);
#else
  if (IsPotMspCbValid[POT] == 0U)
  {
    /* Register the MSP callbacks */
    if (BSP_POT_RegisterDefaultMspCallbacks(POT) != BSP_ERROR_NONE)
    {
      status = BSP_ERROR_MSP_FAILURE;
    }
  }
#endif

  if (status == BSP_ERROR_NONE)
  {
    if (MX_ADC1_Init(&hpot_adc[POT]) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  DeInitialize Potentiometer.
  * @param  POT Potentiometer to be de_init.
  *          This parameter can be POT1
  * @retval BSP error code
  */
int32_t BSP_POT_DeInit(POT_TypeDef POT)
{
  int32_t status = BSP_ERROR_NONE;

  /* POT de-init */
  hpot_adc[POT].Instance = POT_ADC[POT];
  if (HAL_ADC_DeInit(&hpot_adc[POT]) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 0)
  else
  {
    ADC_MspDeInit(&hpot_adc[POT]);
  }
#endif

  return status;
}

/**
  * @brief  Get Potentiometer level.
  * @param  POT Potentiometer.
  *          This parameter can be POT1
  * @retval Potentiometer level(0..100%), negative value means error.
  */
int32_t BSP_POT_GetLevel(POT_TypeDef POT)
{
  int32_t  retval = BSP_ERROR_PERIPH_FAILURE;
  uint32_t AdcValue;

  if (HAL_ADC_Start(&hpot_adc[POT]) == HAL_OK)
  {
    if (HAL_ADC_PollForConversion(&hpot_adc[POT], POT_ADC_POLL_TIMEOUT) == HAL_OK)
    {
      AdcValue = HAL_ADC_GetValue(&hpot_adc[POT]);
      retval = POT_CONVERT2PERC(AdcValue);
    }
  }

  return retval;
}

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register default POT msp callbacks.
  * @param  POT Potentiometer.
  *          This parameter can be POT1
  * @retval BSP status.
  */
int32_t BSP_POT_RegisterDefaultMspCallbacks(POT_TypeDef POT)
{
  int32_t status = BSP_ERROR_NONE;

  /* Register MspInit/MspDeInit callbacks */
  if (HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPINIT_CB_ID, ADC_MspInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  } 
  else if (HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPDEINIT_CB_ID, ADC_MspDeInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsPotMspCbValid[POT] = 1U;
  }
  /* Return BSP status */
  return status;
}

/**
  * @brief  Register BSP POT msp callbacks.
  * @param  POT Potentiometer.
  *          This parameter can be POT1
  * @param  CallBacks Pointer to MspInit/MspDeInit callback functions.
  * @retval BSP status
  */
int32_t BSP_POT_RegisterMspCallbacks(POT_TypeDef POT, BSP_POT_Cb_t *CallBacks)
{
  int32_t status = BSP_ERROR_NONE;

  if (CallBacks == NULL)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit callbacks */
    if (HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsPotMspCbValid[POT] = 1U;
    }
  }
  /* Return BSP status */
  return status; 
}
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Initialize ADC1.
  * @param  hadc ADC handle.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_ADC1_Init(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef        status = HAL_ERROR;
  ADC_ChannelConfTypeDef   CHANNEL_Config;
  uint32_t                 POT_ADC_CHANNEL[POTn] = {POT1_ADC_CHANNEL};
  
  /* ADC configuration */
  hadc->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc->Init.Resolution            = ADC_RESOLUTION_12B;
  hadc->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc->Init.ScanConvMode          = ADC_SCAN_DISABLE;
  hadc->Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc->Init.LowPowerAutoWait      = DISABLE;
  hadc->Init.ContinuousConvMode    = DISABLE;
  hadc->Init.NbrOfConversion       = 1;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.NbrOfDiscConversion   = 1;
  hadc->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.DMAContinuousRequests = DISABLE;
  hadc->Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
  hadc->Init.OversamplingMode      = DISABLE;
  hadc->Init.DFSDMConfig           = ADC_DFSDM_MODE_DISABLE;
  if (HAL_ADC_Init(hadc) == HAL_OK)
  {
    /* ADC channel configuration */
    CHANNEL_Config.Channel      = POT_ADC_CHANNEL[POT1];
    CHANNEL_Config.Rank         = ADC_REGULAR_RANK_1;
    CHANNEL_Config.SamplingTime = ADC_SAMPLETIME_6CYCLES_5;
    CHANNEL_Config.SingleDiff   = ADC_SINGLE_ENDED;
    CHANNEL_Config.OffsetNumber = ADC_OFFSET_NONE;
    CHANNEL_Config.Offset       = 0;
    if (HAL_ADC_ConfigChannel(hadc, &CHANNEL_Config) == HAL_OK)
    {
      if (HAL_ADCEx_Calibration_Start(hadc, ADC_SINGLE_ENDED) == HAL_OK)
      {
        status = HAL_OK;
      }
    }
  }

  return status;
}

/**
  * @}
  */
#endif /* (USE_BSP_POT_FEATURE == 1) */

#if (USE_BSP_IO_CLASS == 1)
/** @addtogroup STM32L552E-EV_COMMON_JOY_Functions
  * @{
  */

/**
  * @brief  Configure Joystick.
  * @param  JOY Joystick to be configured.
  *   This parameter can be JOY1
  * @param  JoyMode Specifies joystick mode.
  *   This parameter can be one of following parameters:
  *     @arg JOY_MODE_GPIO: Joystick pin will be used as simple IO
  *     @arg JOY_MODE_EXTI: Joystick pin will be connected to EXTI line with
  *                         interrupt generation capability
  * @param  JoyPins Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  *     @arg JOY_ALL: all pins
  * @retval BSP error code
  */
int32_t BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins)
{
  int32_t               status = BSP_ERROR_NONE;
  uint32_t              JOY_EXTI_LINE[JOYn] = {JOY1_EXTI_LINE};
  BSP_EXTI_LineCallback JoyCallback[JOYn]   = {JOY1_EXTI_Callback};
  BSP_IO_Init_t         IO_Init;

  /* Initialize the BSP IO driver and configure the joystick pin */
  switch (JoyPins)
  {
    case JOY_SEL:
      IO_Init.Pin = JOY_SEL_PIN[JOY];
      CurrentJoyPins[JOY] |= JOY_SEL_PIN[JOY];
      break;
    case JOY_DOWN:
      IO_Init.Pin = JOY_DOWN_PIN[JOY];
      CurrentJoyPins[JOY] |= JOY_DOWN_PIN[JOY];
      break;
    case JOY_LEFT:
      IO_Init.Pin = JOY_LEFT_PIN[JOY];
      CurrentJoyPins[JOY] |= JOY_LEFT_PIN[JOY];
      break;
    case JOY_RIGHT:
      IO_Init.Pin = JOY_RIGHT_PIN[JOY];
      CurrentJoyPins[JOY] |= JOY_RIGHT_PIN[JOY];
      break;
    case JOY_UP:
      IO_Init.Pin = JOY_UP_PIN[JOY];
      CurrentJoyPins[JOY] |= JOY_UP_PIN[JOY];
      break;
    case JOY_ALL:
      IO_Init.Pin = JOY_ALL_PIN[JOY];
      CurrentJoyPins[JOY] |= JOY_ALL_PIN[JOY];
      break;
    default :
      status = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if (status == BSP_ERROR_NONE)
  {
    IO_Init.Mode = (JoyMode == JOY_MODE_GPIO) ? IO_MODE_INPUT : IO_MODE_IT_FALLING_EDGE;
    IO_Init.Pull = IO_PULLUP;
    status = BSP_IO_Init(0, &IO_Init);
    if ((status == BSP_ERROR_NONE) && (JoyMode == JOY_MODE_EXTI))
    {
      /* Interrupt already enabled on BSP_IO_Init, just register callback */
      if (HAL_EXTI_GetHandle(&hjoy_exti[JOY], JOY_EXTI_LINE[JOY]) == HAL_OK)
      {
        if (HAL_EXTI_RegisterCallback(&hjoy_exti[JOY], HAL_EXTI_RISING_CB_ID, JoyCallback[JOY]) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }
      else
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }

  }

  return status;
}

/**
  * @brief  De-initialize Joystick.
  * @param  JOY Joystick to be de-init.
  *   This parameter can be JOY1
  * @param  JoyPins Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  *     @arg JOY_ALL: all pins
  * @retval BSP error code
  */
int32_t BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins)
{
  int32_t       status = BSP_ERROR_NONE;
  BSP_IO_Init_t IO_Init;

  switch (JoyPins)
  {
    case JOY_SEL:
      IO_Init.Pin = JOY_SEL_PIN[JOY];
      CurrentJoyPins[JOY] &= ~(JOY_SEL_PIN[JOY]);
      break;
    case JOY_DOWN:
      IO_Init.Pin = JOY_DOWN_PIN[JOY];
      CurrentJoyPins[JOY] &= ~(JOY_DOWN_PIN[JOY]);
      break;
    case JOY_LEFT:
      IO_Init.Pin = JOY_LEFT_PIN[JOY];
      CurrentJoyPins[JOY] &= ~(JOY_LEFT_PIN[JOY]);
      break;
    case JOY_RIGHT:
      IO_Init.Pin = JOY_RIGHT_PIN[JOY];
      CurrentJoyPins[JOY] &= ~(JOY_RIGHT_PIN[JOY]);
      break;
    case JOY_UP:
      IO_Init.Pin = JOY_UP_PIN[JOY];
      CurrentJoyPins[JOY] &= ~(JOY_UP_PIN[JOY]);
      break;
    case JOY_ALL:
      IO_Init.Pin = JOY_ALL_PIN[JOY];
      CurrentJoyPins[JOY] &= ~(JOY_ALL_PIN[JOY]);
      break;
    default :
      status = BSP_ERROR_WRONG_PARAM;
      break;
  }
  if (status == BSP_ERROR_NONE)
  {
    IO_Init.Mode = IO_MODE_OFF;
    IO_Init.Pull = IO_NOPULL;
    status = BSP_IO_Init(0, &IO_Init);
  }

  return status;
}

/**
  * @brief  Get Joystick state.
  * @param  JOY Joystick.
  *   This parameter can be JOY1
  * @retval BSP error code if value negative or one of following value:
  *     @arg JOY_NONE
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  */
int32_t BSP_JOY_GetState(JOY_TypeDef JOY)
{
  int32_t retval = (int32_t)JOY_NONE;
  int32_t status;

  if ((CurrentJoyPins[JOY] & JOY_SEL_PIN[JOY]) == JOY_SEL_PIN[JOY])
  {
    /* Read status of JOY_SEL */
    status = BSP_IO_ReadPin(0, JOY_SEL_PIN[JOY]);
    if (status >= 0)
    {
      retval = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_SEL : (int32_t)JOY_NONE;
    }
    else
    {
      retval = status;
    }
  }
  if (((CurrentJoyPins[JOY] & JOY_UP_PIN[JOY]) == JOY_UP_PIN[JOY]) && (retval == (int32_t)JOY_NONE))
  {
    /* Read status of JOY_UP */
    status = BSP_IO_ReadPin(0, JOY_UP_PIN[JOY]);
    if (status >= 0)
    {
      retval = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_UP : (int32_t)JOY_NONE;
    }
    else
    {
      retval = status;
    }
  }
  if (((CurrentJoyPins[JOY] & JOY_DOWN_PIN[JOY]) == JOY_DOWN_PIN[JOY]) && (retval == (int32_t)JOY_NONE))
  {
    /* Read status of JOY_DOWN */
    status = BSP_IO_ReadPin(0, JOY_DOWN_PIN[JOY]);
    if (status >= 0)
    {
      retval = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_DOWN : (int32_t)JOY_NONE;
    }
    else
    {
      retval = status;
    }
  }
  if (((CurrentJoyPins[JOY] & JOY_LEFT_PIN[JOY]) == JOY_LEFT_PIN[JOY]) && (retval == (int32_t)JOY_NONE))
  {
    /* Read status of JOY_LEFT */
    status = BSP_IO_ReadPin(0, JOY_LEFT_PIN[JOY]);
    if (status >= 0)
    {
      retval = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_LEFT : (int32_t)JOY_NONE;
    }
    else
    {
      retval = status;
    }
  }
  if (((CurrentJoyPins[JOY] & JOY_RIGHT_PIN[JOY]) == JOY_RIGHT_PIN[JOY]) && (retval == (int32_t)JOY_NONE))
  {
    /* Read status of JOY_RIGHT */
    status = BSP_IO_ReadPin(0, JOY_RIGHT_PIN[JOY]);
    if (status >= 0)
    {
      retval = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_RIGHT : (int32_t)JOY_NONE;
    }
    else
    {
      retval = status;
    }
  }

  return retval;
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick to be de-init.
  *   This parameter can be JOY1
  * @param  JoyPin Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  * @retval None.
  */
__weak void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(JOY);
  UNUSED(JoyPin);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on JoyPin is triggered. */
}

/**
  * @brief  BSP JOY interrupt handler.
  * @param  JOY Joystick.
  *   This parameter can be JOY1
  * @param  JoyPin Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  * @retval None.
  */
void BSP_JOY_IRQHandler(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(JoyPin);

  HAL_EXTI_IRQHandler(&hjoy_exti[JOY]);
}

/**
  * @}
  */
#endif /* (USE_BSP_IO_CLASS == 1) */

/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Private_Functions STM32L552E-EV COMMON Private Functions
  * @{
  */

/**
  * @brief  Wakeup EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_WAKEUP_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_WAKEUP);
}

/**
  * @brief  Tamper EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_TAMPER_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_TAMPER);
}

#if (USE_BSP_IO_CLASS == 1)
/**
  * @brief  Joystick1 EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_EXTI_Callback(void)
{
  int32_t ItStatus;

  if ((CurrentJoyPins[JOY1] & JOY_SEL_PIN[JOY1]) == JOY_SEL_PIN[JOY1])
  {
    /* Read IT status of JOY_SEL */
    ItStatus = BSP_IO_GetIT(0, JOY_SEL_PIN[JOY1]);
    if (ItStatus == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_SEL);
      /* Clear IT status of JOY_SEL */
      ItStatus = BSP_IO_ClearIT(0, JOY_SEL_PIN[JOY1]);
      if (ItStatus < 0)
      {
        /* Nothing to do */
      }
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_UP_PIN[JOY1]) == JOY_UP_PIN[JOY1])
  {
    /* Read IT status of JOY_UP */
    ItStatus = BSP_IO_GetIT(0, JOY_UP_PIN[JOY1]);
    if (ItStatus == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_UP);
      /* Clear IT status of JOY_UP */
      ItStatus = BSP_IO_ClearIT(0, JOY_UP_PIN[JOY1]);
      if (ItStatus < 0)
      {
        /* Nothing to do */
      }
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_DOWN_PIN[JOY1]) == JOY_DOWN_PIN[JOY1])
  {
    /* Read IT status of JOY_DOWN */
    ItStatus = BSP_IO_GetIT(0, JOY_DOWN_PIN[JOY1]);
    if (ItStatus == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_DOWN);
      /* Clear IT status of JOY_DOWN */
      ItStatus = BSP_IO_ClearIT(0, JOY_DOWN_PIN[JOY1]);
      if (ItStatus < 0)
      {
        /* Nothing to do */
      }
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_LEFT_PIN[JOY1]) == JOY_LEFT_PIN[JOY1])
  {
    /* Read IT status of JOY_LEFT */
    ItStatus = BSP_IO_GetIT(0, JOY_LEFT_PIN[JOY1]);
    if (ItStatus == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_LEFT);
      /* Clear IT status of JOY_LEFT */
      ItStatus = BSP_IO_ClearIT(0, JOY_LEFT_PIN[JOY1]);
      if (ItStatus < 0)
      {
        /* Nothing to do */
      }
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_RIGHT_PIN[JOY1]) == JOY_RIGHT_PIN[JOY1])
  {
    /* Read IT status of JOY_RIGHT */
    ItStatus = BSP_IO_GetIT(0, JOY_RIGHT_PIN[JOY1]);
    if (ItStatus == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_RIGHT);
      /* Clear IT status of JOY_RIGHT */
      ItStatus = BSP_IO_ClearIT(0, JOY_RIGHT_PIN[JOY1]);
      if (ItStatus < 0)
      {
        /* Nothing to do */
      }
    }
  }
}
#endif

#if (USE_BSP_COM_FEATURE == 1)
/**
  * @brief  Initialize UART MSP.
  * @param  huart UART handle.
  * @retval None.
  */
static void UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_Init;
  GPIO_TypeDef    *COM_TX_PORT[COMn] = {COM1_TX_GPIO_PORT, COM2_TX_GPIO_PORT};
  GPIO_TypeDef    *COM_RX_PORT[COMn] = {COM1_RX_GPIO_PORT, COM2_RX_GPIO_PORT};
  uint16_t         COM_TX_PIN[COMn]  = {COM1_TX_PIN, COM2_TX_PIN};
  uint16_t         COM_RX_PIN[COMn]  = {COM1_RX_PIN, COM2_RX_PIN};
  uint8_t          COM_TX_AF[COMn]   = {COM1_TX_AF, COM2_TX_AF};
  uint8_t          COM_RX_AF[COMn]   = {COM1_RX_AF, COM2_RX_AF};
  COM_TypeDef      COM;

  /* Get COM according instance */
  COM = (huart->Instance == COM1_UART) ? COM1 : COM2;

  /* Enable COM and GPIO clocks */
  if (COM == COM1)
  {
    COM1_TX_GPIO_CLK_ENABLE();
    COM1_RX_GPIO_CLK_ENABLE();
    COM1_CLK_ENABLE();
  }
  else /* COM = COM2 */
  {
    /* Enable VddIO2 for GPIOG */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();
    COM2_TX_GPIO_CLK_ENABLE();
    COM2_RX_GPIO_CLK_ENABLE();
    COM2_CLK_ENABLE();
  }

  /* Configure COM Tx as alternate function */
  GPIO_Init.Pin       = COM_TX_PIN[COM];
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init.Pull      = GPIO_PULLUP;
  GPIO_Init.Alternate = COM_TX_AF[COM];
  HAL_GPIO_Init(COM_TX_PORT[COM], &GPIO_Init);

  /* Configure COM Rx as alternate function */
  GPIO_Init.Pin       = COM_RX_PIN[COM];
  GPIO_Init.Alternate = COM_RX_AF[COM];
  HAL_GPIO_Init(COM_RX_PORT[COM], &GPIO_Init);
}

/**
  * @brief  Deinitialize UART MSP.
  * @param  huart UART handle.
  * @retval None.
  */
static void UART_MspDeInit(UART_HandleTypeDef *huart)
{
  GPIO_TypeDef    *COM_TX_PORT[COMn] = {COM1_TX_GPIO_PORT, COM2_TX_GPIO_PORT};
  GPIO_TypeDef    *COM_RX_PORT[COMn] = {COM1_RX_GPIO_PORT, COM2_RX_GPIO_PORT};
  uint16_t         COM_TX_PIN[COMn]  = {COM1_TX_PIN, COM2_TX_PIN};
  uint16_t         COM_RX_PIN[COMn]  = {COM1_RX_PIN, COM2_RX_PIN};
  COM_TypeDef      COM;

  /* Get COM according instance */
  COM = (huart->Instance == COM1_UART) ? COM1 : COM2;

  /* Disable GPIOs and COM clock */
  HAL_GPIO_DeInit(COM_TX_PORT[COM], COM_TX_PIN[COM]);
  HAL_GPIO_DeInit(COM_RX_PORT[COM], COM_RX_PIN[COM]);
  switch (COM)
  {
    case COM1:
      COM1_CLK_DISABLE();
      break;
    case COM2:
      COM2_CLK_DISABLE();
      break;
    default:
      break;
  }
}
#endif /* (USE_BSP_COM_FEATURE == 1) */

#if (USE_BSP_POT_FEATURE == 1)
/**
  * @brief  Initialize ADC MSP.
  * @param  hadc ADC handle.
  * @retval None.
  */
static void ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef  GPIO_Init;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* Enable ADC and GPIO clocks */
  POT1_CHANNEL_GPIO_CLK_ENABLE();
  POT1_CLK_ENABLE();

  /* Configure ADC channel pin */
  GPIO_Init.Pin   = POT1_CHANNEL_GPIO_PIN;
  GPIO_Init.Mode  = GPIO_MODE_ANALOG;
  GPIO_Init.Pull  = GPIO_NOPULL;
  GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(POT1_CHANNEL_GPIO_PORT, &GPIO_Init);
}

/**
  * @brief  Deinitialize ADC MSP.
  * @param  hadc ADC handle.
  * @retval None.
  */
static void ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* Disable GPIO and POT clock */
  HAL_GPIO_DeInit(POT1_CHANNEL_GPIO_PORT, POT1_CHANNEL_GPIO_PIN);
  POT1_CLK_DISABLE();
}
#endif /* (USE_BSP_POT_FEATURE == 1) */

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
