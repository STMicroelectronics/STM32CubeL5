/**
  @page SPI_HalfDuplex_ComPollingIT_Master SPI Half Duplex IT example
  
  @verbatim
  ******************************************************************************
  * @file    SPI/SPI_HalfDuplex_ComPollingIT_Master/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the SPI Half Duplex IT example.
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

Data buffer transmission/reception between 
two boards via SPI using Polling (LL driver) and Interrupt modes (HAL driver).

   _________________________                        _________________________
  |       ___________ ______|                      |__________________       |
  |      |SPI1              |                      |             SPI1 |      |
  |      |                  |                      |                  |      |
  |      |         CLK(PA5) |______________________|(PA5)CLK          |      |
  |      |                  |                      |                  |      |
  |      |         MISO(PA6)|          ____________|(PA6)MISO         |      |
  |      |                  |          |           |                  |      |
  |      |         MOSI(PA7)|__________|           |(PA7)MOSI         |      |
  |      |                  |                      |                  |      |
  |      |__________________|                      |__________________|      |
  |      __                 |                      |                         |
  |     |__|                |                      |                         |
  |     USER                |                      |                         |
  |                      GND|______________________|GND                      |
  |                         |                      |                         |
  |_STM32L5xx Master________|                      |_STM32L5xx Slave_________|


At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 110 MHz.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK & 
GPIO). You may update this function to change SPI configuration.
The Half-Duplex SPI transmission (8bit) is done using LL Driver on Master board (Tx) by using function 
LL_SPI_TransmitData8.
The The Half-Duplex SPI reception (8bit) is done using HAL Driver on Slave board (Rx) by using function 
HAL_SPI_Receive_IT.

Example execution:
First step, press the User push-button, this action initiates a Half-Duplex transfer 
between Master and Slave.
After end of transfer, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.

STM32 board's LEDs can be used to monitor the transfer status:
 - LED1 toggles quickly on master board waiting User push-button to be pressed.
 - LED2 turns ON on slave board if reception is complete and OK.
 - LED3 turns ON when there is an error in reception process. 

@note You need to perform a reset on Slave board, then perform it on Master board
      to have the correct behaviour of this example.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, SPI, Half-duplex, Transmission, Reception, Master, MISO, MOSI, Polling, Interrupt

@par Directory contents 

  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Inc/stm32l5xx_it.h          SPI interrupt handlers header file
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Inc/main.h                  Header for main.c module  
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/stm32l5xx_it.c          SPI interrupt handlers
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/main.c                  Main program
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/system_stm32l5xx.c      STM32L5xx system source file
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/stm32l5xx_hal_msp.c     HAL MSP file    


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - Take care to cable connection between Master and Slave Board:
    Cable shall be smaller than 5 cm and rigid if possible.

  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up
    - Connect Master board PA5 (CN7, D13(pin 10)) to Slave Board PA5 (CN7, D13(pin 10))
    - Connect Master board PA7 (CN7, D11(pin 14)) to Slave Board PA6 (CN7, D12(pin 12))
    - Connect Master board GND to Slave Board GND

@par How to use it ? 

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files (master project) and load your image into target memory
    o Load the project in Master Board
 - Rebuild all files (slave project) and load your image into target memory
    o Load the project in Slave Board
 - Run the example


 */
 