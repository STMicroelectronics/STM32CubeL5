/**
  @page FMC_SRAM_TrustZone FMC SRAM basic functionalities use example with TrustZone enabled

  @verbatim
  ******************************************************************************
  * @file    FMC/FMC_SRAM_TrustZone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FMC SRAM example for TZEN=1 devices.
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

How to configure the FMC controller to access the IS61WV51216BLL SRAM memory split between 
secure and non-secure applications when TrustZone security is activated (Option bit TZEN=1). .

This project is composed of two sub-projects:
- one for the secure application part (FMC_SRAM_TrustZone_S)
- one for the non-secure application part (FMC_SRAM_TrustZone_NS).

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.

The system isolation splits the internal Flash and internal SRAM memories into two halves:
 - the first half of Flash is used for the secure application code,
 - the second half of Flash is used for the non-secure application code,
 - the first half of SRAM is used for the secure application data,
 - the second half of SRAM is used for the non-secure application data.

Secure Application :
In this example, the SRAM device is configured and initialized explicitly
following all initialization sequence steps. After initializing the device, user
can perform read/write operations on it. A data buffer is written to the secure part
of SRAM memory, then read back and checked to verify its correctness.

Then the non-secure application is launched.


Non-Secure Application :
A data buffer is written to the non-secure part of SRAM memory, then read back 
and checked to verify its correctness.


User Option Bytes configuration:
Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes
SECWM1_PSTRT/SECWM1_PEND and SECWM2_PSTRT/SECWM2_PEND should be set according to the application
configuration. Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:
    TZEN=1
    DBANK=1
    SECWM1_PSTRT=0x0  SECWM1_PEND=0x7F  meaning all 128 pages of Bank1 set as secure
    SECWM2_PSTRT=0x1  SECWM2_PEND=0x0   meaning no page of Bank2 set as secure, hence Bank2 set as non-secure


The FMC memory bus width is set to 16bits.

This project is targeted to run on STM32L5 device on boards from STMicroelectronics.

The project configures the maximum system clock frequency at 110Mhz in non-secure application.

The STM32L552E-EV (MB1372) board LEDs can be used to monitor the transfer status:
 - LED4 is ON if the data is read correctly from SRAM by secure application
 - LED4 is blinking with a 1s period in case of issue in secure application
 - LED5 is ON if the data is read correctly from SRAM by non-secure application
 - LED5 is blinking with a 1s period in case of issue in non-secure application

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

Security, TrustZone, Memory, FMC, SRAM, Read, Write

@par Directory contents

 - FMC/FMC_SRAM_TrustZone/Secure/Src/main.c                      Secure Main program
 - FMC/FMC_SRAM_TrustZone/Secure/Src/secure_nsc.c                Secure Non-Secure Callable (NSC) module
 - FMC/FMC_SRAM_TrustZone/Secure/Src/stm32l5xx_hal_msp.c         Secure HAL MSP module
 - FMC/FMC_SRAM_TrustZone/Secure/Src/stm32l5xx_it.c              Secure Interrupt handlers
 - FMC/FMC_SRAM_TrustZone/Secure/Src/system_stm32l5xx_s.c        Secure STM32L5xx system clock configuration file
 - FMC/FMC_SRAM_TrustZone/Secure/Inc/main.h                      Secure Main program header file
 - FMC/FMC_SRAM_TrustZone/Secure/Inc/partition_stm32l552xx.h     STM32L5 Device System Configuration file
 - FMC/FMC_SRAM_TrustZone/Secure/Inc/stm32l5xx_hal_conf.h        Secure HAL Configuration file
 - FMC/FMC_SRAM_TrustZone/Secure/Inc/stm32l5xx_it.h              Secure Interrupt handlers header file
 - FMC/FMC_SRAM_TrustZone/Secure/Inc/stm32l552e_eval_conf.h      Secure BSP configuration file
 - FMC/FMC_SRAM_TrustZone/Secure_nsclib/secure_nsc.h             Secure Non-Secure Callable (NSC) module header file
 - FMC/FMC_SRAM_TrustZone/NonSecure/Src/main.c                   Non-secure Main program
 - FMC/FMC_SRAM_TrustZone/NonSecure/Src/stm32l5xx_hal_msp.c      Non-secure HAL MSP module
 - FMC/FMC_SRAM_TrustZone/NonSecure/Src/stm32l5xx_it.c           Non-secure Interrupt handlers
 - FMC/FMC_SRAM_TrustZone/NonSecure/Src/system_stm32l5xx_ns.c    Non-secure STM32L5xx system clock configuration file
 - FMC/FMC_SRAM_TrustZone/NonSecure/Inc/main.h                   Non-secure Main program header file
 - FMC/FMC_SRAM_TrustZone/NonSecure/Inc/stm32l552e_eval_conf.h   Non-secure BSP Configuration file
 - FMC/FMC_SRAM_TrustZone/NonSecure/Inc/stm32l5xx_hal_conf.h     Non-secure HAL Configuration file
 - FMC/FMC_SRAM_TrustZone/NonSecure/Inc/stm32l5xx_it.h           Non-secure Interrupt handlers header file


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
 - Rebuild FMC_SRAM_TrustZone_S project
 - Rebuild FMC_SRAM_TrustZone_NS project
 - Set the "FMC_SRAM_TrustZone_NS - Debug" as active application (Set as Active)
 - Flash the non-secure binary with Project->Download->Download active application
   (this shall download the \EWARM\FMC_SRAM_TrustZone_NS\Exe\FMC_SRAM_TrustZone_NS.out to flash memory)
 - Set the "FMC_SRAM_TrustZone_S - Debug" as active application (Set as Active)
 - Flash the secure binary with Project->Download->Download active application
   (this shall download the \Secure_nsclib\FMC_SRAM_TrustZone_S.out to flash memory)
 - Run the example

MDK-ARM

 - Open your toolchain
 - Open Multi-projects workspace file Project.uvmpw
 - Select the FMC_SRAM_TrustZone_S project as Active Project (Set as Active Project)
 - Build FMC_SRAM_TrustZone_S project
 - Select the FMC_SRAM_TrustZone_NS project as Active Project (Set as Active Project)
 - Build FMC_SRAM_TrustZone_NS project
 - Load the non-secure binary (F8)
   (this shall download the \MDK-ARM\Objects\FMC_SRAM_TrustZone_NS.axf to flash memory)
 - Select the FMC_SRAM_TrustZone_S project as Active Project (Set as Active Project)
 - Load the secure binary (F8)
   (this shall download the \MDK-ARM\Objects\FMC_SRAM_TrustZone_S.axf to flash memory)
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
