/**
  @page HASH_SHA1SHA224_IT_Suspension  SHA digest calculation suspension example
  
  @verbatim
  ******************************************************************************
  * @file    HASH/HASH_SHA1SHA224_IT_Suspension/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the digest calculation suspension using SHA1 and SHA224 example.
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

How to suspend the HASH peripheral when data are fed in interrupt mode.

The example first computes the HASH digest of a low priority block using SHA-1
algorithm in entering input data in interrupt mode.

The processing is suspended in order to process a higher priority block to
compute its SHA-224 digest. For the higher priority block, data are entered in
interrupt mode as well.
Suspension request is triggered by an interrupts counter which is incremented each time
a 512-bit long block has been fed to the IP. Once the counter has reached an arbitrary
value, the low priority block processing is suspended.

It is shown how to save then how to restore later on the low priority block
context to resume its processing.
For both the low and high priority blocks, the computed digests are compared with
the expected ones.


LED_GREEN is turned on when correct digest values are calculated.
In case of digest computation or initialization issue, LED_RED is turned on.


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

System, Security, HASH, SHA1, SHA224, Interrupt, digest 

@par Directory contents 

  - HASH/HASH_SHA1SHA224_IT_Suspension/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - HASH/HASH_SHA1SHA224_IT_Suspension/Inc/stm32l5xx_nucleo_conf.h Board configuration file
  - HASH/HASH_SHA1SHA224_IT_Suspension/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - HASH/HASH_SHA1SHA224_IT_Suspension/Inc/main.h                  Header for main.c module  
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/stm32l5xx_it.c          Interrupt handlers
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/main.c                  Main program
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/stm32l5xx_hal_msp.c     HAL MSP module
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 



 */
 