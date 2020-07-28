/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  findme_target.h
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*
*   $Date$:      024/07/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Find Me Profile according to unified BlueNRG 
*                DK framework
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header for the find me target profile
******************************************************************************/
#ifndef _FIND_ME_TARGET_
#define _FIND_ME_TARGET_

/**
 * FindMeTarget_Init
 * 
 * @param[in] FindMeTargetcb : callback function to be called
 *           by the profile to notify the application of events
 * 
 * Initializes the Find Me Target profile
 */ 
tBleStatus FindMeTarget_Init(BLE_CALLBACK_FUNCTION_TYPE FindMeTargetcb);
							 
/**
 * FMT_Advertize
 * 
 * This function puts the device into
 * discoverable mode if it is in the
 * proper state to do so
 */ 
tBleStatus FMT_Advertize(void);

/**
 * FMT_Add_Device_To_WhiteList
 * 
 * @param[in] addrType : address type of the bdAddr
 *            to be added to the whitelist
 * @param[in] bdAddr : address of the peer device
 *            that has to be added to the whitelist
 * 
 * Adds a device to the whitelist
 */ 
tBleStatus FMT_Add_Device_To_WhiteList(uint8_t addrType,uint8_t* bdAddr);

/**
 * FMT_LimDisc_Timeout_Handler
 * 
 * This function is called when the advertising
 * for fast connection times out
 */ 
void FMT_LimDisc_Timeout_Handler(void);

/**
 * FMTProfile_StateMachine
 * 
 * @param[in] None
 * 
 * FMT profile's state machine: to be called on application main loop. 
 */ 
tBleStatus FMTProfile_StateMachine(void);

#endif
