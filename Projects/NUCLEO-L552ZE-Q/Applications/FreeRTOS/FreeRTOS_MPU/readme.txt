/**
  @page FreeRTOS_MPU FreeRTOS MPU application

  @verbatim
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_MPU/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS MPU application.
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

@par Application Description

How to use the MPU feature of FreeRTOS.

This application demonstrates the use of the MPU with FreeRTOS to control memory/peripheral access for tasks.
It implements simple recovery mechanisms from prohibited memory accesses.
The application creates one Privileged mode task using xTaskCreate():

  - 'MainTask': Has higher priority than the unprivileged tasks.
                Blocks while waiting for a fault to occur.
                Displays information about the faults and takes corrective actions.
It also has User mode tasks created using the xTaskCreateRestricted() API:

  - 'Task1': Has read only access to shared memory.
             Attempts an illegal write to the shared memory only once.
             Toggles the green LED.
  - 'Task2': Has read only access to shared memory.
             Attempts an illegal write to the shared memory periodically.
             Toggles the blue LED.
The application will try to recover from MemManage fault exception for each task until EXAMPLE_FAULT_COUNT_THRESHOLD is reached for that task. At that point the task will be suspended to
avoid further exceptions.

Successful operation is marked by a toggling green led light, the blue LED will toggle `EXAMPLE_FAULT_COUNT_THRESHOLD` times, and detailed fault information will be printed to the serial port.

On failure, the red led toggles every 500ms.

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

RTOS, FreeRTOS, MPU

@par Directory contents

    - FreeRTOS/FreeRTOS_MPU/Src/main.c                           Main program
    - FreeRTOS/FreeRTOS_MPU/Src/stm32l5xx_it.c                   Interrupt handlers
    - FreeRTOS/FreeRTOS_MPU/Src/system_stm32l5xx.c               STM32L5xx system clock configuration file
    - FreeRTOS/FreeRTOS_MPU/Src/stm32l5xx_hal_timebase_tim.c     HAL TIM timebase implementation
    - FreeRTOS/FreeRTOS_MPU/Src/stm32l5xx_hal_msp.c              MSP Initialization and de-Initialization codes
    - FreeRTOS/FreeRTOS_MPU/Inc/main.h                           Main program header file
    - FreeRTOS/FreeRTOS_MPU/Inc/stm32l5xx_hal_conf.h             HAL Library Configuration file
    - FreeRTOS/FreeRTOS_MPU/Inc/stm32l5xx_it.h                   Interrupt handlers header file
    - FreeRTOS/FreeRTOS_MPU/Inc/FreeRTOSConfig.h                 FreeRTOS Configuration file

@par Hardware and Software environment

  - This application runs on STM32L552ZETxQ devices without security enabled (TZEN=0).

  - This application has been tested with STMicroelectronics NUCLEO-L552ZE-Q board
    and can be easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up
    Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.
      A virtual COM port will then appear in the HyperTerminal.

  - Hyperterminal configuration:
    - Data Length = 8 Bits
    - One Stop Bit
    - No parity
    - BaudRate = 115200 baud
    - Flow control: None

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
