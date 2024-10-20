/**
  @page PKA_PointCheck Point check example
  
  @verbatim
  ******************************************************************************
  * @file    PKA/PKA_PointCheck/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the modular exponentiation example
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

How to use the PKA peripheral to determine if a point is on a curve. This 
allows validating an external public key.

For this example, 2 test vector have been extracted from National Institute of Standards and Technology (NIST)
 - Cryptographic Algorithm Validation Program (CAVP) in order to demonstrate the usage of the hal.
Those reference files can be found under:
"http://csrc.nist.gov/groups/STM/cavp/documents/dss/186-3ecdsatestvectors.zip (ZIP PKV.rsp)"

Those two test vectors have been chosen to demonstrate the behavior in a case where the input point validates
the curve equation and in a case where the input point does not verify the curve equation. Their definition
is included in PKV.c. You can refer to this file for more information.

The selected curve for this example is P-256 (ECDSA-256) published by NIST in 
Federal Information Processing Standards Publication FIPS PUB 186-4. The description
of this curve is present in file Src/prime256v1.c.

In case of success, the LED10 (GREEN) is ON.
In case of any error, the LED10 (GREEN) is toggling slowly.

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

Security, PKA, NIST, CAVP, Point check

@par Directory contents 
  
  - PKA/PKA_PointCheck/Inc/stm32l5xx_hal_conf.h        HAL configuration file
  - PKA/PKA_PointCheck/Inc/stm32l562e_discovery_conf.h Board configuration file
  - PKA/PKA_PointCheck/Inc/stm32l5xx_it.h              Interrupt handlers header file
  - PKA/PKA_PointCheck/Inc/main.h                      Header for main.c module
  - PKA/PKA_PointCheck/Src/stm32l5xx_it.c              Interrupt handlers
  - PKA/PKA_PointCheck/Src/main.c                      Main program
  - PKA/PKA_PointCheck/Src/stm32l5xx_hal_msp.c         HAL MSP module 
  - PKA/PKA_PointCheck/Src/system_stm32l5xx.c          STM32L5xx system source file
  - PKA/PKA_PointCheck/Src/prime256v1.c                Description of P-256 (ECDSA-256)
  - PKA/PKA_PointCheck/Inc/prime256v1.h                Header for prime256v1.c
  - PKA/PKA_PointCheck/Src/PKV.c                       Reflect the content of 2 tests vector from PKV.rsp
  - PKA/PKA_PointCheck/Src/PKV.rsp                     Extract from NIST CAVP
  - PKA/PKA_PointCheck/Inc/PKV.h                       Header of PKV.c

@par Hardware and Software environment

  - This example runs on STM32L562QEIxQ devices.
  
  - This example has been tested with an STMicroelectronics STM32L562E-DK
    board and can be easily tailored to any other supported device 
    and development board.

@par How to use it ? 

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 

 */
 