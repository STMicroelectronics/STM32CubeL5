/**
  @page USART_Communication_TxRx_DMA_Init USART Transmitter/Receiver example (DMA mode)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_Communication_TxRx_DMA_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_Communication_TxRx_DMA_Init example.
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

This example shows how to configure GPIO and USART peripheral
to send characters asynchronously to/from an HyperTerminal (PC) in
DMA mode. This example is based on STM32L5xx USART LL API. Peripheral
initialization is done using LL unitary services functions for optimization
purpose (performance and size).

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
GPIO associated to User push-button is linked with EXTI. 
Virtual Com port feature of STLINK is used for UART communication between board and PC.

Example execution:
After startup from reset and system configuration, LED1 is blinking quickly and wait User push-button action.
USART and DMA are configured.
On press on push button , TX DMA transfer to PC is initiated. 
RX DMA transfer is also ready to retrieve characters from PC ("END" string is expected to be entered by user on PC side (HyperTerminal).
In case of both transfers successfully completed, LED1 is turned on.
In case of errors, LED1 is blinking (1sec period).

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, UART/USART, Asynchronous, RS-232, baud rate, DMA, HyperTerminal, 
Transmitter, Receiver

@par Directory contents 

  - USART/USART_Communication_TxRx_DMA_Init/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - USART/USART_Communication_TxRx_DMA_Init/Inc/main.h                  Header for main.c module
  - USART/USART_Communication_TxRx_DMA_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_Communication_TxRx_DMA_Init/Src/stm32l5xx_it.c          Interrupt handlers
  - USART/USART_Communication_TxRx_DMA_Init/Src/main.c                  Main program
  - USART/USART_Communication_TxRx_DMA_Init/Src/system_stm32l5xx.c      STM32L5xx system source file

@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up
    Connect USART3 TX/RX to respectively RX and TX pins of PC UART (could be done through a USB to UART adapter) :
    - Connect STM32 MCU board USART3 TX pin (GPIO PD.08 connected to pin 14 of Arduino connector CN10)
      to PC COM port RX signal
    - Connect STM32 MCU board USART3 RX pin (GPIO PD.09 connected to pin 16 of Arduino connector CN10)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control). 

  - Launch the program. Press on User push button on board to initiate data transfer.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
