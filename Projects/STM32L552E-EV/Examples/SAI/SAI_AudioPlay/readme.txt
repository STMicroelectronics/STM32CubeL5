/**
  @page SAI_AudioPlay  Description of the SAI audio play example

  @verbatim
  ******************************************************************************
  * @file    SAI/SAI_AudioPlay/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the SAI audio play example.
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

Use of the SAI HAL API to play an audio file in DMA circular mode and handle the buffer update.

      Plug a headphone to ear the sound  /!\ Take care of yours ears.
      Default volume is 70%.
      The audio file is played in loop
      @Note: Copy file 'audio.bin' (available in AudioFile) directly in the flash
      at @0x08040000 using STM32CubeProgrammer
Once started, LED4 is toggling on STM32L552E-EV.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Audio, SAI, DMA, Buffer update, play, headphone, audio protocol

@par Directory contents

  - SAI/SAI_AudioPlay/Inc/stm32l552e_eval_conf.h     BSP configuration file
  - SAI/SAI_AudioPlay/Src/main.c                  Main program
  - SAI/SAI_AudioPlay/Src/system_stm32l5xx.c      STM32L5xx system source file
  - SAI/SAI_AudioPlay/Src/stm32l5xx_it.c          Interrupt handlers
  - SAI/SAI_AudioPlay/Inc/main.h                  Main program header file
  - SAI/SAI_AudioPlay/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - SAI/SAI_AudioPlay/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - SAI/SAI_AudioPlay/AudioFile/audio.bin         Audio wave extract.

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with STMicroelectronics STM32L552E-EV
    board and can be easily tailored to any other supported device
    and development board.

  - STM32L552E-EV Set-up :
    - Plug headset into AUDIO_JACK connector (CN21).

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
