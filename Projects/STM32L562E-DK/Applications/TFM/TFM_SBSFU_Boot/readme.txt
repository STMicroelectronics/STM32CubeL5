/**
  @page TFM_SBSFU_Boot

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

Security, SBSFU, TFM, mcuboot, mbedCrypto, AES, CRYP, FLASH, GTZC, HASH, PKA,
OSPI-HSPI, OTFD, ICACHE, Secure boot, Firmware update, Authentication,
Integrity, Confidentiality, cipher, Signature, ECDSA verification, RSA,
Option bytes, TrustZone

@par Directory contents

  - TFM_SBSFU_Boot/Inc/mcuboot_config/mcuboot_config.h Mcuboot configuration file
  - TFM_SBSFU_Boot/Inc/aes_alt.h                       Header file for aes_alt.c
  - TFM_SBSFU_Boot/Inc/boot_hal_cfg.h                  Platform configuration file for TFM_SBSFU_Boot
  - TFM_SBSFU_Boot/Inc/boot_hal_imagevalid.h           Header file for image validation code in boot_hal.c
  - TFM_SBSFU_Boot/Inc/boot_hal_otfdec.h               Header file for otfdec code in boot_hal.c
  - TFM_SBSFU_Boot/Inc/cmsis.h                         Header file for CMSIS
  - TFM_SBSFU_Boot/Inc/config-boot.h                   Mbed-crypto configuration file
  - TFM_SBSFU_Boot/Inc/ecp_alt.h                       Header file for ecp_alt.c
  - TFM_SBSFU_Boot/Inc/low_level_flash.h               Header file for low_level_flash.c
  - TFM_SBSFU_Boot/Inc/low_level_ospi_flash.h          Header file for low_level_ospi_flash.c
  - TFM_SBSFU_Boot/Inc/low_level_rng.h                 Header file for low_level_rng.c
  - TFM_SBSFU_Boot/Inc/mpu_armv8m_drv.h                Header file for mpu_armv8m_drv.c
  - TFM_SBSFU_Boot/Inc/mx25lm51245g_conf.h             Configuration file for ospi component mx25lm51245g
  - TFM_SBSFU_Boot/Inc/rsa_alt.h                       Header file for rsa_alt.c
  - TFM_SBSFU_Boot/Inc/sha256_alt.h                    Header file for sha256_alt.c
  - TFM_SBSFU_Boot/Inc/stm32l5xx_hal_conf.h            HAL driver configuration file
  - TFM_SBSFU_Boot/Inc/stm32l562e_discovery_conf.h     BSP driver configuration file
  - TFM_SBSFU_Boot/Inc/target_cfg.h                    Header file for target start up
  - TFM_SBSFU_Boot/Inc/tfm_bl2_shared_data.h           Header file for tfm_bl2_shared_data.c
  - TFM_SBSFU_Boot/Src/tfm_low_level_security.h        Header file for tfm_low_level_security.c
  - TFM_SBSFU_Boot/Inc/tfm_peripherals_def.h           Peripheral definitions

  - TFM_SBSFU_Boot/Src/aes_alt.c                       AES HW crypto interface
  - TFM_SBSFU_Boot/Src/bl2_nv_services.c               Non Volatile services for TFM_SBSFU_Boot
  - TFM_SBSFU_Boot/Src/boot_hal.c                      Platform initialization
  - TFM_SBSFU_Boot/Src/ecdsa_alt.c                     ECDSA HW crypto interface
  - TFM_SBSFU_Boot/Src/ecp_alt.c                       ECP HW crypto interface
  - TFM_SBSFU_Boot/Src/ecp_curves_alt.c                ECP curves HW crypto interface
  - TFM_SBSFU_Boot/Src/image_macros_to_preprocess_bl2.c Images definitions to preprocess for bl2
  - TFM_SBSFU_Boot/Src/low_level_com.c                 Uart low level interface
  - TFM_SBSFU_Boot/Src/low_level_device.c              Flash Low level device configuration
  - TFM_SBSFU_Boot/Src/low_level_flash.c               Flash Low level interface
  - TFM_SBSFU_Boot/Src/low_level_ospi_device.c         Ospi Flash Low level device configuration
  - TFM_SBSFU_Boot/Src/low_level_ospi_flash.c          Ospi Flash Low level interface
  - TFM_SBSFU_Boot/Src/low_level_rng.c                 Random generator interface
  - TFM_SBSFU_Boot/Src/mpu_armv8m_drv.c                Mpu low level interface
  - TFM_SBSFU_Boot/Src/rsa_alt.c                       RSA HW crypto interface
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
