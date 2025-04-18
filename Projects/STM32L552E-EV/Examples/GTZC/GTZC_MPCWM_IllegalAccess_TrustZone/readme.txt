/**
  @page GTZC_MPCWM_IllegalAccess_TrustZone example with TZEN=1

  @verbatim
  ******************************************************************************
  * @file    GTZC/GTZC_MPCWM_IllegalAccess_TrustZone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the EVAL GTZC MPCWM-TZIC example with TZEN=1
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

@par Example Description

How to use GTZC MPCWM-TZIC to build any example when TrustZone security is activated (Option bit TZEN=1).

The purpose of this example is to first declare the 1st half of external SRAM (FMC)
(from 0x60000000) as non-secure with HAL GTZC MPCWM services and to demonstrate that
no security violation is detected when non-secure application accesses the 1st half
of external SRAM. LED GREEN toggles during this permanent read and write accesses.

In this example, the SAU region 4 used for external memory access policy is voluntary
modified in user code versus the initial setup from partition_stm32l552xx.h done in
secure SystemInit() in order to update the external SRAM (FMC) non-secure and secure areas.

Any key press on WKUP push-button initiates a SecureFault interrupt because of an
illegal read access from the non-secure world in secure external SRAM area as the
transaction does not respect the SAU access rule.
This toggles the LED RED once and a message is displayed on a terminal via a secure
Non-Secure Callable functions in secure (UART3 secured). LED GREEN toggling resumes.

Any key press on TAMPER push-button initiates a GZTC interrupt because of an
illegal read access from the non-secure world in external SRAM area although
the transaction is allowed by the SAU but not by the GTZC MPCWM.
This toggles the LED RED once and a message is displayed on a terminal via a secure
Non-Secure Callable functions in secure (UART3 secured). LED GREEN toggling resumes.

This project is composed of two sub-projects:
- one for the secure application part (Project_s)
- one for the non-secure application part (Project_ns).

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.

This project mainly shows how to switch from secure application to non-secure application
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

Security, TrustZone, GTZC, MPCWM, Non-secure memory watermark, external SRAM, illegal access 

@par Directory contents

  - Secure/Src/hal_sram_helper.c                 Helper file ton initialize and configure SRAM
  - Secure/Src/main.c                            Secure Main program
  - Secure/Src/secure_nsc.c                      Secure Non-secure callable functions
  - Secure/Src/stm32l5xx_hal_msp.c               Secure HAL MSP module
  - Secure/Src/stm32l5xx_it.c                    Secure Interrupt handlers
  - Secure/Src/system_stm32l5xx_s.c              Secure STM32L5xx system clock configuration file
  - Secure/Inc/hal_sram_helper.h                 Helper file ton initialize and configure SRAM
  - Secure/Inc/main.h                            Secure Main program header file
  - Secure/Inc/partition_stm32l552xx.h           STM32L5 Device System Configuration file
  - Secure/Inc/stm32l5xx_hal_conf.h              Secure HAL Configuration file
  - Secure/Inc/stm32l5xx_it.h                    Secure Interrupt handlers header file
  - Secure/Inc/stm32l552e_eval_conf.h            Secure EVAL configuration header file
  - Secure_nsclib/secure_nsc.h                   Secure Non-Secure Callable (NSC) module header file
  - NonSecure/Src/main.c                         Non-secure Main program
  - NonSecure/Src/stm32l5xx_hal_msp.c            Non-secure HAL MSP module
  - NonSecure/Src/stm32l5xx_it.c                 Non-secure Interrupt handlers
  - NonSecure/Src/system_stm32l5xx_ns.c          Non-secure STM32L5xx system clock configuration file
  - NonSecure/Inc/main.h                         Non-secure Main program header file
  - NonSecure/Inc/stm32l5xx_hal_conf.h           Non-secure HAL Configuration file
  - NonSecure/Inc/stm32l5xx_it.h                 Non-secure Interrupt handlers header file
  - NonSecure/Inc/stm32l552e_eval_conf.h         Non-secure EVAL configuration header file


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

  - Output Traces
      This example activates on secure side the USART3 for trace over STLink UART link (Virtual COM Port).
      So you may open an Hyperterminal on your PC
      Hyperterminal configuration:
        - 115200 baud rate
        - 8 bit data
        - no parity
        - 1 bit stop
        - no flow control


@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

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
   - Double click on �STM32 Cortex-M C/C++ Application�
   - Select  �Startup� >  �Add� > 
     - Select the xxxxx_NS project 
 - Click Debug/Run to debug/run the example
 NOTE:
   - The default Debug configuration runtime option sets "Halt on exception" which 
     makes the code execution halt on SecureFault_Handler() on voluntary security violation.
     Press "Resume (F8)" to continue execution.


 */
