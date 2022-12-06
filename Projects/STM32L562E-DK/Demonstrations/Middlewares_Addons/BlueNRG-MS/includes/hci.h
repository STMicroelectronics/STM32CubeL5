/**
  ******************************************************************************
  * @file    hci.h
  * @author  AMG RF FW team
  * @version V1.1.0
  * @date    18-July-2016
  * @brief   Header file for framework required for handling HCI interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/
#ifndef __HCI_H_
#define __HCI_H_

#include "bluenrg_types.h"

/** 
 * @addtogroup HIGH_LEVEL_INTERFACE HIGH_LEVEL_INTERFACE
 * @{
 */

/**
 * @defgroup HCI_TL HCI_TL
 * @{
 */

/** 
 * @defgroup HCI_TL_functions HCI_TL functions
 * @{
 */

/**
 * @brief Initialize the Host Controller Interface. 
 *        This function must be called before any data can be received 
 *        from BLE controller.
 *
 * @param  pData: ACI events callback function pointer
 *         This callback is triggered when an user event is received from 
 *         the BLE core device. 
 * @param  pConf: Configuration structure pointer
 * @retval None
 */
void hci_init(void(* UserEvtRx)(void* pData), void* pConf);

/**
 * @brief  Processing function that must be called after an event is received from
 *         HCI interface. 
 *         It must be called outside ISR. It will call user_notify() if necessary.
 *
 * @param  None
 * @retval None
 */ 
void hci_user_evt_proc(void);

/**
 * @}
 */

/**
 * @}
 */ 

/**
 * @}
 */  
#endif /* __HCI_H_ */
