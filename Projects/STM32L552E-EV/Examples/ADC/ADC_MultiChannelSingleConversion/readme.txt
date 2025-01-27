/**
  @page ADC_MultiChannelSingleConversion ADC example

  @verbatim
  ******************************************************************************
  * @file    Examples/ADC/ADC_MultiChannelSingleConversion/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_MultiChannelSingleConversion example.
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
Use ADC to convert a several channels using sequencer in discontinuous mode, 
conversion data are transferred by DMA into an array, indefinitely (circular mode).

Example configuration:
The channels converted are one external channel and two internal 
channels (VrefInt and temperature sensors), in sequence conversion mode,
one by one from SW trigger.
DMA is configured to transfer conversion data in an array, in circular mode.

Example execution:
From the start, the ADC converts the selected channels in sequence, one by one
(discontinuous mode) at each trig from WKUP push-button click.
DMA transfers conversion data to the array, DMA transfer complete interruption occurs.
Results array is updated indefinitely (DMA in circular mode).
LED4 is turned on when the DMA transfer is completed (results array full)
and turned off at next DMA half-transfer (result array first half updated).
When the ADC sequence is complete (the three ADC conversions are carried out), 
the voltages and temperature are computed and stored in variables:
    uhADCxConvertedData_VoltageGPIO_mVolt
    uhADCxConvertedData_VrefInt_mVolt
    hADCxConvertedData_Temperature_DegreeCelsius
    uhADCxConvertedData_VrefAnalog_mVolt
	
Note: If DMA buffer is full when user click on WKUP push-button, the buffer is reset
(to ease user observe behavior).

For debug: variables to monitor with debugger watch window:
 - "aADCxConvertedData": ADC group regular conversion data (array of data)

Connection needed:
None.
Note: Optionally, a voltage can be supplied to the analog input pin (cf pin below),
      between 0V and Vdda=3.3V, to perform a ADC conversion on a determined
      voltage level.
      Otherwise, this pin can be let floating (in this case ADC conversion data
      will be undetermined).

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog input: PC3 (connector CN6 pin 30)
  1 GPIO for push button
  DMA

Board settings:
 - ADC is configured to convert one external channel ADC_CHANNEL_4
   (connector CN6 pin 30) and the two internal ones ADC_CHANNEL_VREFINT,
   ADC_CHANNEL_TEMPSENSOR.
 - The voltage input on the ADC external channel can be provided by an external source
   connected to connector CN6 pin 30.



STM32L552E-EV board LED is be used to monitor the program execution status:
 - Normal operation: LED4 is turned-on/off in function of ADC conversion
   result.
    - Toggling: "On" upon conversion completion (full DMA buffer filled)
                "Off" upon half conversion completion (half DMA buffer filled)
 - Error: In case of error, LED4 is toggling twice at a frequency of 1Hz.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

ADC, analog digital converter, conversion, voltage, channel, analog input, sequencer, temperature sensor, VrefInt

@par Directory contents 

  - ADC/ADC_MultiChannelSingleConversion/Inc/stm32l552e_eval_conf.h     BSP configuration file
  - ADC/ADC_MultiChannelSingleConversion/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - ADC/ADC_MultiChannelSingleConversion/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - ADC/ADC_MultiChannelSingleConversion/Inc/main.h                        Header for main.c module  
  - ADC/ADC_MultiChannelSingleConversion/Inc/stm32l5xx_conf.h        HAL configuration file
  - ADC/ADC_MultiChannelSingleConversion/Src/stm32l5xx_it.c          Interrupt handlers
  - ADC/ADC_MultiChannelSingleConversion/Src/stm32l5xx_hal_msp.c     HAL MSP module
  - ADC/ADC_MultiChannelSingleConversion/Src/main.c                        Main program
  - ADC/ADC_MultiChannelSingleConversion/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L5xx devices.
    
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


 */
