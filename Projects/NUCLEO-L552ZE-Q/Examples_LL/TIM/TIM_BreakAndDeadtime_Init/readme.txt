/**
  @page TIM_BreakAndDeadtime_Init TIM example

  @verbatim
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_BreakAndDeadtime_Init/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_BreakAndDeadtime_Init example.
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

Configuration of the TIM peripheral to
 generate three center-aligned PWM and complementary PWM signals,
 insert a defined deadtime value,
 use the break feature,
 and lock the break and dead-time configuration.
This example is based on the STM32L5xx TIM LL API. The peripheral initialization
uses LL initialization function to demonstrate LL Init.

TIM1CLK is fixed to 110 MHz, the TIM1 Prescaler is set to have
TIM1 counter clock = 10 MHz.

The TIM1 auto-reload is set to generate PWM signal at 100 Hz:

The Three Duty cycles are computed as the following description:
The channel 1 duty cycle is set to 50% so channel 1N is set to 50%.
The channel 2 duty cycle is set to 25% so channel 2N is set to 75%.
The channel 3 duty cycle is set to 12.5% so channel 3N is set to 87.5%.

A dead time equal to 4 us is inserted between
the different complementary signals, and the Lock level 1 is selected.
  - The OCx output signal is the same as the reference signal except for the rising edge,
    which is delayed relative to the reference rising edge.
  - The OCxN output signal is the opposite of the reference signal except for the rising
    edge, which is delayed relative to the reference falling edge

Note that calculated duty cycles apply to the reference signal (OCxREF) from
which outputs OCx and OCxN are generated. As dead time insertion is enabled the
duty cycle measured on OCx will be slightly lower.

The break Polarity is used at High level.

The TIM1 waveforms can be displayed using an oscilloscope.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Timer, TIM, PWM, Signal, Duty cycle, Dead time, Break-time, Break polarity, Oscilloscope.

@par Directory contents

  - TIM/TIM_BreakAndDeadtime_Init/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - TIM/TIM_BreakAndDeadtime_Init/Inc/main.h                  Header for main.c module
  - TIM/TIM_BreakAndDeadtime_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_BreakAndDeadtime_Init/Src/stm32l5xx_it.c          Interrupt handlers
  - TIM/TIM_BreakAndDeadtime_Init/Src/main.c                  Main program
  - TIM/TIM_BreakAndDeadtime_Init/Src/system_stm32l5xx.c      STM32L5xx system source file

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up
    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH1  PE9: connected to pin 4 of CN10 connector
      - TIM1_CH1N PE8: connected to pin 18 of CN10 connector
      - TIM1_CH2  PE11: connected to pin 6 of CN10 connector
      - TIM1_CH2N PE10: connected to pin 24 of CN10 connector
      - TIM1_CH3  PE13: connected to pin 10 of CN10 connector
      - TIM1_CH3N PE12: connected to pin 26 of CN10 connector

    - Connect the TIM1 break to the GND. To generate a break event, switch this
      pin level from 0V to 3.3V.
      - TIM1_BKIN  PE15: connected to pin 30 of CN10 connector

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
