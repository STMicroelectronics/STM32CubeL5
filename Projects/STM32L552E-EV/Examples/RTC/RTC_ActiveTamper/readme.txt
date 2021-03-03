/**
  @page RTC_ActiveTamper example

  @verbatim
  ******************************************************************************
  * @file    RTC/RTC_ActiveTamper/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the RTC ACTIVE TAMPER example.
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

How to program active tampers.

In the associated software, the system clock is set to 110 MHz the SysTick is programmed to
generate an interrupt each 1 ms.
The LSE clock is used as RTC clock source by default.

This example activate/deactivate Active Tampers and update the Seed at run time.
2 tampers Input share the same tamper Output.
Tamper interrupts and erase of backup registers are checked.

Please connect the following pins together :
  - TAMP_IN5  (PA1   - CN6 Pin23)
  - TAMP_IN6  (PF7   - CN5 Pin31)
  - TAMP_OUT2 (PC13  - CN5 Pin26)
  - (Optional) Oscilloscope probe to visualize the signal

Execute the software, wait a little and disconnect a pin.
One of the below scenario should occur :
 - LED4 toggles for ever : an error occurs.
 - LED4 is ON : Tamper interrupt detected and backup registers are erased.

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

RTC, Active Tamper, LSE,  Reset, System, Tamper

@par Directory contents

  - RTC/RTC_ActiveTamper/Inc/stm32l5xx_hal_conf.h     HAL configuration file
  - RTC/RTC_ActiveTamper/Inc/stm32l5xx_it.h           Header for stm32l5xx_it.c
  - RTC/RTC_ActiveTamper/Inc/main.h                         Header file for main.c
  - RTC/RTC_ActiveTamper/Src/system_stm32l5xx.c       STM32L5xx system clock configuration file
  - RTC/RTC_ActiveTamper/Src/stm32l5xx_it.c           Interrupt handlers
  - RTC/RTC_ActiveTamper/Src/main.c                         Main program
  - RTC/RTC_ActiveTamper/Src/stm32l5xx_hal_msp.c      HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices without security enabled (TZEN=0).

  - This example has been tested with STMicroelectronics STM32L552E-EV
    evaluation board and can be easily tailored to any other supported device
    and development board.

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Connect all pins as required
  - Run the example
  - Disconnect a pin

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
