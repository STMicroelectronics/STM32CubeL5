/* USER CODE BEGIN Header */
/**
  @page ADC_SingleConversion_TriggerTimer_DMA ADC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/ADC/ADC_SingleConversion_TriggerTimer_DMA/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_SingleConversion_TriggerTimer_DMA example.
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
Use ADC to convert a single channel at each trig from timer, 
conversion data are transferred by DMA into an array, indefinitely (circular mode).

Example configuration:
ADC is configured to convert a single channel, in single conversion mode,
from HW trigger: timer peripheral.
DMA is configured to transfer conversion data in an array, in circular mode.
A timer is configured in time base and to generate TRGO events.

Example execution:
From the start, the ADC converts the selected channel at each trig from timer.
DMA transfers conversion data to the array, DMA transfer complete interruption occurs.
Results array is updated indefinitely (DMA in circular mode).
LED1 is turned on when the DMA transfer is completed (results array full)
and turned off at next DMA half-transfer (result array first half updated).

For debug: variables to monitor with debugger watch window:
 - "aADCxConvertedData": ADC group regular conversion data (array of data)
 - "aADCxConvertedData_Voltage_mVolt": ADC conversion data computation to physical values (array of data)

Connection needed:
None, if ADC channel and DAC channel are selected on the same GPIO.
Otherwise, connect a wire between DAC channel output and ADC input.

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog input: PA4 (Arduino connector CN7 pin 9, Morpho connector CN11 pin 32)
  DAC
  1 GPIO for DAC channel output PA4 (Arduino connector CN7 pin 9, Morpho connector CN11 pin 32)
  1 GPIO for push button
  DMA
  Timer

Board settings:
 - ADC is configured to convert ADC_CHANNEL_9 (Arduino connector CN7 pin 9, Morpho connector CN11 pin 32).
 - The voltage input on ADC channel is provided from DAC (DAC_CHANNEL_1).
   ADC input from pin PA4 and DAC output to pin PA4:
   If same pin is used no connection is required, it is done internally. Otherwise, user need to connect a wire between Arduino connector CN7 pin 9, Morpho connector CN11 pin 32 and Arduino connector CN7 pin 9, Morpho connector CN11 pin 32
 - Voltage is increasing at each click on User push-button, from 0 to maximum range in 4 steps.
   Clicks on User push-button follow circular cycles: At clicks counter maximum value reached, counter is set back to 0.


To observe voltage level applied on ADC channel through GPIO, connect a voltmeter on
pin PA4 (Arduino connector CN7 pin 9, Morpho connector CN11 pin 32).

NUCLEO-L552ZE-Q C-02 board LED is be used to monitor the program execution status:
 - Normal operation: LED1 is turned-on/off in function of ADC conversion
   result.
    - Toggling: "On" upon conversion completion (full DMA buffer filled)
                "Off" upon half conversion completion (half DMA buffer filled)
 - Error: In case of error, LED1 is toggling at a frequency of 1Hz.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

ADC, analog digital converter, conversion, voltage, channel, analog input, DMA transfer, trigger

@par Directory contents 

  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/stm32l5xx_nucleo_conf.h     BSP configuration file
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/main.h                        Header for main.c module  
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/stm32l5xx_it.c          Interrupt handlers
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/main.c                        Main program
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/stm32l5xx_hal_msp.c     HAL MSP module
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L5xx devices.
    
  - This example has been tested with NUCLEO-L552ZE-Q C-02 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
