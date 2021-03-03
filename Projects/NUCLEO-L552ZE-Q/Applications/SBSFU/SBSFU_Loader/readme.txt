/**
  @page SBSFU_Loader

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application generates the SBSFU loader binary
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application description

This project is used to generate the SBSFU_Loader loader image file.
The SBSFU_Loader application consists in two distinct projects: the SBSFU_Loader
Secure and SBSFU_Loader Non Secure projects.
It has to be compiled after all SBSFU other ptojects.

In order to ease the development process, prebuild and postbuild commands are
integrated in each toolchain project.
The prebuild command is in charge of preparing the scatter file according to common
flash layout description in linker folder.
The postbuild command is in charge of preparing the loader image.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, SBSFU, FLASH, TrustZone

@par Directory contents

  - SBSFU_Loader/Secure/Inc/low_level_flash.h               Header file for low_level_flash.c
  - SBSFU_Loader/Secure/Inc/main.h                          Header file for main.c
  - SBSFU_Loader/Secure/Inc/stm32l5xx_hal_conf.h            HAL driver configuration file

  - SBSFU_Loader/Secure/Src/low_level_device.c              Flash Low level device configuration
  - SBSFU_Loader/Secure/Src/low_level_flash.c               Flash Low level interface
  - SBSFU_Loader/Secure/Src/main.c                          Main application file
  - SBSFU_Loader/Secure/Src/secure_nsc.c                    Secure Non-secure callable functions
  - SBSFU_Loader/Secure/Src/system_stm32l5xx.c              System Init file
  - SBSFU_Loader/Secure/Src/tick.c                          Tick functionalities

  - SBSFU_Loader/Secure_nsclib/secure_nsc.h                 Secure Non-Secure Callable (NSC) module header file

  - SBSFU_Loader/NonSecure/Inc/com.h                        Header file for com.c
  - SBSFU_Loader/NonSecure/Inc/common.h                     Header file for common.c
  - SBSFU_Loader/NonSecure/Inc/fw_update_app.h              Header file for fw_update_app.c
  - SBSFU_Loader/NonSecure/Inc/low_level_flash.h            Header file for low_level_flash.c
  - SBSFU_Loader/NonSecure/Inc/main.h                       Header file for main.c
  - SBSFU_Loader/NonSecure/Inc/stm32l5xx_hal_conf.h         HAL driver configuration file
  - SBSFU_Loader/NonSecure/Inc/ymodem.h                     Header file for ymodem.c

  - SBSFU_Loader/NonSecure/Src/com.c                        Com functionalities
  - SBSFU_Loader/NonSecure/Src/common.c                     Common functionalities
  - SBSFU_Loader/NonSecure/Src/dummy_nsc.c                  Dummy function
  - SBSFU_Loader/NonSecure/Src/fw_update_app.c              Firmware update functionalities
  - SBSFU_Loader/NonSecure/Src/low_level_device.c           Flash Low level device configuration
  - SBSFU_Loader/NonSecure/Src/low_level_flash.c            Flash Low level interface
  - SBSFU_Loader/NonSecure/Src/main.c                       Main application file
  - SBSFU_Loader/NonSecure/Src/system_stm32l5xx.c           System Init file
  - SBSFU_Loader/NonSecure/Src/tick.c                       Tick functionalities
  - SBSFU_Loader/NonSecure/Src/ymodem.c                     Ymodem functionalities

@par How to use it ?

In order to build the SBSFU_Loader Secure and Non Secure projects, you must do the following:
 - Open your preferred toolchain
 - Rebuild the two projects

Then refer to SBSFU\readme.txt for SBSFU usage.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
