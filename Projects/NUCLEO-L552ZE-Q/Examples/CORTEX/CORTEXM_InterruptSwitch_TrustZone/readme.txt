/**
  @page CORTEXM_InterruptSwitch_TrustZone Interrupt switch from secure to non-secure example

  @verbatim
  ******************************************************************************
  * @file    CORTEX/CORTEXM_InterruptSwitch_TrustZone/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the CORTEX Interrupt switch example.
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

How to first use an interrupt in secure application and later assign it to the
non-secure application when TrustZone security is activated (Option bit TZEN=1).

In this example, the interrupt associated to EXTI line 13 (connected to
the USER push-button) is used by the secure application that is waiting from a user push-button
key press event before assigning then the interrupt to the non-secure application with
NVIC_SetTargetState() API and launching the non-secure application.

At the first USER push-button key press, the LED1 is switched on by the secure application. Any 
further USER push-button key press makes the LED2 managed by the non-secure application toggling.

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

System, Trustzone, Cortex, Interrupt

@par Directory contents

  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/main.c                     Secure Main program
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/stm32l5xx_hal_msp.c        Secure HAL MSP module
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/stm32l5xx_it.c             Secure Interrupt handlers
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/system_stm32l5xx_s.c       Secure STM32L5xx system clock configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/main.h                     Secure Main program header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/partition_stm32l552xx.h    STM32L5 Device System Configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/stm32l5xx_hal_conf.h       Secure HAL Configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/stm32l5xx_it.h             Secure Interrupt handlers header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure_nsclib/secure_nsc.h            Secure Non-Secure Callable (NSC) module header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/main.c                  Non-secure Main program
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/stm32l5xx_hal_msp.c     Non-secure HAL MSP module
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/stm32l5xx_it.c          Non-secure Interrupt handlers
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/system_stm32l5xx_ns.c   Non-secure STM32L5xx system clock configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Inc/main.h                  Non-secure Main program header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Inc/stm32l5xx_hal_conf.h    Non-secure HAL Configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Inc/stm32l5xx_it.h          Non-secure Interrupt handlers header file

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
