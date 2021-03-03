/**
  @page USART_SlaveMode USART Slave Mode example

  @verbatim
  ******************************************************************************
  * @file    USART/USART_SlaveMode/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the USART_SlaveMode example.
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

This example describes an USART-SPI communication (transmit/receive) between two
boards where the USART is configured as a slave.

Boards: STM32L562E-DK (embeds a STM32L562QEIxQ device)

   _________________________                        _________________________
  |          _______________|                      |_______________          |
  |         |      SPI3     |                      |    USART1     |         |
  |         |               |                      |               |         |
  |         | PG9 (CN11.pin 6)|SCK_________________CK|(CN11.pin 4)  PB5 |         |
  |         |               |                      |               |         |
  |         | PG10 (CN3.pin 9)|MISO________________TX|(CN11.pin 10)  PB6 |         |
  |         |               |                      |               |         |
  |         | PD6 (CN3.pin 8)|MOSI________________RX|(CN11.pin 9) PB7 |         |
  |         |               |                      |               |         |
  |         |_______________|                      |_______________|         |
  |      __                 |                      |                         |
  |     |__|                |                      |                         |
  |     USER                |                      |                         |
  |                      GND|______________________|GND                      |
  |                         |                      |                         |
  |_STM32L5xx_Master________|                      |_STM32L5xx_Slave_________|


Two identical boards are connected as shown on the picture above.
Board 1: SPI Master board
Board 2: USART Slave board

In a first step, after the user presses the User push-button, Master (SPI) starts
the communication by sending aTxBuffer and receiving aRxBuffer through
HAL_SPI_TransmitReceive(), at the same time Slave (USART) transmits aTxBuffer
and receives aRxBuffer through HAL_USART_TransmitReceive().
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to
check buffers correctness.

WARNING: as both boards do not behave the same way, "SLAVE_BOARD" compilation
switch is defined in Src/main.c and must be enabled at compilation time before
loading the executable in the USART slave board.
The SPI master board needs to be loaded with an executable software obtained
with SLAVE_BOARD disabled.

STM32L562E-DK board LEDs are used to monitor the transfer status:
- While board 1 is waiting for the user to press the User push-button, its
  LED10 is blinking rapidly (100 ms period).
- After the transmission/reception processes begin LED10 is turned off.
- When the test passes, LED10 on both boards is turned on, otherwise the
  test has failed.
- If there is an initialization or transfer error, LED9 blinks slowly (500 ms).

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32L562QEIxQ Devices.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Connectivity, UART/USART, SPI interface, full-duplex, Polling, Receiver, Transmitter, Synchronous, Slave,

@par Directory contents

  - USART/USART_SlaveMode/Inc/stm32l562e_discovery_conf.h     BSP configuration file
  - USART/USART_SlaveMode/Src/main.c                 Main program
  - USART/USART_SlaveMode/Src/system_stm32l5xx.c     STM32L5xx system clock configuration file
  - USART/USART_SlaveMode/Src/stm32l5xx_it.c         Interrupt handlers
  - USART/USART_SlaveMode/Src/stm32l5xx_hal_msp.c    HAL MSP module
  - USART/USART_SlaveMode/Inc/main.h                 Main program header file
  - USART/USART_SlaveMode/Inc/stm32l5xx_hal_conf.h   HAL Configuration file
  - USART/USART_SlaveMode/Inc/stm32l5xx_it.h         Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32L562QEIxQ devices.

  - This example has been tested with two STM32L562E-DK boards embedding
    a STM32L562QEIxQ device and can be easily tailored to any other supported device
    and development board.

  - STM32L562E-DK set-up
    - Connect Master board PG9 (SCK) to Slave Board PB5 (USART CK)
    - Connect Master board PG10 (MISO) to Slave Board PB6 (USART TX)
    - Connect Master board PD6 (MOSI) to Slave Board PB7 (USART RX)
    - Connect Master board GND to Slave Board GND

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
   - Uncomment "#define SLAVE_BOARD" and load the project in Slave Board
   - Comment "#define SLAVE_BOARD" and load the project in Master Board
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
