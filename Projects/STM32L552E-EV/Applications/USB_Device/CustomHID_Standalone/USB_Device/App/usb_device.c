/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    USB_Device/CustomHID_Standalone/USB_Device/App/usb_device.c
  * @author  MCD Application Team
  * @brief   This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
__IO uint32_t remotewakeupon = 0;
uint8_t HID_Buffer[4];
uint8_t BUTTON_WAKEUP_PB_State = 0, BUTTON_TAMPER_PB_State = 0;
extern PCD_HandleTypeDef hpcd_USB_FS;
extern USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops;
uint8_t SendBuffer[2];
#define CURSOR_STEP     5
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
extern void SystemClockConfig_Resume(void);
/* USER CODE END PFP */

extern void Error_Handler(void);
/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef CUSTOM_HID_Desc;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */
/**
  * @brief EXTI Rising detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == BUTTON_WAKEUP_PIN)
  {
    SendBuffer[0] = BUTTON_WAKEUP_REPORT_ID;

    if(BUTTON_WAKEUP_PB_State == 0)
    {
      SendBuffer[1] = 0x01;
      BUTTON_WAKEUP_PB_State = 1;
    }
    else
    {
      SendBuffer[1] = 0x00;
      BUTTON_WAKEUP_PB_State = 0;
    }
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, SendBuffer, 2);
  }

  if(GPIO_Pin == BUTTON_TAMPER_PIN)
  {
    SendBuffer[0] = BUTTON_TAMPER_REPORT_ID;

    if(BUTTON_TAMPER_PB_State == 0)
    {
      SendBuffer[1] = 0x01;
      BUTTON_TAMPER_PB_State = 1;
    }
    else
    {
      SendBuffer[1] = 0x00;
      BUTTON_TAMPER_PB_State = 0;
    }
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, SendBuffer, 2);
  }
}

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_Device_Init(void)
{
  /* USER CODE BEGIN USB_Device_Init_PreTreatment */

  /* USB Clock Initialization */
  /* USER CODE END USB_Device_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &CUSTOM_HID_Desc, DEVICE_FS) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CUSTOM_HID) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_CUSTOM_HID_RegisterInterface(&hUsbDeviceFS, &USBD_CustomHID_fops_FS) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Device_Init_PostTreatment */

  /* USER CODE END USB_Device_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
