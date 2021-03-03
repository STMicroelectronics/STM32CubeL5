/**
  @page Information  STM32L562E-DK Information Demonstration Firmware

  @verbatim
  ******************************************************************************
  * @file    Information/readme.txt
  * @author  MCD Application Team
  * @brief   Description of STM32L562E-DK Demonstration Information
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
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
architecture. The STM32Cube demonstration platform is built around TouchGFX graphical
interface. It is based on the STM32Cube HAL, BSP and several middleware components.

@par Demonstration Overview

The demonstration starts by displaying a splash screen animation with the ST logo and goes
into the launcher screen where the user can select between one of the sub-demonstrations.

Within the launcher, the user selects which sub-demonstration to run by sweeping on the
screen from left to right or right to left and pressing on the selected sub-demonstration
logo icon.

The sections below describe the features of each sub-demonstration.

 + Measurements
 --------------
 This module allows to perform consumption measurement based on the Energy meter
 solution present on the MB1373 board. To do this the user is invited to switch SW1 to
 PM_MEAS mode.

 Following low power modes with RTC enabled with LSE and VDD at 3.3V can be measured :
 - RUN with internal DC/DC converter (SMPS)
    - at 24 Mhz with internal SMPS in low power mode (voltage scaling 2)
 - SLEEP mode at 24 Mhz
 - LPRUN mode at 2 Mhz
 - LPSLEEP mode at 2Mhz
 - STOP2 mode
 - STANDBY mode
 Exit is done by tapping on the BACK icon.


 + AI
 --------------
 This module consists of an Handwritten Character Recognition (HCR) Neural Network (NN)
 demonstration (AI Keras model).
 User is invited to draw any alpha numerical character on the touch panel: as soon as the
 end of typing is detected, the HCR NN is executed. If the HCR NN succeeds in identifying
 the character, the output of the HCR NN is displayed in the center of the screen.
 Some specific strings enable to display a logo on the screen:
 - STAI
 - MAIL
 - PLAY
 - CALL
 - CHAT
 A press on the USER push-button will enter the space character.
 Exit is done by tapping on the BACK icon.


 + BLE
 --------------
 This module provides an example of Motion sensors usage. User is invited to launch
 'ST BLE Sensor' application on his smartphone (available on Android & iOS play store).
 LED10 green toggles until the BLE connection to MB1373C is set up.
 Once the connection is done, user can scroll the various 'ST BLE Sensor' displays to get
 the corresponding motion experience thanks to the STM32L562E-DK embedded sensor and motion
 algorithms using the 3D accelerometer & the 3D gyroscope LSM6DSO.
 Exit is done by tapping on the BACK icon.


 + Audio Player
 --------------
 This module provides a complete audio playback solution and delivers a high-quality
 music experience. It plays music in WAV format: by default from a .wav file located in
 the internal Flash memory or from .wav files read from a SD card (improved display user
 experience in that case).
 Exit is done by tapping on the BACK icon.


 + Information
 --------------
 This module displays the following information:
  the MCU name
  the board name
  the MCU core and speed
  the demonstration firmware version
 Exit is done by tapping on the BACK icon.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Demonstration, TouchGFX, Information, MCU name, MCU core, MCU speef, board name, version

@par Hardware and Software environment

  - This demonstration runs on STM32L562QEIQx devices.

  - This application has been tested with STM32L562E-DK board (MB1373C)
    can be easily tailored to any other supported device and development board.

@par How to use it ?

 1 - Open IAR Workbench (EWARM V8.40.2 minimum)

 2 - Build individually all demonstrations modules
     - Open workspace and select MenuLauncher\EWARM\MenuLauncher.eww
       Build project (F7)
       Close workspace

     - Open workspace and select Modules\Measurements\EWARM\Measurements.eww
       Build project (F7)
       Close workspace

     - Open workspace and select Modules\AI\EWARM\AI.eww
       Build project (F7)
       Close workspace

     - Open workspace and select Modules\BLE\EWARM\BLE.eww
       Build project (F7)
       Close workspace

     - Open workspace and select Modules\AudioPlayer\EWARM\AudioPlayer.eww
       Build project (F7)
       Close workspace

     - Open workspace and select Modules\Information\EWARM\Information\EWARM.eww
       Build project (F7)
       Close workspace

 3 - Create the demonstration firmware image
     Make sure \Utilities\PC_Software\HexTools.zip is unzipped under \Utilities\PC_Software\HexTools
     Make sure you have installed STM32CubeProgrammer under the default proposed installation folder
     Go in Binary folder and proceed as follows
     - launch 'generate_OoB_hexfile.bat' to generate the single demonstration firmware image
     - connect the STM32L562E-DK board to PC with USB cable in the STLK USB connector (CN17)
     - program the demonstration firmware by launching 'program_hexfile.bat'

 4 - Optionally plug-in a SD card with WAV files

 5 - Run the demonstration

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
