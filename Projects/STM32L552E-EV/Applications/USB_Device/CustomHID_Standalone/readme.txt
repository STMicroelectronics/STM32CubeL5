/**
  @page CustomHID_Standalone USB Device Human Interface application

  @verbatim
  ******************************************************************************
  * @file    USB_Device/CustomHID_Standalone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the USB CustomHID application.
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application shows how to use the USB device application based on the Custom HID Class on the STM32L5xx devices.
  This is a typical application on how to use the stm32l5xx USB Device peripheral, where the STM32 MCU is
enumerated as a HID compliant device using the native PC Host HID driver to which the STM32L552E-EV
board is connected.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals,
initialize the Flash interface and the systick. The user is provided with the SystemClock_Config()
function to configure the system clock (SYSCLK). The Full Speed (FS) USB module uses
internally a 48-MHz clock which is coming from a specific output of two PLLs (PLL or PLL SAI) or from MSI.

To test the application, user must proceed as follows:
 - Start the "USB HID Demonstrator" PC applet, available for download from www.st.com,
   and connect STM32 USB to PC
 - The device should be detected and shown in the USB HID target box
 - Press Graphic View button
 - Select "use SET_FEATURE" or "use SET_REPORT" in order to use SET_REPORT request to send HID Report
   for LED control
 - Use the potentiometer of the STM32L552E-EV board to transfer the result of the converted voltage
  (via the ADC) the to the PC host (these values are sent to the PC using the endpoint1 IN). To use
  potentiometer, put JP12 in position 1-2 (POT) and JP13 in position 2-3 (OPAMP).
 - Make sure that following report ID are configured: LED4 ID (0x1), LED5 ID(0x2), LED6 ID(0x3),
   LED7 ID(0x4), BUTTON_WAKEUP(0x5), BUTTON_TAMPER(0x6) and Potentiometer_ID(0x7).      to have correct HAL operation.
For more details about the STM32Cube USB Device library, please refer to UM1734
"STM32Cube USB Device library".

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, USB_Device, USB, CustomHID

@par Directory contents

  - USB_Device/CustomHID_Standalone/Core/Src/main.c                      Main program
  - USB_Device/CustomHID_Standalone/Core/Src/system_stm32l5xx.c    STM32L5xx system clock configuration file
  - USB_Device/CustomHID_Standalone/Core/Src/stm32l5xx.c           Interrupt handlers
  - USB_Device/CustomHID_Standalone/Core/Src/stm32l5xx_hal_msp.c   HAL MSP module
  - USB_Device/CustomHID_Standalone/Core/Inc/main.h                      Main program header file
  - USB_Device/CustomHID_Standalone/Core/Inc/stm32l5xx__it.h       Interrupt handlers header file
  - USB_Device/CustomHID_Standalone/Core/Inc/stm32l5xx__hal_conf.h HAL configuration file
  - USB_Device/CustomHID_Standalone/USB_Device/Target/usbd_conf.c       General low level driver configuration
  - USB_Device/CustomHID_Standalone/USB_Device/App/usbd_desc.c           USB device descriptor
  - USB_Device/CustomHID_Standalone/USB_Device/App/usbd_custom_hid_if.c  USB Device Custom HID interface
  - USB_Device/CustomHID_Standalone/USB_Device/App/usbd_conf.h           USB device driver Configuration file
  - USB_Device/CustomHID_Standalone/USB_Device/App/usbd_desc.h           USB device descriptor header file
  - USB_Device/CustomHID_Standalone/USB_Device/App/usbd_custom_hid_if.h  Header for usb_customhid_if.c file


@par Hardware and Software environment

  - This application runs on STM32L5xx devices.

  - This application has been tested with STMicroelectronics STM32L552E-EV
    board and can be easily tailored to any other supported device
    and development board.

    -Connect the STM32L552E-EV board CN1 to the PC through "TYPE-C" to "Standard A" cable.

@note Make sure that :
  - jumper JP12 is on FS position (1-2)(POT)
  - jumper JP13 is on FS position (2-3) (OPAMP)

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
