/**
  @page FLASH_DualBoot  FLASH Dual Boot example

  @verbatim
  ******************************************************************************
  * @file    FLASH/FLASH_DualBoot/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FLASH Dual boot example.
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

Guide through the configuration steps to program internal Flash memory bank 1 and bank 2,
and to swap between both of them by mean of the FLASH HAL API.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 110 MHz.

Below are the steps to run this example:
1- Select bank2 configuration by commenting FLASH_BANK1 and uncommenting FLASH_BANK2 defines
   in "main.h", and generate its binary (ie: FLASH_DualBoot.bin)

2- Load this binary at the bank2 of the flash(at the address 0x08040000) using
   STM32 CubeProgrammer (www.st.com) or any similar tool.
@note:
 - You can avoid step 1 by directly loading the binary file provided with the example
 - You have to configure your preferred toolchain in order to generate the binary
   file after compiling the project.
 - You can use STM32CubeProgrammer or any similar tool to insure DBANK=1 and initially
   reset the SWAP_BANK bit (bank 1 and bank 2 address are not swapped).

3- Select bank1 configuration by uncommenting FLASH_BANK1 and commenting FLASH_BANK2 defines
   in "main.h", and run it, this project will be loaded in the bank1 of the flash: at the
   address 0x08000000

4- Click the WKUP push-button to swap between the two banks

- If program in bank1 is selected, a message will be displayed on LCD glass
  and LED4 will remain toggling while LED5 is turn on.

- If program in bank2 is selected, a message will be displayed on LCD glass
  and LED5 will remain toggling while LED4 is turn on.

- If error occurs both LED4 and LED5 blink (period : 2s).

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Memory, Flash, Dual-Boot, Dual-Bank, Execute, Binary, Option-Bytes

@par Directory contents

  - FLASH/FLASH_DualBoot/Inc/stm32l552e_eval_conf.h     BSP configuration file
  - FLASH/FLASH_DualBoot/Inc/stm32l5xx_hal_conf.h        HAL Configuration file
  - FLASH/FLASH_DualBoot/Inc/stm32l5xx_it.h              Header for stm32l5xx_it.c
  - FLASH/FLASH_DualBoot/Inc/main.h                      Header for main.c module
  - FLASH/FLASH_DualBoot/Src/stm32l5xx_it.c              Interrupt handlers
  - FLASH/FLASH_DualBoot/Src/main.c                      Main program
  - FLASH/FLASH_DualBoot/Src/stm32l5xx_msp_hal.c         MSP initialization and de-initialization
  - FLASH/FLASH_DualBoot/Src/system_stm32l5xx.c          STM32L5xx system clock configuration file
  - FLASH/FLASH_DualBoot/Binary/FLASH_DualBoot.bin       Binary file to load at bank2

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
