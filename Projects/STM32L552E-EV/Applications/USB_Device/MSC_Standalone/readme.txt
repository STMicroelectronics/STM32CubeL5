/**
  @page MSC_Standalone USB Device Mass Storage (MSC) application

  @verbatim
  ******************************************************************************
  * @file    USB_Device/MSC_Standalone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the USB MSC application.
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

This application shows how to use the USB device application based on the Mass Storage Class (MSC) on the STM32L5xx devices.

This is a typical application on how to use the STM32L5xx USB Device peripheral to communicate with a PC
Host using the Bulk Only Transfer (BOT) and Small Computer System Interface (SCSI) transparent commands,
while the microSD card is used as storage media. The STM32 MCU is enumerated as a MSC device using the
native PC Host MSC driver to which the STM32L552E-EV board is connected.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals,
initialize the Flash interface and the systick. The user is provided with the SystemClock_Config()
function to configure the system clock (SYSCLK). The Full Speed (FS) USB module uses
internally a 48-MHz clock, which is generated from an internal PLL.



When the application is started, the user has just to plug the USB cable into a PC host and the device
is automatically detected. A new removable drive appears in the system window and write/read/format
operations can be performed as with any other removable drive.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

For more details about the STM32Cube USB Device library, please refer to UM1734
"STM32Cube USB Device library".

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, USB_Device, USB, MSC, Full Speed, flash, microSD card

@par Directory contents

  - USB_Device/MSC_Standalone/Core/Src/main.c                    Main program
  - USB_Device/MSC_Standalone/Core/Src/system_stm32l5xx.c        stm32l5xx system clock configuration file
  - USB_Device/MSC_Standalone/Core/Src/stm32l5xx_it.c            Interrupt handlers
  - USB_Device/MSC_Standalone/Core/Src/stm32l5xx_hal_msp.c       HAL MSP Module
  - USB_Device/MSC_Standalone/USB_Device/App/usb_device.c        USB Device application code
  - USB_Device/MSC_Standalone/USB_Device/App/usb_desc.c          USB device descriptor
  - USB_Device/MSC_Standalone/USB_Device/App/usbd_storage_if.c   Internal flash memory management
  - USB_Device/MSC_Standalone/USB_Device/Target/usbd_conf.c      General low level driver configuration
  - USB_Device/MSC_Standalone/Core/Inc/main.h                    Main program header file
  - USB_Device/MSC_Standalone/Core/Inc/stm32l5xx_it.h            Interrupt handlers header file
  - USB_Device/MSC_Standalone/Core/Inc/stm32l5xx_hal_conf.h      HAL configuration file
  - USB_Device/MSC_Standalone/USB_Device/App/usb_device.h        USB Device application header file
  - USB_Device/MSC_Standalone/USB_Device/App/usbd_desc.h         USB device descriptor header file
  - USB_Device/MSC_Standalone/USB_Device/App/usbd_storage_if.h   Internal flash memory management header file
  - USB_Device/MSC_Standalone/USB_Device/Target/usbd_conf.h      USB device driver Configuration file


@par Hardware and Software environment

  - This application runs on STM32L5xx devices.

  - This application has been tested with STMicroelectronics STM32L552E-EV boards
    and can be easily tailored to any other supported device and development board.
  - STM32L552E-EV Set-up
    - Insert a microSD card into the STM32L552E-EV
  - Connect the STM32L552E-EV board CN1 to the PC through "TYPE-C" to "Standard A" cable

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
