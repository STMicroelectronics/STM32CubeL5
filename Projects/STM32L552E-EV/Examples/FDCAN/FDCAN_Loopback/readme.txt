/**
  @page FDCAN_Loopback FDCAN Loopback example

  @verbatim
  ******************************************************************************
  * @file    FDCAN/FDCAN_Loopback/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FDCAN_Loopback example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to configure the FDCAN to operate in loopback mode.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 110 MHz.

Then:
  - FDCAN module is configured to operate in external loopback mode,
    with Nominal Bit Rate of 1 MBit/s and Data Bit Rate of 8 MBit/s.
  - FDCAN module is configured to receive:
     - messages with pre-defined standard ID to Rx FIFO 0
     - messages with pre-defined extended ID to Rx FIFO 1
  - FDCAN module is started.
  - The following messages are then sent:
     - one standard ID message matching Rx FIFO 0 filter
     - two extended ID messages matching Rx FIFO 1 filter

Main program checks that the three messages are received as expected
If the result is OK, LED4 start blinking
If the messages are not correctly received, or if at any time of the process an error is encountered,
LED5 is turned ON.

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

FDCAN, Loopback, Polling

@par Directory contents

  - FDCAN/FDCAN_Loopback/Inc/stm32l552e_eval_conf.h     BSP configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32l5xx_it.h          Header for stm32l5xx_it.c
  - FDCAN/FDCAN_Loopback/Inc/main.h                  Header for main.c module
  - FDCAN/FDCAN_Loopback/Src/stm32l5xx_it.c          Interrupt handlers
  - FDCAN/FDCAN_Loopback/Src/main.c                  Main program
  - FDCAN/FDCAN_Loopback/Src/stm32l5xx_hal_msp.c     HAL MSP module
  - FDCAN/FDCAN_Loopback/Src/system_stm32l5xx.c      stm32l5xx system source file

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with an STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.

  - STM32L552E-EV set-up:
		- Jumper JP17 => fitted on position [1-2] to select high-speed mode operation of the CAN transeiver(on each board).
		- Jumper JP18 => fitted to connect terminal resistor on CAN physical link (on each board).
      
@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
