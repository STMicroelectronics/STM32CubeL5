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

Considered board is STM32L562E-DK. 

The project configures the maximum system clock frequency at 110Mhz.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

This example shows how to use the different functionalities of components 
available on the board by switching between all tests using User button.

Red LED toggles every 500ms whenever any error is detected.

 ** LED **
This example shows how to switch on, switch off, toggle all leds.

 ** LCD **
This example shows how to use the different LCD features to display string
with different fonts, to display different shapes and to draw a bitmap.

 ** TOUCHSCREEN **
This example shows how to use the different touchscreen features.

 ** AUDIO PLAY **
This example shows how to play an audio file.

      Plug a headphone to the CN13 connector to hear the sound  /!\ Take care of yours ears.
      Default volume is 80%.
      @Note: Copy file 'audio.bin' (available in BSP\AudioFile folder) directly in the flash 
      at @0x08040000 using STM32CubeProgrammer.

 ** AUDIO RECORD **
This example will use microphones available on the board. Automatically, sound 
will be recorded and played on the CN13 connector.
LED green will blink during record.

 ** SD **
This example shows how to erase, write and read the SD card and also 
how to detect the presence of the card.

  ** OSPI NOR **
This example shows how to erase, write and read data available on the Octal Flash memory
available on the board.

  ** MOTION SENSOR **
This example shows how to use motion sensor mounted on board (accelerometer and gyroscope
capabilities).

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

BSP, Board Support Package, Audio Play, Audio Record, Button, Display, LCD, Headphone, Microphone, 
LED, OctoSPI NOR Flash, SD Card, Sensor, Accelerometer, Gyroscope, Touchscreen

@par Directory contents 

  - BSP/Src/main.c                      Main program
  - BSP/Src/system_stm32l5xx.c          STM32L5xx system clock configuration file
  - BSP/Src/stm32l5xx_it.c              Interrupt handlers
  - BSP/Src/led.c                       Led display features
  - BSP/Src/lcd.c                       LCD drawing features
  - BSP/Src/touchscreen.c               LCD touchscreen features
  - BSP/Src/sd.c                        SD features
  - BSP/Src/audio_play.c                Audio Playback features
  - BSP/Src/audio_record.c              Audio Record features
  - BSP/Src/ospi_nor.c                  OctoSPI NOR features
  - BSP/Src/motion_sensor.c             Motion Sensor features
  - BSP/Inc/main.h                      Main program header file
  - BSP/Inc/stm32l5xx_hal_conf.h        HAL Configuration file
  - BSP/Inc/stm32l562e_discovery_conf.h Board Configuration file
  - BSP/Inc/stm32l5xx_it.h              Interrupt handlers header file
  - BSP/Inc/stlogo.h                    Image used for BSP LCD example
  - BSP/Inc/ft6x06_conf.h               LCD touchscreen configuration file
  - BSP/Inc/mx25lm51245g_conf.h         OSPI NOR memory configuration file
  - BSP/AudioFile/audio.bin             Audio wave file.
        
@par Hardware and Software environment  

  - This template runs on STM32L562QEIxQ devices without security enabled (TZEN=0).

  - This template has been tested with STMicroelectronics STM32L562E-DK (MB1373)
    board and can be easily tailored to any other supported device
    and development board.
  
@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
