/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
* File Name          : master_basic_profile.h
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.0
* Date               : 15-October-2014
* Description        : Header for master_basic_profile.c module
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MASTER_BASIC_PROFILE_H
#define __MASTER_BASIC_PROFILE_H

/* Includes ------------------------------------------------------------------*/
#include "sm.h"

/* Defines -------------------------------------------------------------------*/

/********* Device Discovery Procedure *********/

/**
 * @brief Limited discovery procedure used during device discovery. 
 * Define used in Master_DeviceDiscovery()
 */
#define LIMITED_DISCOVERY_PROCEDURE 0x01

/**
 * @brief General discovery procedure used during device discovery.
 * Define used in Master_DeviceDiscovery()
 */
#define GENERAL_DISCOVERY_PROCEDURE 0x02

/**
 * @brief Device found during the device discovery procedure
 * Define used in Master_DeviceDiscovery_CB()
 */
#define DEVICE_DISCOVERED 0x01

/**
 * @brief Device discovery procedure ended from the application with
 * function call  Mater_TerminateDiscovery().
 * Define used in Master_DeviceDiscovery_CB().
 */
#define DEVICE_DISCOVERY_PROCEDURE_ENDED 0x02

/**
 * @brief Device discovery procedure timeout.
 * Define used in Master_DeviceDiscovery_CB()
 */
#define DEVICE_DISCOVERY_PROCEDURE_TIMEOUT 0x03

/******** Connection Procedure *********/
/**
 * @brief Direct connection establishment procedure
 * used during device connection.
 * procedure. Define used in Master_DeviceConnection()
 */
#define DIRECT_CONNECTION_PROCEDURE 0x03

/**
 * @brief Auto connection establishment
 * procedure used during the device connection.
 * Define used in Master_DeviceConnection()
 */
#define AUTO_CONNECTION_PROCEDURE   0x04

/**
 * @brief Connection established event
 * raised to signal the application that the connection
 * procedure is ended with success.
 * Define used in Master_Connection_CB()
 */
#define CONNECTION_ESTABLISHED_EVT 0x01

/**
 * @brief Connection failed event raised to signal
 * the application that the connection procedure
 * is failed. 
 * Define used in Master_Connection_CB()
 */
#define CONNECTION_FAILED_EVT      0x02

/**
 * @brief Disconnection event.
 * Define used in Master_Connection_CB()
 */
#define DISCONNECTION_EVT          0x03

/**
 * @brief Stop connection procedure event is raised when
 * there is an error during the switch between fast 
 * scan connection procedure and redeuce power connection procedure.
 * Define used in Master_Connection_CB()
 */
#define STOP_CONNECTION_PROCEDURE_EVT 0x04

/**
 * @brief Connection update parameters event raised
 * when the master basic profile receives the l2cap
 * connection parameter request from the peer device.
 * Define used in Master_Connection_CB()
 */
#define CONNECTION_UPDATE_PARAMETER_REQUEST_EVT 0x05

/********* Services & Characteristics Management *********/

/**
 * @brief Primary Service Discovery procedure ended event.
 * Define used in Master_ServiceCharacPeerDiscovery_CB()
 */
#define PRIMARY_SERVICE_DISCOVERY 0x01

/**
 * @brief Get Characteristics of a Service 
 * procdure ended event. 
 * Define used in Master_ServiceCharacPeerDiscovery_CB()
 */
#define GET_CHARACTERISTICS_OF_A_SERVICE 0x02

/**
 * @brief Find all included services procedure ended event. 
 * Define used in Master_ServiceCharacPeerDiscovery_CB()
 */
#define FIND_INCLUDED_SERVICES 0x03

/**
 * @brief Find all descriptors for a given characteristic
 * procedure ended event. 
 * Define used in Master_ServiceCharacPeerDiscovery_CB()
 */
#define FIND_CHARAC_DESCRIPTORS 0x04

/********* Data Exchange Procedure *********/
/**
 * @brief This event signals the status of the
 * Notification and Indication change characteristic 
 * configuration descriptor procedure executed from the
 * application.
 * Define used in Master_PeerDataExchange_CB() 
 */ 
#define NOTIFICATION_INDICATION_CHANGE_STATUS 0x05

/**
 * @brief This event signals the status 
 * of the Read Characteristic value procedure
 * executed from the application.
 * Define used in Master_PeerDataExchange_CB()
 */
#define READ_VALUE_STATUS 0x06

/**
 * @brief This event signals the status
 * of the Write Characteristic value procedure
 * executed from the application.
 * Define used in Master_PeerDataExchange_CB()
 */
#define WRITE_VALUE_STATUS 0x07

/**
 * @brief This event signals the status of
 * Read Long Characteristic value procedure 
 * executed from the application. 
 * Define used in Master_PeerDataExchange_CB()
 */
#define READ_LONG_VALUE_STATUS 0x08

/**
 * @brief This event signals the Notification data received
 * form the peer device. 
 * Define used in Master_PeerDataExchange_CB()
 */
#define NOTIFICATION_DATA_RECEIVED 0x09

/**
 * @brief This event signals the Indication data received
 * from the peer device.
 * Define used in Master_PeerDataExchange_CB()
 */
#define INDICATION_DATA_RECEIVED 0x0A

/**
 * @brief This event signals the Attribute modification
 * from the peer device.
 * Define used in Master_PeerDataExchange_CB()
 */
#define ATTRIBUTE_MODIFICATION_RECEIVED 0x0B

/**
 * @brief This event is given to the application when a write request, write command or signed write command
 * is received by the server from the client. This event will be given to the application only if the
 * event bit for this event generation is set when the characteristic was added.
 * When this event is received, the application has to check whether the value being requested for write
 * is allowed to be written and respond with the command Master_WriteResponse().
 * Define used in Master_PeerDataPermitExchange_CB()
 */
#define WRITE_PERMIT_REQUEST 0x0C

/**
 * @brief This event is given to the application when a read request or read blob request is received by the server
 * from the client. This event will be given to the application only if the event bit for this event generation
 * is set when the characteristic was added.
 * On receiving this event, the application can update the value of the handle if it desires and when done
 * it has to use the Master_AllowRead() command to indicate to the stack that it can send the response to the client.
 * Define used in Master_PeerDataPermitExchange_CB()
 */
#define READ_PERMIT_REQUEST 0x0D

/********* Master Basic Profile Error Code *********/

/**
 * @brief The insufficient authentication error code
 * is received when the master wants read/write
 * a peer device characteristic with authentication security enabled,
 * and the pairing/bonding procedure is still not started.
 * Define used in Master_Pairing_CB()
 */
#define ERROR_INSUFFICIENT_AUTHENTICATION 0x05

/**
 * @brief The insufficient authorization error code
 * is received when the master wants read/write
 * a characteristic with authorization security enabled,
 * and the pairing/bonding procedure is still not started
 * Define used in Master_Pairing_CB()
 */
#define ERROR_INSUFFICIENT_AUTHORIZATION  0x08

/**
 * @brief The insufficient encryption error code
 * is received when the master wnats read/write
 * a characteristic with encryption security enabled,
 * and the pairing/bonding procedure is still not started.
 * Define used in Master_Pairing_CB()
 */
#define ERROR_INSUFFICIENT_ENCRYPTION     0x0F

/********* Security *********/

/**
 * @brief Pass key request event code is
 * received when the pairing/bonding proceudre
 * is started with the MITM option enabled.
 * Define used in Master_Pairing_CB()
 */
#define PASS_KEY_REQUEST 0x10

#define IDB04A1 0
#define IDB05A1 1

/* Typedef Definition --------------------------------------------------------*/

/**
 * notification callback to the application
 */ 
typedef void (* BLE_HCI_EVENT_CB)(void *pckt);



/**
 * @brief Master security parameter settings
 */
typedef struct securitySetS {
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
} securitySetType;

/**
 * @brief Device discovery parameters
 */
typedef struct devDiscoveryS {
  /** Device discovery procedure used:
   *  - LIMITED_DISCOVERY_PROCEDURE
   *  - GENERAL_DISCOVERY_PROCEDURE
   */
  uint8_t procedure;
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
  /** Type of our address used during advertising (PUBLIC_ADDR, RANDOM_ADDR). */
  uint8_t own_address_type;
} devDiscoveryType;

/**
 * @brief Connection Device parameters
 */
typedef struct connDevS {
  /** Connection procedure to use:\n
   *  - DIRECT_CONNECTION_PROCEDURE\n
   *  - AUTO_CONNECTION_PROCEDURE
   */
  uint8_t procedure;
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
  uint8_t peer_addr[6];
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
} connDevType;

/**
 * @brief Connection Parameters received from peer peripheral device 
 * during an l2cap connection parameter update request.
 */
typedef struct connUpdateParamS {
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
  /** Minimum length of connection needed for the LE connection.\n
   *  Range: 0x0000 - 0xFFFF\n
   *  Time = N x 0.625 msec.
   *  This parameter is used only for BlueNRG_MS
   */
  uint16_t min_conn_length;
  /** Maximum length of connection needed for the LE connection.\n
   *  Range: 0x0000 - 0xFFFF\n
   *  Time = N x 0.625 msec.
   *  This parameter is used only for BlueNRG_MS
   */
  uint16_t max_conn_length;
} connUpdateParamType;

/**
 * @brief Data received from the peer peripheral device
 * during notification, indication and write request.
 */
typedef struct dataReceivedS {
  /** Attribute Value handle */
  uint16_t attr_handle;
  /** Data received lentgh */
  uint8_t  data_length;
  /** Data received */
  uint8_t  *data_value;
} dataReceivedType;

/**
 * @brief Data received from the peer peripheral device when a write or a read request
 * is received and the application has set the permit request event in the characteristic.
 */
typedef struct dataPermitExchangeS {
  /** The handle of the attribute for which the read request has been made by the client */
  uint16_t attr_handle; 
  /** Length of data field for write permit procedure. In case of read permit procedure is always 1 */
  uint8_t  data_length; 
  /** The data that the client has requested to write. It is not used in case of read permit procedure */  
  uint8_t  *data; 
  /** Contains the offset from which the read permit procedure has been requested. It is not used in case of write permit procedure */
  uint8_t  offset;
} dataPermitExchangeType;

/**
 * @brief Master initialization parameters
 */
typedef struct initDevS {
  /** Public address */
  uint8_t public_addr[6];
  /** Length of the device name. If the length is equal to zero the device name is ignored.
   *  For the BlueNRG-MS the device name length can be grater than 8. */
  uint8_t device_name_len;
  /** Device name to configure. */
  uint8_t *device_name;
  /** Transmit power level
   * Possible value are:
   * 0 = -14 dBm, 
   * 1 = -11 dBm, 
   * 2 = -8 dBm, 
   * 3 = -5 dBm, 
   * 4 = -2 dBm, 
   * 5 =  2 dBm, 
   * 6 =  4 dBm, 
   * 7 =  8 dBm
   */
  uint8_t txPowerLevel;
  /** Application event callback */
  void(*BLE_HCI_Event_CB)(void *pckt);
  
  void (*Master_DeviceDiscovery_CB)(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				    uint8_t *data_length, uint8_t *data, 
				    uint8_t *RSSI);
  void (*Master_Connection_CB)(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);
  
  void (*Master_Pairing_CB)(uint16_t *conn_handle, uint8_t *status);
  
  void (*Master_PeerDataExchange_CB)(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);
  
  void (*Master_PeerDataPermitExchange_CB)(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);
  
  void (*Master_ServiceCharacPeerDiscovery_CB)(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);
  
} initDevType;

/* Function Prototypes -------------------------------------------------------*/

/**
 * @name Master Configuration Functions
 * @{
 */

/**
  * @brief  Initializes the Device in Master mode, 
  * sets the public address and the TX otuput power.
  * @param param Master initialization parameters
  * (see @ref initDevType structure)
  * @retval Status of the call.
  */
uint8_t Master_Init(initDevType *param);

//@} \\END Master Configuration Functions

/**
 * @name Master Device Discovery Functions
 *@{
 */

/**
  * @brief  Starts the device discovery procedure. The procedure is terminated 
  * when either the upper layers issue a command to terminate the procedure 
  * (see Master_TerminateDiscovery() function call) or a 
  * timeout happens (timeout value is 10.24 sec).
  * @param param Device discovery parameters (see @ref devDiscoveryType structure)
  * @retval Status of the call.
  */
uint8_t Master_DeviceDiscovery(devDiscoveryType *param);

/**
  * @brief  Forces the end of the GAP device discovery procedure.
  * @retval Status of the call.
  */
uint8_t Master_TerminateDiscovery(void);

/**
  * @brief  This callback returns the remote device's information discovered during the scan.
  * @param status of the device discovery callback. Valid values are:
  * - DEVICE_DISCOVERED
  * - DEVICE_DISCOVERY_PROCEDURE_ENDED
  * - DEVICE_DISCOVERY_PROCEDURE_TIMEOUT
  * @param  addr_type Address Type, possible values are:
  * - 0 = Public Device Address
  * - 1 = Random Device Address
  * @param  addr Remote device address
  * @param  data_length Lenght of advertising or scan response data.
  * @param  data Advertising or scan response data 
  * @param  RSSI RSSI (signed integer).
  * @retval None
  */

extern void Master_ANC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				      uint8_t *data_length, uint8_t *data, 
				      uint8_t *RSSI);
extern void Master_HRC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				      uint8_t *data_length, uint8_t *data, 
				      uint8_t *RSSI);
extern void Master_TC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				      uint8_t *data_length, uint8_t *data, 
				      uint8_t *RSSI);
extern void Master_FML_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				      uint8_t *data_length, uint8_t *data, 
				      uint8_t *RSSI);
extern void Master_BPC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				      uint8_t *data_length, uint8_t *data, 
				      uint8_t *RSSI);
extern void Master_HTC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				      uint8_t *data_length, uint8_t *data, 
				      uint8_t *RSSI);
extern void Master_GC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
				      uint8_t *data_length, uint8_t *data, 
				      uint8_t *RSSI);


//@} \\END Master Device Discovery Functions

/**
 * @name Master Device Connection Functions
 *@{
 */

/**
  * @brief Starts the connection procedure.   
  * @param param Connection Device parameters (see @ref connDevType structure)
  * @retval Status of the call.
  */
uint8_t Master_DeviceConnection(connDevType *param);

/**
  * @brief  This callback returns the connection procedure status
  * @param  connection_evt Connection event. Possible values are:
  * - @ref CONNECTION_ESTABLISHED_EVT
  * - @ref CONNECTION_FAILED_EVT
  * - @ref DISCONNECTION_EVT
  * - @ref STOP_CONNECTION_PROCEDURE_EVT
  * - @ref CONNECTION_UPDATE_PARAMETER_REQUEST_EVT
  * @param  status Connection status. 
  * @param  connection_handle Connection handle
  * @param  param Connection parameters suggested from the slave, used 
  * ONLY when the CONNECTION_UPDATE_PARAMETER_REQUEST_EVT is raised.
  * @retval None
  */
extern void Master_ANC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);
extern void Master_HRC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);
extern void Master_TC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);
extern void Master_FML_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);
extern void Master_BPC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);
extern void Master_HTC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);
extern void Master_GC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
				 uint16_t *connection_handle, connUpdateParamType *param);

/**
 * @brief Terminates the GAP connection procedure ongoing.
 * @retval Status of the call
 * @note This function call ends only the GAP connection procedure
 * ongoing, at this step the central is not still connected.
 * If the central is connected, to close the connection call Master_CloseConnection()
 */
uint8_t Master_TerminateGAPConnectionProcedure(void);

/**
 * @brief Close the connection with the peer device connected.
 * @param handle Connection handle to close
 * @retval Status of the call
 */
uint8_t Master_CloseConnection(uint16_t handle);

/**
 * @brief This function shall be called by the master when it receives a
 * connection parameter update request from the peer device. If the master rejects these parameters can send the
 * response with the accepted parameter set to FALSE or ignore the request. This function shall be managed only if the 
 * master has already discovered all the services and characteristics.
 * If theese discovery procedure are ongoing, the master basic profile sends automatically the connection parameter response
 * with status rejected.
 * @param conn_handle Connection handle
 * @param accepted TRUE if the peripheral peer request is accepted, FALSE otherwise.
 * @param param Connection parameter to update
 * @retval Status of the call.
 */
uint8_t Master_ConnectionUpdateParamResponse(uint16_t conn_handle, uint8_t accepted, connUpdateParamType *param);

//@} \\END Master Device Connection Functions

/**
 * @name Master Discovery Services & Characteristics Functions
 *@{
 */

/**
 * @brief Gets all primary service from the peer device.
 * The primary service allocated on the connected peer device are availables 
 * when the Master_ServiceCharacPeerDiscovery_CB() is raised with the procedure value euqal to @ref PRIMARY_SERVICE_DISCOVERY
 * @param conn_handle Connection handle
 * @param numPrimaryService Number of services allocated on the connected peer device.
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param primaryService Array with the primary service information. 
 * The array will be a sequence of elements with the following format:
 * - 1 byte  UUID Type (1 = 16 bit, 2 = 128 bit)
 * - 2 bytes Service Start Handle
 * - 2 bytes Service End Handle
 * - n bytes (2 or 16) Service UUID
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param size Max size of the primaryService array. If the information read from the peer device exceeds this size
 * a BLE_STATUS_INSUFFICIENT_RESOURCES error is returned.
 * @retval Status of call
 */
uint8_t Master_GetPrimaryServices(uint16_t conn_handle, uint8_t *numPrimaryService, uint8_t *primaryService, uint16_t size);

/**
 * @brief Gets all characteristics of a service from the peer device.
 * The characteristics of a service allocated on the connected peer device are available 
 * when the Master_ServiceCharacPeerDiscovery_CB() is raised with the procedure value euqal to @ref GET_CHARACTERISTICS_OF_A_SERVICE 
 * @param conn_handle Connection handle
 * @param start_service_handle Start Handle of a given service
 * @param end_service_handle End Handle of a given service
 * @param numCharac Number of characteristics allocated for a given service
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param charac Array with the characterisitcs information. 
 * The array will be a sequence of elements with the following format:
 * - 1 byte  UUID Type (1 = 16 bit, 2 = 128 bit)
 * - 2 bytes Characteristic Handle
 * - 1 bytes Characteristic Properties
 * - 2 bytes Characteristic Value Handle
 * - n bytes (2 or 16) Characteristics UUID
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param size Max size of the charac array. If the information read from the peer device exceeds this size
 * a BLE_STATUS_INSUFFICIENT_RESOURCES error is returned
 * @retval Status of call
 */
uint8_t Master_GetCharacOfService(uint16_t conn_handle, uint16_t start_service_handle, uint16_t end_service_handle,
				  uint8_t *numCharac, uint8_t *charac, uint16_t size);

/**
 * @brief Starts the procedure to find all the included sevices from the peer device.
 * The included services are available when the Master_ServiceCharacPeerDiscovery_CB() is raised
 * with the procedure value euqal to @ref FIND_INCLUDED_SERVICES
 * @param conn_handle Connection handle
 * @param start_service_handle Start Handle of a given service
 * @param end_service_handle End Handle of a given service
 * @param numIncludedServices Number of included services found in a given service
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param includedServices Array with the included services information. 
 * The array will be a sequence of elements with the following format:
 * - 1 byte size of the information (6 or 8)
 * - 2 bytes Characteristic Handle
 * - 2 bytes Start Included Handle
 * - 2 bytes End Included Handle
 * - 2 bytes UUID only if it is a 16-bit Bluetooth UUID 
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param size Max size of the included services array. If the information read from the peer device exceeds this size
 * a BLE_STATUS_INSUFFICIENT_RESOURCES error is returned
 * @retval Status of call
 */
uint8_t Master_GetIncludeServices(uint16_t conn_handle, uint16_t start_service_handle, uint16_t end_service_handle,
				  uint8_t *numIncludedServices, uint8_t *includedServices, uint16_t size);

/**
 * @brief Starts the procedure to find all the characteristic descirptors from the peer device.
 * The characteristic descriptors are available when the Master_ServiceCharacPeerDiscovery_CB() is raised
 * with the procedure value euqal to @ref FIND_CHARAC_DESCRIPTORS
 * @param conn_handle Connection handle
 * @param start_value_charac_handle Value Handle of a given characteristic
 * @param end_charac_handle End Handle of a given characteristic
 * @param numCharacDesc Number of descriptors found in a given characterisric
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param characDesc Array with the descriptors associated to a given characteritstic. 
 * The array will be a sequence of elements with the following format:
 * - 1 byte Format handle data (1 = lists with 16 bit UUID, 2 = lists with 128 bit UUID)
 * - 2 bytes Characteristic descriptor Handle
 * - n bytes (2 or 16) UUID according the format field
 * When the Master_ServiceCharacPeerDiscovery_CB() will be received, the content of this parameter will be available.
 * @param size Max size of the charac descriptor array. If the information read from the peer device exceeds this size
 * a BLE_STATUS_INSUFFICIENT_RESOURCES error is returned
 * @retval Status of call
 */
uint8_t Master_GetCharacDescriptors(uint16_t conn_handle, uint16_t start_value_charac_handle, uint16_t end_charac_handle,
				    uint8_t *numCharacDesc, uint8_t *characDesc, uint16_t size);

/**
 * @brief This callback will be called at the end of the following procedure:
 * - @ref PRIMARY_SERVICE_DISCOVERY
 * - @ref GET_CHARACTERISTICS_OF_A_SERVICE
 * - @ref FIND_INCLUDED_SERVICES
 * - @ref FIND_CHARAC_DESCRIPTORS
 * @param procedure Master procedure ended
 * @param status Status of the procedure
 * @param connection_handle Connection handle
 * @note All the information about the primary service, characteristics, included services.... will be 
 * returned on the variables used to start the procedure.
 */
extern void Master_ANC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);
extern void Master_HRC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);
extern void Master_TC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);
extern void Master_FML_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);
extern void Master_BPC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);
extern void Master_HTC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);
extern void Master_GC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle);

//@} \\END Master Discovery Services & Characteristics Functions

/**
 * @name Master Data Exchange Functions
 *@{
 */

/**
 * @brief This function sets the notification and indication status on the peer BLE device.
 * @param conn_handle Connction handle
 * @param config_desc_handle Configuration descriptor handle
 * @param notification_enable TRUE if the client enables the server notification. FALSE otherwise
 * @param indication_enable TRUE if the client enables the server indication. FALSE otherwise
 * @note The Master_PeerDataExchange_CB() function is called with the status of the  Master_NotifIndic_Status() commands.
 * The procedure parameter in the Master_PeerDataExchange_CB() will be set to @ref NOTIFICATION_INDICATION_CHANGE_STATUS 
 * @retval Status of call
 */
uint8_t Master_NotifIndic_Status(uint16_t conn_handle, uint16_t config_desc_handle, 
				 uint8_t notification_enable, uint8_t indication_enable);

/**
 * @brief This function starts a read procedure on the peer BLE device.
 * @param conn_handle Connction handle
 * @param attr_value_handle Attribute value handle of the characteristic to read
 * @param data_length Length of data received
 * When the Master_PeerDataExchange_CB() will be received, the content of this parameter will be available.
 * @param data Data read from the peer device
 * When the Master_PeerDataExchange_CB() will be received, the content of this parameter will be available.
 * @param size  Max size of the data array. If the information read from the peer device exceeds this size
 * a BLE_STATUS_INSUFFICIENT_RESOURCES error is returned
 * @note The Master_PeerDataExchange_CB() function is called with the status of the read commands.
 * The procedure parameter in the Master_PeerDataExchange_CB() will be set to @ref READ_VALUE_STATUS 
 * @retval Status of call
 */
uint8_t Master_Read_Value(uint16_t conn_handle, uint16_t attr_value_handle, 
			  uint16_t *data_length, uint8_t *data, uint16_t size); 


/**
 * @brief This function starts a read long procedure on the peer BLE device.
 * @param conn_handle Connction handle
 * @param attr_value_handle Attribute value handle of the characteristic to read
 * @param data_length Length of data received
 * When the Master_PeerDataExchange_CB() will be received, the content of this parameter will be available.
 * @param data Data read from the peer device
 * When the Master_PeerDataExchange_CB() will be received, the content of this parameter will be available.
 * @param offset Offset from which the value needs to be read
 * @param size  Max size of the data array. If the information read from the peer device exceeds this size
 * a BLE_STATUS_INSUFFICIENT_RESOURCES error is returned
 * @note The Master_PeerDataExchange_CB() function is called with the status of the read commands.
 * The procedure parameter in the Master_PeerDataExchange_CB() will be set to @ref READ_LONG_VALUE_STATUS 
 * @retval Status of call
 */
uint8_t Master_Read_Long_Value(uint16_t conn_handle, uint16_t attr_value_handle, 
			       uint16_t *data_length, uint8_t *data, 
			       uint8_t offset, uint16_t size);

/**
 * @brief This function starts a write procedure on the peer BLE device.
 * @param conn_handle Connction handle
 * @param attr_value_handle Attribute handle of the characteristic value to write
 * @param value_len Length of the data to write
 * @param attr_value Data to write
 * @note The Master_PeerDataExchange_CB() function is called with the status of the write procedure.
 * The procedure parameter in the Master_PeerDataExchange_CB() will be set to @ref WRITE_VALUE_STATUS
 * @retval Status of call
 */
uint8_t Master_Write_Value(uint16_t conn_handle, uint16_t attr_value_handle, 
			   uint8_t value_len, uint8_t *attr_value);

/**
 * @brief This function executes a write without response on the peer BLE device.
 * @param conn_handle Connction handle
 * @param attr_value_handle Attribute handle of the characteristic value to write
 * @param value_len Length of the data to write
 * @param attr_value Data to write
 * @note This function doesn't have a response from the peer device, so the  
 * Master_PeerDataExchange_CB() function is not called
 * @retval Status of call
 */
uint8_t Master_WriteWithoutResponse_Value(uint16_t conn_handle,  uint16_t attr_value_handle,
					  uint8_t value_len, uint8_t* attr_value);

/**
 * @brief This function allows or rejects a write reqeust from the peer device.
 * @param conn_handle Conenction handle
 * @param attr_value_handle Handle of the attribute to write
 * @param write_status 0x00: The value can be written to the attribute specified by attr_handle\n
 * 		       0x01: The value cannot be written to the attribute specified by the attr_handle.
 * @param err_code  The error code that has to be passed to the peer device in case the write has to be rejected.
 * @param attr_value_len Length of the value to be written
 * @param attr_value Value to be written
 * @retval Status of call
 * @note This function must be called inside the Master_PeerDataPermitExchange_CB() callback
 */
uint8_t Master_WriteResponse(uint16_t conn_handle, uint16_t attr_value_handle, uint8_t write_status, uint8_t err_code,
			     uint8_t attr_value_len, uint8_t *attr_value);

/**
 * @brief This function allows the master to send a response to a read reqeust from the peer device.
 * @param conn_handle Conenction handle
 * @retval Status of call
 * @note This function must be called inside the Master_PeerDataPermitExchange_CB() callback
 */
uint8_t Master_AllowRead(uint16_t conn_handle);

/**
 * @brief This callback will be called at the end of the following procedure:
 * - @ref NOTIFICATION_INDICATION_CHANGE_STATUS
 * - @ref READ_VALUE_STATUS
 * - @ref READ_LONG_VALUE_STATUS
 * - @ref WRITE_VALUE_STATUS
 * - @ref NOTIFICATION_DATA_RECEIVED
 * - @ref INDICATION_DATA_RECEIVED
 * - @ref ATTRIBUTE_MODIFICATION_RECEIVED
 * @param procedure Master procedure ended.
 * @param status Status of the procedure
 * @param connection_handle Connection handle
 * @param data Data received type. This param is used ONLY when the master receives notifications, indications
 * and attribute modifications.
 * @note For the READ_VALUE_STATUS and READ_LONG_VALUE_STATUS procedures, the characteristic value
 * will be returned on the variables used during the procedure call.
 */
extern void Master_ANC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);
extern void Master_HRC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);
extern void Master_FML_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);
extern void Master_TC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);
extern void Master_BPC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);
extern void Master_HTC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);
extern void Master_GC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				       uint16_t *connection_handle, dataReceivedType *data);

/**
 * @brief  This callback will be called when a read or write operation procedure is required from the peer device
 * and the application has set the permit request event in the characteristic.
 * @param procedure Master procedure to serve:\n
 * - @ref READ_PERMIT_REQUEST\n
 * - @ref WRITE_PERMIT_REQUEST
 * @param connection_handle Connection handle
 * @param data Characteristic information to allow the read or write operation. (see @ref dataPermitExchangeType)
 * and attribute modifications.
 */
extern void Master_ANC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);
extern void Master_HRC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);
extern void Master_TC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);
extern void Master_FML_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);
extern void Master_BPC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);
extern void Master_HTC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);
extern void Master_GC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data);

//@} \\END Master Data Exchange Functions

/**
 * @name Master Security Functions
 *@{
 */

/**
 * @brief Sets the security parameter on the master device
 * @param param Master security settings (see @ref securitySetType strucure)
 * @retval Status of call.
 */
uint8_t Master_SecuritySet(securitySetType *param);

/**
 * @brief This function starts the pairing and bonding procedure with the peer BLE device.
 * @param conn_handle Connction handle
 * @param force_rebond TRUE if the master device wants force the rebond.
 * @note The Master_Pairing_CB() function is called with the status of the pairing procedure.
 * @retval Status of call
 */
uint8_t Master_Start_Pairing_Procedure(uint16_t conn_handle, uint8_t force_rebond);

/**
 * @brief This command should be sent by the Host in response to @ref PASS_KEY_REQUEST received from the
 * Master_Pairing_CB(). The command parameter contains the pass key which will be used during 
 * the pairing process
 * @param conn_handle Connection handle
 * @param key Pass key, value range: 0 – 999999 (decimal)
 * @retval Status of call
*/
uint8_t Master_Passkey_Response(uint16_t conn_handle, uint32_t key);

/**
 * @brief Clear the security database.
 * @note  All the devices in the security database will be removed.
 * @retval Status of call
 */
uint8_t Master_ClearSecurityDatabase(void);

/**
  * @brief  This callback returns the pairing procedure status.
  * @param  conn_handle Connection handle
  * @param  status Status of the pairing request. Possible values are:
  * - @ref ERROR_INSUFFICIENT_AUTHENTICATION
  * - @ref ERROR_INSUFFICIENT_AUTHORIZATION
  * - @ref ERROR_INSUFFICIENT_ENCRYPTION
  * - BLE_STATUS_SUCCESS
  * - @ref PASS_KEY_REQUEST
  * @retval None
  * @note When this callback returns an ERROR value, means that the master is not paired
  * and bonded to the peer device and the application tries to executes an operation that requires pairing and bonding.
  * When a procedure raises an ERROR status, the application needs to start the pairing procedure
  * and at the end needs to repeat the procedure that has generated the error.
  * In case of @ref PASS_KEY_REQUEST status you need to call inside the Master_Pairing_CB() the function
  * Master_PassKey_Response()
  */
extern void Master_ANC_Pairing_CB(uint16_t *conn_handle, uint8_t *status);
extern void Master_HRC_Pairing_CB(uint16_t *conn_handle, uint8_t *status);
extern void Master_TC_Pairing_CB(uint16_t *conn_handle, uint8_t *status);
extern void Master_FML_Pairing_CB(uint16_t *conn_handle, uint8_t *status);
extern void Master_BPC_Pairing_CB(uint16_t *conn_handle, uint8_t *status);
extern void Master_HTC_Pairing_CB(uint16_t *conn_handle, uint8_t *status);
extern void Master_GC_Pairing_CB(uint16_t *conn_handle, uint8_t *status);

//@} \\END Master Security  Functions

/**
 * @name Master Common Services Functions
 *@{
 */

/**
  * @brief  This function needs to be called inside a main loop to run the Master state machines
  * @retval The return value is a bitmask to signal the application if the Host micro can enter in power save or not.
  * The bitmask format is:
  * - 1 bit flag to signal if the micro can enter in power save or not. This is the LSB.
  * - 1 bit flag to signal if the micro needs to enable the power save with a timeout.
  * - 30 bits power save timeout, the latest MSB. These 30 bits are valid if the second bit is set. At the end of the timeout, the application
  *   shall call the function Master_HostExitTimeoutPowerSave() to signal the master basic profile that the host micro has applied the power save with 
  *   timeout as suggested from the basic profile.
  */
uint32_t Master_Process(initDevType *param);

/**
 * @brief Sets the TX output power level used during the communication
 * Default value is -2.1 dBm
 * @param level Power Level, valid range is [0 - 7]. The following table determines 
 * the output power level (dBm):
 * - 0 = -15
 * - 1 = -11.7
 * - 2 = -8.4
 * - 3 = -5.1
 * - 4 = -2.1
 * - 5 = 1.4
 * - 6 = 4.7
 * - 7 = 8.0
 * @retval Status of the call
 */
uint8_t Master_SetTxPower(uint8_t level);

/**
 * @brief This function signals at the master that the host micro exits from a power save
 * with timeout as advised from the master basic profile.
 * @retval Status of the call
 */
uint8_t Master_HostExitTimeoutPowerSave(void);

//@} \\END Master Common Services Functions

#endif /* __MASTER_BASIC_PROFILE_H */
