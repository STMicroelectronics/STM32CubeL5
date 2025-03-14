/**
  @page FreeRTOS_ThreadCreation FreeRTOS Thread Creation example
 
  @verbatim
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS Thread Creation example.
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

How to implement thread creation using CMSIS RTOS API. 

This application creates two threads with the same priority, which execute in 
a periodic cycle of 15 seconds. 

In the first 5 seconds, the thread 1 toggles LED4 each 200 ms and the 
thread 2 toggles LED5 each 500 ms.
In the following 5 seconds, the thread 1 suspends itself and the thread 2
continue toggling LED5.
In the last 5 seconds, the thread 2 resumes execution of thread 1 then 
suspends itself, the thread 1 toggles the LED4 each 500 ms.    

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority()
      function.
 
@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

@note The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the 
      OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications 
on STM32Cube with RTOS".

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

RTOS, FreeRTOS, Threading

@par Directory contents
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/main.c                       Main program
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/app_FreeRTOS.c               Code for freertos applications
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/stm32l5xx_hal_timebase_tim.c HAL timebase file
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/stm32l5xx_it.c               Interrupt handlers
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/stm32l5xx_hal_msp.c          MSP Initialization file
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/system_stm32l5xx.c           STM32L5xx system clock configuration file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/main.h                       Main program header file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/stm32l5xx_hal_conf.h         HAL Library Configuration file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/stm32l552e_eval_hal_conf.h         BSP Configuration file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/stm32l5xx_it.h               Interrupt handlers header file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/FreeRTOSConfig.h             FreeRTOS Configuration file

@par Hardware and Software environment

  - This application runs on STM32L552ZETxQ devices.
    
  - This application has been tested with STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.
    

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
  

 */
