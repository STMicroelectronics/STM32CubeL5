/**
  @page UART_TwoBoards_ComPolling UART TwoBoards Communication Polling example

  @verbatim
  ******************************************************************************
  * @file    UART/UART_TwoBoards_ComPolling/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the UART Two Boards Communication Polling example.
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

UART transmission (transmit/receive) in Polling mode 
between two boards.

Board: NUCLEO-L552ZE-Q (embeds a STM32L552ZETxQ device)
Tx Pin: PA.02 (Arduino connector CN9 pin A1)
Rx Pin: PA.03 (Arduino connector CN9 pin A0)
   _________________________                       _________________________ 
  |           ______________|                     |______________           |
  |          |USART         |                     |         USART|          |
  |          |              |                     |              |          |
  |          |           TX |_____________________| RX           |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |           RX |_____________________| TX           |          |
  |          |              |                     |              |          |
  |          |______________|                     |______________|          |
  |                         |                     |                         |
  |                      GND|_____________________|GND                      |
  |_STM32_Board 1___________|                     |_STM32_Board 2___________|



Two identical boards are connected as shown on the picture above.
Board 1: transmitting then receiving board
Board 2: receiving then transmitting board

The user presses the User push-button on board 1.
Then, board 1 sends in polling mode a message to board 2 that sends it back to 
board 1 in polling mode as well.
Finally, board 1 and 2 compare the received message to that sent.
If the messages are the same, the test passes.


WARNING: as both boards do not behave the same way, "TRANSMITTER_BOARD" compilation
switch is defined in /Src/main.c and must be enabled
at compilation time before loading the executable in the board that first transmits
then receives.
The receiving then transmitting board needs to be loaded with an executable
software obtained with TRANSMITTER_BOARD disabled. 

NUCLEO-L552ZE-Q board LEDs are used to monitor the transfer status:
- While board 1 is waiting for the user to press the User push-button, its LED2 is
  blinking rapidly (100 ms period).
- When the test passes, LED2 is turned on.
- If there is an initialization or transfer error, LED3 is turned on.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 110 MHz.


The UART is configured as follows:
    - BaudRate = 9600 baud  
    - Word Length = 8 bits (8 data bits, no parity bit)
    - One Stop Bit
    - No parity
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

Connectivity, UART/USART, baud rate, RS-232, full-duplex, Polling, parity, stop bit,
Transmitter, Receiver, Asynchronous

@par Directory contents 

  - UART/UART_TwoBoards_ComPolling/Inc/stm32l5xx_nucleo_conf.h     BSP configuration file
  - UART/UART_TwoBoards_ComPolling/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - UART/UART_TwoBoards_ComPolling/Inc/stm32l5xx_it.h          interrupt handlers header file
  - UART/UART_TwoBoards_ComPolling/Inc/main.h                  Header for main.c module  
  - UART/UART_TwoBoards_ComPolling/Src/stm32l5xx_it.c          interrupt handlers
  - UART/UART_TwoBoards_ComPolling/Src/main.c                  Main program
  - UART/UART_TwoBoards_ComPolling/Src/stm32l5xx_hal_msp.c     HAL MSP module
  - UART/UART_TwoBoards_ComPolling/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment 

  - This example runs on STM32L552ZETxQ devices.    
  - This example has been tested with two NUCLEO-L552ZE-Q boards embedding
    a STM32L552ZETxQ device and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-L552ZE-Q set-up
    - Connect a wire between 1st board PA2 pin (Uart Tx) and 2nd board PA3 pin (Uart Rx)
    - Connect a wire between 1st board PA3 pin (Uart Rx) and 2nd board PA2 pin (Uart Tx)
    - Connect 1st board GND to 2nd Board GND    

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
