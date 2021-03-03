/**
  @page OSPI_NOR_ReadWrite_DMA_DTR OSPI NOR Read/Write in DMA mode example
  
  @verbatim
  ******************************************************************************
  * @file    OSPI/OSPI_NOR_ReadWrite_DMA_DTR/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the OSPI NOR Read/Write in DMA mode example.
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

How to use a OSPI NOR memory in DMA mode.

This example describes how to erase part of a OSPI NOR memory, write data in DMA mode, read data in DMA mode 
and compare the result in an infinite loop. The memory is configured in octal DTR mode.

LED_GREEN toggles each time a new comparison is good
LED_RED is on as soon as a comparison error occurs
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

Memory, OCTOSPI, NOR, DMA, DTR-mode

@par Directory contents 

  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/main.c                 Main program
  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/system_stm32l5xx.c     STM32L5xx system clock configuration file
  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/stm32l5xx_it.c         Interrupt handlers 
  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/stm32l5xx_hal_msp.c    HAL MSP module
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/main.h                 Main program header file  
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/stm32l5xx_it.h         Interrupt handlers header file
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/stm32l562e_discovery_conf.h      BSP configuration file  

        
@par Hardware and Software environment  

  - This example runs on STM32L562QEIxQ devices.
    
  - This example has been tested with STM32L562E-DK board and can be 
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
