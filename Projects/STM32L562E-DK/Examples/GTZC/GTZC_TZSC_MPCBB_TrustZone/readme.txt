/**
  @page GTZC_TZSC_MPCBB_TrustZone GTZC TZSC-MPCBB example with secure fault with TZEN=1

  @verbatim
  ******************************************************************************
  * @file    GTZC/GTZC_TZSC_MPCBB_TrustZone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the GTZC_TZSC_MPCBB_TrustZone example with TZEN=1
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

How to use HAL GTZC MPCBB to build any example with SecureFault detection when TrustZone
security is activated (Option bit TZEN=1).

The purpose of this example is to declare the 2nd half of SRAM1 (from 0x20018000) as non-secure
with HAL GTZC MPCBB services and to demonstrate that a security violation is detected when
non-secure application accesses secure SRAM1 memory.

The non-secure application toggles LED10 every second until the security violation is detected.

The security violation detection at SAU level is triggered by two means:
- automatically after 5 seconds
- before through the USER push-button key press
Any key press on USER push-button initiates an access in secure SRAM1 (SRAM1 base address).
The secure fault is detected from the secure application SecureFault_Handler which calls a
non-secure registered callback function to inform the non-secure application. The non-secure
application switches on the error LED9.

This project is composed of two sub-projects:
- one for the secure application part (xxxxx_S)
- one for the non-secure application part (xxxxx_NS).

Please remember that on system with security enabled:
- the system always boots in secure and the secure application is responsible for
launching the non-secure application.
- the SystemInit() function in secure application sets up the SAU/IDAU, FPU and
secure/non-secure interrupts allocations defined in partition_stm32l562xx.h file.

This project shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories
into two halves:
 - the first half for the secure application and
 - the second half for the non-secure application.

User Option Bytes configuration:
Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes
SECWM1_PSTRT/SECWM1_PEND and SECWM2_PSTRT/SECWM2_PEND should be set according to the application
configuration. Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:
    TZEN=1
    DBANK=1
    SECWM1_PSTRT=0x0  SECWM1_PEND=0x7F  meaning all 128 pages of Bank1 set as secure
    SECWM2_PSTRT=0x1  SECWM2_PEND=0x0   meaning no page of Bank2 set as secure, hence Bank2 non-secure

This project is targeted to run on STM32L5 device on boards from STMicroelectronics.

This example configures the maximum system clock frequency at 110Mhz.

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

Security, TrustZone, GTZC, MPCBB, Memory protection, Block-Based, Memory, internal SRAM, illegal access 

@par Directory contents

  - Secure/Src/main.c                            Secure Main program
  - Secure/Src/secure_nsc.c                      Secure Non-secure callable functions
  - Secure/Src/stm32l5xx_hal_msp.c               Secure HAL MSP module
  - Secure/Src/stm32l5xx_it.c                    Secure Interrupt handlers
  - Secure/Src/system_stm32l5xx_s.c              Secure STM32L5xx system clock configuration file
  - Secure/Inc/main.h                            Secure Main program header file
  - Secure/Inc/partition_stm32l562xx.h           STM32L5 Device System Configuration file
  - Secure/Inc/stm32l5xx_hal_conf.h              Secure HAL Configuration file
  - Secure/Inc/stm32l5xx_it.h                    Secure Interrupt handlers header file
  - Secure_nsclib/secure_nsc.h                   Secure Non-Secure Callable (NSC) module header file
  - NonSecure/Src/main.c                         Non-secure Main program
  - NonSecure/Src/stm32l5xx_hal_msp.c            Non-secure HAL MSP module
  - NonSecure/Src/stm32l5xx_it.c                 Non-secure Interrupt handlers
  - NonSecure/Src/system_stm32l5xx_ns.c          Non-secure STM32L5xx system clock configuration file
  - NonSecure/Inc/main.h                         Non-secure Main program header file
  - NonSecure/Inc/stm32l5xx_hal_conf.h           Non-secure HAL Configuration file
  - NonSecure/Inc/stm32l5xx_it.h                 Non-secure Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32L562QEIxQ devices with security enabled (TZEN=1).

  - This example has been tested with STM32L562E-DK (MB1373)
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
 - Select the Project_s project as Active Project (Set as Active Project)
 - Build Project_s project
 - Select the Project_ns project as Active Project (Set as Active Project)
 - Build Project_ns project
 - Load the non-secure binary (F8)
   (this shall download the \MDK-ARM\xxxxx_ns\Exe\Project_ns.axf to flash memory)
 - Select the Project_s project as Active Project (Set as Active Project)
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
 NOTE:
   - The default Debug configuration runtime option sets "Halt on exception" which 
     makes the code execution halt on SecureFault_Handler() on voluntary security violation.
     Press "Resume (F8)" to get the expected error led switched on.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
