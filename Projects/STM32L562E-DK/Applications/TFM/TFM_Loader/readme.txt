/**
  @page TFM_Loader

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application generates the TFM loader binary
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

This project is used to generate the TFM_Loader loader binary file.
The TFM_Loader application consists in one single project: the TFM_Loader
Non Secure project.
It has to be compiled after all TFM other ptojects.

In order to ease the development process, prebuild and postbuild commands are
integrated in each toolchain project.
The prebuild command is in charge of preparing the scatter file according to common
flash layout description in linker folder.
The postbuild command is in charge of preparing the loader image.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, SBSFU, TFM, FLASH, OSPI-HSPI, TrustZone

@par Directory contents

  - TFM_Loader/Inc/com.h                           Header file for com.c
  - TFM_Loader/Inc/common.h                        Header file for common.c
  - TFM_Loader/Inc/fw_update_app.h                 Header file for fw_update_app.c
  - TFM_Loader/Inc/low_level_flash.h               Header file for low_level_flash.c
  - TFM_Loader/Inc/low_level_ospi_flash.h          Header file for low_level_ospi_flash.c
  - TFM_Loader/Inc/main.h                          Header file for main.c
  - TFM_Loader/Inc/mx25lm51245g_conf.h             Configuration file for ospi component mx25lm51245g
  - TFM_Loader/Inc/stm32l5xx_hal_conf.h            HAL driver configuration file
  - TFM_Loader/Inc/stm32l562e_discovery_conf.h     BSP driver configuration file
  - TFM_Loader/Inc/ymodem.h                        Header file for ymodem.c

  - TFM_Loader/Src/com.c                           Com functionalities
  - TFM_Loader/Src/common.c                        Common functionalities
  - TFM_Loader/Src/fw_update_app.c                 Firmware update functionalities
  - TFM_Loader/Src/low_level_device.c              Flash Low level device configuration
  - TFM_Loader/Src/low_level_flash.c               Flash Low level interface
  - TFM_Loader/Src/low_level_ospi_device.c         Ospi Flash Low level device configuration
  - TFM_Loader/Src/low_level_ospi_flash.c          Ospi Flash Low level interface
  - TFM_Loader/Src/main.c                          Main application file
  - TFM_Loader/Src/system_stm32l5xx.c              System Init file
  - TFM_Loader/Src/tick.c                          Tick functionalities
  - TFM_Loader/Src/ymodem.c                        Ymodem functionalities

@par How to use it ?

In order to build the TFM_Loader project, you must do the following:
 - Open your preferred toolchain
 - Rebuild the project

Then refer to TFM\readme.txt for TFM usage.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
