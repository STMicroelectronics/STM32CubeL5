/**
  @page ADC_Oversampling ADC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/ADC/ADC_Oversampling/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_Oversampling example.
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
Use ADC to convert a single channel but using oversampling feature to increase resolution. 

Example configuration:
ADC is configured to convert a single channel, in single conversion mode,
from SW trigger.
Oversampling is configured to perform 128 conversions
with a right shift of 4 before returning a result.

Example execution:
The ADC performs conversions of the selected channel in continuous mode from SW start
trigger.
Then, SW is waiting for conversion to complete. When done, it reads conversion data from
ADC data register, stores it into a variable, and convert it into mVolt in another one?

For debug: variables to monitor with debugger watch window:
 - "uhADCxConvertedData": ADC group regular conversion data
 - "uhADCxConvertedData_Voltage_mVolt": ADC conversion data computation to physical values

Connection needed:
None, if ADC channel and DAC channel are selected on the same GPIO.
Otherwise, connect a wire between DAC channel output and ADC input.

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog input: PA4 (connector CN6 pin 17)
  DAC
  1 GPIO for DAC channel output PA4 (connector CN6 pin 17)
  1 GPIO for push button

Board settings:
 - ADC is configured to convert ADC_CHANNEL_9 (connector CN6 pin 17).
 - The voltage input on ADC channel is provided from DAC (DAC1_CHANNEL_1).
   ADC input from pin PA4 and DAC output to pin PA4:
   If same pin is used no connection is required, it is done internally. Otherwise, user need to connect a wire between connector CN6 pin 17 and connector CN6 pin 17
 - Voltage is increasing at each click on WKUP push-button, from 0 to maximum range in 4 steps.
   Clicks on WKUP push-button follow circular cycles: At clicks counter maximum value reached, counter is set back to 0.


To observe voltage level applied on ADC channel through GPIO, connect a voltmeter on
pin PA4 (connector CN6 pin 17).

STM32L552E-EV board LED is be used to monitor the program execution status:
 - Normal operation: LED4 is toggling at each conversion.
 - Error: In case of error, LED4 is toggling twice at a frequency of 1Hz.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

ADC, analog digital converter, conversion, voltage, channel, analog input, oversampling

@par Directory contents 

  - ADC/ADC_Oversampling/Inc/stm32l552e_eval_conf.h     BSP configuration file
  - ADC/ADC_Oversampling/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - ADC/ADC_Oversampling/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - ADC/ADC_Oversampling/Inc/main.h                        Header for main.c module  
  - ADC/ADC_Oversampling/Src/stm32l5xx_it.c          Interrupt handlers
  - ADC/ADC_Oversampling/Src/main.c                        Main program
  - ADC/ADC_Oversampling/Src/stm32l5xx_hal_msp.c     HAL MSP module
  - ADC/ADC_Oversampling/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32l5xx devices.
    
  - This example has been tested with STM32L552E-EV board and can be
    easily tailored to any other supported device and development board.


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
