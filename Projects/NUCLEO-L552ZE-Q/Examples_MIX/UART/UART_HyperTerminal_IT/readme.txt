/**
  @page UART_HyperTerminal_IT UART HyperTerminal IT example (HAL/LL mixed usage example)

  @verbatim
  ******************************************************************************
  * @file    UART/UART_HyperTerminal_IT/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the UART HAL/LL mixed Hyperterminal example.
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

Use of a UART to transmit data (transmit/receive)
between a board and an HyperTerminal PC application in Interrupt mode. This example
describes how to use the USART peripheral through the STM32L5xx UART HAL
and LL API, the LL API being used for performance improvement.

Board: NUCLEO-L552ZE-Q
Tx Pin: PG.07 (available through ST-Link)
Rx Pin: PG.08 (available through ST-Link)
   _________________________
  |           ______________|                       _______________
  |          |LPUART1        |                      | HyperTerminal |
  |          |              |                      |               |
  |          |           TX |______________________|RX             |
  |          |              |                      |               |
  |          |              |  Virtual Com Port    |               |
  |          |              |                      |               |
  |          |           RX |______________________|TX             |
  |          |              |                      |               |
  |          |______________|                      |_______________|
  |                         |
  |                         |
  |                         |
  |                         |
  |_STM32_Board_____________|

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 110 MHz for STM32L5xx Devices.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware.
You may update this function to change UART configuration.

The UART/Hyperterminal communication is then initiated.
Receive and Transmit functions which allow respectively
the reception of Data from Hyperterminal and the transmission of a predefined data
buffer, are implemented using LL USART API.

The Asynchronous communication aspect of the UART is clearly highlighted as the
data buffers transmission/reception to/from Hyperterminal are done simultaneously.

For this example the TX buffer (aTxStartMessage) is predefined and the RX buffer (aRxBuffer)
size is limited to 10 data by the mean of the RXBUFFERSIZE define in the main.c file.

In a first step the received data will be stored in the RX buffer and the
TX buffer content will be displayed in the Hyperterminal interface.
In a second step the received data in the RX buffer will be sent back to
Hyperterminal and displayed.

STM32 Nucleo board's LEDs can be used to monitor the transfer status:
 - LED1 is ON when transmission/reception of 10 characters is complete.
 - LED2 is ON when the reception process is complete.
 - LED3 is ON when there is an error in transmission/reception process.
 - LED3 toggles when there another error is detected.

The UART is configured as follows:
    - BaudRate = 9600 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

@note When the parity is enabled, the computed parity is inserted at the MSB
      position of the transmitted data.

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

Connectivity, UART/USART, baud rate, RS-232, HyperTerminal, full-duplex, Interrupt
Transmitter, Receiver, Asynchronous

@par Directory contents

  - UART/UART_Hyperterminal_IT/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - UART/UART_Hyperterminal_IT/Inc/stm32l5xx_it.h          IT interrupt handlers header file
  - UART/UART_Hyperterminal_IT/Inc/main.h                  Main program header file
  - UART/UART_Hyperterminal_IT/Src/stm32l5xx_it.c          IT interrupt handlers
  - UART/UART_Hyperterminal_IT/Src/main.c                  Main program
  - UART/UART_Hyperterminal_IT/Src/stm32l5xx_hal_msp.c     HAL MSP file
  - UART/UART_Hyperterminal_IT/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.

  - This example has been tested with STMicroelectronics NUCLEO-L552ZE-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-L552ZE-Q Set-up
    Example is delivered for using Virtual Com port feature of STLINK for connection between NUCLEO-L552ZE-Q C-02 and PC,
    Please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled
    on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)
    GPIOs connected to LPUART1 TX/RX (PG7 and PG8) are automatically mapped
    on RX and TX pins of PC UART Com port selected on PC side (please ensure VCP com port is selected).

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration
    - Word Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
    - Flow control: None

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
