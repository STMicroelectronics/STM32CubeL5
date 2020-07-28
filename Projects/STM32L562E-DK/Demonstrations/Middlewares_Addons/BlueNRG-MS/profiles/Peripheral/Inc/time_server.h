/*******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  time_server.h
*
*   COMPILER        -  gnu gcc
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      31/07/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Redesigned the Time Profile's implementation header file for 
*                server role according to unified BlueNRG DK framework.
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  This file have Time Profile's Server Role's declarations. 
*
*******************************************************************************/
#ifndef _TIME_SERVER_H_
#define _TIME_SERVER_H_

#include "time_profile_types.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define NEXT_DST_CHANGE_SERVICE_BITMASK		(0x01)
#define REFERENCE_TIME_UPDATE_SERVICE_BITMASK   (0x02)

#define REFERENCE_UPDATE_TIMEOUT		(300)/* 5 minutes */

/******************************************************************************
* function prototypes
*****************************************************************************/
/**
 * TimeServer_Init
 * 
 * @param[in] servicesToBeSupported: bit mask of the optional services 
 *            to be supported
 * @param[in] timeServerCallbackFunc : function to be called by the profile
 *            to notify the application of profile specific events
 * 
 * Initializes the time profile in server role
 * 
 * @return BLE_STATUS_SUCCESS if the time server is successfully initialized
 */ 
tBleStatus TimeServer_Init(uint8_t servicesToBeSupported, 
                           BLE_CALLBACK_FUNCTION_TYPE timeServerCallbackFunc);

/**
 * TimeServer_Make_Discoverable
 * 
 * @param[in] useBoundedDeviceList: advertize with whitelist which contains 
 *             devices that were previously bonded
 * 
 * puts the device into discoverable mode
 * 
 * @return BLE_STATUS_SUCCESS if the command to put the device into
 * discoverable mode was successfully sent
 */ 
tBleStatus TimeServer_Make_Discoverable(uint8_t useBoundedDeviceList);

/**
 * TimeServer_Update_Current_Time_Value
 * 
 * @param[in] timeValue : the time with which the current time
 *            characteristic has to be updated
 * 
 * updates the current time characteristic with the timeValue
 * specified
 */ 
tBleStatus TimeServer_Update_Current_Time_Value(tCurrentTime timeValue);

/**
 * TimeServer_Update_Local_Time_Information
 * 
 * @param[in] localTimeInfo : the local time information with which the local time
 *            information characteristic has to be updated
 * 
 * updates the localTimeInfo characteristic with the value specified
 */ 
tBleStatus TimeServer_Update_Local_Time_Information(tLocalTimeInformation localTimeInfo);

/**
 * TimeServer_Update_Reference_Time_Information
 * 
 * @param[in] refTimeInfo : the new reference time information
 * 
 * updates the reference time information characteristic with 
 * the value specified
 */ 
tBleStatus TimeServer_Update_Reference_Time_Information(tReferenceTimeInformation refTimeInfo);

/**
 * TimeServer_Update_Next_DST_Change
 * 
 * @param[in] timeDST : the new DST information
 * 
 * updates the next DST change characteristic with 
 * the value specified
 */ 
tBleStatus TimeServer_Update_Next_DST_Change(tTimeWithDST timeDST);

/**
 * Update_Reference_Time
 * 
 * @param[in] errCode: the error code with which the update
 *            completed
 * @param[in] newTime : new time to update the current time
 */ 
void Update_Reference_Time(uint8_t errCode,tCurrentTime* newTime);

/**
 * Allow_Curtime_Char_Read
 * 
 * sends the allow read command to the controller
 */ 
tBleStatus Allow_Curtime_Char_Read(void);

/**
 * TimeServer_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB().
 */ 
void TimeServer_Event_Handler(void *pckt);

/**
 * TimeServer_StateMachine
 * 
 * @param[in] None
 * 
 * TimeServer profile's state machine: to be called on application main loop. 
 */ 
tBleStatus TimeServer_StateMachine(void);


#endif  /* _TIME_SERVER_H_ */
