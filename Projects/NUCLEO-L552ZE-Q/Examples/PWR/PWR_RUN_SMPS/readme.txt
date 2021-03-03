/**
  @page PWR_RUN_SMPS Power Mode Example

  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_RUN_SMPS/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the Power RUN SMPS Mode example.
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

How to use the SMPS step down converter in RUN mode.

From system reset, it illustrates the recommended transitions that make use of combinations between
VOS range, System clock frequency and SMPS HP/LP modes.

In order to keep the example simple SMPS bypass <-> SMPS ON (HP/LP) transitions are not covered in this example.

This example allows to compare the power consumption amongst different settings.

The examples has 12 steps (run in loop step 1->12 then ->4->12 etc.).

Each step has a specific combination of SMPS configuration, frequency in use,
and regulator in use (VOS Range).

1) SMPS configuration among
- Bypass (SMPS is not providing power supply)
- SMPS High-power mode (HPM): achieving a high efficiency at high current load.
- SMPS Low-power mode (LPM): achieving a high efficiency at low current load.

2) Frequency in use (some with use of PLL)

3) VOS range
- Main regulator Range 0: high performance;
  - It provides a typical output voltage at 1.28 V.
  - It is used when the system clock frequency is up to 110 MHz.
  - The Flash access time for read access is minimum, write and erase
    operations are possible.
- Main regulator Range 1: medium performance;
  - It provides a typical output voltage at 1.2 V.
  - It is used when the system clock frequency is up to 80 MHz.
  - The Flash access time for read access is minimum, write and erase
    operations are possible.
- Main regulator Range 2: low-power range.
  - It provides a typical output voltage at 1.0 V.
  - The system clock frequency can be up to 26 MHz.
  - The Flash access time for a read access is increased as compared to Range
    1; write and erase operations are not possible.

The 12 steps highlight:

- How to reach (& measure the best performing power consumption setting at three
operating frequencies):

  A) 24 MHz
  with
  - SMPS Low-power mode (LPM)
  - Main regulator in Range 2.
  Illustrated at step 12 that lasts 20 seconds

  B) 80 MHz
  with
  - SMPS High-power mode (HPM)
  - Main regulator in Range 1.
  Illustrated at step 6 that lasts 20 seconds

  C) 110 MHz
  with
  - SMPS High-power mode (HPM)
  - Main regulator in Range 0.
  Illustrated at step 9 that lasts 20 seconds

- the steps to go into these configurations.

@note To measure the current consumption in RUN mode, please connect to JP5.

@note This example in not as relevant in DEBUG mode due to the fact
      that the power consumption figures are impacted.

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

Power, PWR, Run mode, SMPS, Voltage scaling, Dynamic frequency change

@par Directory contents

  - PWR/PWR_RUN_SMPS/Inc/stm32l5xx_nucleo_conf.h  BSP configuration file
  - PWR/PWR_RUN_SMPS/Inc/stm32l5xx_conf.h         HAL Configuration file
  - PWR/PWR_RUN_SMPS/Inc/stm32l5xx_it.h           Header for stm32l5xx_it.c
  - PWR/PWR_RUN_SMPS/Inc/main.h                   Header file for main.c
  - PWR/PWR_RUN_SMPS/Src/system_stm32l5xx.c       STM32L5xx system clock configuration file
  - PWR/PWR_RUN_SMPS/Src/stm32l5xx_it.c           Interrupt handlers
  - PWR/PWR_RUN_SMPS/Src/stm32l5xx_hal_msp.c      HAL MSP module
  - PWR/PWR_RUN_SMPS/Src/main.c                   Main program

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices without security enabled (TZEN=0).

  - This example has been tested with This example has been tested with
    STMicroelectronics NUCLEO-L552ZE-Q MB1361-A (144 pins)
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Disable the IDE
 - Start power meter device (connected @ JP5).
 - Reset the board and start measurements ().
 - You may change "DELAY_FOR_POWER_MEASUREMENTS" as per your test preferences.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
