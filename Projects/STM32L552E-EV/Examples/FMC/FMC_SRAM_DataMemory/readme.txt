/**
  @page FMC_SRAM_DataMemory SRAM functionalities example
  
  @verbatim
  ******************************************************************************
  * @file    FMC/FMC_SRAM_DataMemory/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the FMC SRAM example.
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

How to configure the FMC controller to access the IS61WV51216BLL SRAM memory including heap and stack.
 
At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 110 MHz.
  
The example scenario does not reflect a real application case, its purpose is to
provide only the procedure to follow to use the external SRAM as data memory.

This example does not use the default library startup file. It uses a modified 
startup file provided with the example. The user has to add the new startup 
file in the project source list. While startup, the SRAM is configured 
and initialized to be ready to contain data.

The user has to configure his preferred toolchain using the provided linker file.
The RAM zone is modified in order to use the external SRAM as a RAM.

At this stage, all the used data can be located in the external SRAM.

The user can use the debugger's watch to evaluate "uwTabAddr" and "MSPValue" variables
values which should be above 0x60000000.

If uwTabAddr and MSPValue values are in the external SRAM, LED4 is ON, otherwise the LED5 is ON.


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

Memory, FMC, SRAM, Read, Write, Data-Memory, Heap, Stack

@par Directory contents 

  - FMC/FMC_SRAM_DataMemory/Inc/stm32l552e_eval_conf.h     BSP configuration file
  - FMC/FMC_SRAM_DataMemory/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - FMC/FMC_SRAM_DataMemory/Inc/main.h                  Header for main.c module  
  - FMC/FMC_SRAM_DataMemory/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - FMC/FMC_SRAM_DataMemory/Src/main.c                  Main program
  - FMC/FMC_SRAM_DataMemory/Src/stm32l5xx_hal_msp.c     HAL MSP module  
  - FMC/FMC_SRAM_DataMemory/Src/stm32l5xx_it.c          Interrupt handlers
  - FMC/FMC_SRAM_DataMemory/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.


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
