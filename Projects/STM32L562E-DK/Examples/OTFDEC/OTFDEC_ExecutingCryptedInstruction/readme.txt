/**
  @page OTFDEC_ExecutingCryptedInstruction example

  @verbatim
  ******************************************************************************
  * @file    OTFDEC_ExecutingCryptedInstruction/readme.txt
  * @author  MCD Application Team
  * @brief   This example describes how to execute encrypted instructions located
  *          on the OCTOSPI external flash using the OTFDEC peripheral.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description
How to execute ciphered instructions stored in external NOR flash using the OTFDEC peripheral.

This project is configured for STM32L562xx devices using STM32CubeL5 HAL and running on
STM32L562E-DK board from STMicroelectronics mounted with an STM32L562QE-Q chip.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
HCLK is configured at 110 MHz.

First, the example writes encrypted instructions in external NOR flash through OCTOSPI.
These encrypted instructions will be seen like "00" in memory mapped mode (instruction only mode).
Then, OTFDEC decrypts these instructions on the fly when they are executed.
The result of executing these instructions (a+b+15) is displayed on the debugger Terminal I/O.
If the result is the expected one, the green LED is turned on. Otherwise, the red LED is turned on.

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

Security, OTFDEC, OCTOSPI, Memory-mapped, Deciphering, Execute-In-Place, XiP

@par Directory contents

  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Src/main.c                 Main program
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Src/stm32l5xx_it.c         Interrupt handlers
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Src/stm32l5xx_hal_msp.c    HAL MSP module
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Src/hal_octospi_utility.c  Octospi Utility
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Inc/main.h                 Main program header file
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Inc/hal_octospi_utility.h  Octospi Utility header file
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Inc/stm32l5xx_it.h         Interrupt handlers header file
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Inc/stm32l562e_discovery_conf.h BSP configuration file
  - OTFDEC/OTFDEC_ExecutingCryptedInstruction/Inc/hal_macronix.h         Macronix file

@par Hardware and Software environment

  - This example runs on STM32L562QEIxQ devices without security enabled (TZEN=0).

  - This example has been tested with STMicroelectronics STM32L562E-DK board mounted with a STM32L562QE-Q chip
    and can be easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example




 */
