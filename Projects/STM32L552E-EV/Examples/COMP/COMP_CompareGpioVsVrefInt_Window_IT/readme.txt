/**
  @page COMP_CompareGpioVsVrefInt_Window_IT COMP example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/COMP/COMP_CompareGpioVsVrefInt_Window_IT/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the COMP_CompareGpioVsVrefInt_Window_IT Example.
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

This example shows how to make an analog watchdog using the COMP peripherals in window mode.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals, initialize
the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32L552ZETxQ Devices :
  - The upper threshold is set to VREFINT = 1.22V
  - The lower threshold is set to VREFINT / 4 = 1.22V / 4 = 0.305V
  - The input voltage is configured to be connected to PB2

LEDs on STM32L552E-EV can be used to monitor the voltage level compared to comparators thresholds:

- LED_GREEN blinks quickly when input voltage is above the higher threshold.
- LED_GREEN blinks slowly (once every 1 s) if the input voltage is under the lower threshold.
- If the input voltage is within the thresholds, the MCU is set in STOP1 mode. LED_GREEN does not blink.
- If LED_RED is on, it indicates that the system generated an error.

NB: The MCU wakes up from STOP1 mode when the input voltage is out of the threshold window.

@note Care must be taken when using HAL_Delay(), this function provides 
      accurate delay (in milliseconds) based on variable incremented in SysTick ISR. 
      This implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower) than the 
      peripheral interrupt. Otherwise the caller ISR process will be blocked. 
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set 
      to 1 millisecond to have correct HAL operation.

@note This example can not be used in DEBUG mode due to the fact that the Cortex-M33 core is no longer 
      clocked during low power mode so debugging features are disabled. Power on/off is needed to run 
      correctly the example.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

comparator, window mode, voltage compare

@par Directory contents 

  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/stm32l552e_eval_conf.h  BSP configuration file
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/stm32l5xx_it.h          COMP interrupt handlers header file
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/main.h                  Header for main.c module
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/stm32l5xx_it.c          COMP interrupt handlers
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/main.c                  Main program
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/stm32l5xx_hal_msp.c     HAL MSP file 
  - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment 

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.

  - Apply an external variable voltage on PB2 (connected on pin 3 on CN6) with average voltage 1.22V.
     
@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */