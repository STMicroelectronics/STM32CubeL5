/**
  @page FatFs_uSD_RTOS    FatFs with uSD disk drive application
 
  @verbatim
  ******************************************************************************
  * @file    FatFs/FatFs_uSD_RTOS/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the FatFs with uSD disk drive application
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
  */

@par Application Description

This application provides a description on how to use STM32Cube firmware with FatFs 
middleware component as a generic FAT file system module, in order to develop an
application exploiting FatFs offered features with uSD disk drive 
configuration. 

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock
(SYSCLK) to run at 110 MHz.
 
The application is based on writing and reading back a text file from a drive,
and it's performed using FatFs APIs to access the FAT volume as described
in the following steps: 

 - Link the uSD disk I/O driver;
 - Register the file system object (mount) to the FatFs module for the uSD drive;
 - Create a FAT file system (format) on the uSD drive;
 - Create and Open new text file object with write access;
 - Write data to the text file;
 - Close the open text file;
 - Open text file object with read access;
 - Read back data from the text file;
 - Close the open text file;
 - Check on read data from text file;

It is worth noting that the application manages any error occurred during the 
access to FAT volume, when using FatFs APIs.

It is possible to fine tune needed FatFs features by modifying defines values 
in FatFs configuration file ffconf.h available under the project includes 
directory, in a way to fit the application requirements. 
 
STM32L552E-EV's board's LEDs can be used to monitor the application status:
  - LED4 ON when the application runs successfully.
  - LED6 ON when any error occurs. 


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

For more details about FatFs implementation on STM32Cube, please refer to UM1721 "Developing Applications 
on STM32Cube with FatFs".

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

File system, FatFs, FreeRTOS, SD Card, File system, FAT, Format, Mount, Read, Write

@par Directory contents
 
  - FatFs/FatFs_uSD_RTOS/Core/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - FatFs/FatFs_uSD_RTOS/Core/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - FatFs/FatFs_uSD_RTOS/Core/Inc/main.h                        Main program header file
  - FatFs/FatFs_uSD_RTOS/Core/Inc/stm32l552e_eval_conf.h     BSP configuration file    (FatFs)
  - FatFs/FatFs_uSD_RTOS/FATFS/Target/sd_diskio.h               FatFS uSD diskio driver header file
  - FatFs/FatFs_uSD_RTOS/FATFS/Target/ffconf.h                  FAT file system module configuration file   
  - FatFs/FatFs_uSD_RTOS/Core/Src/stm32l5xx_it.c          Interrupt handlers
  - FatFs/FatFs_uSD_RTOS/Core/Src/stm32l5xx_hal_msp.c     MSP configuration file
  - FatFs/FatFs_uSD_RTOS/Core/Src/main.c                        Main program
  - FatFs/FatFs_uSD_RTOS/FATFS/Target/sd_diskio.c               FatFS uSD diskio driver implementation
  - FatFs/FatFs_uSD_RTOS/Core/Src/system_stm32l5xx.c      stm32l5xx system clock configuration file
  - FatFs/FatFs_uSD_RTOS/FATFS/App/app_fatfs.c                  FatFs application code
  - FatFs/FatFs_uSD_RTOS/FATFS/App/app_fatfs.h                  Header file for App_fatfs.c file    
 
@par Hardware and Software environment

  - This application runs on STM32L552xx  devices.
    
  - This application has been tested with STMicroelectronics STM32L552E-EV  
    evaluation boards and can be easily tailored to any other supported device 
    and development board.  


@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the application


 */