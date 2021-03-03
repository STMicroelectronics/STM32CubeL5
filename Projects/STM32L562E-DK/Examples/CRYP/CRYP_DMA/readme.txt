/**
  @page CRYP_DMA  Encrypt and Decrypt data using AES Algo in ECB chaining
   mode using DMA
  
  @verbatim
  ******************************************************************************
  * @file    CRYP/CRYP_DMA/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the CRYP AES Algorithm in ECB mode with DMA Example
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

How to use the CRYP peripheral to encrypt and decrypt data using the AES-128 
algorithm with ECB chaining mode in DMA mode.

DMA is used to transfer data from memory to the AES processor
input as well as to transfer data from AES processor output to memory.

64-byte buffers are ciphered and deciphered (4 AES blocks)
Ciphering/Deciphering with a 128-bit long key is used with data type set to 8-bit (byte swapping).

This example unrolls as follows: 
- AES Encryption (Plain Data --> Encrypted Data)
- AES Decryption (Encrypted Data --> Decrypted Data)
 
Some messages can be displayed on hyperterminal from STLink Virtual Com Port.
Please configure hyperterminal serial link as follows:
    - BaudRate = 115200 baud.
    - Data size = 8 Bits.
    - none parity.
    - Hardware flow control disabled.

When all ciphering and deciphering operations are successful, LED_GREEN is turned on.
In case of ciphering or deciphering issue, LED_RED is turned on. 

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, Cryptography, CRYPT, AES, ECB, DMA, cipher, UART

@par Directory contents 

  - CRYP/CRYP_DMA/Inc/stm32l5xx_hal_conf.h         HAL configuration file
  - CRYP/CRYP_DMA/Inc/stm32l562e_discovery_conf.h  Board configuration file
  - CRYP/CRYP_DMA/Inc/stm32l5xx_it.h               Interrupt handlers header file
  - CRYP/CRYP_DMA/Inc/main.h                       Header for main.c module  
  - CRYP/CRYP_DMA/Src/stm32l5xx_it.c               Interrupt handlers
  - CRYP/CRYP_DMA/Src/main.c                       Main program
  - CRYP/CRYP_DMA/Src/stm32l5xx_hal_msp.c          HAL MSP module
  - CRYP/CRYP_DMA/Src/system_stm32l5xx.c           STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L562QEIxQ devices.

  - This example has been tested with STM32L562E-DK board and can be easily 
    tailored to any other supported device and development board.
  
@par How to use it ? 

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
