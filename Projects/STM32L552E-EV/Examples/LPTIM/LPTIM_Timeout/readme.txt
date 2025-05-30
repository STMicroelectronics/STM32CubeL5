/**
  @page LPTIM_Timeout LPTIM Timeout example with LSE clock source
  
  @verbatim
  ******************************************************************************
  * @file    LPTIM/LPTIM_Timeout/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the LPTIM Timeout example.
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

How to implement, through the HAL LPTIM API, a timeout with the LPTIMER peripheral, to wake up 
the system from a low-power mode.

The main() function configures the LPTIMER and goes in STOP mode.
In order to start the timer a first trigger is needed on (PC.03)(LPTIM_ETR).
Any successive trigger event on (PC.03) will reset the counter and the timer 
will restart. The timeout value corresponds to the compare value (32768).
If after the first trigger no other trigger occurs within the expected time frame, 
the MCU is woken-up by the compare match event and LED_GREEN toggles.

In this example the LPTIM is clocked by the LSE 

  Timeout = (Compare + 1) / LPTIM_Clock
          = (32767 + 1) / LSE_Clock_Frequency

Once the system is woken up it remains in run mode. The led keeps toggling each time the timer expires.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Timer, Low Power Timer, Wake up, Stop mode, LSI, Run mode

@par Directory contents  

  - LPTIM/LPTIM_Timeout/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - LPTIM/LPTIM_Timeout/Inc/stm32l552e_eval_conf.h  Board configuration file
  - LPTIM/LPTIM_Timeout/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_Timeout/Inc/main.h                  Header for main.c module  
  - LPTIM/LPTIM_Timeout/Src/stm32l5xx_it.c          Interrupt handlers
  - LPTIM/LPTIM_Timeout/Src/main.c                  Main program
  - LPTIM/LPTIM_Timeout/Src/stm32l5xx_hal_msp.c     HAL MSP module
  - LPTIM/LPTIM_Timeout/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L552ZETxQ devices.
    
  - This example has been tested with STMicroelectronics STM32L552E-EV
    board and can be easily tailored to any other supported device
    and development board.   
	
  - Connect an external trigger (ETR) to PC.03 (pin 30 in CN6 connector). 
    If the trigger is higher than 1Hz, the counter is regularly reset, the system stays in STOP mode.
    If the trigger is lower than 1Hz, the counter expires and the system is woken up.

@note Due to the sharing of some I/Os of STM32L552ZET6QU by multiple peripherals,
      the following limitations apply in using the LED features:
      The green LED I/O cannot be operated simultaneously with JTAG SWO

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred tool chain 
 - Rebuild all files and load your image into target memory
 - Run the example 


 */
