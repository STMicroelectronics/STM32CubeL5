/**
  @page TIM_TimeBase_Init TIM example

  @verbatim
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_TimeBase_Init/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_TimeBase_Init example.
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

Configuration of the TIM peripheral to generate a timebase. This
example is based on the STM32L5xx TIM LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

In this example TIM1 input clock TIM1CLK is set to APB2 clock PCLK2 (TIM1CLK = PCLK2).
	As APB2 pre-scaler is equal to 1 PCLK2 = HCLK, and since AHB pre-scaler is equal to 1,
      => TIM1CLK = SystemCoreClock (110 MHz)

To set the TIM1 counter clock frequency to 10 KHz, the pre-scaler (PSC) is calculated as follows:
PSC = (TIM1CLK / TIM1 counter clock) - 1
PSC = (SystemCoreClock /10 KHz) - 1

SystemCoreClock is set to 110 MHz for STM32L5xx Devices.

The auto-reload (ARR) is calculated to get a timebase period of 100ms,
meaning that initial timebase frequency is 10 Hz.
ARR = (TIM1 counter clock / timebase frequency) - 1
ARR = (TIM1 counter clock / 10) - 1

Update interrupts are enabled. Within the update interrupt service routine, pin PC7
(connected to LED1 on board NUCLEO-L552ZE-Q) is toggled. So the period of
blinking of LED1 = 2 * timebase period.

User push-button can be used to modify the timebase period from 100 ms
to 1 s in 100 ms steps. To do so, every time User push-button is pressed, the
autoreload register (ARR) is updated. In up-counting update event is generated
at each counter overflow (when the counter reaches the auto-reload value).

Finally the timebase frequency is calculated as follows:
timebase frequency = TIM1 counter clock /((PSC + 1)*(ARR + 1)*(RCR + 1))

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Timers, TIM, Time base

@par Directory contents

  - TIM/TIM_TimeBase_Init/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - TIM/TIM_TimeBase_Init/Inc/main.h                  Header for main.c module
  - TIM/TIM_TimeBase_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_TimeBase_Init/Src/stm32l5xx_it.c          Interrupt handlers
  - TIM/TIM_TimeBase_Init/Src/main.c                  Main program
  - TIM/TIM_TimeBase_Init/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
