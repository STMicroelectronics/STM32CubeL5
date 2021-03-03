/**
  @page Crypto_Selftest Selftest Application

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    mbedTLS/Crypto_Selftest/readme.txt
  * @author  MCD Application Team
  * @brief   This application runs crypto through mbed TLS selftest functions.
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

@par Application Description

This application implements on STM32L562E-DK board a set of cryptographic features
through mbed TLS selftest functions of individual mbed TLS components selectively
chosen in a single configuration file "mbedtls_config.h".

This application contains in particular proprietary implementations (xxx_alt.c
alternate files) with adaptation layers (HAL API) which activates the
crypto engines (processors or accelerators available in the HW) from mbed TLS.
Following IPs are executed :
- RNG : True Random Number Generator
- CRYP : Cryptographic processor
- HASH : Hash processor

Tests result will be displayed on LCD.

A detailed log is also displayed on the hyperTerminal to inform user about all
cryptographic algorithms which are executed according to the mbed TLS configuration.
UART must be configured with the required settings (to be done only once):
   - When the board is connected to a PC,
     open a serial terminal emulator, find the board's COM port and configure it with:
      - 8N1, 9600 bauds, no HW flow control
      - set the line endings to LF or CR-LF (Transmit) and LF (receive).

Executed tests may be passed, failed or skipped.
"Skipped" means test is not executed since there are restrictions in Hw
(all of them remain compatible with standardizations and certifications).

Currently, there are 8 test suites by default covering the following features :
MD5, SHA1, SHA256 (including SHA224),
AES (including ECB, CBC, CFB, OFB, CTR, XTS mode of operations),
CCM,
GCM,
ENTROPY, TIMING

In case of success, the green led (LED10) is toggling at the end of execution.
in case of error, the red led (LED9) continuously blinks.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in HAL time base ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the HAL time base interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.


@par mbed TLS Library Configuration

Mbed TLS can be built with a larger variety of features so that more test suites can be run.
User can customize the set of features and trigger more tests by enabling more features in the
configuration file "mbedtls_config.h" available under the project includes directory.


@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, mbedTLS, cryptography, crypto processor,
symmetric algorithms, AES, modes of operation, ECB, CBC, CFB, OFB, CTR, XTS,
hash algorithms, sha1, sha256, Message Digest, MD5,
True Random number generator, RNG, CRYP, HASH,


@par Directory contents

Inc
  - mbedTLS/Crypto_Selftest/Inc/main.h                        Main program header file
  - mbedTLS/Crypto_Selftest/Inc/mbedtls.h                     mbed TLS selftest crypto thread header file
  - mbedTLS/Crypto_Selftest/Inc/mbedtls_config.h              mbedTLS library configuration options
  - mbedTLS/Crypto_Selftest/Inc/FreeRTOSConfig.h              FreeRTOS configuration options
  - mbedTLS/Crypto_Selftest/Inc/stm32l5xx_hal_conf.h          HAL configuration file
  - mbedTLS/Crypto_Selftest/Inc/stm32l5xx_it.h                Interrupt handlers header file
  - mbedTLS/Crypto_Selftest/Inc/stm32l562e_discovery_conf.h   stm32l562e_discovery configuration file
  - mbedTLS/Crypto_Selftest/Inc/aes_alt.h                     AES HW implementation header file
  - mbedTLS/Crypto_Selftest/Inc/ccm_alt.h                     CCM HW implementation header file
  - mbedTLS/Crypto_Selftest/Inc/gcm_alt.h                     GCM HW implementation header file
  - mbedTLS/Crypto_Selftest/Inc/cryp_stm32.h                  CRYP HW common header file
  - mbedTLS/Crypto_Selftest/Inc/sha1_alt.h                    SHA1 HW implementation header file
  - mbedTLS/Crypto_Selftest/Inc/sha256_alt.h                  SHA224/256 HW implementation header file
  - mbedTLS/Crypto_Selftest/Inc/md5_alt.h                     MD5 HW implementation header file
  - mbedTLS/Crypto_Selftest/Inc/hash_stm32.h                  HASH HW common header file
  - mbedTLS/Crypto_Selftest/Inc/timing_alt.h                  timing HW implementation header file

Src
  - mbedTLS/Crypto_Selftest/Src/main.c                         Main program
  - mbedTLS/Crypto_Selftest/Src/mbedtls.c                      mbed TLS selftest crypto thread
  - mbedTLS/Crypto_Selftest/Src/app_FreeRTOS.c                 Code for freertos applications
  - mbedTLS/Crypto_Selftest/Src/stm32l5xx_hal_timebase_tim.c   HAL time base functions
  - mbedTLS/Crypto_Selftest/Src/stm32l5xx_it.c                 Interrupt handlers
  - mbedTLS/Crypto_Selftest/Src/stm32l5xx_hal_msp.c            HAL MSP module
  - mbedTLS/Crypto_Selftest/Src/system_stm32l5xx.c             STM32L5xx system source file
  - mbedTLS/Crypto_Selftest/Src/aes_alt.c                      AES HW implementation file
  - mbedTLS/Crypto_Selftest/Src/ccm_alt.c                      CCM HW implementation file
  - mbedTLS/Crypto_Selftest/Src/gcm_alt.c                      GCM HW implementation file
  - mbedTLS/Crypto_Selftest/Src/cryp_stm32.c                   CRYP HW common file
  - mbedTLS/Crypto_Selftest/Src/sha1_alt.c                     SHA1 HW implementation file
  - mbedTLS/Crypto_Selftest/Src/sha256_alt.c                   SHA224/256 HW implementation file
  - mbedTLS/Crypto_Selftest/Src/md5_alt.c                      MD5 HW implementation file
  - mbedTLS/Crypto_Selftest/Src/hash_stm32.c                   HASH HW common file
  - mbedTLS/Crypto_Selftest/Src/timing_alt.c                   timing HW implementation file
  - mbedTLS/Crypto_Selftest/Src/hardware_rng.c                 RNG HW implementation file

@par Hardware and Software environment
  - This example runs on STM32L562QEIxQ (TZEN = 0) devices.

  - This example has been tested with STM32L562E-DK board and can be
    easily tailored to any other supported device and development board with security.

  - STM32L562E-DK Set-up:
    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Optionally, disable/enable others mbed TLS components
 - Rebuild all files and load your image into target memory

    - Connect the STM32L562E-DK board to the PC through 'USB micro B-Male
      to A-Male' cable to the connector:
      - CN17: to use ST-LINK

 - Run the example

 @par License ?
- Question:
This application is delivered under ST Ultimate Liberty (SLA0044) license
that is basically an ST proprietary license. However, several files
delivered in this component has an Apache 2.0 license header most often
with an ARM copyright declaration. Why? What license terms do actually apply
for those files and more generally for this application ?

- Answer:
This application is based on the Open Source mbed TLS reference implementation
that has been ported on STM32 microcontroller.

This mbed TLS reference is licensed under Apache 2.0 license.
Apache 2.0 license authorizes that “Derivative Works as a whole” using Apache 2.0
code to be delivered with another license provided compatibility is respected.
This is the case as what is delivered her under SLA0044 license.
Particularly, Apache 2.0 license requires that:
1.	We give a copy of this License: this is available in the
    APACHE-2.0.txt file at the root of the mbed TLS component; and
2.	We cause any modified files to carry prominent notices stating that we
    changed the files: these are the files modified by us with the addition of
	STMicroelectronics copyright to the original ARM Limited copyright; and
3.	We retain, in the Source form that we distribute, all copyright and
    attribution notices from the original Source form of the Work: this is the
	reason why ARM Limited original copyright declarations and license heading
	have been retained; and
4.	Any Derivative Works that we distribute must include a readable copy of the
    attribution notices contained within original work, in at least one of
	three possible places (within a NOTICE text file distributed as part of the
	Derivative Works, within the Source form or documentation or within a
	display generated by the Derivative Works): we deliver to you the readable
	copy of the attribution notices within the Source form.

As a conclusion, we confirm that this application is licensed under SLA0044
license terms and that they use parts of ARM copyrighted components originally
licensed under Apache 2.0 license. Respecting the original Apache 2.0 license
terms obliges us to retain ARM copyright declarations as well as Apache 2.0
file headers.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */