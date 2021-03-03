/**
  @page TIM_PWMOutput TIM example

  @verbatim
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_PWMOutput example.
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

Use of a timer peripheral to generate a
PWM output signal and update the PWM duty cycle. This example is based on the
STM32L5xx TIM LL API. The peripheral initialization uses
LL unitary service functions for optimization purposes (performance and size).

In this example TIM3 input clock TIM3CLK is set to APB2 clock (PCLK2),
since APB2 pre-scaler is equal to 1.
    TIM3CLK = PCLK2
    PCLK2 = HCLK
    => TIM3CLK = HCLK = SystemCoreClock (110 MHz)

To set the TIM3 counter clock frequency to 10 KHz, the pre-scaler (PSC) is
calculated as follows:
PSC = (TIM3CLK / TIM3 counter clock) - 1
PSC = (SystemCoreClock /10 KHz) - 1

SystemCoreClock is set to 110 MHz for STM32L5xx Devices.

Auto-reload (ARR) is calculated to get a time base period of 10 ms,
meaning a time base frequency of 100 Hz.
ARR = (TIM3 counter clock / time base frequency) - 1
ARR = (TIM3 counter clock / 100) - 1

Initially, the capture/compare register (CCR2) of the output channel is set to
half the auto-reload value meaning a initial duty cycle of 50%.
Generally speaking this duty cycle is calculated as follows:
Duty cycle = (CCR2 / ARR) * 100

The timer output channel is mapped on the pin PC.07 (connected to LED1 on board
NUCLEO-L552ZE-Q).

User push-button can be used to change the duty cycle from 0% up to 100% by
steps of 10%. Duty cycle is periodically measured. It can be observed through
the debugger by watching the variable uwMeasuredDutyCycle.

@note The LED1 is not toggling. In fact, if the dutycycle is 0% so the LED1 is OFF. When pushing
successively the user button, the LED1 is ON and its luminosity rises as the dutycycle value keeps
increasing.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Timers, TIM, Output, Signal, PWM, Frequency, Duty cycle, Waveform, Auto-reload.

@par Directory contents

  - TIM/TIM_PWMOutput/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMOutput/Inc/main.h                  Header for main.c module
  - TIM/TIM_PWMOutput/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_PWMOutput/Src/stm32l5xx_it.c          Interrupt handlers
  - TIM/TIM_PWMOutput/Src/main.c                  Main program
  - TIM/TIM_PWMOutput/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with NUCLEO-L552ZE-Q C-02 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up:
      - TIM3_CH2  PC.07: connected to pin 19 of CN12 connector

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
