/**
  @page RCC_ClockConfig_TrustZone RCC Clock Config with Trustzone example

  @verbatim
  ******************************************************************************
  * @file    RCC/RCC_ClockConfig_TrustZone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the RCC Clock Config with Trustzone example.
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

Configuration of the system clock (SYSCLK) in Run mode from the secure application
upon request from the non-secure application, using the RCC HAL API when TrustZone security is activated (Option bit TZEN=1).

In this example, after startup, the SYSCLK is configured to 4Mhz by the secure application
and the PLL, SYSCLK and AHB/APB prescaler are set as secure. The secure application
provides then non-secure callable functions to either configure the SYSCLK to maximum
frequency using the PLL with MSI as clock source or configure the SYSCLK back to the 
4Mhz reset value. The User push-button (connected to External line 13) will be
used by the non-secure application to change alternatively the SYSCLK frequency:
- from 4Mhz with MSI to 110Mhz with main PLL based on MSI
- from 110Mhz to 4Mhz from MSI.

Each time the User push-button is pressed External line 13 interrupt is generated in the
non-secure application and a flag raised to instruct the non-secure main program to request
a switch of SYSCLK configuration to the secure application.

- If the 110Mhz clock is selected, the following steps will be followed to switch
   the SYSCLK source to PLL with MSI as oscillator clock source:
     a- Configure the Power voltage scaling range for the 110Mhz target frequency 
     b- Configure the PLL with MSI as source and activate the PLL 
     c- Select the PLL as system clock source and apply the output frequency to the
	    HCLK, PCLK1 and PCLK2 with the adapted flash access latency.

- If the 4Mhz clock is selected, the following steps will be followed to switch
   the SYSCLK source to MSI:
     a- Select the MSI as system clock source with adapted flash access latency
     b- Deactivate the PLL
     c- Configure the Power voltage scaling range for the 4Mhz frequency 

LED1 is toggled with a timing defined by the HAL_Delay() API: slower for 4Mhz SYSCLK or faster at 110Mhz
LED3 is switched on by the secure application in case of error.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The secure and non-secure applications need to ensure that the SysTick time base is always
      set to 1 millisecond to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Security, TrustZone, RCC, System, Clock Configuration, System clock, Oscillator, PLL

@par Directory contents

  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Src/main.c                     Secure Main program
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Src/stm32l5xx_hal_msp.c        Secure HAL MSP module
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Src/stm32l5xx_it.c             Secure Interrupt handlers
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Src/system_stm32l5xx_s.c       Secure STM32L5xx system clock configuration file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Inc/main.h                     Secure Main program header file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Inc/partition_stm32l552xx.h    STM32L5 Device System Configuration file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Inc/stm32l5xx_hal_conf.h       Secure HAL Configuration file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure/Inc/stm32l5xx_it.h             Secure Interrupt handlers header file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/Secure_nsclib/secure_nsc.h            Secure Non-Secure Callable (NSC) module header file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/NonSecure/Src/main.c                  Non-secure Main program
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/NonSecure/Src/stm32l5xx_hal_msp.c     Non-secure HAL MSP module
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/NonSecure/Src/stm32l5xx_it.c          Non-secure Interrupt handlers
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/NonSecure/Src/system_stm32l5xx_ns.c   Non-secure STM32L5xx system clock configuration file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/NonSecure/Inc/main.h                  Non-secure Main program header file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/NonSecure/Inc/stm32l5xx_hal_conf.h    Non-secure HAL Configuration file
  - RCC/RCC_ClockConfig_TrustZone/TrustZone/NonSecure/Inc/stm32l5xx_it.h          Non-secure Interrupt handlers header file

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices with security enabled (TZEN=1).

  - This example has been tested with NUCLEO-L552ZE-Q (MB1361-C02) 
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)
     TZEN = 1                            System with TrustZone-M enabled
     DBANK = 1                           Dual bank mode
     SECWM1_PSTRT=0x0  SECWM1_PEND=0x7F  All 128 pages of internal Flash Bank1 set as secure
     SECWM2_PSTRT=0x1  SECWM2_PEND=0x0   No page of internal Flash Bank2 set as secure, hence Bank2 non-secure


@par How to use it ?

In order to make the program work, you must do the following :

 - Make sure that the system is configured with the security enable (TZEN=1) (option byte)

IAR
 - Open your toolchain
 - Open Multi-projects workspace file Project.eww
 - Rebuild xxxxx_S project
 - Rebuild xxxxx_NS project
 - Set the "xxxxx_NS" as active application (Set as Active)
 - Flash the non-secure binary with Project->Download->Download active application
   (this shall download the \EWARM\xxxxx_NS\Exe\xxxxx_NS.out to flash memory)
 - Set the "xxxxx_S" as active application (Set as Active)
 - Flash the secure binary with Project->Download->Download active application
   (this shall download the \Secure_nsclib\xxxxx_S.out to flash memory)
 - Run the example


MDK-ARM

 - Open your toolchain
 - Open Multi-projects workspace file Project.uvmpw
 - Select the xxxxx_S project as Active Project (Set as Active Project)
 - Build xxxxx_S project
 - Select the xxxxx_NS project as Active Project (Set as Active Project)
 - Build xxxxx_NS project
 - Load the non-secure binary (F8)
   (this shall download the \MDK-ARM\xxxxx_ns\Exe\Project_ns.axf to flash memory)
 - Select the xxxxx_S project as Active Project (Set as Active Project)
 - Load the secure binary (F8)
   (this shall download the \MDK-ARM\xxxxx_s\Exe\Project_s.axf to flash memory)
 - Run the example


STM32CubeIDE
 - Open STM32CubeIDE
 - File > Import. Point to the STM32CubeIDE folder of the example project. Click Finish.
 - Build configuration: Set the same active build configuration: Debug (default) or Release for both projects xxxxx_S & xxxxx_NS
 - Select and build the xxxxx_NS project, this will automatically trigger the build of xxxxx_S project
 - Select the xxxxx_S project and select "Debug configuration" or "Run configuration" in function of the active build configuration
   - Double click on “STM32 Cortex-M C/C++ Application”
   - Select  “Startup” >  “Add” > 
     - Select the xxxxx_NS project 
 - Click Debug/Run to debug/run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
