/**
  @page TouchSensing_1touchkey example
 
  @verbatim
  ******************************************************************************
  * @file    TouchSensing\TouchSensing_1touchkey\readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TouchSensing 1 touchkey sensor example.
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       http://www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description

Use of the STMTouch driver with 1 touchkey sensor.
This application also uses the Environment Change System (ECS) and
Detection Time Out (DTO).

Observed behaviour:

- The LED4 is ON when the TS1 sensor is touched.
- The LED6 blinks each time the ECS process is finished.
- The LED5 blinks continuously in case of error.

- On the LCD are also displayed:
  * The ECS state (ON/OFF).
  * The TS1 touchkey sensor state (RELEASE, DETECT, ...) and the delta value.

- The ECS is ON when no touch is detected (sensor is in the RELEASE state).
  The ECS is always present but its behaviour can be modified using some parameters in
  the tsl_conf.h file.

- You can experiment the DTO by touching a sensor for few seconds. The RELEASE state
  is automatically entered and a re-calibration is performed after the timeout is reached.
  This timeout is programmable by the TSLPRM_DTO parameter in the tsl_conf.h file.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Touch sensing, TouchSensing, TSC, Polling

@par Project Directory contents

    - TouchSensing_1touchkey\Inc\main.h                Header for main.c file
    - TouchSensing_1touchkey\Inc\stm32l5xx_hal_conf.h  HAL Library configuration file
    - TouchSensing_1touchkey\Inc\stm32l5xx_it.h        Header for stm32l5xx_it.c file
    - TouchSensing_1touchkey\Inc\stmCriticalSection.h  Header for stmCriticalSection.c file
    - TouchSensing_1touchkey\Inc\tsl_conf.h            STMTouch driver configuration file
    - TouchSensing_1touchkey\Inc\tsl_user.h            Header for tsl_user.c file

    - TouchSensing_1touchkey\Src\main.c                Main program file
    - TouchSensing_1touchkey\Src\stm32l5xx_hal_msp.c   HAL MSP module file
    - TouchSensing_1touchkey\Src\stm32l5xx_it.c        Interrupt handlers file
    - TouchSensing_1touchkey\Src\stmCriticalSection.c  STMStudio lock/unlock mechanism file
    - TouchSensing_1touchkey\Src\system_stm32l5xx.c    System initialization file
    - TouchSensing_1touchkey\Src\tsl_user.c            Touchsensing channels/banks description file
    
@par Hardware and Software environment

  - This application runs on STM32L552ZETxQ devices.
    
  - This example has been tested with STM32L552E-EV board. It can be
    easily tailored to any other devices that embed the TSC peripheral and to
    any other development board supporting touchsensing sensor.

  - STM32L552E-EV Set-up :
    - Make sure that TFT-LCD MB989C is mounted on the board (LCD CN18).
    
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
