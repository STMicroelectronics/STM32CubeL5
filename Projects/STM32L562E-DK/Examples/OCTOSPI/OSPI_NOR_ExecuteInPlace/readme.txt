/**
  @page OSPI_NOR_ExecuteInPlace OSPI NOR execute in place example
  
  @verbatim
  ******************************************************************************
  * @file    OSPI/OSPI_NOR_ExecuteInPlace/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the code execution from OSPI NOR memory example.
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

How to execute code from OSPI memory after code loading.

This example describes how to execute a part of the code from a OSPI NOR memory. To do this, 
a section is created where the function is stored.

At the startup, the OSPI NOR memory is erased, then the data are copied from the initialization
section of the flash to the OSPI NOR memory. And finally the OSPI NOR is configured in memory-mapped 
mode and the code in executed in an infinite loop.

LED10 and LED9 toggle in a forever loop.
LED_RED is on as soon as an error is returned by HAL API, and no more LED toggles.

In this example, HCLK is configured at 110 MHz.

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

Memory, OCTOSPI, NOR, Memory-mapped, Execute-In-Place, XiP

@par Directory contents 

  - OSPI/OSPI_NOR_ExecuteInPlace/Src/main.c                 Main program
  - OSPI/OSPI_NOR_ExecuteInPlace/Src/system_stm32l5xx.c     STM32L5xx system clock configuration file
  - OSPI/OSPI_NOR_ExecuteInPlace/Src/stm32l5xx_it.c         Interrupt handlers 
  - OSPI/OSPI_NOR_ExecuteInPlace/Src/stm32l5xx_hal_msp.c    HAL MSP module
  - OSPI/OSPI_NOR_ExecuteInPlace/Inc/main.h                 Main program header file  
  - OSPI/OSPI_NOR_ExecuteInPlace/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - OSPI/OSPI_NOR_ExecuteInPlace/Inc/stm32l5xx_it.h         Interrupt handlers header file
  - OSPI/OSPI_NOR_MemoryMapped/Inc/stm32l562e_discovery_conf.h      BSP configuration file  

        
@par Hardware and Software environment  

  - This example runs on STM32L562QEIxQ devices.
    
  - This example has been tested with STM32L562E-DK board and can be 
    easily tailored to any other supported device and development board.

  - A specific region for the OSPI NOR memory has been added in the scatter file 
   in order to map functions in this memory

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */
