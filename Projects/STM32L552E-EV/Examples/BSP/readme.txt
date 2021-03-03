/**
  @page BSP  Example on how to use the BSP drivers

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @author  MCD Application Team
  * @brief   Description of the BSP example.
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

How to use the different BSP drivers of the board.

Considered board is STM32L552E-EV.

The project configures the maximum system clock frequency at 110Mhz.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

This example shows how to use the different functionalities of components
available on the board by switching between all tests using Wakeup button.

Red LED toggles every 500ms whenever any error is detected.

 ** Push the Wakeup push-button to start first Test.
Press Wakeup push-button to start another test:

 ** LED **
This example shows how to switch on, switch off, toggle all leds.

 ** JOYSTICK **
Use the joystick button to move a pointer inside a rectangle
(up/down/right/left) and change the pointer color(select).

 ** LCD **
This example shows how to use the different LCD features to display string
with different fonts, to display different shapes and to draw a bitmap.

 ** TOUCHSCREEN **
This example shows how to use the different touchscreen features.

 ** AUDIO PLAY **
This example shows how to play/pause/resume or change the volume (min 50% / max 100 %)
of an audio playback using Key or Joystick button.

      Plug a headphone to the CN21 connector to hear the sound  /!\ Take care of yours ears.
      Default volume is 80%.
      A press of JOY_SEL button will pause the audio file play (LED green ON
       and LED orange ON).
      Another press on JOY_SEL button will resumes audio file play (only LED green ON)
      @Note: Copy file 'audio.bin' (available in BSP\AudioFile folder) directly in the flash
      at @0x08040000 using STM32CubeProgrammer.

 ** AUDIO RECORD **
This example will use microphones available on the board. Automatically, sound
will be recorded and played on the CN21 connector.
LED green will blink during record.

 ** SD **
This example shows how to erase, write and read the SD card and also
how to detect the presence of the card.

 ** SRAM **
This example shows how to erase, write and read data available on the external
SRAM available on the board.
@note : Due to IO sharing between FMC and LTDC interfaces, LCD display is switched off
during SRAM operations (specially noticeable for few seconds during SRAM erase)

  ** OSPI NOR **
This example shows how to erase, write and read data available on the Octal Flash memory
available on the board.

  ** OSPI RAM **
This example shows how to write and read data available on the Octal HyperRAM memory
available on the board.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

BSP, Board Support Package, Audio Play, Audio Record, Button, Display, LCD, Headphone, Microphone,
Joystick, LED, OctoSPI NOR Flash, OctoSPI HyperRAM, SD Card, SRAM, Touchscreen

@par Directory contents

  - BSP/Src/main.c                 Main program
  - BSP/Src/system_stm32l5xx.c     STM32L5xx system clock configuration file
  - BSP/Src/stm32l5xx_it.c         Interrupt handlers
  - BSP/Src/led.c                  Led display features
  - BSP/Src/joystick.c             joystick feature
  - BSP/Src/lcd.c                  LCD drawing features
  - BSP/Src/touchscreen.c          LCD touchscreen features
  - BSP/Src/sd.c                   SD features
  - BSP/Src/audio_play.c           Audio Playback features
  - BSP/Src/audio_record.c         Audio Record features
  - BSP/Src/ospi_nor.c             OctoSPI NOR features
  - BSP/Src/ospi_ram.c             OctoSPI HyperRAM features
  - BSP/Src/sram.c                 SRAM features
  - BSP/Inc/main.h                 Main program header file
  - BSP/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - BSP/Inc/stm32l552e_eval_conf.h Board Configuration file
  - BSP/Inc/stm32l5xx_it.h         Interrupt handlers header file
  - BSP/Inc/stlogo.h               Image used for BSP LCD example
  - BSP/Inc/stmpe811_conf    .h    LCD configuration file
  - BSP/Inc/mx25lm51245g_conf.h    OSPI NOR memory configuration file
  - BSP/Inc/iss66wvh8m8_conf.h     OSPI RAM memory configuration file
  - BSP/AudioFile/audio.bin        Audio wave extract.

@par Hardware and Software environment

  - This template runs on STM32L552ZETxQ devices without security enabled (TZEN=0).

  - This template has been tested with STMicroelectronics STM32L552E-EV (MB1372)
    board and can be easily tailored to any other supported device
    and development board.

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
