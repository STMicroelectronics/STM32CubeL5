t
  @verbatim
  ******************************************************************************
  *
  *         Portions COPYRIGHT 2016-2019 STMicroelectronics, All Rights Reserved
  *         Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
  *
  * @file    st_readme.txt 
  * @author  MCD Application Team
  * @brief   This file lists the main modification done by STMicroelectronics on
  *          trustedfirmware for integration with STM32Cube solution.
  ******************************************************************************
  *
  * original licensing conditions
  * as issued by SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  @endverbatim

### 08-July-2019 ###
========================
    use  TF-Mv1.0-RC1
	Modify mbed-crypto include to fix build issue
	Add windows executeable scripts in bl2/ext/mcuboot/scripts
	Remove gpio platform service not relevant for STM32 platform
	Add bl2_main function to allow STM32 specific main
	Add weak function to jump in install application for STM32 customization
	remove directory  platform/ext/target
### 03-September-2019 ###
==========================
    TFM Crypto: Modify mbed-Crypto build config to use hardware for Random Generation
	BL2 Shared Data: Area is move on top of SRAM in dedicated place(no overlay) 
	TFM core: Add function to get boot value from BL2 Shared Data
    BL2:Add a weak function call in bl2_main to set BL2 Shared Data specific to STM32 
	BL2 Shared Data: Create TLV minor definition for TLV MAJOR Core to support BL2 Shared Data
	specific to STM32
	TFM EAT : fix to pass non regression test
### 02-December-2019 ###
==========================
    use  TF-Mv1.0-RC2
	BL2: fix key for secure /non secure authentication
	BL2 / TFM core : modification for IAR build
	TFM core : add interface to set non secure mpu
