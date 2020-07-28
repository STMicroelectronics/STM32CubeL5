/**
  ******************************************************************************
  * @file    stm32_crc_lock.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    04-July-2018
  * @brief   Code for CRC Lock mechanism : restricted internal file
  ******************************************************************************
  ==============================================================================
                        ##### How to use this Module #####
  ==============================================================================
  This module provides mechanism allowing to protect binaries/libraries used by STM32 devices.
  The protection mechanism is based on the CRC peripheral and works as follow :
  - The binaries/ library must call the function CRC_Lock with the following parameters :
    -  crc_in  = 0xF407A5C2. This is the CRC input pattern
    -  crc_out = 0xB5E8B5CD. This is the CRC expected output pattern
    - The CRC lock function will then initialize the CRC peripheral with the input value crc_in (0xF407A5C2).
      and check the CRC output that must be the equal to the crc_out (0xB5E8B5CD) in case of an STM32 product
    - Return value :
    - 0 : lock failed , the CRC lock fails, the CRC output is not equal to the expected crc_out value (0xB5E8B5CD)
          i.e the given binary/library is running on other micro than STM32
    - 1 : lock success, the CRC lock succeed i.e the CRC output is equal to the expected crc_out value (0xB5E8B5CD)
          i.e the given binary/library is running on an STM32 device.

  - The application using the given binaries/ library must activate the CRC peripheral clock.

  Notes :
    - When the CRC_Lock function fails (returns 0) the binary/library must take the necessary actions
    to prevent code from running (goes to an infinite while (1), abort the initialization ...)

  - On the STM32H7x7, STM32H7x3 and STM32H7x5 lines the device ID is located @0x5C001000
      on other STM32 families device ID is located 0xE0042000
      on STM32H7x7,H7x3 and H7x5 lines, reading from 0xE0042000 returns zero
      on other STM32 families reading from 0x5C001000 may lead to hardfault
      Thus a read from 0xE0042000 is used to differentiate the H7 lines from other STM32 families
   (do not read from 0x5C001000)

  - The STM32H7x7, STM32H7x3 and STM32H7x5 devices comes with a cortex-M7 or M4(dual core lines):
    a Check on the Cortex-M7/M4 CPUID is added to early filter these Shark devices and use
    the legacy CRC address if not a Cortex-M7/M4 device.

  ==============================================================================
                        ##### Revision History #####
  ==============================================================================
  * @version V2.1.0
  * @date    04-July-2018
     - Add check on Cortex-M CPU ID to distinguish the case of the STM32H7(CM4 or CM7) from the other STM32 devices (with a different Cortex-M)

  * @version V2.0.0
  * @date    17-Sep-2018
     - Add support for shark devices : STM32H7x7,H7x3 and H7x5 and STM32H75xx
     - Add support for tiny shark devices : STM32H7B3x,STM32H7A3x
  ******************************************************************************
  * @attention
  * This is a RESTRICTED INTERNAL file to be used to protect ST binaries/libraries from running
  * on a non STM32 device.
  * This file MUST NOT BE PUBLIC
  *****************************************************************************/
    
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_CRC_LOCK_H
#define STM32_CRC_LOCK_H

#ifdef __cplusplus
 extern "C" {
#endif


uint32_t CRC_Lock(uint32_t crc_in, uint32_t crc_out);



#ifdef __cplusplus
}
#endif 

#endif // STM32_CRC_LOCK_H
