/**
  @page TFM secure boot and secure firmware update

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application generates the TFM secure boot and secure firmware
  *          update binary
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

@par Application description

This project is used to generate the TFM_SBSFU_Boot binary file.
The core function of this application relies on the trusted firmware (TFM)
middleware and the mbedtls middleware (for cryptographic part).

It has to be compiled before TFM_Appli Secure and Non Secure projects.

In order to ease the developement process, a prebuild command is integrated
in each toolchain project. The prebuild command is in charge of preparing the
scatter file according to common flash layout description in linker folder.

@par Directory contents

  - TFM_SBSFU_Boot/Inc/cmsis.h                         Header file for CMSIS
  - TFM_SBSFU_Boot/Inc/config-boot.h                   Mbedtls configuration file
  - TFM_SBSFU_Boot/Inc/main.h                          Header file for main.c
  - TFM_SBSFU_Boot/Inc/mpu_armv8m_drv.h                Header file for mpu_armv8m_drv.c
  - TFM_SBSFU_Boot/Inc/tfm_low_level_security_rss.h    Header to use RSS service to activate HDP
  - TFM_SBSFU_Boot/Inc/sha256_alt.h                    Header file for sha256_alt.c
  - TFM_SBSFU_Boot/Inc/stm32l5xx_hal_conf.h            HAL configuration file
  - TFM_SBSFU_Boot/Inc/target_cfg.h                    Header file for target start up
  - TFM_SBSFU_Boot/Inc/tfm_bl2_shared_data.h           Header file for tfm_bl2_shared_data.c
  - TFM_SBSFU_Boot/Src/tfm_low_level_security.h        Header file for tfm_low_level_security.c
  - TFM_SBSFU_Boot/Inc/tfm_peripherals_def.h           Peripheral definitions

  - TFM_SBSFU_Boot/Src/bl2_nv_services                 Non Volatile services for TFM boot
  - TFM_SBSFU_Boot/Src/image_macros_to_preprocess_ns.c Images definitions to preprocess for non secure
  - TFM_SBSFU_Boot/Src/image_macros_to_preprocess_s.c  Images definitions to preprocess for secure
  - TFM_SBSFU_Boot/Src/jumper.c                        Jump function to TFM secure application
  - TFM_SBSFU_Boot/Src/low_level_com.c                 Uart low level interface
  - TFM_SBSFU_Boot/Src/low_level_flash.c               Flash Low level interface
  - TFM_SBSFU_Boot/Src/low_level_rng.c                 Random generator interface
  - TFM_SBSFU_Boot/Src/main.c                          Main application file
  - TFM_SBSFU_Boot/Src/mpu_armv8m_drv.c                Mpu low level interface
  - TFM_SBSFU_Boot/Src/sha256_alt.c                    SHA256 HW crypto interface
  - TFM_SBSFU_Boot/Src/stm32l5xx_hal_msp.c             HAL MSP module
  - TFM_SBSFU_Boot/Src/system_stm32l5xx.c              System Init file
  - TFM_SBSFU_Boot/Src/tfm_bl2_shared_data.c           BL2 shared data services
  - TFM_SBSFU_Boot/Src/tfm_low_level_security.c        Security Low level services

  - Linker/flash_layout.h                         Flash mapping
  - Linker/region_defs.h                          RAM and FLASH regions definitions

@par How to use it ?

In order to build the TFM_SBSFU_Boot project, you must do the following:
 - Open your preferred toolchain 
 - Rebuild the project

Then refer to TFM\readme.txt for TFM usage.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
