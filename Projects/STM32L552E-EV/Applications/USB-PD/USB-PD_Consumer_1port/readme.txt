/**
  @page USB-PD_Consumer_1port USBPD Consumer example

  @verbatim
  ******************************************************************************
  * @file    USB-PD/USB-PD_Consumer_1port/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the consumer example.
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description

How to create a simple type C Consumer.

This application initializes the type C port 1 in sink mode with only one PDO at 5V.

Connect UCPD cube Monitor on the VCP associated to our board (only available if USB cable is connected)
The UCPD Cube Monitor will be used to trace all the messages exchange between the ports partner.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority()
      function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

@note The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
      OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

USB_PD_Lib, UCPD, Type C, USBPD, FreeRTOS

@par Directory contents
    - USB-PD/USB-PD_Consumer_1port/Src/main.c                       Main program file
    - USB-PD/USB-PD_Consumer_1port/Src/app_freertos.c               Code for FreeRTOS application file
    - USB-PD/USB-PD_Consumer_1port/Src/stm32l5xx_hal_msp.c          MSP Initialization file
    - USB-PD/USB-PD_Consumer_1port/Src/stm32l5xx_hal_timebase_tim.c HAL Timebase Initialization file
    - USB-PD/USB-PD_Consumer_1port/Src/stm32l5xx_it.c               Interrupt handlers file
    - USB-PD/USB-PD_Consumer_1port/Src/system_stm32l5xx.c           STM32L5xx system clock configuration file
    - USB-PD/USB-PD_Consumer_1port/Src/usbpd.c                      UCPD init file
    - USB-PD/USB-PD_Consumer_1port/Src/usbpd_dpm_core.c             Device policy manager core file
    - USB-PD/USB-PD_Consumer_1port/Src/usbpd_dpm_user.c             Device policy manager user file
    - USB-PD/USB-PD_Consumer_1port/Src/usbpd_pwr_if.c               Power if management file
    - USB-PD/USB-PD_Consumer_1port/Src/usbpd_pwr_user.c             Power management user file
    - USB-PD/USB-PD_Consumer_1port/Src/usbpd_vdm_user.c             User vendor define message management file
    - USB-PD/USB-PD_Consumer_1port/Inc/FreeRTOSConfig.h             FreeRTOS Configuration file
    - USB-PD/USB-PD_Consumer_1port/Inc/main.h                       Main program header file
    - USB-PD/USB-PD_Consumer_1port/Inc/stm32_assert.h               Assert program header file
    - USB-PD/USB-PD_Consumer_1port/Inc/stm32l5xx_hal_conf.h         HAL Library Configuration file
    - USB-PD/USB-PD_Consumer_1port/Inc/stm32l5xx_it.h               Interrupt handlers header file
    - USB-PD/USB-PD_Consumer_1port/Inc/tracer_emb_conf.h            Setting file for UCPD tracer
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd.h                      Header of UCPD init file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_devices_conf.h         UCPD device configuration file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_dpm_conf.h             UCPD stack configuration file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_dpm_core.h             Device policy manager core header file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_dpm_user.h             Device policy manager user header file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_gui_memmap.h           Memory mapping configuration configuration file for Cube-Monitor-UCPD link
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_pdo_defs.h             PDO definition header file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_pwr_if.h               Power if management header file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_pwr_user.h             Power management header file
    - USB-PD/USB-PD_Consumer_1port/Inc/usbpd_vdm_user.h             Vendor define message management header file


@par Hardware and Software environment

  - This application runs on STM32L552ZET6Q devices.

  - This application has been tested with STM32L552E-EVAL board.


@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Run Cube UCPD Monitor or open trace tools
 - Connect a type C source device on the Type-C connector 1

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
