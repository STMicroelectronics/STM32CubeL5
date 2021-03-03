/**
  @page FatFs_MultiDrives   FatFs with multi drives application

  @verbatim
  ******************************************************************************
  * @file    FatFs/FatFs_MultiDrives/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FatFs with multi drives application
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description

How to use STM32Cube firmware with FatFs middleware component as a generic
FAT file system module. This example develops an application that exploits
FatFs features, with multidrive (SRAM, uSD) configurations.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock
(SYSCLK) to run at 110 MHz.

The application is based on writing and reading back a text file from two drives,
and it's performed using FatFs APIs to access the FAT volume as described
in the following steps:

 - Link the RAM disk (SRAM) and uSD disk I/O drivers;
 - Register the file system object (mount) to the FatFs module for both drives;
 - Create a FAT file system (format) on both logical drives;
 - Create and Open new text file objects with write access;
 - Write data to the text files;
 - Close the open text files;
 - Open text file objects with read access;
 - Read back data from the text files;
 - Check on read data from text file;
 - Close the open text files;

It is worth noting that the application manages any error occurred during the
access to FAT volume, when using FatFs APIs. Otherwise, user can check if the
written text file is available on the uSD card and the RAM disk (SRAM).

It is possible to fine tune needed FatFs features by modifying defines values
in FatFs configuration file ffconf.h available under the project includes
directory, in a way to fit the application requirements. In this case, two drives
are used, so the Max drive number is set to: _VOLUMES = 2 in ffconf.h file.

It is possible to dynamically hot plug/unplug the uSD card, as the application will
handle the plug/unplug events.


@note: for some uSD's, replacing it  while the application is running makes the application
       fail. It is recommended to reset the board using the "Reset button" after replacing
       the uSD.

STM32L552E-EV's LED can be used to monitor the application status:
  - LED4 is ON when the application runs successfully.
  - LED5 is ON when any error occurs.
  - LED5 toggle when SD card is unplugged.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

File system, FatFs, SD Card, SRAM, FAT Volume, Format, Mount, Read, Write, multidrives

@par Directory contents

  - FatFs/FatFs_uSD_Standalone/Core/Inc/stm32l5xx_hal_conf.h               HAL configuration file
  - FatFs/FatFs_uSD_Standalone/Core/Inc/stm32l5xx_it.h                     Interrupt handlers header file
  - FatFs/FatFs_uSD_Standalone/Core/Inc/main.h                             Header for main.c module
  - FatFs/FatFs_uSD_Standalone/Core/Src/stm32l5xx_it.c                     Interrupt handlers
  - FatFs/FatFs_uSD_Standalone/Core/Src/main.c                             Main program
  - FatFs/FatFs_uSD_Standalone/Core/Src/system_stm32l5xx.c                 STM32L5xx system source file
  - FatFs/FatFs_uSD_Standalone/FATFS/Target/ffconf.h                       FAT file system module configuration file
  - FatFs/FatFs_uSD_Standalone/FATFS/Target/sd_diskio.h                    uSD diskio header file
  - FatFs/FatFs_uSD_Standalone/FATFS/Target/sd_diskio.c                    FatFs uSD diskio driver
  - FatFs/FatFs_RAMDisk/FATFS/Target/sram_diskio.h            FatFS sram diskio driver header file
  - FatFs/FatFs_RAMDisk/FATFS/Target/sram_diskio.c            FatFS sram diskio driver implementation
  - FatFs/FatFs_uSD_Standalone/Core/Inc/stm32l552e_eval_conf.h        BSP configuration file
  - FatFs/FatFs_uSD_Standalone/FATFS/App/app_fatfs.h                       Header file for App_fatfs.c file
  - FatFs/FatFs_uSD_Standalone/FATFS/App/app_fatfs.c                       FatFs application code

 @par Hardware and Software environment

  - This application runs on STM32L552ZETxQ devices

  - This application has been tested with STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following:
 - Insert a microSD card in the board appropriate slot
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
