/**
  @page OTFDEC_DataDecrypt example

  @verbatim
  ******************************************************************************
  * @file    OTFDEC_DataDecrypt/readme.txt
  * @author  MCD Application Team
  * @brief   This example describes how to decrypt data located on the OCTOSPI external flash.
  ******************************************************************************
  * @attention
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
How to decrypt data located on the OCTOSPI external flash using the OTFDEC peripheral.
Decrypted information is displayed on the debugger Terminal I/O.

This project is configured for STM32L562xx devices using STM32CubeL5 HAL and running on
STM32L562E-DK board from STMicroelectronics mounted with an STM32L562QE-Q chip.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
HCLK is configured at 110 MHz.

First, the example writes encrypted data in external NOR flash. These encrypted data will be read back in memory mapped mode.
Then, OTFDEC is configured and enabled; when ciphered data located on external NOR flash are accessed to be displayed on the
debugger Terminal I/O, the OTFDEC decrypts them on the fly leading to deciphered data display.
The deciphered text is compared with the expected one. If the result is the expected one, the green LED is turned on.
Otherwise, the red LED is turned on.

- When resorting to IAR Embedded Workbench IDE, decrypted data are displayed on
  debugger Terminal I/O as follows: View --> Terminal I/O.
- When resorting to MDK-ARM KEIL IDE, decrypted data are displayed on
  debugger as follows:  View --> Serial Viewer --> Debug (printf) Viewer.
- When resorting to STM32CubeIDE, decrypted data are displayed on
  debugger as follows: Window--> Show View--> Console.
  In Debug configuration : 
    1- Use ST-LINK (OpenOCD).
    2- Add the command "monitor arm semihosting enable" into startup Initialization Commands.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The example needs to ensure that the SysTick time base is always set to 1 millisecond
to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, OTFDEC, OCTOSPI, AES CTR, Memory-mapped, Deciphering

@par Directory contents

  - OTFDEC/OTFDEC_DataDecrypt/Src/main.c                 Main program
  - OTFDEC/OTFDEC_DataDecrypt/Src/stm32l5xx_it.c         Interrupt handlers
  - OTFDEC/OTFDEC_DataDecrypt/Src/stm32l5xx_hal_msp.c    HAL MSP module
  - OTFDEC/OTFDEC_DataDecrypt/Src/hal_octospi_utility.c  Octospi Utility
  - OTFDEC/OTFDEC_DataDecrypt/Inc/main.h                 Main program header file
  - OTFDEC/OTFDEC_DataDecrypt/Inc/hal_octospi_utility.h  Octospi Utility header file
  - OTFDEC/OTFDEC_DataDecrypt/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - OTFDEC/OTFDEC_DataDecrypt/Inc/stm32l5xx_it.h         Interrupt handlers header file
  - OTFDEC/OTFDEC_DataDecrypt/Inc/stm32l562e_discovery_conf.h BSP configuration file
  - OTFDEC/OTFDEC_DataDecrypt/Inc/hal_macronix.h         Macronix file


@par Hardware and Software environment

  - This example runs on STM32L562xx (TZEN = 0) devices.

  - This example has been tested with STMicroelectronics STM32L562E-DK board mounted with an STM32L562QE-Q chip
    and can be easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example



 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
