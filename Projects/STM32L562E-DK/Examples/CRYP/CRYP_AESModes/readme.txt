/**
  @page CRYP_AESModes  Encrypt and Decrypt data using AES Algo in ECB/CBC/CTR
  chaining modes with all possible key sizes.
  
  @verbatim
  ******************************************************************************
  * @file    CRYP/CRYP_AESModes/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the CRYP AES Algorithm in all modes and all key sizes
             Example
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description 

How to use the CRYP peripheral to encrypt and decrypt data using AES in chaining
modes (ECB, CBC, CTR).

In this example, the following key sizes are used: 128 or 256.

Ciphering/Deciphering with a 128-bit long key is used in polling mode with data type set to 8-bit (byte swapping).
Ciphering/Deciphering with a 256-bit long key is used in interrupt mode with data type set to 32-bit (no swapping).
For ECB and CBC, key derivation is carried out at the same time as decryption.

With the 256-bit long key processing, the ciphering output is used as deciphering input. 

This example successively carries out

1. ECB mode:
   - AES128 ECB encryption
   - AES256 ECB encryption
   - AES128 ECB decryption and key derivation
   - AES256 ECB decryption and key derivation
2. CBC mode:
   - AES128 CBC encryption
   - AES256 CBC encryption
   - AES128 CBC decryption and key derivation
   - AES256 CBC decryption and key derivation
3. CTR mode:
   - AES128 CTR encryption
   - AES256 CTR encryption
   - AES128 CTR decryption
   - AES256 CTR decryption

Some messages can be displayed on hyperterminal from STLink Virtual Com Port.
Please configure hyperterminal serial link as follows:
    - BaudRate = 115200 baud.
    - Data size = 8 Bits.
    - none parity.
    - Hardware flow control disabled.

When all ciphering and deciphering operations are successful, LED_GREEN is turned on.
In case of ciphering or deciphering issue, LED_RED is turned on.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, Cryptography, CRYPT, AES, ECB, CBC, CTR, MAC, USART

@par Directory contents 

  - CRYP/CRYP_AESModes/Inc/stm32l5xx_hal_conf.h         HAL configuration file
  - CRYP/CRYP_AESModes/Inc/stm32l562e_discovery_conf.h  Board configuration file
  - CRYP/CRYP_AESModes/Inc/stm32l5xx_it.h               Interrupt handlers header file
  - CRYP/CRYP_AESModes/Inc/main.h                       Header for main.c module  
  - CRYP/CRYP_AESModes/Src/stm32l5xx_it.c               Interrupt handlers
  - CRYP/CRYP_AESModes/Src/main.c                       Main program
  - CRYP/CRYP_AESModes/Src/stm32l5xx_hal_msp.c          HAL MSP module
  - CRYP/CRYP_AESModes/Src/system_stm32l5xx.c           STM32L5xx system source file

@par Hardware and Software environment

@par How to use it ? 

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
