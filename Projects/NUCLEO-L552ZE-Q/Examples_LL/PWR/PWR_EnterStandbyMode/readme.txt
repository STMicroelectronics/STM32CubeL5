/**
  @page PWR_EnterStandbyMode PWR standby example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/PWR/PWR_EnterStandbyMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR STANDBY mode example.
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

How to enter the Standby mode and wake up from this mode by using an external 
reset or a wakeup interrupt.

In the associated software, the system clock is set to 110 MHz.
An EXTI line is connected to the User push-button through PC.13 and configured to generate an 
interrupt on falling edge upon key press.
LED1 toggles in order to indicate that MCU is in RUN mode:
- quickly (each 200ms) if program starts from reset
- slowly (each 500ms) if program wakes up from Standby mode

When a falling edge is detected on the EXTI line, an interrupt is generated. 
In the EXTI handler routine, the wake-up pin LL_PWR_WAKEUP_PIN2 is enabled and the 
corresponding wake-up flag cleared. Then, the system enters Standby mode causing 
LED1 to stop toggling. 

Next, the user can wake-up the system in pressing the User push-button which is 
connected to the wake-up pin LL_PWR_WAKEUP_PIN2.
A falling edge on WKUP pin will wake-up the system from Standby.

Alternatively, an external RESET of the board will lead to a system wake-up as well.

After wake-up from Standby mode, program execution restarts in the same way as 
after a RESET and LED1 restarts toggling.

LED1 is used to monitor the system state as follows:
 - LED1 fast toggling: system in RUN mode
 - LED1 slow toggling: system in RUN mode after exiting from Standby mode
 - LED1 off : system in Standby mode

These steps are repeated in an infinite loop.

@note System in Standby mode : LED state at this step depends on blinking state at the instant of user button is pressed.

@note To measure the current consumption in Standby mode, remove JP5 jumper 
      and connect an ampere meter to JP5 to measure IDD current.

@note This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M33 core is no longer clocked during low power mode 
      so debugging features are disabled.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Power, PWR, Standby mode, Interrupt, EXTI, Wakeup, Low Power, External reset,

@par Directory contents 

  - PWR/PWR_EnterStandbyMode/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - PWR/PWR_EnterStandbyMode/Inc/main.h                  Header for main.c module
  - PWR/PWR_EnterStandbyMode/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_EnterStandbyMode/Src/stm32l5xx_it.c          Interrupt handlers
  - PWR/PWR_EnterStandbyMode/Src/main.c                  Main program
  - PWR/PWR_EnterStandbyMode/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-L552ZE-Q
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-L552ZE-Q Set-up
    - LED1 connected to PC.07 pin
    - User push-button connected to pin PC.13 (External line 13)
    - WakeUp Pin LL_PWR_WAKEUP_PIN2 connected to PC.13

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
