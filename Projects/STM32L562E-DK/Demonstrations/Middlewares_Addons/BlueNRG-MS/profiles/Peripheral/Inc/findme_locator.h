/******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  findme_locator.h
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*
*   $Date$:      25/07/2014
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
#ifndef _FIND_ME_LOCATOR_
#define _FIND_ME_LOCATOR_

/**********************************************************************************
* MACROS
**********************************************************************************/
/**
 * error codes for EVT_FML_DISCOVERY_CMPLT
 */
#define ALERT_CHARACTERISTIC_NOT_FOUND	(0x01)
#define IMM_ALERT_SERVICE_NOT_FOUND     (0x02)

/* Alert levels */
#define NO_ALERT		        (0x00)
#define MILD_ALERT                      (0x01)
#define HIGH_ALERT                      (0x02)
/**********************************************************************************
* Function Prototypes
**********************************************************************************/
/**
 * FindMeLocator_Init
 * 
 * @param[in] bleSecReq : security requirements of the profile
 * @param[in] FindMeTargetcb : callback function to be called
 *           by the profile to notify the application of events
 * 
 * Initializes the Find Me Target profile
 */ 
tBleStatus FindMeLocator_Init(tSecurityParams* bleSecReq,
			      BLE_CALLBACK_FUNCTION_TYPE FindMeTargetcb);
							 
/**
 * FML_Advertize
 * 
 * This function puts the device into
 * discoverable mode if it is in the
 * proper state to do so
 */ 
tBleStatus FML_Advertize(void);

/**
 * FML_Add_Device_To_WhiteList
 * 
 * @param[in] bdAddr : address of the peer device
 *            that has to be added to the whitelist
 */ 
tBleStatus FML_Add_Device_To_WhiteList(uint8_t addrType,uint8_t* bdAddr);

/**
 * FML_LimDisc_Timeout_Handler
 * 
 * This function is called when the advertising
 * for fast connection times out
 */ 
void FML_LimDisc_Timeout_Handler(void);

/**
 * FML_ALert_Target
 * 
 * @param[in] alertLevel : thw alert level the target has to be
 *            configured to\n
 *            NO_ALERT	   (0x00)\n
 *            MILD_ALERT   (0x01)\n
 *            HIGH_ALERT   (0x02)
 * 
 * Starts a write without response GATT procedure
 * to write the alertvalue to the find me target
 * 
 * @return returns BLE_STATUS_SUCCESS if the parameters
 * are valid and the procedure has been started else
 * returns error
 */ 
tBleStatus FML_ALert_Target(uint8_t alertLevel);

/**
 * FMLProfile_StateMachine
 * 
 * @param[in] None
 * 
 * FML profile's state machine: to be called on application main loop. 
 */ 
tBleStatus FMLProfile_StateMachine(void);

#endif
