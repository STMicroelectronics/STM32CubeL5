/**
  @page TIM_PWMOutput TIM PWM Output example
  
  @verbatim
  ******************************************************************************
  * @file    TIM/TIM_PWMOutput/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWM signals generation using TIM4
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

This example shows how to configure the TIM peripheral in PWM (Pulse Width Modulation) 
mode.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32L562QEIxQ Devices :
The CPU at 110 MHz 

SystemCoreClock is set to 110 MHz for STM32L5xx Devices.

    In this example TIM4 input clock (TIM4CLK) is set to APB1 clock (PCLK1),
    since APB1 prescaler is equal to 1.
      TIM4CLK = PCLK1
      PCLK1 = HCLK
      => TIM4CLK = HCLK = SystemCoreClock

    To get TIM4 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM4CLK / TIM4 counter clock) - 1
       Prescaler = ((SystemCoreClock) /1 MHz) - 1

    To get TIM4 output clock at 24 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM4 counter clock / TIM4 output clock) - 1
           = 40

    TIM4 Channel1 duty cycle = (TIM4_CCR1/ TIM4_ARR + 1)* 100 = 50%
    TIM4 Channel2 duty cycle = (TIM4_CCR2/ TIM4_ARR + 1)* 100 = 37.5%
    TIM4 Channel3 duty cycle = (TIM4_CCR3/ TIM4_ARR + 1)* 100 = 25%
    TIM4 Channel4 duty cycle = (TIM4_CCR4/ TIM4_ARR + 1)* 100 = 12.5%

LED9 is ON when there are an error.

The PWM waveforms can be displayed using an oscilloscope.

@note The duty cycles values mentioned above are theoretical (obtained when the system clock frequency is exactly 110 MHz).
      They might be slightly different depending on system clock frequency precision.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.
	  

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Keywords

Timer, Output, signal, PWM, Oscilloscope, Frequency, Duty cycle, Waveform

@par Directory contents

  - TIM/TIM_PWMOutput/Inc/stm32l562e_discovery_conf.h     BSP configuration file
  - TIM/TIM_PWMOutput/Inc/stm32l5xx_hal_conf.h    HAL configuration file
  - TIM/TIM_PWMOutput/Inc/stm32l5xx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMOutput/Inc/main.h                  Header for main.c module  
  - TIM/TIM_PWMOutput/Src/stm32l5xx_it.c          Interrupt handlers
  - TIM/TIM_PWMOutput/Src/main.c                  Main program
  - TIM/TIM_PWMOutput/Src/stm32l5xx_hal_msp.c     HAL MSP file
  - TIM/TIM_PWMOutput/Src/system_stm32l5xx.c      STM32L5xx system source file


@par Hardware and Software environment

  - This example runs on STM32L562QEIxQ devices.
  - In this example, the clock is set to 110 MHz.
    
  - This example has been tested with STMicroelectronics STM32L562E-DK 
    board and can be easily tailored to any other supported device 
    and development board.      

  - STM32L562E-DK Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:
        - TIM4_CH1 : PD.12 (pin 4 in CN12 connector)
        - TIM4_CH2 : PD.13 (pin 6 in CN12 connector)
        - TIM4_CH3 : PB.08 (pin 7 in CN12 connector)
        - TIM4_CH4 : PB.09 (pin 2 in CN11 connector)


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
