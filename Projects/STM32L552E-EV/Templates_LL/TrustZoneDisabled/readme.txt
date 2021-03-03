/**
  @page Templates_LL  Description of the Templates_LL example
  
  @verbatim
  ******************************************************************************
  * @file    Templates_LL/Legacy/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the Templates_LL example.
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

Reference template based on the STM32Cube LL API that can be used
to build any firmware application.

This project is targeted to run on STM32L552ZEQ device on STM32L552E-EV board from STMicroelectronics. 

The reference template project configures the maximum system clock frequency at 110Mhz.

This project LL template provides:
 - Inclusion of all LL drivers (include files in "main.h" and LL sources files in IDE environment, with option "USE_FULL_LL_DRIVER" in IDE environment)
   Note: If optimization is needed afterwards, user can perform a cleanup by removing unused drivers.
 - Definition of LED and user button (file: main.h)
   Note: User button name printed on board may differ from naming "user button" in code: "key button", ...
 - Clock configuration (file: main.c)

This project LL template does not provide:
 - Functions to initialize and control LED and user button
 - Functions to manage IRQ handler of user button

To port a LL example to the targeted board:
1. Select the LL example to port.
   To find the board on which LL examples are deployed, refer to LL examples list in "STM32CubeProjectsList.html", table section "Examples_LL"
   or Application Note: STM32Cube firmware examples for STM32L5 Series

2. Replace source files of the LL template by the ones of the LL example, except code specific to board.
   Note: Code specific to board is specified between tags:
         /* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
         /* ==============   BOARD SPECIFIC CONFIGURATION CODE END      ============== */
         
   - Replace file main.h, with updates:
     - Keep LED and user button definition of the LL template under tags
    
   - Replace file main.c, with updates:
     - Keep clock configuration of the LL template: function "SystemClock_Config()"
     - Depending of LED availability, replace LEDx_PIN by another LEDx (number) available in file main.h
     
   - Replace file stm32l5xx_it.h
   - Replace file stm32l5xx_it.c


@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Directory contents 

  - Templates_LL/Legacy/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - Templates_LL/Legacy/Inc/main.h                  Header for main.c module
  - Templates_LL/Legacy/Inc/stm32_assert.h          Template file to include assert_failed function
  - Templates_LL/Legacy/Src/stm32l5xx_it.c          Interrupt handlers
  - Templates_LL/Legacy/Src/main.c                  Main program
  - Templates_LL/Legacy/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This template runs on STM32L552ZETxQ devices.
    
  - This template has been tested with STM32L552E-EV board and can be
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
