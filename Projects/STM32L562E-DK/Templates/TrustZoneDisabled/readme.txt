/**
  @page Templates  Description of the Templates example for TZEN=0 devices

  @verbatim
  ******************************************************************************
  * @file    Templates/TrustZoneDisabled/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the Templates example for TZEN=0 devices.
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

This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when security is not enabled (TZEN=0).

This project is targeted to run on STM32L562QE-Q device on STM32L562E-DK board from STMicroelectronics.

The reference template project configures the maximum system clock frequency at 110Mhz.

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

Reference, Template, TrustZone disabled

@par Directory contents

  - Templates/TrustZoneDisabled/Src/main.c                 Main program
  - Templates/TrustZoneDisabled/Src/system_stm32l5xx.c     STM32L5xx system clock configuration file
  - Templates/TrustZoneDisabled/Src/stm32l5xx_it.c         Interrupt handlers
  - Templates/TrustZoneDisabled/Src/stm32l5xx_hal_msp.c    HAL MSP module
  - Templates/TrustZoneDisabled/Inc/main.h                 Main program header file
  - Templates/TrustZoneDisabled/Inc/stm32l562xx_discovery_conf.h  BSP Configuration file
  - Templates/TrustZoneDisabled/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - Templates/TrustZoneDisabled/Inc/stm32l5xx_it.h         Interrupt handlers header file


@par Hardware and Software environment

  - This template runs on STM32L562QEIxQ devices without security enabled (TZEN=0).

  - This template has been tested with STMicroelectronics STM32L562E-DK (MB1373)
    board and can be easily tailored to any other supported device
    and development board.


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
