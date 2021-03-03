/**
  @page TIM_TimeBase TIM example

  @verbatim
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_TimeBase/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_TimeBase example.
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

Configuration of the TIM peripheral to generate a timebase. This
example is based on the STM32L5xx TIM LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

   In this example TIM8 input clock TIM8CLK is set to APB2 clock (PCLK2),
   since APB2 pre-scaler is equal to 1.
      TIM8CLK = PCLK2
      PCLK2 = HCLK
      => TIM8CLK = SystemCoreClock (110 MHz)

To set the TIM8 counter clock frequency to 10 KHz, the pre-scaler (PSC) is calculated as follows:
PSC = (TIM8CLK / TIM8 counter clock) - 1
PSC = (SystemCoreClock /10 KHz) - 1

SystemCoreClock is set to 110 MHz for STM32L5xx Devices.

The auto-reload (ARR) is calculated to get a timebase period of 100ms,
meaning that initial timebase frequency is 10 Hz.
ARR = (TIM8 counter clock / timebase frequency) - 1
ARR = (TIM8 counter clock / 10) - 1

Update interrupts are enabled. Within the update interrupt service routine, pin PC.07
(connected to LED1 on board NUCLEO-L552ZE-Q) is toggled. So the period of
blinking of LED1 = 2 * timebase period.

User push-button can be used to modify the timebase period from 100 ms
to 1 s in 100 ms steps. To do so, every time User push-button is pressed, the
autoreload register (ARR) is updated. In up-counting update event is generated
at each counter overflow (when the counter reaches the auto-reload value).

Finally the timebase frequency is calculated as follows:
timebase frequency = TIM8 counter clock /((PSC + 1)*(ARR + 1)*(RCR + 1))

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Timers, TIM, Time base

@par Directory contents

  - TIM/TIM_TimeBase/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - TIM/TIM_TimeBase/Inc/main.h                  Header for main.c module
  - TIM/TIM_TimeBase/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_TimeBase/Src/stm32l5xx_it.c          Interrupt handlers
  - TIM/TIM_TimeBase/Src/main.c                  Main program
  - TIM/TIM_TimeBase/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552xx devices.

  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
