/**
  @page TIM_DMA TIM example

  @verbatim
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_DMA/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_DMA example.
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

Use of the DMA with a timer update request
to transfer data from memory to Timer Capture Compare Register 3 (TIMx_CCR3). This
example is based on the STM32L5xx TIM LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

The following configuration values are used in this example:
  - TIM1CLK = SystemClock
  - Counter repetition = 3
  - Prescaler = 0
  - TIM1 counter clock = SystemClock
  - SystemCoreClock is set to 110 MHz for STM32L5xx Devices.

The objective is to configure TIM1 channel 1 to generate PWM edge aligned
signal with a frequency equal to 17.57 KHz, and a variable duty cycle that
is changed by the DMA after a specific number of Update DMA request.

The number of this repetitive requests is defined by the TIM1 Repetition counter,
each 4 Update Requests, the TIM1 Channel 1 Duty Cycle changes to the next new
value defined by the aCCValue buffer.

The PWM waveform can be displayed using an oscilloscope.

Whenever a DMA transfer fails LED2 flashes with a frequency of 1 Hz.

@note PWM signal frequency value mentioned above is theoretical (obtained when
      the system clock frequency is exactly 110 MHz). Since the generated system
      clock frequency may vary from one board to another observed PWM signal
      frequency might be slightly different.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Timers, TIM, DMA, PWM, Frequency, Duty Cycle, Waveform, Oscilloscope, Output, Signal.

@par Directory contents

  - TIM/TIM_DMA/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - TIM/TIM_DMA/Inc/main.h                  Header for main.c module
  - TIM/TIM_DMA/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_DMA/Src/stm32l5xx_it.c          Interrupt handlers
  - TIM/TIM_DMA/Src/main.c                  Main program
  - TIM/TIM_DMA/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552xx devices.

  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up
    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH1  PE.09: connected to pin 4 of CN10 connector

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
