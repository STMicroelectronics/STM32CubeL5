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
*   FILENAME        -  time_client.h
*
*   COMPILER        -  gnu gcc
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      25/08/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Redesigned the Time Profile's implementation header file for 
*                client role according to unified BlueNRG DK framework. 
*
********************************************************************************
*
*  File Description 
*  ---------------------
* This file have all Time Client's declarations. 
* 
*******************************************************************************/
#ifndef _TIME_CLIENT_H_
#define _TIME_CLIENT_H_

#include "time_profile_types.h"
/*******************************************************************************
* Macros
*******************************************************************************/
/** masks for the services 
 * and characteristics discovered
 */
#define CURRENT_TIME_SERVICE_MASK                         (0x0001)
#define NEXT_DST_CHANGE_SERVICE_MASK                      (0x0002)
#define REFERENCE_UPDATE_TIME_SERVICE_MASK                (0x0004)
#define CURRENT_TIME_CHAR_MASK                            (0x0008)
#define LOCAL_TIME_INFORMATION_CHAR_MASK                  (0x0010)
#define REFERENCE_TIME_INFORMATION_CHAR_MASK              (0x0020)
#define TIME_WITH_DST_CHAR_MASK                           (0x0040)
#define TIME_UPDATE_CONTROL_POINT_CHAR_MASK               (0x0080)
#define TIME_UPDATE_STATE_CHAR_MASK                       (0x0100)

/* error codes */
#define CT_CHARACTERISTIC_NOT_FOUND							(0x01)
#define CT_SERVICE_NOT_FOUND								(0x02)

/*******************************************************************************
* function prototypes
*******************************************************************************/
/**
 * TimeClient_Init
 * 
 * @param[in] profileCallbackFunc : callback to be called by the profile to
 *            notify the application of the profile specific events
 * 
 * Initializes the time client and registers the time client profile with
 * the main profile
 */ 
tBleStatus TimeClient_Init(BLE_CALLBACK_FUNCTION_TYPE profileCallbackFunc);

/**
 * TimeClient_Make_Discoverable
 * 
 * @param[in] useBoundedDeviceList: advertize with whitelist which contains 
 *             devices that were previously bonded
 * 
 * puts the device into discoverable mode
 * 
 * @return BLE_STATUS_SUCCESS if the command to put the device into
 * discoverable mode was successfully sent
 */ 
tBleStatus TimeClient_Make_Discoverable(uint8_t useBoundedDeviceList);

/**
 * TimeClient_Get_Current_Time
 * 
 * starts a GATT procedure to read the current time
 * characteristic. Once the read is complete the read value
 * is given to the application through an event\n
 * 
 * the event data will have the following format:\n
 * byte --  content\n
 * 0    --  year\n
 * 2    --  month\n
 * 3	--  date\n
 * 4	--	hours\n
 * 5	--	minutes\n
 * 6	--	seconds\n
 * 7	--	day_of_week\n
 * 8	--	fractions256\n
 * 9	--	adjust_reason
 */ 
tBleStatus TimeClient_Get_Current_Time(void);

/**
 * TimeClient_Get_Local_Time_Information
 * 
 * Starts a gatt read procedure to read the local time
 * information characteristic on the server. The information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Local_Time_Information(void);

/**
 * TimeClient_Get_Time_Accuracy_Info_Of_Server
 * 
 * Starts a gatt read procedure to read the reference time
 * information characteristic on the server. the information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Time_Accuracy_Info_Of_Server(void);

/**
 * TimeClient_Get_Next_DST_Change_Time
 * 
 * Starts a gatt read procedure to read the time with DST
 * characteristic on the server. The information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Next_DST_Change_Time(void);

/**
 * TimeClient_Get_Server_Time_Update_State
 * 
 * Starts a gatt read procedure to read the time update state
 * characteristic on the server. The information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Server_Time_Update_State(void);

/**
 * TimeClient_Update_Reference_Time_On_Server
 * 
 * @param[in] ctlValue: the value passed starts
 * a procedure to cancel or start a update process\n
 * 
 * Starts a gatt write without response procedure to 
 * write the time update control point characteristic  
 * on the server. The information read will be passed 
 * to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Update_Reference_Time_On_Server(uint8_t ctlValue);


/**
 * TimeClient_Update_Reference_Time_On_Server
 * 
 * @param[in] ctlValue: the value passed starts
 * a procedure to cancel or start a update process\n
 * 
 * Starts a gatt write without response procedure to 
 * write the time update control point characteristic  
 * on the server. The information read will be passed 
 * to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Update_Reference_Time_On_Server(uint8_t ctlValue);

/**
 * TimeClient_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB().
 */ 
void TimeClient_Event_Handler(void *pckt);

/**
 * TimeClient_StateMachine
 * 
 * @param[in] None
 * 
 * TimeClient profile's state machine: to be called on application main loop. 
 */ 
tBleStatus TimeClient_StateMachine(void);

/**
 * TimeClient_SetReset_Time_Update_Notification
 * @param[in] onoff : A value of 1 enables notifictions
 *            and 1 value of 0 disables notifications
 * 
 * By default notification of any change in Time on Time server 
 * will remain disabled. If the application on Time client need
 * the time update notification, it need to enable the notification
 * using this function.Once enabled, the notification of time 
 * update on time server will remain enabled. if the application 
 * dont want to receive the notification any more, it need to 
 * disable the notification using this function. 
 */
tBleStatus TimeClient_SetReset_Time_Update_Notification(uint8_t onoff);

#endif  /* _TIME_CLIENT_H_ */
