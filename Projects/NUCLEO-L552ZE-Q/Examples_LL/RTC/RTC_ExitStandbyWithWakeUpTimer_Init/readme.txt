/**
  @page RTC_ExitStandbyWithWakeUpTimer_Init RTC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_ExitStandbyWithWakeUpTimer_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC example.
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
The peripheral initialization uses LL unitary service functions for optimization purposes (performance and size).
  
In this example, after start-up, SYSCLK is configured to the max frequency using
the PLL with MSI as clock source.

LSE oscillator clock is used as RTC clock source.
The user can use also LSI as RTC clock source by uncommenting the adequate 
line on the main.c file.
 @code
   #define RTC_CLOCK_SOURCE_LSI  
   /* #define RTC_CLOCK_SOURCE_LSE */
 @endcode
LSI oscillator clock is delivered by a 32 kHz RC.
LSE is delivered by a 32.768 kHz crystal.

The EXTI_Line17 is connected internally to the RTC Wake-Up Timer (WUT) event.

Example execution:
	The RTC Wake-Up Timer (WUT) is configured to generate an event every 3 seconds and the system enters in STANDBY mode.
	The program enters again in STANDBY mode after each wake-up.
	After 5 wake-up (5 * 3 seconds = 15 seconds), the program checks that the RTC seconds value is also 15 seconds.
	It means that the test is OK.
	
One of the below scenario can occur :
	 - LED_GREEN (pin PC.07) toggles for ever : an error occurs.
	 - LED_GREEN (pin PC.07) toggles 2 times : wake-up from standby.
	 - LED_GREEN (pin PC.07) is ON after 15 seconds : Test is over, result is OK.	

@note This example can not be used in DEBUG mode, this is due to the fact
      that the core is no longer clocked during low power mode
      so debugging features are disabled.
      To use the DEBUG mode uncomment __DBGMCU_EnableDBGStandbyMode in main() but
      current consumption in STANDBY mode will increase a lot.	 



@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

System, RTC, RTC Wakeup timer, Standby mode, LSE, LSI, Interrupt

@par Directory contents 

  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Inc/main.h                  		 Header for main.c module
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Inc/stm32_assert.h          		 Template file to include assert_failed function
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/stm32l5xx_it.c          Interrupt handlers
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/main.c                        Main program
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
