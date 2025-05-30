/**
  @page OSPI_NOR_MemoryMapped OSPI NOR memory mapped example
  
  @verbatim
  ******************************************************************************
  * @file    OSPI/OSPI_NOR_MemoryMapped/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the OSPI NOR memory mapped mode example.
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

How to use a OSPI NOR memory in memory-mapped mode.

This example describes how to erase a part of a OSPI NOR memory, write data in memory-mapped mode and 
access to OSPI NOR memory in memory-mapped mode to check the data in an infinite loop.

LED_GREEN toggles each time the data have been checked
LED_RED is on as soon as a data is wrong
LED_RED toggles as soon as an error is returned by HAL API

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

Memory, OCTOSPI, NOR, Memory-mapped

@par Directory contents 

  - OSPI/OSPI_NOR_MemoryMapped/Src/main.c                 Main program
  - OSPI/OSPI_NOR_MemoryMapped/Src/system_stm32l5xx.c     STM32L5xx system clock configuration file
  - OSPI/OSPI_NOR_MemoryMapped/Src/stm32l5xx_it.c         Interrupt handlers 
  - OSPI/OSPI_NOR_MemoryMapped/Src/stm32l5xx_hal_msp.c    HAL MSP module
  - OSPI/OSPI_NOR_MemoryMapped/Inc/main.h                 Main program header file  
  - OSPI/OSPI_NOR_MemoryMapped/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - OSPI/OSPI_NOR_MemoryMapped/Inc/stm32l5xx_it.h         Interrupt handlers header file
  - OSPI/OSPI_NOR_MemoryMapped/Inc/stm32l552e_eval_conf.h      HAL configuration file  

        
@par Hardware and Software environment  

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with STM32L552E-EV board and can be 
    easily tailored to any other supported device and development board.

  - The JP7 jumper should be on position [1-2] (OSPI_FLASH_NCS)

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */
