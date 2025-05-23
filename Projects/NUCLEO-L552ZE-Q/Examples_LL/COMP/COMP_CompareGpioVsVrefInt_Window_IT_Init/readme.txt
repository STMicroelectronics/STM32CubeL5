/**
  @page COMP_CompareGpioVsVrefInt_Window_IT_Init COMP example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/COMP/COMP_CompareGpioVsVrefInt_Window_IT_Init/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the COMP_CompareGpioVsVrefInt_Window_IT_Init example.
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

How to use a pair of comparator peripherals to compare a voltage level applied on
a GPIO pin to two thresholds: the internal voltage reference (VREFINT) and a fraction
of the internal voltage reference (VREFINT/2), in interrupt mode. This example is
based on the STM32L5xx COMP LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

Example configuration:
Comparator instances COMP1 and COMP2 are configured to work together
in window mode to compare 2 thresholds:
 - COMP1 input plus set to a GPIO pin (cf pin below) in analog mode.
   COMP2 input plus is not used (connected internally to the 
   same input plus as COMP1).
 - COMP1 input minus set to internal voltage reference VrefInt 
   (voltage level 1.2V, refer to device datasheet for min/typ/max values)
 - COMP2 input minus set to 1/2 of internal voltage reference VrefInt 
   (voltage level 0.6V)
Comparator interruption is enabled through EXTI lines 21 and 22
with trigger edge set to both edges rising and falling.
Comparators are set in power mode medium speed.

Example execution:
From the main program execution, comparator is enabled.
Each time the voltage level applied on GPIO pin (comparator input plus) 
is crossing VrefInt or 1/2 VrefInt voltage thresholds (comparator input minus),
the comparator with corresponding threshold generates an interruption.
LED_GREEN is turned on or off depending on the pair of comparators output state:
turned on if comparator voltage level is inside the window (between VrefInt
and 1/2 VrefInt: between 1.2V and 0.6V), turned off otherwise.

Connection needed:
A voltage must be supplied to the analog input pin (cf pin below),
between Vdda=3.3V and 0V. Otherwise this pin can be let floating (but in this
case COMP comparison level will be undetermined).

To check comparator window thresholds, you must use an external power supply
to set voltage level below, above or within comparator thresholds.

Other peripheral used:
  1 GPIO for LED_GREEN
  1 GPIO for analog input of comparator: PB.02 (connector CN9 pin 13)
  EXTI line 21 (connected to COMP1 output)
  EXTI line 22 (connected to COMP2 output)
  
@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Comparator, voltage, analog input, channel output, 1/2 VREFINT, COMP1, COMP2, Vdda, interrupt

@par Directory contents 

  - COMP/COMP_CompareGpioVsVrefInt_Window_IT_Init/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT_Init/Inc/main.h                  Header for main.c module
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT_Init/Src/stm32l5xx_it.c          Interrupt handlers
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT_Init/Src/main.c                  Main program
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT_Init/Src/system_stm32l5xx.c      STM32L5xx system source file


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
