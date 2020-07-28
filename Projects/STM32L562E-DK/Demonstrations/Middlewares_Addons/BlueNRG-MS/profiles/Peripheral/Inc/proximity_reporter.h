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
*   FILENAME        -  proximity_reporter.h
*
*   COMPILER        -  EWARM
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      04/09/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Proximity Reporter Profile implementation header file. 
*
********************************************************************************
*
*  File Description 
*  ---------------------
*
*******************************************************************************/
#ifndef _PROXIMITY_REPORTER_H_
#define _PROXIMITY_REPORTER_H_

/*******************************************************************************
* Macros
*******************************************************************************/
/* alert levels */
#define NO_ALERT		      (0x00)
#define MILD_ALERT                    (0x01)
#define HIGH_ALERT                    (0x02)

/******************************************************************************
* function prototypes
******************************************************************************/
/**
 * ProximityReporter_Init
 * 
 * @param[in] immAlertTxPowerSupport : set to a non zero value if the TX power
 *            level and immediate alert services are to be exposed by the profile
 *            else set to 0
 * @param[in] profileCallbackFunc : the callback to be called to notify the
 *            application of the profile specific events
 */ 
tBleStatus ProximityReporter_Init(uint8_t immAlertTxPowerSupport, 
				  BLE_CALLBACK_FUNCTION_TYPE profileCallbackFunc);

/**
 * ProximityReporter_Make_Discoverable
 * 
 * @param[in] useBoundedDeviceList: advertize with whitelist which contains 
 *             devices that were previously bonded
 * 
 * puts the device into discoverable mode
 * 
 * @return BLE_STATUS_SUCCESS if the command to put the device into
 * discoverable mode was successfully sent
 */ 
tBleStatus ProximityReporter_Make_Discoverable(uint8_t useBoundedDeviceList);

/**
 * ProximityReporterProfile_StateMachine
 * 
 * @param[in] None
 * 
 * Proximity Reporter profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ProximityReporterProfile_StateMachine(void);


#endif  /* _PROXI_REPORTER_H_ */
