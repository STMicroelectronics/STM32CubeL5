/**
  @page USART_SyncCommunication_FullDuplex_DMA_Init USART Synchronous Transmitter/Receiver example (DMA mode)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_SyncCommunication_FullDuplex_DMA_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_SyncCommunication_FullDuplex_DMA_Init example.
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

Configuration of GPIO, USART, DMA and SPI peripherals to transmit 
bytes between a USART and an SPI (in slave mode) in DMA mode. This example is based on the STM32L5xx USART LL API. The peripheral 
initialization uses LL unitary service functions for optimization purposes (performance and size).

This example works with only one NUCLEO-L552ZE-Q.

USART1 Peripheral is acts as Master.
SPI1 Peripheral is configured in Slave mode.
GPIO associated to User push-button is linked with EXTI.

Example execution:

LED1 is blinking Fast (200ms) and wait User push-button action.
Press User push-button on BOARD start a Full-Duplex communication through DMA.
On USART1 side, Clock will be generated on SCK line, Transmission done on TX, reception on RX.
On SPI1 side, reception is done through the MOSI Line, transmission on MISO line.

At end of transmission, both received buffers are compared to expected ones.
In case of both transfers successfully completed, LED1 is turned on.
In case of errors, LED1 is blinking Slowly (1s).

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, UART/USART, Synchronous, baud rate, DMA, SPI interface, Receiver, Transmitter, full-duplex

@par Directory contents 

  - USART/USART_SyncCommunication_FullDuplex_DMA_Init/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - USART/USART_SyncCommunication_FullDuplex_DMA_Init/Inc/main.h                  Header for main.c module
  - USART/USART_SyncCommunication_FullDuplex_DMA_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_SyncCommunication_FullDuplex_DMA_Init/Src/stm32l5xx_it.c          Interrupt handlers
  - USART/USART_SyncCommunication_FullDuplex_DMA_Init/Src/main.c                  Main program
  - USART/USART_SyncCommunication_FullDuplex_DMA_Init/Src/system_stm32l5xx.c      STM32L5xx system source file

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up
    - Connect USART1 SCK PA.08 to Slave SPI1 SCK PB.03
    - Connect USART1 TX PB.06 to Slave SPI1 MOSI PB.05
    - Connect USART1 RX PA.10 to Slave SPI1 MISO PB.04

Board connector:
  PA.08 :   connected to pin 31 of Arduino connector CN10
  PB.06 :   connected to pin 15 of Arduino connector CN9
  PA.10 :   connected to pin 33 of Morpho connector CN12
  PB.03 :   connected to pin 15 of Arduino connector CN7
  PB.05 :   connected to pin 13 of Arduino connector CN7
  PB.04 :   connected to pin 11 of Arduino connector CN7

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Launch the program. Press on User push-button on board to initiate data transfers.


 */
