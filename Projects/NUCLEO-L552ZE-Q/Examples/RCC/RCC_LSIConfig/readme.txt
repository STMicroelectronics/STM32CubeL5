/**
  @page RCC_LSIConfig RCC Clock Config example
  
  @verbatim
  ******************************************************************************
  * @file    RCC/RCC_LSIConfig/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RCC Clock Config example.
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

Enabling/disabling of the low-speed internal (LSI) RC oscillator (about 32 KHz) at run time, using the RCC HAL API.


Board: NUCLEO-L552ZE-Q C-02 (embeds a STM32L552ZET6Q device)
Microcontroller Clock Output MCO1 Pin: PA.08

   _________________________                   _______________________
  |     _________CN10_______|                 |  _______________      |
  |    |   RCC              |                 | |               | O   |
  |    |                    |                 | |   _   _   _   | O   |
  |    |(pin 23) MCO1(PA.08)|_________________| | _| |_| |_| |_ | --  |
  |    |                    |                 | |_______________| --  |
  |    |                    |                 |                       |
  |    |                    |                 |_oscilloscope__________|
  |    |                    |
  |    |                    |
  |    |____________________|
  |                         |
  |                         |
  |_STM32_Board ____________|

In this example, after startup SYSCLK is configured to the max frequency using the PLL with
MSI as clock source, the User push-button (connected to External line 13)
is used to enable/disable LSI:
each time the User push-button is pressed, External line 13 interrupt is generated and the ISR
enables the LSI when it is off, disables it when it is on.


When 4 LEDs are available on the board
- when LSI is off, LED1 is toggling (every 300 ms)
- when LSI is on, LED2 is toggling (every 300 ms)


LSI clock waveform is available on the MCO1 pin PA.08 and can be captured
on an oscilloscope.

In case of configuration error, LED1 is slowly toggling (1 sec. period).

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

System, RCC, PLL, PLLCLK, SYSCLK, LSI, Clock, Oscillator,

@par Directory contents

  - RCC/RCC_LSIConfig/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - RCC/RCC_LSIConfig/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - RCC/RCC_LSIConfig/Inc/main.h                        Header for main.c module
  - RCC/RCC_LSIConfig/Src/stm32l5xx_it.c          Interrupt handlers
  - RCC/RCC_LSIConfig/Src/main.c                        Main program
  - RCC/RCC_LSIConfig/Src/system_stm32l5xx.c      STM32L5xx system source file
  - RCC/RCC_LSIConfig/Src/stm32l5xx_hal_msp.c     HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with NUCLEO-L552ZE-Q C-02
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
