/**
  @page SBSFU_Boot

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application generates the SBSFU secure boot and secure firmware
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

@par Application Licence Clarification

The application SBSFU_Boot is licenced by ST under Ultimate Liberty license SLA0044.
Read the file SBSFU_Boot/q_a.txt for Application Licence Clarification.

@par Application description

This project is used to generate the SBSFU_Boot binary file.
The core function of this application relies on the mcuboot middleware, the trusted
firmware (TFM) middleware and the mbed-crypto middleware.

It has to be compiled first.

In order to ease the development process, a prebuild command and postbuild command are
integrated in each toolchain project.
The prebuild command is in charge of preparing the scatter file according to common
flash layout description in linker folder.
The postbuild command is in charge of preparing the board programming scripts and
the images preparation scripts according to the flash layout and crypto scheme.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, SBSFU, mcuboot, mbedCrypto, AES, CRYP, FLASH, GTZC, HASH, PKA,
ICACHE, Secure boot, Firmware update, Authentication, Integrity, Confidentiality,
cipher, Signature, ECDSA verification, RSA, Option bytes, TrustZone

@par Directory contents

  - SBSFU_Boot/Inc/mcuboot_config/mcuboot_config.h Mcuboot configuration file
  - SBSFU_Boot/Inc/boot_hal_cfg.h                  Platform configuration file for SBSFU_Boot
  - SBSFU_Boot/Inc/boot_hal_imagevalid.h           Header file for image validation code in boot_hal.c
  - SBSFU_Boot/Inc/boot_hal_otfdec.h               Header file for otfdec code in boot_hal.c
  - SBSFU_Boot/Inc/cmsis.h                         Header file for CMSIS
  - SBSFU_Boot/Inc/config-boot.h                   Mbed-crypto configuration file
  - SBSFU_Boot/Inc/low_level_flash.h               Header file for low_level_flash.c
  - SBSFU_Boot/Inc/mpu_armv8m_drv.h                Header file for mpu_armv8m_drv.c
  - SBSFU_Boot/Inc/sha256_alt.h                    Header file for sha256_alt.c
  - SBSFU_Boot/Inc/stm32l5xx_hal_conf.h            HAL driver configuration file
  - SBSFU_Boot/Inc/target_cfg.h                    Header file for target start up
  - SBSFU_Boot/Src/tfm_low_level_security.h        Header file for tfm_low_level_security.c
  - SBSFU_Boot/Inc/tfm_peripherals_def.h           Peripheral definitions

  - SBSFU_Boot/Src/bl2_nv_services.c               Non Volatile services for SBSFU_Boot
  - SBSFU_Boot/Src/boot_hal.c                      Platform initialization
  - SBSFU_Boot/Src/image_macros_to_preprocess_bl2.c Images definitions to preprocess for bl2
  - SBSFU_Boot/Src/low_level_com.c                 Uart low level interface
  - SBSFU_Boot/Src/low_level_device.c              Flash Low level device configuration
  - SBSFU_Boot/Src/low_level_flash.c               Flash Low level interface
  - SBSFU_Boot/Src/mpu_armv8m_drv.c                Mpu low level interface
  - SBSFU_Boot/Src/sha256_alt.c                    SHA256 HW crypto interface
  - SBSFU_Boot/Src/system_stm32l5xx.c              System Init file
  - SBSFU_Boot/Src/tfm_low_level_security.c        Security Low level services

  - Linker/flash_layout.h                         Flash mapping
  - Linker/region_defs.h                          RAM and FLASH regions definitions

@par How to use it ?

In order to build the SBSFU_Boot project, you must do the following:
 - Open your preferred toolchain
 - Rebuild the project

Then refer to SBSFU\readme.txt for SBSFU usage.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
