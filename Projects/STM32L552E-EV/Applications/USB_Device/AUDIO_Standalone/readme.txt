/**
  @page AUDIO_Standalone USB AUDIO application

  @verbatim
  ******************************************************************************
  * @file    USB_Device/AUDIO_Standalone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the USB AUDIO Standalone.
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

This application shows how to use the implementation of the audio streaming (Out: Speaker/Headset)
capability on the STM32L5xx devices.

It follows the "Universal Serial Bus Device Class Definition for Audio Devices Release 1.0 March 18,
1998" defined by the USB Implementers Forum for reprogramming an application through USB-FS-Device.
Following this specification, it is possible to manage only Full Speed USB mode (High Speed is not supported).
This class is natively supported by most Operating Systems: no need for specific driver setup.

This is a typical application on how to use the stm32l5xx USB Device peripheral and SAI peripheral to
stream audio data from USB Host to the audio codec implemented on the STM32L552E-EV board.

The device supports the following audio features:
  - Pulse Coded Modulation (PCM) format
  - sampling rate: 48KHz.
  - Bit resolution: 16
  - Number of channels: 2
  - No volume control
  - Mute/Unmute capability
  - Asynchronous Endpoints

In order to overcome the difference between USB clock domain and STM32 clock domain,
the Add-Remove mechanism is implemented at class driver level.
This is a basic solution that doesn't require external components. It is based
on adding/removing one sample at a periodic basis to speed-up or slowdown
the audio output process. This allows to resynchronize it with the input flow.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

For more details about the STM32Cube USB Device library, please refer to UM1734
"STM32Cube USB Device library".


It is possible to fine tune needed USB Device features by modifying defines values in USBD configuration
file “usbd_conf.h” available under the project includes directory, in a way to fit the application
requirements, such as:
 - USBD_AUDIO_FREQ, specifying the sampling rate conversion from original audio file sampling rate to the
   sampling rate supported by the device.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, USB_Device, USB, AUDIO, Audio Playback

@par Directory contents

  - USB_Device/AUDIO_Standalone/Core/Src/main.c                            Main program
  - USB_Device/AUDIO_Standalone/Core/Src/stm32l5xx_hal_msp.c               MSP Initialization and de-Initialization codes
  - USB_Device/AUDIO_Standalone/Core/Src/system_stm32l5xx.c                stm32l5xx system clock configuration file
  - USB_Device/AUDIO_Standalone/Core/Src/stm32l5xx_it.c                    Interrupt handlers
  - USB_Device/AUDIO_Standalone/USB_Device/Target/usbd_conf.c              General low level driver configuration
  - USB_Device/AUDIO_Standalone/USB_Device/App/usbd_desc.c                 USB device AUDIO descriptor
  - USB_Device/AUDIO_Standalone/USB_Device/App/usbd_device.c               USB Device
  - USB_Device/AUDIO_Standalone/USB_Device/App/usb_audio_if.c          USB Device Custom AUDIO interface
  - USB_Device/AUDIO_Standalone/Core/Inc/main.h                            Main program header file
  - USB_Device/AUDIO_Standalone/Core/Inc/stm32l5xx_it.h                    Interrupt handlers header file
  - USB_Device/AUDIO_Standalone/Core/Inc/stm32l5xx_hal_conf.h              HAL configuration file
  - USB_Device/AUDIO_Standalone/USB_Device/Target/usbd_conf.h              USB device driver Configuration file
  - USB_Device/AUDIO_Standalone/USB_Device/App/usbd_desc.h                 USB device AUDIO descriptor header file
  - USB_Device/AUDIO_Standalone/USB_Device/App/usbd_device.h               USB Device header
  - USB_Device/AUDIO_Standalone/USB_Device/App/usb_audio_if.h           USB Device Custom AUDIO interface header file


@@par Hardware and Software environment

  - This application runs on STM32L5xx devices.

  - This application has been tested with STMicroelectronics STM32L552E-EV board
    and can be easily tailored to any other supported device and development board.

  - STM32L552E-EV board Set-up
    -Connect the STM32L552E-EV board CN1 to the PC through "TYPE-C" to "Standard A" cable.


@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Open an audio player application (Windows Media Player) and play music on the PC host

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

