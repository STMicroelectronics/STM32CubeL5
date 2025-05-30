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

This application implements two tasks in Privileged mode using the xTaskCreate() API functions. 

The first task "WriteTask" alternate the privilieged variable ("VAR") value every 500ms between '0' 
and '1'. When user push on "USER button" The task update "VAR" value to '-1' and reset it self to user mode.

The second task "ReadTask" :

  - Turn off LED1 when VAR = 0;
  - Turn on LED1 when VAR = 1;
  - Toggles LED3 every 500ms when VAR = -1 (WriteTask is in user mode).

This project is targeted to run on STM32L552ZEQ device on NUCLEO-L552ZE-Q board from STMicroelectronics.

The reference application project configures the maximum system clock frequency at 110Mhz.

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
    - FreeRTOS/FreeRTOS_MPU/Inc/main.h                           Main program header file
    - FreeRTOS/FreeRTOS_MPU/Inc/stm32l5xx_hal_conf.h             HAL Library Configuration file
    - FreeRTOS/FreeRTOS_MPU/Inc/stm32l5xx_it.h                   Interrupt handlers header file
    - FreeRTOS/FreeRTOS_MPU/Inc/FreeRTOSConfig.h                 FreeRTOS Configuration file
    - FreeRTOS/FreeRTOS_MPU/Inc/stm32l552e_nucleo_conf.h         NUCLEO-L552ZE-Q BSP config file

@par Hardware and Software environment

  - This application runs on STM32L552ZETxQ devices without security enabled (TZEN=0).

  - This application has been tested with STMicroelectronics NUCLEO-L552ZE-Q board 
    and can be easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
