/**
  @page PWR_EnterStopMode PWR standby example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/PWR/PWR_EnterStopMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR Stop 1 mode example.
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

How to enter the Stop 1 mode.

After start-up LED1 is toggling during 5 seconds, then the system automatically 
enter in Stop 1 mode (Final state).

LED1 is used to monitor the system state as follows:
 - LED1 toggling : system in RUN mode
 - LED1 off : system in Stop 1 mode

@note To measure MCU current consumption on board STM32L5,
      board configuration must be applied:
      - remove jumper JP5 and connect an amperemeter to measure current between the 2 connectors of JP5.

@note This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M33 core is no longer clocked during low power mode 
      so debugging features are disabled.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Power, PWR, stop mode, Interrupt, Low Power

@par Directory contents 

  - PWR/PWR_EnterStopMode/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - PWR/PWR_EnterStopMode/Inc/main.h                  Header for main.c module
  - PWR/PWR_EnterStopMode/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_EnterStopMode/Src/stm32l5xx_it.c          Interrupt handlers
  - PWR/PWR_EnterStopMode/Src/main.c                  Main program
  - PWR/PWR_EnterStopMode/Src/system_stm32l5xx.c      STM32L5xx system source file

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-L552ZE-Q
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-L552ZE-Q Set-up
    - LED1 connected to PC.07 pin

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
