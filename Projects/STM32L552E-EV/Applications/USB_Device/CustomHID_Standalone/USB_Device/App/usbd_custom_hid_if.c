/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    USB_Device/CustomHID_Standalone/USB_Device/App/usb_customhid_if.c
  * @author  MCD Application Team
  * @brief   USB Device Custom HID interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019  STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "main.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern ADC_HandleTypeDef hadc2;
uint32_t ADCConvertedValue = 0;
uint32_t ADC_Prev_ConvertedValue = 0;
extern USBD_HandleTypeDef hUsbDeviceFS;
extern TIM_HandleTypeDef htim1;
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
  0x06, 0xFF, 0x00,      /* USAGE_PAGE (Vendor Page: 0xFF00) */
  0x09, 0x01,            /* USAGE (Demo Kit)               */
  0xa1, 0x01,            /* COLLECTION (Application)       */
  /* 6 */

  /* LED4 */
  0x85, LED4_REPORT_ID,  /*     REPORT_ID (1)         */
  0x09, 0x01,            /*     USAGE ( LED4)               */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
  0x75, 0x08,            /*     REPORT_SIZE (8)            */
  0x95, LED4_REPORT_COUNT, /*     REPORT_COUNT (1)           */
  0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */

  0x85, LED4_REPORT_ID,  /*     REPORT_ID (1)              */
  0x09, 0x01,            /*     USAGE (LED4)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 26 */

  /* LED5 */
  0x85, LED5_REPORT_ID,  /*     REPORT_ID 2         */
  0x09, 0x02,            /*     USAGE (LED5)               */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
  0x75, 0x08,            /*     REPORT_SIZE (8)            */
  0x95, LED5_REPORT_COUNT, /*     REPORT_COUNT (1)           */
  0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */

  0x85, LED5_REPORT_ID,  /*     REPORT_ID (2)              */
  0x09, 0x02,            /*     USAGE (LED5)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 46 */

  /* LED6 */
  0x85, LED6_REPORT_ID,  /*     REPORT_ID (3)         */
  0x09, 0x03,            /*     USAGE (LED6)               */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
  0x75, 0x08,            /*     REPORT_SIZE (8)            */
  0x95, LED6_REPORT_COUNT, /*     REPORT_COUNT (1)           */
  0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */

  0x85, LED6_REPORT_ID,  /*     REPORT_ID (3)              */
  0x09, 0x03,            /*     USAGE (LED6)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 66 */

  /* LED7 */
  0x85, LED7_REPORT_ID,  /*     REPORT_ID 4)         */
  0x09, 0x04,            /*     USAGE (LED7)               */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
  0x75, 0x08,            /*     REPORT_SIZE (8)            */
  0x95, LED7_REPORT_COUNT, /*     REPORT_COUNT (1)           */
  0xB1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */

  0x85, LED7_REPORT_ID,  /*     REPORT_ID (4)              */
  0x09, 0x04,            /*     USAGE (LED7)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 86 */

  /* BUTTON_WAKEUP Button */
  0x85, BUTTON_WAKEUP_REPORT_ID,   /*     REPORT_ID (5)              */
  0x09, 0x05,            /*     USAGE (BUTTON_WAKEUP)        */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
  0x75, 0x01,            /*     REPORT_SIZE (1)            */
  0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */

  0x09, 0x05,            /*     USAGE (Push Button)        */
  0x75, 0x01,            /*     REPORT_SIZE (1)            */
  0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */

  0x75, 0x07,            /*     REPORT_SIZE (7)            */
  0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */
  0x85, BUTTON_WAKEUP_REPORT_ID,   /*     REPORT_ID (2)              */

  0x75, 0x07,            /*     REPORT_SIZE (7)            */
  0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */
  /* 114 */

  /* BUTTON_TAMPER Button */
  0x85, BUTTON_TAMPER_REPORT_ID,/*     REPORT_ID (6)              */
  0x09, 0x06,            /*     USAGE (BUTTON_TAMPER) */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
  0x75, 0x01,            /*     REPORT_SIZE (1)            */
  0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */

  0x09, 0x06,            /*     USAGE (Tamper Push Button) */
  0x75, 0x01,            /*     REPORT_SIZE (1)            */
  0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */

  0x75, 0x07,            /*     REPORT_SIZE (7)            */
  0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */
  0x85, BUTTON_TAMPER_REPORT_ID,/*     REPORT_ID (6)              */

  0x75, 0x07,            /*     REPORT_SIZE (7)            */
  0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */
  /* 142 */

  /* ADC IN */
  0x85, ADC_REPORT_ID,   /*     REPORT_ID                 */
  0x09, 0x07,            /*     USAGE (ADC IN)             */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
  0x26, 0xff, 0x00,      /*     LOGICAL_MAXIMUM (255)      */
  0x75, 0x08,            /*     REPORT_SIZE (8)            */
  0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */
  0x85, ADC_REPORT_ID,   /*     REPORT_ID (7)              */
  0x09, 0x07,            /*     USAGE (ADC in)             */
  0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */
  /* 161 */
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&ADCConvertedValue, 1);
  HAL_TIM_Base_Start(&htim1);
  /* Configure LED4 ,LED5 , LED6 and LED7 */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);
  BSP_LED_Init(LED7);

  /* Enable GPIOC clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_EXTI);
  BSP_PB_Init(BUTTON_TAMPER, BUTTON_MODE_EXTI);
  
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  /*
  Add your de-initialization code here
  */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
  switch(event_idx)
  {
  case 1: /* LED4 */
    (state == 1) ? BSP_LED_On(LED4) : BSP_LED_Off(LED4);
    break;

  case 2: /* LED5 */
    (state == 1) ? BSP_LED_On(LED5) : BSP_LED_Off(LED5);
    break;
  case 3: /* LED6 */
    (state == 1) ? BSP_LED_On(LED6) : BSP_LED_Off(LED6);
    break;
  case 4: /* LED7 */
    (state == 1) ? BSP_LED_On(LED7) : BSP_LED_Off(LED7);
    break;

  default:
    BSP_LED_Off(LED4);
    BSP_LED_Off(LED5);
    BSP_LED_Off(LED6);
    BSP_LED_Off(LED7);
    break;
  }
  
  /* Start next USB packet transfer once data processing is completed */
  USBD_CUSTOM_HID_ReceivePacket(&hUsbDeviceFS);
  
  return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
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

