/*******************************************************************************
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
*   FILENAME        -  proximity_monitor.h
*
*   COMPILER        -  EWARM
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      19/08/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Proximity Monitor Profile implementation header file.
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  This file will have Declarations for the implementation of 
*  Proximity Monitor Profile. 
*
*******************************************************************************/
#ifndef _PROXIMITY_MONITOR_H_
#define _PROXIMITY_MONITOR_H_

/*******************************************************************************
* Macros
*******************************************************************************/
/* alert levels */
#define NO_ALERT		          (0x00)
#define MILD_ALERT                        (0x01)
#define HIGH_ALERT                        (0x02)

#define PATH_LOSS_THRESHOLD               (0x7F)

#define LINK_LOSS_ALERT_LEVEL		  (MILD_ALERT)
#define PATH_LOSS_ALERT_LEVEL		  (MILD_ALERT)
/**
 * Error codes for EVT_PM_DISCOVERY_CMPLT
 */ 
#define PM_LINK_LOSS_SERVICE_NOT_FOUND	  (0x01)

/******************************************************************************
 * function prototypes
 *****************************************************************************/
/**
 * ProximityMonitor_Init
 * 
 * @param[in] profileCallbackFunc : the callback function to be called by the
 *            proifle to notify the application of the profile specific events
 * 
 * Initializes the proximity monitor
 * 
 * @return BLE_STATUS_SUCCESS if the initialization was successful
 */ 
tBleStatus ProximityMonitor_Init(BLE_CALLBACK_FUNCTION_TYPE profileCallbackFunc);

/**
 * ProximityMonitor_Make_Discoverable
 * 
 * @param[in] useBoundedDeviceList: advertize with whitelist which contains 
 *             devices that were previously bonded
 * 
 * puts the device into discoverable mode
 * 
 * @return BLE_STATUS_SUCCESS if the command to put the device into
 * discoverable mode was successfully sent
 */ 
tBleStatus ProximityMonitor_Make_Discoverable(uint8_t useBoundedDeviceList);

/**
 * ProximityMonitor_Notify_Path_Loss
 *
 * This function is added to enable/disable the alert
 * for pathLoss alerts from the command line.
 * Note : This function is only for testing using PTS as devices cannot
 * be physically moved away.
 *
 * @return BLE_STATUS_SUCCESS if the command was successfully sent 
 */
tBleStatus ProximityMonitor_Notify_Path_Loss(uint8_t enableAlert);

/**
 * ProximityMonitorProfile_StateMachine
 * 
 * @param[in] None
 * 
 * Proximity Monitor profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ProximityMonitorProfile_StateMachine(void);


#endif  /* _PROXIMITY_MONITOR_H_ */
