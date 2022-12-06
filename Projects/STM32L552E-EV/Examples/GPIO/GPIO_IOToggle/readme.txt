/**
  @page GPIO_IOToggle GPIO IO Toggle example

  @verbatim
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the GPIO IO Toggle example.
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

How to configure and use GPIOs through the HAL API.

PB.03 IO (configured in output pushpull mode) toggles in a forever loop.
On STM32L552E-EV board this IO is connected to LED4.

In this example, HCLK is configured at 110 MHz.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The example needs to ensure that the SysTick time base is always set to 1 millisecond
to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

System, GPIO, Input, Output, Alternate function, Push-pull, Toggle

@par Directory contents

  - GPIO/GPIO_IOToggle/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - GPIO/GPIO_IOToggle/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - GPIO/GPIO_IOToggle/Inc/main.h                  Header for main.c module
  - GPIO/GPIO_IOToggle/Src/stm32l5xx_it.c          Interrupt handlers
  - GPIO/GPIO_IOToggle/Src/stm32l5xx_hal_msp.c     MSP initializations and de-initializations
  - GPIO/GPIO_IOToggle/Src/main.c                  Main program
  - GPIO/GPIO_IOToggle/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices without security enabled (TZEN=0).

  - This example has been tested with STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.


@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
