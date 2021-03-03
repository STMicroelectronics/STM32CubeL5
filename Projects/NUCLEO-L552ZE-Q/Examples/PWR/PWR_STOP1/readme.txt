/**
  @page PWR_STOP1 Power Stop 1 Mode Example
  
  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_STOP1/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the Power Stop 1 Mode example.
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
 
How to enter the Stop 1 mode and wake up from this mode by using an interrupt.

In the associated software, the system clock is set to 110 MHz, an EXTI line
is connected to the User push-button through PC.13 and configured to generate an
interrupt on falling edge.
The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick 
interrupt handler, LED1 is toggled in order to indicate whether the MCU is in STOP1 mode or RUN mode.

When a rising edge is detected on the EXTI line, an interrupt is generated. In the
EXTI handler routine, the wake-up pin PWR_WAKEUP_PIN2 is enabled and the corresponding
wake-up flag cleared. Then, the system enters STOP1 mode causing LED1 to stop toggling.
A rising edge on WKUP pin will wake-up the system from STOP1.
Alternatively, an external RESET of the board will lead to a system wake-up as well.

After wake-up from STOP1 mode, program execution restarts in the same way as after 
a RESET and LED1 restarts toggling.

Two leds LED1 and LED3 are used to monitor the system state as follows:
 - LED3 On: configuration failed (system will go to an infinite loop)
 - LED1 toggling: system in Run mode
 - LED1 off : system in STOP1 mode

@note To measure the current consumption in STOP1 mode, remove jumper JP5
      and connect an amperemeter to measure IDD current.

@note This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M33 core is no longer clocked during low power mode 
      so debugging features are disabled.

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

Power, PWR, STOP1 mode, Interrupt, EXTI, Wakeup, Low Power, External reset

@par Directory contents 

  - PWR/PWR_STOP1/Inc/stm32l5xx_nucleo_conf.h     BSP configuration file
  - PWR/PWR_STOP1/Inc/stm32l5xx_conf.h         HAL Configuration file
  - PWR/PWR_STOP1/Inc/stm32l5xx_it.h           Header for stm32l5xx_it.c
  - PWR/PWR_STOP1/Inc/main.h                         Header file for main.c
  - PWR/PWR_STOP1/Src/system_stm32l5xx.c       STM32L5xx system clock configuration file
  - PWR/PWR_STOP1/Src/stm32l5xx_it.c           Interrupt handlers
  - PWR/PWR_STOP1/Src/main.c                         Main program
  - PWR/PWR_STOP1/Src/stm32l5xx_hal_msp.c      HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices
    
  - This example has been tested with STMicroelectronics NUCLEO-L552ZE-Q C-02
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-L552ZE-Q C-02 Set-up
    - Use LED1 and LED3 connected respectively to PC.07 and PA.09 pins
    - User push-button connected to PC.13 pin (External line 13)
    - WakeUp Pin PWR_WAKEUP_PIN2 connected to PC.13
    
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Once the image is loaded, power off the NUCLEO board by unplugging
   the power cable then power on the board again 
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
