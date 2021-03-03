/**
  @page USART_Communication_Rx_IT USART Receiver example (IT Mode)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_Communication_Rx_IT/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_Communication_Rx_IT example.
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

Configuration of GPIO and USART peripherals to receive characters 
from an HyperTerminal (PC) in Asynchronous mode using an interrupt. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size).

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
GPIO associated to User push-button is linked with EXTI.
USART RX Not Empty interrupt is enabled.

Example execution:
When character is received on USART Rx line, a RXNE interrupt occurs.
USART IRQ Handler routine is then checking received character value.
On a specific value ('S' or 's'), LED1 is turned On.
Received character is echoed on Tx line.
On press on User push-button, LED1 is turned Off.
In case of errors, LED1 is blinking.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, UART/USART, Asynchronous, RS-232, baud rate, Interrupt, HyperTerminal, Receiver

@par Directory contents 

  - USART/USART_Communication_Rx_IT/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - USART/USART_Communication_Rx_IT/Inc/main.h                  Header for main.c module
  - USART/USART_Communication_Rx_IT/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_Communication_Rx_IT/Src/stm32l5xx_it.c          Interrupt handlers
  - USART/USART_Communication_Rx_IT/Src/main.c                  Main program
  - USART/USART_Communication_Rx_IT/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with NUCLEO-L552ZE-Q C-02 board and can be
    easily tailored to any other supported device and development board.
    
  - NUCLEO-L552ZE-Q C-02 Set-up
    Connect USART3 TX/RX to respectively RX and TX pins of PC UART (could be done through a USB to UART adapter):
    - Connect STM32 MCU board USART3 TX pin (GPIO PD.08 connected to pin D1 of CN10 Arduino connector)
      to PC COM port RX signal
    - Connect STM32 MCU board USART3 RX pin (GPIO PD.09 connected to pin D0 of CN10 Arduino connector)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal
    
  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control). 

  - Launch the program. Enter characters on PC communication SW side.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
