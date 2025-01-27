/**
  @page IWDG_RefreshUntilUserEvent IWDG example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/IWDG/IWDG_RefreshUntilUserEvent/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the IWDG_RefreshUntilUserEvent example.
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

How to configure the IWDG peripheral to ensure periodical counter update and 
generate an MCU IWDG reset when a User push-button is pressed. The peripheral 
is initialized with LL unitary service functions to optimize 
for performance and size.

Example Configuration:
Configure the IWDG (prescaler, counter) and enable it.
Infinite refresh of the IWDG down-counter done in the main loop.
LED1 is blinking fast & continuously.

Example Execution:
When User push-button is pressed, the down-counter automatic refresh mechanism is 
disabled and thus, reset occurs. After a reset, when re-entering the main function, 
RCC IWDG Reset Flag is checked and if we are back from a IWDG reset the LED1
is switched ON.

Waiting a new User push-button pressed to re-activate the IWDG

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

System, IWDG, reload counter, MCU Reset

@par Directory contents 

  - IWDG/IWDG_RefreshUntilUserEvent/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - IWDG/IWDG_RefreshUntilUserEvent/Inc/main.h                  Header for main.c module
  - IWDG/IWDG_RefreshUntilUserEvent/Inc/stm32_assert.h          Template file to include assert_failed function
  - IWDG/IWDG_RefreshUntilUserEvent/Src/stm32l5xx_it.c          Interrupt handlers
  - IWDG/IWDG_RefreshUntilUserEvent/Src/main.c                  Main program
  - IWDG/IWDG_RefreshUntilUserEvent/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with NUCLEO-L552ZE-Q C-02 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
