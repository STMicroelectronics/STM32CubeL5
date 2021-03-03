/**
  @page RTC_TimeStamp_Init RTC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_TimeStamp_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC example.
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

Configuration of the Timestamp using the RTC LL API. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size).

In this example, after start-up, SYSCLK is configured to the max frequency using
the PLL with MSI as clock source.

LSE oscillator clock is used as RTC clock source.
The user can use also LSI as RTC clock source by uncommenting the adequate 
line on the main.c file.
 @code
   #define RTC_CLOCK_SOURCE_LSI  
   /* #define RTC_CLOCK_SOURCE_LSE */
 @endcode
LSI oscillator clock is delivered by a 32 kHz RC.
LSE is delivered by a 32.768 kHz crystal.

The associated firmware performs the following:
1. After start-up the program configure the RTC (Time date) and enable the feature 
   timeStamp. 

2. When applying a rising edge on the time stamp pin (PC.13), 
   a time stamp event is detected and the calendar is saved in the time stamp structures.
   The current date and time stamp are updated and displayed.
   Note: On this board, time stamp pin is connected to User push-button (connection to pin PC.13).
         Therefore, no external connection is needed: each action on User push-button
         will trig a time stamp event.
   on the debugger (live watch) in aShowTimeStamp and aShowDateStamp variables .
   The current time and date are updated and displayed on the debugger (live watch) in aShowTime, aShowDate variables .

- LED1 is toggling : This indicates that the system generates an error.
@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

RTC, Timer, Timestamp, Counter, LSE, LSI, Current time, Real Time Clock

@par Directory contents 

  - RTC/RTC_TimeStamp_Init/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - RTC/RTC_TimeStamp_Init/Inc/main.h                  Header for main.c module
  - RTC/RTC_TimeStamp_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - RTC/RTC_TimeStamp_Init/Src/stm32l5xx_it.c          Interrupt handlers
  - RTC/RTC_TimeStamp_Init/Src/main.c                  Main program
  - RTC/RTC_TimeStamp_Init/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
