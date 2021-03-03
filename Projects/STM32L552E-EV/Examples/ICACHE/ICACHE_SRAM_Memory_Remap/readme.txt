/**
  @page ICACHE_SRAM_Memory_Remap example

  @verbatim
  ******************************************************************************
  * @file    ICACHE/ICACHE_SRAM_Memory_Remap/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the example ICACHE_SRAM_Memory_Remap.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to execute code from an external SRAM remapped region configured through the ICACHE HAL driver.

This example describes how to remap the IS61WV102416BLL-10MLI external SRAM to C-bus and execute code located in this memory.

First some pieces of code is copied from internal FLASH to external SRAM memory.
Then the code execution from external SRAM toggles the Green led and turns it on.
Finally once the memory is remapped to C-bus region via ICACHE, the remapped code
toggles the Green led and turns it off.

This project is targeted to run on STM32L552xx devices on STM32L552E-EV board from STMicroelectronics.

At the beginning of the main program the HAL_Init() function is called initialize the systick.

The SystemClock_Config() function is used to configure the system clock for STM32L552ZE-Q devices :
The CPU at 110MHz

STM32L552E-EV board's leds can be used to monitor the example execution:
 - Green led toggling and off when no error detected.
 - Red led is on when any error occurred.

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

System, ICACHE, FMC, SRAM, Memory Remap

@par Directory contents

  - ICACHE/ICACHE_SRAM_Memory_Remap/Src/main.c                  Main program
  - ICACHE/ICACHE_SRAM_Memory_Remap/Src/stm32l5xx_hal_msp.c     MSP initializations and de-initializations
  - ICACHE/ICACHE_SRAM_Memory_Remap/Src/system_stm32l5xx.c      STM32L5xx system source file
  - ICACHE/ICACHE_SRAM_Memory_Remap/Src/stm32l5xx_it.c          Interrupt handlers
  - ICACHE/ICACHE_SRAM_Memory_Remap/Inc/main.h                  Main program header file
  - ICACHE/ICACHE_SRAM_Memory_Remap/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - ICACHE/ICACHE_SRAM_Memory_Remap/Inc/stm32l552e_eval_conf.h  Board configuration file
  - ICACHE/ICACHE_SRAM_Memory_Remap/Inc/stm32l5xx_it.h          Interrupt handlers header file

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices without security enabled (TZEN=0).

  - This example has been tested with STMicroelectronics STM32L552E-EV
    board and can be easily tailored to any other supported device
    and development board.

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
