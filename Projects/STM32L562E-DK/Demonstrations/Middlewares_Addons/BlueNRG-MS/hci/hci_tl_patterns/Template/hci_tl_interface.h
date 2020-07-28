/**
  ******************************************************************************
  * @file    hci_tl_interface_template.h
  * @author  CL
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HCI_TL_INTERFACE_H
#define __HCI_TL_INTERFACE_H

#ifdef __cplusplus
 extern "C" {
#endif 

/** 
 * @addtogroup LOW_LEVEL_INTERFACE LOW_LEVEL_INTERFACE
 * @{
 */
 
/** 
 * @defgroup LL_HCI_TL_INTERFACE HCI_TL_INTERFACE
 * @{
 */
 
/** 
 * @defgroup LL_HCI_TL_INTERFACE_TEMPLATE TEMPLATE
 * @{
 */ 

/* Includes ------------------------------------------------------------------*/

/* Exported Defines ----------------------------------------------------------*/

/* Exported Functions --------------------------------------------------------*/
/** 
 * @defgroup LL_HCI_TL_INTERFACE_TEMPLATE_Functions Exported Functions
 * @{
 */
/**
 * @brief  Register hci_tl_interface IO bus services and the IRQ handlers.
 *         This function must be implemented by the user at the application level.
 *         E.g., in the following, it is provided an implementation example in the case of the SPI:
 *         @code
           void hci_tl_lowlevel_init(void)
           {
             tHciIO fops;  
  
             //Register IO bus services 
             fops.Init    = HCI_TL_SPI_Init;
             fops.DeInit  = HCI_TL_SPI_DeInit;
             fops.Send    = HCI_TL_SPI_Send;
             fops.Receive = HCI_TL_SPI_Receive;
             fops.Reset   = HCI_TL_SPI_Reset;
             fops.GetTick = BSP_GetTick;
  
             hci_register_io_bus (&fops);
  
             //Register event irq handler 
             HAL_EXTI_GetHandle(&hexti0, EXTI_LINE_0);
             HAL_EXTI_RegisterCallback(&hexti0, HAL_EXTI_COMMON_CB_ID, hci_tl_lowlevel_isr);
             HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
             HAL_NVIC_EnableIRQ(EXTI0_IRQn);
           }
 *         @endcode
 *
 * @param  None
 * @retval None
 */
void hci_tl_lowlevel_init(void);

/**
 * @brief HCI Transport Layer Low Level Interrupt Service Routine.
 *        The Interrupt Service Routine must be called when the BlueNRG-MS 
 *        reports a packet received or an event to the host through the 
 *        BlueNRG-MS interrupt line.
 *        E.g. in case of data travelling over SPI
 *        @code
          void hci_tl_lowlevel_isr(void)
          {
            while(IsDataAvailable())
            {        
              hci_notify_asynch_evt(NULL);
            }
          }
 *        @endcode
 *        where IsDataAvailable() checks the status of the SPI external interrupt pin 
 *        @code
          static int32_t IsDataAvailable(void)
          {
            return (HAL_GPIO_ReadPin(HCI_TL_SPI_EXTI_PORT, HCI_TL_SPI_EXTI_PIN) == GPIO_PIN_SET);
          } 
 *        @endcode  
 *
 * @param  None
 * @retval None
 */
void hci_tl_lowlevel_isr(void);

/**
 * @}
 */
 
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __HCI_TL_INTERFACE_TEMPLATE_H_ */
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

