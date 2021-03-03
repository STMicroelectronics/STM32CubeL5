/**
  @page Demonstrations  STM32L552E-EV Demonstration Firmware

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @author  MCD Application Team
  * @brief   Description of STM32L552E-EV Demonstration
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       http://www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Demo Description

The STM32Cube demonstration platform comes on top of the STM32Cube as a
firmware package that offers a full set of software components based on a modular
architecture. All modules can be reused separately in standalone applications. All
these modules are managed by the STM32Cube demonstration kernel that allows to
dynamically add new modules and access common resources (storage, memory management,
real-time operating system). The STM32Cube demonstration platform is built around a
basic GUI interface. It is based on the STM32Cube HAL BSP and several middleware components.

@par Demonstration Overview

At startup once the STMicroelectronics logo display is completed, the supported demos
will be viewed per icon. Use the JOYSTICK to navigate through the icons and select the
demonstration modules.

 @note All icons are stored into external microSD card (CN25).

Insure the Binary\SD_card content is present at the root of the microSD card

Below you find an overview of the different modules present in the demonstration:

 + UCPD
 --------
 This module corresponds to USB port Type C SINK capability and monitors the USB Type-C
 cable connection in CN1 connector and displays the connection state.
 Use the TAMPER button to exit UCPD application

 + Low Power Modes
 --------------
 This module allows to perform consumption measurement based on the IDD circuitry present
 on the MB1372 board and displays the current measured by the IDD component.
 Following modes with RTC enabled with LSE and VDD at 3.3V can be measured :
 - RUN SMPS modes
    - at 110 Mhz with internal SMPS bypassed
    - at 110 Mhz with internal SMPS in high power mode (voltage scaling 0)
    - at 24 Mhz with internal SMPS in low power mode (voltage scaling 2)
 - SLEEP mode at 110 Mhz from PLL
 - STOP2 mode
 - STANDBY mode
 Wakeup is managed either by an EXTI linked to IDD circuitry or by a RTC Alarm event.
 @note JP8 (MFX_MEAS) must be set to 1-2 position for this purpose

 + Audio
 --------------
 Two audio solutions based on STM32L5 device are demonstrated here:
 - An audio player sub-module provides a complete audio playback solution and delivers a high-quality
 music experience. It plays music in WAV format from the .wav files from the USER folder of
 the connected SD card but it may be extended to support other compressed formats such as MP3 and WMA
 audio formats.
 - An audio recorder sub-module provides a complete audio record solution and delivers a high-quality
 record experience. It supports record in WAV format at 44.1khz sample rate in mono. The recorded file
 rec.wav is saved on the SD card (it is overwritten at each new record).

 + Files browser
 --------------
 The File browser module is a system module that allows to explore the USER folder of
 the connected SD card.

 + Images viewer
 --------------
 The Image viewer module allows to display BMP pictures read from the USER folder of the
 the connected SD card. Navigation across the images is done with Joystick RIGHT/LEFT keys.
 SEL key exists the module.

 + Calendar
 --------------
 The Calendar module allows to set the date and time and to configure an alarm.
 LED green (LD4) is ON when the alarm is activated and it toggles for a few seconds
 when the alarm expires.


@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority()
      function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Demonstration, Audio Player, Audio Recorder, Calendar, File Browser,  Measurements, UCPD, SD card

@par Hardware and Software environment

  - This demonstration runs on STM32L552ZETxQ devices.

  - This demonstration has been tested with STM32L552E-EV (MB1372-C02) evaluation board and
    can be easily tailored to any other supported device and development board.

  - Copy Binary\SD_card content at the root of the microSD card

  @note Jumpers setup on MB1372_L552ZEQ-C02:
   JP1  : OPENED        / JP4  : 1-2 (LEFT)    / JP5  : 1-2 (LEFT)    / JP6  : CLOSED
   JP7  : 1-2 (RIGHT)   / JP8  : 1-2 (LEFT)    / JP9  : 1-2 (DOWN)    / JP10 : CLOSED
   JP11 : 1-2 (LEFT)    / JP12 : 1-2 POT       / JP13 : 2-3 OPAMP     / JP14 : OPENED
   JP15 : 1-2 (VDD)     / JP16 : 5V-STLK       / JP17 : 1-2 (RIGHT)   / JP18 : OPENED
   JP19 : OPENED        / JP20 : OPENED        / JP21 : OPENED        / JP22 : OPENED
   JP23 : OPENED        / CN12 : OPENED        / CN23 : OPENED

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
   - Open STM32CubeProgrammer
   - Connect the STM32L552E-EV board to PC with USB cable through CN22 (USB_STLK)
   - Program the generated "STM32CubeL5_Demo_STM32L552E-EV.hex" file from the toolchain output folder
   - Run the demonstration

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

