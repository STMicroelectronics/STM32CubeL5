/**
  @page HAL_RegisterCallbacks_TIM HAL callback registration for Timer example

  @verbatim
  ******************************************************************************
  * @file    HAL/HAL_RegisterCallbacks_TIM/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the HAL callback registration for Timer example
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

Register a callback function called every second based on TIM peripheral
configuration to generate a timebase of one second with the corresponding
interrupt request.

    In this example TIM6 input clock (TIM6CLK)  is set to APB1 clock (PCLK1),
    since APB1 prescaler is equal to 1:
      TIM6CLK = PCLK1
      PCLK1 = HCLK
      => TIM6CLK = HCLK = SystemCoreClock
    To get TIM6 counter clock at 10 KHz, the Prescaler is computed as following:
    Prescaler = (TIM6CLK / TIM6 counter clock) - 1
    Prescaler = (SystemCoreClock /10 KHz) - 1

SystemCoreClock is set to 110 MHz for STM32L5xx Devices.

The TIM6 ARR register value is equal to 10000 - 1,
Update rate = TIM6 counter clock / (Period + 1) = 1 Hz,
So the TIM6 generates an interrupt each 1 s

When the counter value reaches the auto-reload register value, the TIM update
interrupt is generated and, in the handler routine, the user registered callback
function toggles the pin PC.07 (connected to LED1 on board NUCLEO-L552ZE-Q Rev C)
at the following frequency: 0.5Hz.


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, TIM, callback registering

@par Directory contents

  - HAL/HAL_RegisterCallbacks_TIM/Inc/stm32l5xx_nucleo_conf.h BSP configuration file
  - HAL/HAL_RegisterCallbacks_TIM/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - HAL/HAL_RegisterCallbacks_TIM/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - HAL/HAL_RegisterCallbacks_TIM/Inc/main.h                  Header for main.c module
  - HAL/HAL_RegisterCallbacks_TIM/Src/stm32l5xx_it.c          Interrupt handlers
  - HAL/HAL_RegisterCallbacks_TIM/Src/main.c                  Main program
  - HAL/HAL_RegisterCallbacks_TIM/Src/stm32l5xx_hal_msp.c     HAL MSP file
  - HAL/HAL_RegisterCallbacks_TIM/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with STMicroelectronics NUCLEO-L552ZE-Q C-02 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
