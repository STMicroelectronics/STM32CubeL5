/*******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2015 STMicroelectronics International NV
*
*   FILENAME        -  time_server.h
*
*   COMPILER        -  EWARM
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*   $Date$:      02/23/2015
*   $Revision$:  first version
*   $Author$:    
*   Comments:    Time Server Profile Central role implementation header file 
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  This file has Time Server Profile Central role's declarations. 
*
*******************************************************************************/
#ifndef _TIME_SERVER_H_
#define _TIME_SERVER_H_

#include <host_config.h>
#include <hci.h>

#include "bluenrg_aci.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"

#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>
#include "time_profile_types.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Time server profile main states */
#define TIME_SERVER_STATE_UNINITIALIZED           0x0000
#define TIME_SERVER_STATE_INITIALIZED             0x0001
#define TIME_SERVER_STATE_DEVICE_DISCOVERY        0x0002
#define TIME_SERVER_STATE_START_CONNECTION        0x0003
#define TIME_SERVER_STATE_CONNECTED               0x0004
#define TIME_SERVER_STATE_CONN_PARAM_UPDATE_REQ   0x0005

/* substates under TIME_SERVER_STATE_CONNECTED main state */
#define TIME_SERVER_STATE_CONNECTED_IDLE          (0x06)
#define TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE (0x07)

/* Events to be notified to user application */
#define EVT_TS_START_REFTIME_UPDATE	          (0x08)			
#define EVT_TS_STOP_REFTIME_UPDATE		  (0x09)		
#define EVT_TS_CURTIME_READ_REQ		          (0x0A)
#define EVT_TS_CHAR_UPDATE_CMPLT                  (0x0B)

#define NEXT_DST_CHANGE_SERVICE_BITMASK		(0x01)
#define REFERENCE_TIME_UPDATE_SERVICE_BITMASK   (0x02)

#define REFERENCE_UPDATE_TIMEOUT		(300)/* 5 minutes */

/******************************************************************************
* Type definitions
******************************************************************************/
/*  Central Information */
typedef struct tipsInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
  /** flag for defining services to be supported */
  uint8_t servicesToBeSupported;
} tipsInitDevType;

typedef struct tipsSecurityS {
  /** Set the IO capabilities of the device.*/
  uint8_t ioCapability;
  /** Man in the middle mode */
  uint8_t mitm_mode;
  /** Out of Band authentication enabled */
  uint8_t oob_enable;
  /** Out of Band data */
  uint8_t oob_data[16];
  /** Bonding Mode flag */
  uint8_t bonding_mode;
  /** Use fixed pin during pairing process */
  uint8_t use_fixed_pin;
  /** Fixed pin to be used during pairing if 
      MIMT protection is enabled. 
      Any random value between 0 to 999999 
  */
  uint32_t fixed_pin;
} tipsSecurityType;

typedef struct tipsConnDevS {
  /** The master shall use the fastScanInterval and fastScanWindow parameters for the duration
   *  set in this variable. After this duration the master should switch to the reducedPowerScanInterval
   *  and reducedPowerScanWindow to reduce the power consumption
   */
  uint32_t fastScanDuration;
  /** Fast time interval from when the Controller started its last LE scan until it begins
   *  the subsequent LE scan. The scan interval should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan interval has a duration of 30 sec.
   */
  uint16_t fastScanInterval;
  /** Amount of time for the duration of the LE Fast scan. Scan_Window shall be less than
   *  or equal to Scan_Interval. The scan window should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan window ha a duration of 30 sec.
   */
  uint16_t fastScanWindow;
  /** Reduced power time interval from when the Controller started its last LE scan until it begins
   *  the subsequent LE scan. The scan interval should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan interval will be used after 30 s to reduce the power consumption.
   */
  uint16_t reducedPowerScanInterval;
  /** Amount of time for the duration of the LE reduced power scan. Scan_Window shall be less than
   *  or equal to Scan_Interval. The scan window should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan interval will be used after 30 s to reduce the power consumption.
   */
  uint16_t reducedPowerScanWindow;
  /** Type of the peer address (0=PUBLIC_ADDR, 1=RANDOM_ADDR). */
  uint8_t peer_addr_type;
  /** Address of the peer device with which a connection has to be established. */
  uint8_t *peer_addr;
  /** Type of our address used during advertising (0=PUBLIC_ADDR, 1=RANDOM_ADDR). */
  uint8_t own_addr_type;
  /** Minimum value for the connection event interval. This shall be less than or
   *  equal to Conn_Interval_Max.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */
  uint16_t conn_min_interval;
  /** Maximum value for the connection event interval. This shall be greater than or
   *  equal to Conn_Interval_Min.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */
  uint16_t conn_max_interval;
  /** Slave latency for the connection in number of connection events.\n
   *  Range: 0x0000 to 0x01F4
   */
  uint16_t conn_latency;
  /** Supervision timeout for the LE Link.\n
   *  Range: 0x000A to 0x0C80\n
   *  Time = N x 10 msec\n
   *  Time Range: 100 msec to 32 seconds
   */
  uint16_t supervision_timeout;
  /** Minimum length of connection needed for the LE connection.\n
   *  Range: 0x0000 - 0xFFFF\n
   *  Time = N x 0.625 msec.
   */
  uint16_t min_conn_length;
  /** Maximum length of connection needed for the LE connection.\n
   *  Range: 0x0000 - 0xFFFF\n
   *  Time = N x 0.625 msec.
   */
  uint16_t max_conn_length;
} tipsConnDevType;

typedef struct tipsDevDiscS {
  /** Type of our address used during advertising (PUBLIC_ADDR, RANDOM_ADDR). */
  uint8_t own_addr_type;
  /** Time interval from when the Controller started its last LE scan until it begins
   *  the subsequent LE scan. The scan interval should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   */
  uint16_t scanInterval;
  /** Amount of time for the duration of the LE scan. The scan window shall be less than
   *  or equal to scan interval. The scan window should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   */
  uint16_t scanWindow;
} tipsDevDiscType;

typedef struct tipsConnUpdateParamS {
  /** This is the identifier which associates the request to the
   * response. The same identifier has to be returned by the upper
   * layer in the command Master_ConnectionUpdateParamResponse().
   */
  uint8_t  identifier; 
  /** Minimum value for the connection event interval. This shall be less than or
   *  equal to interval_max.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */  
  uint16_t interval_min;
  /** Maximum value for the connection event interval. This shall be greater than or
   *  equal to interval_min.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */
  uint16_t interval_max;
  /** Slave latency for the connection in number of connection events.\n
   *  Range: 0x0000 to 0x01F4
   */
  uint16_t slave_latency;
  /** Supervision timeout for the LE Link.\n
   *  Range: 0x000A to 0x0C80\n
   *  Time = N x 10 msec\n
   *  Time Range: 100 msec to 32 seconds
   */
  uint16_t timeout_mult;
} tipsConnUpdateParamType;

/* Profile Context variable */
typedef struct TimeServerContextS {
  uint32_t state;
  uint8_t subState;
  uint8_t public_addr[6];
  uint16_t connHandle;
  
  /**
   * flag which is set at the time of initialization
   * by the application. It indicates whether the optional
   * services of Next DST Change and Reference Update Time 
   * are supported
   */ 
  uint8_t servicesSupported;
  
  uint8_t ctCharNotifyDelayTimeout;
  
  uint8_t ctCharNotificationConfig;

  /**
   * handle of current time service
   */ 
  uint16_t currentTimeServiceHandle;
      
  /**
   * handle of current time characteristic
   */ 
  uint16_t currentTimeCharHandle;
      
  /**
   * handle of local time characteristic
   */ 
  uint16_t localTimeInfoCharHandle;
      
  /**
   * handle of reference time characteristic
   */ 
  uint16_t referenceTimeInfoCharHandle;
      
  /**
   * handle of next DST change service
   */ 
  uint16_t nextDSTChangeServiceHandle;
      
  /**
   * handle of DST characteristic
   */ 
  uint16_t timeWithDSTCharHandle;
      
  /**
   * handle of Reference Time Update service
   */ 
  uint16_t referenceUpdateTimeServiceHandle;
      
  /**
   * handle of time update control point characteristic
   */ 
  uint16_t timeUpdateControlPointCharHandle;
      
  /**
   * handle of time update state characteristic
   */ 
  uint16_t timeUpdateStateCharHandle;
      
  /**
   * time update state
   */
  uint8_t timeUpdateState;
      
  /**
   * ID of the timer started for
   * the delay between current time updates
   */
  tTimerID delayTimerID; 
  
  /**
   * ID of the timer started for delay
   * handling
   */ 
  tTimerID timerID; //TBR???
     
  /**
   * this falg is used to send the allow
   * read command
   */ 
  uint8_t timeServerAllowRead;
	
}TimeServerContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* profile context */
extern TimeServerContext_Type TimeServerContext; 

/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref tipsInitDevType)
 * @retval Status of call
 */
tBleStatus TimeServer_Init(tipsInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref tipsDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the TimeServer_DeviceDiscovery_CB()
 */
tBleStatus TimeServer_DeviceDiscovery(tipsDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref tipsSecurityType)
 * @retval Status of call
 */
tBleStatus TimeServer_SecuritySet(tipsSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * TimeServer_SecuritySet()  
 * @retval Status of call
 */
tBleStatus TimeServer_StartPairing(void);

/**
  * @brief Clear Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus TimeServer_Clear_Security_Database();

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback TimeServer_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus TimeServer_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref tipsConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * TimeServer_ConnectionStatus_CB() 
 */
tBleStatus TimeServer_DeviceConnection(tipsConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the TimeServer_ConnectionStatus_CB()
 */
tBleStatus TimeServer_DeviceDisconnection(void);

/**
 * @brief This function shall be called by the master when it receives a
 * connection parameter update request from the peer device. If the master rejects these parameters can send the
 * response with the accepted parameter set to FALSE or ignore the request. This function shall be managed only if the 
 * master has already discovered all the services and characteristics.
 * If theese discovery procedure are ongoing, the master basic profile sends automatically the connection parameter response
 * with status rejected.
 * @param accepted TRUE if the peripheral peer request is accepted, FALSE otherwise.
 * @param param Connection parameter to update
 * @retval Status of the call.
 */
tBleStatus TimeServer_ConnectionParameterUpdateRsp(uint8_t accepted, tipsConnUpdateParamType *param);

/**
 * @brief Add the Phone Alert Status service & related characteristics
 * @param[in]: None
 * @retval Status of the call.
 */ 
tBleStatus TimeServer_Add_Services_Characteristics(void);

/**
 * @brief Run the profile central state machine. Is it needed? TBR???
 * @param  None
 * @retval None
 */
void TimeServer_StateMachine(void);

/**
 * @brief It updates the current time characteristic with the timeValue
 * specified.
 * @param[in] timeValue : the time with which the current time
 *            characteristic has to be updated
 * @retval Function status
 */ 
tBleStatus TimeServer_Update_Current_Time_Value(tCurrentTime timeValue);

/**
 * @brief It updates the localTimeInfo characteristic with the value specified
 * @param[in] localTimeInfo : the local time information with which the local time
 *            information characteristic has to be updated
 * @retval Function status
 */ 
tBleStatus TimeServer_Update_Local_Time_Information(tLocalTimeInformation localTimeInfo);

/**
 * @brief updates the reference time information characteristic with 
 * the value specified.
 * @param[in] refTimeInfo : the new reference time information
 * @retval Function status
 */ 
tBleStatus TimeServer_Update_Reference_Time_Information(tReferenceTimeInformation refTimeInfo);

/**
 * @brief It updates the next DST change characteristic with 
 * the value specified
 * @param[in] timeDST : the new DST information
 * @retval Function status
 */ 
tBleStatus TimeServer_Update_Next_DST_Change(tTimeWithDST timeDST);

/**
 * @brief It updates the reference time
 * @param[in] errCode: the error code with which the update
 *            completed
 * @param[in] newTime : new time to update the current time
 * @retval None
 */ 
void Update_Reference_Time(uint8_t errCode,tCurrentTime* newTime); /* TBR : It seems not used */

/**
 * @brief It sends the allow read command to the controller
 * @param None
 * @retval None
 */ 
tBleStatus TimeServer_Allow_Curtime_Char_Read();//TBR???

/**
 * @brief It handles the update control point request
 * @param None
 * @retval None
 */ 
void TimeServer_Update_Control_Point_Handler(tTimeUpdateControlPoint update_control_point_value);

/***************  Public Function callback ******************/

/**
 * @brief This callback contains all the information of the device discovered
 * during the central proceudure
 * @param status Status of the device discovery procedure
 * @param addr_type Address type of the device discovered
 * @param addr Address of the device discovered
 * @param data_length Data length of the information from the discovered device
 * @param data Data of the discvered device
 * @param RSSI RSSI received during the device discovery procedure
 * @retval None
 */
extern void TimeServer_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                          uint8_t data_length, uint8_t *data, 
                                          uint8_t RSSI);

/**
 * @brief This callback contains all the information of the service discovery procedure
 * @param status Status of the service discovery procedure
 * @param numServices Nubmer of services discovered
 * @param services Data of the services discovered.
 *  * The array will be a sequence of elements with the following format:
 * - 1 byte  UUID Type (1 = 16 bit, 2 = 128 bit)
 * - 2 bytes Service Start Handle
 * - 2 bytes Service End Handle
 * - n bytes (2 or 16) Service UUID
 * @retval None
 */
extern void TimeServer_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

/**
 * @brief This callback contains the status of the connection procedure
 * @param connection_evt Connection event returned during the connection procedure.
 * Valid values are:\n
 * - CONNECTION_ESTABLISHED_EVT\n
 * - CONNECTION_FAILED_EVT\n
 * - DISCONNECTION_EVT\n
 * @param status Status of the event
 * @retval None
 */
extern void TimeServer_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * TimeServer_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref tipsConnUpdateParamType for more details)
 * @retval None
 */
extern void TimeServer_ConnectionParameterUpdateReq_CB(tipsConnUpdateParamType *param);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the TimeServer_SendPinCode() function to set the pin code.
 */
extern void TimeServer_PinCodeRequired_CB(void);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void TimeServer_Pairing_CB(uint16_t conn_handle, uint8_t status);

/**
* @brief  User callback which is called each time a specific event occurs.
*         
* @param  event_value  (EVT_TS_START_REFTIME_UPDATE, EVT_TS_CURTIME_READ_REQ
*                       EVT_TS_STOP_REFTIME_UPDATE, EVT_TS_CHAR_UPDATE_CMPLT)
* @retval None
*/
extern void TimeServer_Notify_State_To_User_Application_CB(uint8_t event_value);


#endif  /* _TIME_SERVER_H_ */
