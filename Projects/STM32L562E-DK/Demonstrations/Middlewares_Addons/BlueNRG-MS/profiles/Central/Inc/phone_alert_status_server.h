/******************************************************************************
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
*   FILENAME        -  phone_alert_status_server.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/16/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    phone alert server header file
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the phone alert server profile role
* 
******************************************************************************/


/******************************************************************************
 * Include Files
******************************************************************************/

#ifndef _PHONE_ALERT_STATUS_SERVER_H_
#define _PHONE_ALERT_STATUS_SERVER_H_

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

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Phone Alert Status Server States */
#define PHONE_ALERT_STATUS_SERVER_UNINITIALIZED           0x0000
#define PHONE_ALERT_STATUS_SERVER_CONNECTED_IDLE          0x0001
#define PHONE_ALERT_STATUS_SERVER_INITIALIZED             0x0002
#define PHONE_ALERT_STATUS_SERVER_DEVICE_DISCOVERY        0x0003
#define PHONE_ALERT_STATUS_SERVER_START_CONNECTION        0x0004
#define PHONE_ALERT_STATUS_SERVER_CONNECTED               0x0005
#define PHONE_ALERT_STATUS_SERVER_CONN_PARAM_UPDATE_REQ   0x0006

/******************************************************************************
* Type definitions
******************************************************************************/
/* Phone Alert Status Server Central Information */
typedef struct passInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
  /** Alert status init value*/
  uint8_t alert_status_init_value;
  /** Ringer setting init value */
  uint8_t ringer_setting_init_value;
} passInitDevType;

typedef struct passSecurityS {
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
} passSecurityType;

typedef struct passConnDevS {
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
} passConnDevType;

typedef struct passDevDiscS {
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
} passDevDiscType;

typedef struct passConnUpdateParamS {
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
} passConnUpdateParamType;

/* Phone Alert Status Server Context variable */
typedef struct PhoneAlertStatusServerContextS {
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  
  uint8_t alert_status_value; 
  uint8_t ringer_setting_value;
  BOOL mute_once_silence; 
  
  /**
   * the phone alert status service handle
   */ 
  uint16_t PASServiceHandle;
	
  /**
   * Alert status characteristic
   * handle
   */ 
  uint16_t AlertStatusCharHandle;
	
  /**
   * Ringer setting
   * characteristic handle
   */ 
  uint16_t RingerSettingCharHandle;

  /**
   * Ringer control point characteristic
   * handle
   */ 
  uint16_t RingerControlPointCharHandle;
  
  //uint8_t fullConf;

} PhoneAlertStatusServerContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* phone alert status profile context */
extern PhoneAlertStatusServerContext_Type phone_alert_status_server_context; 

/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref passInitDevType)
 * @retval Status of call
 */
tBleStatus PASS_Init(passInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref passDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the PASS_DeviceDiscovery_CB()
 */
tBleStatus PASS_DeviceDiscovery(passDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref passSecurityType)
 * @retval Status of call
 */
tBleStatus PASS_SecuritySet(passSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * PASS_SecuritySet()  
 * @retval Status of call
 */
tBleStatus PASS_StartPairing(void);

/**
  * @brief Clear Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus PASS_Clear_Security_Database();

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback PASS_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus PASS_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref passConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * PASS_ConnectionStatus_CB() 
 */
tBleStatus PASS_DeviceConnection(passConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the PASS_ConnectionStatus_CB()
 */
tBleStatus PASS_DeviceDisconnection(void);

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
tBleStatus PASS_ConnectionParameterUpdateRsp(uint8_t accepted, passConnUpdateParamType *param);

/**
 * @brief Add the Phone Alert Status service & related characteristics
 * @param[in]: None
 * @retval Status of the call.
 */ 
tBleStatus PASS_Add_Services_Characteristics(void);

/**
 * @brief Run the profile central state machine. Is it needed? TBR???
 * @param  None
 * @retval None
 */
void PASS_StateMachine(void);

/**
  * @brief  Alert Control Point characteristic handler
  * @param  value: alert control point value
  * @retval tBleStatus: function status
  */
void PASS_Alert_Control_Point_Handler(uint8_t alert_control_point_value);

/**
  * @brief  Set the alert status characteristic value 
  * @param  value: alert status value
  * @retval tBleStatus: function status
  */
tBleStatus PASS_Set_AlertStatus_Value(uint8_t value);

/**
  * @brief  Set the ringer setting characteristic value 
  * @param  value: ringer setting value
  * @retval tBleStatus: function status
  */
tBleStatus PASS_Set_RingerSetting_Value(uint8_t value);

/**
* @brief  It returns the mute_once_silence status
* @param  None
* @retval BOOL: TRUE, FALSE
*/
BOOL PASS_Get_Mute_Once_Silence_Ringer_Status(void);

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
extern void PASS_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
extern void PASS_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void PASS_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * PASS_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref passConnUpdateParamType for more details)
 * @retval None
 */
extern void PASS_ConnectionParameterUpdateReq_CB(passConnUpdateParamType *param);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the PASS_SendPinCode() function to set the pin code.
 */
extern void PASS_PinCodeRequired_CB(void);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void PASS_Pairing_CB(uint16_t conn_handle, uint8_t status);

/**
* @brief  User defined callback which is called each time a 
*         Alert Control Point arrives: user specific implementation 
*         action should be done accordingly.
* @param  alert_control_point_value: received alert level control point 
* @retval None
*/
extern void PASS_Ringer_State_CB(uint8_t alert_control_point_value);

#endif /* _PHONE_ALERT_STATUS_SERVER_H_ */
