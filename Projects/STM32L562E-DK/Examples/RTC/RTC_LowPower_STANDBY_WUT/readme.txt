/**
  @page RTC_LowPower_STANDBY_WUT example

  @verbatim
  ******************************************************************************
  * @file    RTC/RTC_LowPower_STANDBY_WUT/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the RTC STANDBY WUT example.
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

How to periodically enter and wake up from STANDBY mode thanks to the RTC Wake-Up Timer (WUT).

In the associated software, the system clock is set to 110 MHz the SysTick is programmed to
generate an interrupt each 1 ms.
The LSE clock is used as RTC clock source by default.
The EXTI_Line17 is connected internally to the RTC Wake-Up Timer (WUT) event.

The RTC Wake-Up Timer (WUT) is configured to generate an event every 3 seconds and the system enters in STANDBY mode.
The program enters again in STANDBY mode after each wake-up.
After 5 wake-up (5 * 3 seconds = 15 seconds), the program checks that the RTC seconds value is also 15 seconds.
It means that the test is OK.

One of the below scenario can occur :
 - LED_GREEN toggles for ever : an error occurs.
 - LED_GREEN toggles 2 times : wake-up from standby.
 - LED_GREEN is ON after 15 seconds : Test is over, result is OK.

@note This example can not be used in DEBUG mode, this is due to the fact
      that the Cortex-M33 core is no longer clocked during low power mode
      so debugging features are disabled.
      To use the DEBUG mode uncomment HAL_DBGMCU_EnableDBG in main() but
      current consumption in STANDBY mode will be increased a lot.

@note Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
      the RTC clock source; in this case the Backup domain will be reset in
      order to modify the RTC Clock source, as consequence RTC registers (including
      the backup registers) and RCC_CSR register are set to their reset values.

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

RTC, Wake Up, wakeuptimer, Standby, LSE

@par Directory contents

  - RTC/RTC_LowPower_STANDBY_WUT/Inc/stm32l5xx_hal_conf.h     HAL configuration file
  - RTC/RTC_LowPower_STANDBY_WUT/Inc/stm32l5xx_it.h           Header for stm32l5xx_it.c
  - RTC/RTC_LowPower_STANDBY_WUT/Inc/main.h                   Header file for main.c
  - RTC/RTC_LowPower_STANDBY_WUT/Src/system_stm32l5xx.c       STM32L5xx system clock configuration file
  - RTC/RTC_LowPower_STANDBY_WUT/Src/stm32l5xx_it.c           Interrupt handlers
  - RTC/RTC_LowPower_STANDBY_WUT/Src/main.c                   Main program
  - RTC/RTC_LowPower_STANDBY_WUT/Src/stm32l5xx_hal_msp.c      HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32L562QEIxQ devices without security enabled (TZEN=0).

  - This example has been tested with STMicroelectronics STM32L562E (144 pins)
    diso board and can be easily tailored to any other supported device
    and development board.

 @par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
