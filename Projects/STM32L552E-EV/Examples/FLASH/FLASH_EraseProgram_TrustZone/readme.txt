/**
  @page FLASH_EraseProgram_TrustZone FLASH Erase and Program example with TrustZone enabled

  @verbatim
  ******************************************************************************
  * @file    FLASH/FLASH_EraseProgram_TrustZone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the Templates example for TZEN=1 devices.
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

How to configure and use the FLASH HAL API to erase and program the internal
Flash memory when TrustZone security is activated (Option bit TZEN=1).

This project is composed of two sub-projects:
- one for the secure application part (FLASH_EraseProgram_TrustZone_S)
- one for the non-secure application part (FLASH_EraseProgram_TrustZone_NS).

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.

The system isolation splits the internal Flash and internal SRAM memories into two halves:
 - the first half of Flash is used for the secure application code,
 - the second half of Flash is used for the non-secure application code,
 - the first half of SRAM is used for the secure application data,
 - the second half of SRAM is used for the non-secure application data.

Secure Application :
After Reset, the Flash memory Program/Erase Controller is locked. HAL_FLASH_Unlock() function
is used to enable the FLASH control register access.
Before programming the desired secured addresses, an erase operation is performed using
the flash erase page feature. The erase procedure is done by filling the erase init
structure giving the starting erase page and the number of pagess to erase.
At this stage, all these pages will be erased one by one separately.

@note: if problem occurs on a page, erase will be stopped and faulty page will
be returned to user (through variable 'PageError').

@note: although code is the same for page erase between secure and non secure project, it matters 
to mention that secure firmware is only able to erase secure pages within User Flash (Bank1) 
and non-secure firmware is only able to erase non-secure pages within User Flash (Bank2).

Once this operation is finished, page double-word programming operation will be performed
in the secure part of Flash memory. The written data is then read back and checked.

Then the non-secure application is launched.


Non-Secure Application :
Same erase procedure is done on non-secured addresses. 

@note: if problem occurs on a page, erase will be stopped and faulty page will
be returned to user (through variable 'PageError').

Once this operation is finished, page double-word programming operation will be performed
in the non-secure part of the Flash memory. The written data is then read back and checked.


User Option Bytes configuration:
Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes
SECWM1_PSTRT/SECWM1_PEND and SECWM2_PSTRT/SECWM2_PEND should be set according to the application
configuration. Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:
    TZEN=1
    DBANK=1
    SECWM1_PSTRT=0x0  SECWM1_PEND=0x7F  meaning all 128 pages of Bank1 set as secure
    SECWM2_PSTRT=0x1  SECWM2_PEND=0x0   meaning no page of Bank2 set as secure, hence Bank2 set as non-secure


This project is targeted to run on STM32L5 device on boards from STMicroelectronics.

The project configures the maximum system clock frequency at 110Mhz in non-secure application.

The STM32L552E-EV (MB1372) board LEDs can be used to monitor the transfer status:
 - LED4 is ON when there are no errors detected after programming in secure area
 - LED4 is blinking with a 1s period when there are errors detected after programming in secure area
 - LED4 is blinking (100ms on, 2s off) when there is an issue during erase or program procedure in secure area
 - LED5 is ON when there are no errors detected after programming in non-secure area
 - LED5 is blinking with a 1s period when there are errors detected after programming in non-secure area
 - LED5 is blinking (100ms on, 2s off) when there is an issue during erase or program procedure in non-secure area

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

Security, TrustZone, Memory, Flash, Erase, Program

@par Directory contents

  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Src/main.c                     Secure Main program
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Src/stm32l5xx_hal_msp.c        Secure HAL MSP module
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Src/stm32l5xx_it.c             Secure Interrupt handlers
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Src/system_stm32l5xx_s.c       Secure STM32L5xx system clock configuration file
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Inc/main.h                     Secure Main program header file
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Inc/partition_stm32l552xx.h    STM32L5 Device System Configuration file
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Inc/stm32l5xx_hal_conf.h       Secure HAL Configuration file
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Inc/stm32l5xx_it.h             Secure Interrupt handlers header file
  - FLASH/FLASH_EraseProgram_TrustZone/Secure/Inc/stm32l552e_eval_conf.h     Secure BSP configuration file
  - FLASH/FLASH_EraseProgram_TrustZone/Secure_nsclib/secure_nsc.h            Secure Non-Secure Callable (NSC) module header file
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Src/main.c                  Non-secure Main program
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Src/stm32l5xx_hal_msp.c     Non-secure HAL MSP module
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Src/stm32l5xx_it.c          Non-secure Interrupt handlers
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Src/system_stm32l5xx_ns.c   Non-secure STM32L5xx system clock configuration file
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Inc/main.h                  Non-secure Main program header file
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Inc/stm32l552e_eval_conf.h  Non-secure BSP Configuration file
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Inc/stm32l5xx_hal_conf.h    Non-secure HAL Configuration file
  - FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Inc/stm32l5xx_it.h          Non-secure Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices with security enabled (TZEN=1).

  - This example has been tested with STMicroelectronics STM32L552E-EV (MB1372)
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)
     TZEN = 1                            System with TrustZone-M enabled
     DBANK = 1                           Dual bank mode
     SECWM1_PSTRT=0x0  SECWM1_PEND=0x7F  All 128 pages of internal Flash Bank1 set as secure
     SECWM2_PSTRT=0x1  SECWM2_PEND=0x0   No page of internal Flash Bank2 set as secure, hence Bank2 non-secure


@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :

 - Make sure that the system is configured with the security enable (TZEN=1) (option byte)

IAR
 - Open your toolchain
 - Open Multi-projects workspace file Project.eww
 - Rebuild FLASH_EraseProgram_TrustZone_S project
 - Rebuild FLASH_EraseProgram_TrustZone_NS project
 - Set the "FLASH_EraseProgram_TrustZone_NS - Debug" as active application (Set as Active)
 - Flash the non-secure binary with Project->Download->Download active application
   (this shall download the \EWARM\FLASH_EraseProgram_TrustZone_NS\Exe\FLASH_EraseProgram_TrustZone_NS.out to flash memory)
 - Set the "FLASH_EraseProgram_TrustZone_S - Debug" as active application (Set as Active)
 - Flash the secure binary with Project->Download->Download active application
   (this shall download the \Secure_nsclib\FLASH_EraseProgram_TrustZone_S.out to flash memory)
 - Run the example


MDK-ARM

 - Open your toolchain
 - Open Multi-projects workspace file Project.uvmpw
 - Select the FLASH_EraseProgram_TrustZone_S project as Active Project (Set as Active Project)
 - Build FLASH_EraseProgram_TrustZone_S project
 - Select the FLASH_EraseProgram_TrustZone_NS project as Active Project (Set as Active Project)
 - Build FLASH_EraseProgram_TrustZone_NS project
 - Load the non-secure binary (F8)
   (this shall download the \MDK-ARM\Objects\FLASH_EraseProgram_TrustZone_NS.axf to flash memory)
 - Select the FLASH_EraseProgram_TrustZone_S project as Active Project (Set as Active Project)
 - Load the secure binary (F8)
   (this shall download the \MDK-ARM\Objects\FLASH_EraseProgram_TrustZone_S.axf to flash memory)
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
