/**
  ******************************************************************************
  * @file    stm32l552e_eval.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for:
  *          - LEDs, push-buttons, potentiometer, joystick  and COM ports available
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L552E_EVAL_H
#define STM32L552E_EVAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l552e_eval_conf.h"
#include "stm32l552e_eval_errno.h"
#if (USE_BSP_IO_CLASS == 1)
#include "stm32l552e_eval_io.h"
#endif

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L552E-EV
  * @{
  */

/** @addtogroup STM32L552E-EV_COMMON
  * @{
  */

/** @defgroup STM32L552E-EV_COMMON_Exported_Types STM32L552E-EV COMMON Exported Types
  * @{
  */
typedef enum
{
  LED4 = 0,
  LED_GREEN = LED4,
  LED5 = 1,
  LED_RED = LED5,
#if (USE_BSP_IO_CLASS == 1)
  LED6 = 2,
  LED_YELLOW = LED6,
  LED7 = 3,
  LED_ORANGE = LED7
#endif
} Led_TypeDef;

typedef enum
{
  BUTTON_WAKEUP = 0,
  BUTTON_TAMPER = 1
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

#if (USE_BSP_COM_FEATURE == 1)
typedef enum
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;

typedef enum
{
  COM_WORDLENGTH_7B = UART_WORDLENGTH_7B,
  COM_WORDLENGTH_8B = UART_WORDLENGTH_8B,
  COM_WORDLENGTH_9B = UART_WORDLENGTH_9B
} COM_WordLengthTypeDef;

typedef enum
{
  COM_STOPBITS_1 = UART_STOPBITS_1,
  COM_STOPBITS_2 = UART_STOPBITS_2
} COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE = UART_PARITY_NONE,
  COM_PARITY_EVEN = UART_PARITY_EVEN,
  COM_PARITY_ODD  = UART_PARITY_ODD
} COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE    = UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS     = UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS     = UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS = UART_HWCONTROL_RTS_CTS
} COM_HwFlowCtlTypeDef;

typedef struct
{
  uint32_t              BaudRate;
  COM_WordLengthTypeDef WordLength;
  COM_StopBitsTypeDef   StopBits;
  COM_ParityTypeDef     Parity;
  COM_HwFlowCtlTypeDef  HwFlowCtl;
} COM_InitTypeDef;

#define MX_UART_InitTypeDef COM_InitTypeDef
#endif /* (USE_BSP_COM_FEATURE == 1) */

#if (USE_BSP_POT_FEATURE == 1)
typedef enum
{
  POT1 = 0
} POT_TypeDef;
#endif

#if (USE_BSP_IO_CLASS == 1)
typedef enum
{
  JOY_NONE  = 0x00,
  JOY_SEL   = 0x01,
  JOY_DOWN  = 0x02,
  JOY_LEFT  = 0x04,
  JOY_RIGHT = 0x08,
  JOY_UP    = 0x10,
  JOY_ALL   = 0x1F
} JOYPin_TypeDef;

typedef enum
{
  JOY_MODE_GPIO = 0,
  JOY_MODE_EXTI = 1
} JOYMode_TypeDef;

typedef enum
{
  JOY1 = 0
} JOY_TypeDef;
#endif

#if (USE_BSP_COM_FEATURE == 1)
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  pUART_CallbackTypeDef  pMspInitCb;
  pUART_CallbackTypeDef  pMspDeInitCb;
} BSP_COM_Cb_t;
#endif
#endif

#if (USE_BSP_POT_FEATURE == 1)
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
typedef struct
{
  pADC_CallbackTypeDef  pMspInitCb;
  pADC_CallbackTypeDef  pMspDeInitCb;
} BSP_POT_Cb_t;
#endif
#endif

/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Exported_Constants STM32L552E-EV COMMON Exported Constants
  * @{
  */

/**
  * @brief STM32L552E EVAL BSP Driver version number
  */
#define STM32L552E_EVAL_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define STM32L552E_EVAL_BSP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define STM32L552E_EVAL_BSP_VERSION_SUB2   (0x03U) /*!< [15:8]  sub2 version */
#define STM32L552E_EVAL_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define STM32L552E_EVAL_BSP_VERSION       ((STM32L552E_EVAL_BSP_VERSION_MAIN << 24U)\
                                          |(STM32L552E_EVAL_BSP_VERSION_SUB1 << 16U)\
                                          |(STM32L552E_EVAL_BSP_VERSION_SUB2 << 8U )\
                                          |(STM32L552E_EVAL_BSP_VERSION_RC))

#define STM32L552E_EVAL_BSP_BOARD_NAME  "STM32L552E-EV"
#define STM32L552E_EVAL_BSP_BOARD_ID    "MB1372C"

/** @defgroup STM32L552E-EV_COMMON_LED STM32L552E-EV COMMON LED
  * @{
  */
#if (USE_BSP_IO_CLASS == 1)
#define LEDn                                    4U
#else
#define LEDn                                    2U
#endif

#define LED4_GPIO_PORT                          GPIOB
#define LED4_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED4_PIN                                GPIO_PIN_3

#define LED5_GPIO_PORT                          GPIOD
#define LED5_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED5_PIN                                GPIO_PIN_3

#if (USE_BSP_IO_CLASS == 1)
#define LED6_PIN                                IO_PIN_11

#define LED7_PIN                                IO_PIN_13
#endif
/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_BUTTON STM32L552E-EV COMMON BUTTON
  * @{
  */
#define BUTTONn                                 2U

#define BUTTON_WAKEUP_GPIO_PORT                 GPIOC
#define BUTTON_WAKEUP_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUTTON_WAKEUP_PIN                       GPIO_PIN_13
#define BUTTON_WAKEUP_EXTI_LINE                 EXTI_LINE_13
#define BUTTON_WAKEUP_EXTI_IRQn                 EXTI13_IRQn

#define BUTTON_TAMPER_GPIO_PORT                 GPIOA
#define BUTTON_TAMPER_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUTTON_TAMPER_PIN                       GPIO_PIN_1
#define BUTTON_TAMPER_EXTI_LINE                 EXTI_LINE_1
#define BUTTON_TAMPER_EXTI_IRQn                 EXTI1_IRQn
/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @defgroup STM32L552E-EV_COMMON_COM STM32L552E-EV COMMON COM
  * @{
  */
#define COMn                                    2U

#define COM1_UART                               USART3
#define COM1_CLK_ENABLE()                       __HAL_RCC_USART3_CLK_ENABLE()
#define COM1_CLK_DISABLE()                      __HAL_RCC_USART3_CLK_DISABLE()
#define COM1_TX_GPIO_PORT                       GPIOB
#define COM1_TX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define COM1_TX_PIN                             GPIO_PIN_10
#define COM1_TX_AF                              GPIO_AF7_USART3
#define COM1_RX_GPIO_PORT                       GPIOB
#define COM1_RX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define COM1_RX_PIN                             GPIO_PIN_11
#define COM1_RX_AF                              GPIO_AF7_USART3

#define COM2_UART                               LPUART1
#define COM2_CLK_ENABLE()                       __HAL_RCC_LPUART1_CLK_ENABLE()
#define COM2_CLK_DISABLE()                      __HAL_RCC_LPUART1_CLK_DISABLE()
#define COM2_TX_GPIO_PORT                       GPIOG
#define COM2_TX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOG_CLK_ENABLE()
#define COM2_TX_PIN                             GPIO_PIN_7
#define COM2_TX_AF                              GPIO_AF8_LPUART1
#define COM2_RX_GPIO_PORT                       GPIOG
#define COM2_RX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOG_CLK_ENABLE()
#define COM2_RX_PIN                             GPIO_PIN_8
#define COM2_RX_AF                              GPIO_AF8_LPUART1

/**
  * @}
  */
#endif /* (USE_BSP_COM_FEATURE == 1) */

#if (USE_BSP_POT_FEATURE == 1)
/** @defgroup STM32L552E-EV_COMMON_POT STM32L552E-EV COMMON POTENTIOMETER
  * @{
  */
#define POTn                                    1U

#define POT1_ADC                                ADC1
#define POT1_ADC_CHANNEL                        ADC_CHANNEL_5
#define POT1_CLK_ENABLE()                       __HAL_RCC_ADC_CLK_ENABLE()
#define POT1_CLK_DISABLE()                      __HAL_RCC_ADC_CLK_ENABLE()
#define POT1_CHANNEL_GPIO_PORT                  GPIOA
#define POT1_CHANNEL_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define POT1_CHANNEL_GPIO_PIN                   GPIO_PIN_0
/**
  * @}
  */
#endif /* (USE_BSP_POT_FEATURE == 1) */

#if (USE_BSP_IO_CLASS == 1)
/** @defgroup STM32L552E-EV_COMMON_JOY STM32L552E-EV COMMON JOYSTICK
  * @{
  */
#define JOYn                                    1U

#define JOY1_SEL_PIN                            IO_PIN_0
#define JOY1_DOWN_PIN                           IO_PIN_1
#define JOY1_LEFT_PIN                           IO_PIN_2
#define JOY1_RIGHT_PIN                          IO_PIN_3
#define JOY1_UP_PIN                             IO_PIN_4
#define JOY1_ALL_PIN                            (JOY1_SEL_PIN | JOY1_DOWN_PIN | JOY1_LEFT_PIN | JOY1_RIGHT_PIN | JOY1_UP_PIN)
#define JOY1_EXTI_LINE                          EXTI_LINE_0
#define JOY1_EXTI_IRQn                          EXTI0_IRQn
/**
  * @}
  */
#endif

/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Exported_Variables STM32L552E-EV COMMON Exported Variables
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
extern UART_HandleTypeDef hcom_uart[COMn];
#endif

#if (USE_BSP_POT_FEATURE == 1)
extern ADC_HandleTypeDef hpot_adc[POTn];
#endif
/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_Exported_Functions STM32L552E-EV COMMON Exported Functions
  * @{
  */
int32_t        BSP_GetVersion(void);
const uint8_t* BSP_GetBoardName(void);
const uint8_t* BSP_GetBoardID(void);

/** @defgroup STM32L552E-EV_COMMON_LED_Functions STM32L552E-EV COMMON LED Functions
  * @{
  */
int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);
/**
  * @}
  */

/** @defgroup STM32L552E-EV_COMMON_BUTTON_Functions STM32L552E-EV COMMON BUTTON Functions
  * @{
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t BSP_PB_DeInit(Button_TypeDef Button);
int32_t BSP_PB_GetState(Button_TypeDef Button);
void    BSP_PB_Callback(Button_TypeDef Button);
void    BSP_PB_IRQHandler(Button_TypeDef Button);
/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @defgroup STM32L552E-EV_COMMON_COM_Functions STM32L552E-EV COMMON COM Functions
  * @{
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t BSP_COM_DeInit(COM_TypeDef COM);

#if (USE_COM_LOG == 1)
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM);
#endif

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *CallBacks);
#endif

HAL_StatusTypeDef MX_LPUART1_Init(UART_HandleTypeDef* huart, MX_UART_InitTypeDef *MXInit);
HAL_StatusTypeDef MX_USART3_Init(UART_HandleTypeDef* huart, MX_UART_InitTypeDef *MXInit);
/**
  * @}
  */
#endif /* (USE_BSP_COM_FEATURE == 1) */

#if (USE_BSP_POT_FEATURE == 1)
/** @defgroup STM32L552E-EV_COMMON_POT_Functions STM32L552E-EV COMMON POTENTIOMETER Functions
  * @{
  */
int32_t BSP_POT_Init(POT_TypeDef POT);
int32_t BSP_POT_DeInit(POT_TypeDef POT);
int32_t BSP_POT_GetLevel(POT_TypeDef POT);

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
int32_t BSP_POT_RegisterDefaultMspCallbacks(POT_TypeDef POT);
int32_t BSP_POT_RegisterMspCallbacks(POT_TypeDef POT, BSP_POT_Cb_t *CallBacks);
#endif

HAL_StatusTypeDef MX_ADC1_Init(ADC_HandleTypeDef* hadc);
/**
  * @}
  */
#endif /* (USE_BSP_POT_FEATURE == 1) */

#if (USE_BSP_IO_CLASS == 1)
/** @defgroup STM32L552E-EV_COMMON_JOY_Functions STM32L552E-EV COMMON JOYSTICK Functions
  * @{
  */
int32_t BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins);
int32_t BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins);
int32_t BSP_JOY_GetState(JOY_TypeDef JOY);
void    BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);
void    BSP_JOY_IRQHandler(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);
/**
  * @}
  */
#endif

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

#endif /* STM32L552E_EVAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
