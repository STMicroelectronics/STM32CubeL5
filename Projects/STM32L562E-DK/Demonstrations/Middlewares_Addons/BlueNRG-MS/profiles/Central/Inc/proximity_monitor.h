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
*   FILENAME        -  proximity_monitor.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/01/2015
*   $Revision$:  first version
*   $Author$:    
*   Comments:    proximity monitor
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the proximity monitor application profioe central role
* 
******************************************************************************/

#ifndef _PROXIMITY_MONITOR_H_
#define _PROXIMITY_MONITOR_H_

/* Alert Level Characteristic Values */

/**
 * @brief No Alert definition for the characteristic
 */
#define NO_ALERT   0

/**
 * @brief Mild Alert definition for the characteristic
 */
#define MILD_ALERT 1

/**
 * @brief High Alert definition for the characteristic
 */
#define HIGH_ALERT 2

/* Proximity Monitor Error Condition */

/**
 * @brief Error during the service discovery procedure
 */
#define PXM_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define PXM_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define PXM_ERROR_IN_CHARAC_DISCOVERY        0x03

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define PXM_ERROR_IN_CHARAC_DESC_DISCOVERY   0x04

/**
 * @brief Error during the Link Loss Alert Level configuration procedure
 */
#define PXM_ERROR_IN_CONFIG_LL_ALERT_LEVEL   0x05

/**
 * @brief Error during the Write Procedure
 */
#define PXM_ERROR_IN_WRITE_PROCEDURE         0x06

/**
 * @brief Error during the enable of the Tx Power notification procedure
 */
#define PXM_ERROR_IN_ENABLE_TP_NOTIFICATION  0x07

/**
 * @brief Error during the Immediate Alert level configuration procedure
 */
#define	PXM_ERROR_IN_CONFIG_IA_ALERT_LEVEL   0x08

/**
 * @brief Error during the connection procedure procedure
 */
#define PXM_ERROR_IN_CONNECTION              0x09

/**
 * @brief Error during the read tx power procedure
 */
#define PXM_ERROR_IN_READ_TX_POWER           0x0A

/* Proximity Monitor Central Information */
typedef struct pxmInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
} pxmInitDevType;

typedef struct pxmSecurityS {
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
} pxmSecurityType;

typedef struct pxmConnDevS {
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
} pxmConnDevType;

typedef struct pxmDevDiscS {
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
} pxmDevDiscType;

typedef struct pxmConnUpdateParamS {
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
} pxmConnUpdateParamType;

typedef struct pxmConfDevS {
  /** Alert Level of Link Loss Service */
  uint8_t llAlertLevel;
  /* Alert Level of Immediate Alert Service */
  uint8_t iaAlertLevel;
  /** If TRUE the alert on path loss mechanism is enabled and managed inside the
   *  state machine with the threshold and timeout set by the user during the 
   *  full configuration init procedure
   */
  uint8_t alertPathLossEnabled;
  /** Path Loss threshold. If the path loss exceeds the threshold 
   *  the proximity monitor start the alert on the reporter and
   *  signals the alert to the application
   */
  int8_t pathLossTheshold;
  /** Timeout to verify the alert on path loss.
   *  This value is in ms
   */
  uint32_t pathLossTimeout;
} pxmConfDevType;

/* Proximity Monitor Public Function call */

/**
 * @brief Init the Proximity Monitore device
 * @param param Contains all the values to initialize the device
 * (see @ref pxmInitDevType)
 * @retval Status of call
 */
uint8_t PXM_Init(pxmInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref pxmDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the PXM_DeviceDiscovery_CB()
 */
uint8_t PXM_DeviceDiscovery(pxmDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref pxmSecurityType)
 * @retval Status of call
 */
uint8_t PXM_SecuritySet(pxmSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * PXM_SecuritySet()  
 * @retval Status of call
 */
uint8_t PXM_StartPairing(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback PXM_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
uint8_t PXM_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref pxmConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * PXM_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t PXM_DeviceConnection(pxmConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the PXM_ConnectionStatus_CB()
 */
uint8_t PXM_DeviceDisconnection(void);

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
uint8_t PXM_ConnectionParameterUpdateRsp(uint8_t accepted, pxmConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the PXM_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t PXM_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the PXM_CharacOfService_CB()
 * This function MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t PXM_DiscCharacServ(uint16_t uuid_service);

/**
 * @brief Discovery all the characteristic descriptors on 
 * the Tx Power Level service
 * @retval Status of call
 * @note All the information of the Tx Power Level 
 * characteristic descriptor will be returned in 
 * PXM_CharacDesc_CB()
 * This function MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t PXM_TxPwrLvl_DiscCharacDesc(void);

/**
 * @brief Configures the Alert Level of the Link Loss service
 * @param level Alert level to set. Valid values are:\n
 * - @ref NO_ALERT\n
 * - @ref MILD_ALERT\n
 * - @ref HIGH_ALERT\n
 * @retval Status of call
 * @note The procedure will be ended and the status of the 
 * call will be returned on the PXM_ConfigureAlert_CB()
 */
uint8_t PXM_ConfigureLinkLossAlert(uint8_t level);

/**
 * @brief Reads the Tx Power of the peer device
 * @retval Status of call
 * @note The value of the Tx Power will be returned
 * on the function PXM_DataValueRead_CB()
 */
uint8_t PXM_ReadTxPower(void);

/**
 * @brief Enables the Tx Power Notification 
 * @retval Status of call
 * @note When the enable Tx Power notification procedure ends
 * the status will be returned on the PXM_EnableNotification_CB()
 * This function MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t PXM_EnableTxPowerNotification(void);

/**
 * @brief Configures the Alert Level for the Immediate Alert service
 * @param level  Alert level to set. Valid values are:\n
 * - @ref NO_ALERT\n
 * - @ref MILD_ALERT\n
 * - @ref HIGH_ALERT\n
 * @retval Status of call
 */
uint8_t PXM_ConfigureImmediateAlert(uint8_t level);

/**
 * @brief Runs a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref pxmConnDevType)
 * @param confParam Configuration parameters for the Alert Level
 * (see @ref pxmConfDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * PXM_StateMachine() to run the proximity monitor internal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * PXM_FullConfError_CB().
 */
uint8_t PXM_ConnConf(pxmConnDevType connParam, pxmConfDevType confParam);

/**
 * @brief Runs the proximity monitor internal state machine. This function is
 * required when the application executes the PXM_ConnConf()
 * @retval None
 */
void PXM_StateMachine(void);

/**
 * @brief Returns the RSSI value from the currrent connection
 * @param value RSSI value returned
 * @retval Status of call
 */
uint8_t PXM_GetRSSI(int8_t *value);

/* Proximity Monitor Public Function callback */

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
extern void PXM_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
 * @note This callback MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void PXM_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void PXM_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the proximity monitor receives a connection
 * parameter update request from the proximity reporter device connected.
 * Theresponse from the proximity central device may be ignored or sent by 
 * PXM_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by proximity reporter device.
 * (see @ref pxmConnUpdateParamType for more details)
 * @retval None
 */
extern void PXM_ConnectionParameterUpdateReq_CB(pxmConnUpdateParamType *param);

/**
 * @brief This callback contains all the information of the characterisitcs of service discovery procedure
 * @param status Status of the characteristics of service discovery procedure
 * @param numCharac Number of characteristics found for that service on the peer device
 * @param charac Data of the characteristics of a service discovered.
 * The array will be a sequence of elements with the following format:\n
 * - 1 byte  UUID Type (1 = 16 bit, 2 = 128 bit)\n
 * - 2 bytes Characteristic Handle\n
 * - 1 bytes Characteristic Properties\n
 * - 2 bytes Characteristic Value Handle\n
 * - n bytes (2 or 16) Characteristics UUID\n
 * @retval None
 * @note This callback MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void PXM_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback contains all the information of the characterisitcs descriptor
 * of a service
 * @param status Status of the charcteristics descriptor discovery procedure
 * @param numCharac Number of characteristic descriptors of a service found on the peer device
 * @param charac Data of the characteristic descriptots discovered.
 * The array will be a sequence of elements with the following format:\n
 * - 1 byte Format handle data (1 = lists with 16 bit UUID, 2 = lists with 128 bit UUID)\n
 * - 2 bytes Characteristic descriptor Handle\n
 * - n bytes (2 or 16) UUID according the format field\n
 * @retval None
 * @note This callback MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void PXM_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callack returns the status of the Link Loss Alert Configuration procedure on the
 * proximity reporter peer device
 * @param status Status of the configuration procedure 
 * @retval None
 */
extern void PXM_ConfigureAlert_CB(uint8_t status);

/**
 * @brief This callback returns the Tx Power characteristic value of the 
 * proximity reporter peer device. Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void PXM_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This funtion is called from the proximity monitor profile 
 * to alert the application that the peer device is disconnected,
 * so, the application needs to alert the user with the right level.
 * @param level Link Loss Alert level
 * @retval None
 */
extern void PXM_LinkLossAlert(uint8_t level);

/**
 * @brief This funtion is called from the proximity monitor profile 
 * to alert the application that the path loss exceeds a threshold during the 
 * connection with the peer device. So, the application needs to alert the user with the right level.
 * @param level Path Loss Aler level
 * @retval None
 */
extern void PXM_PathLossAlert(uint8_t level);

/**
 * @brief This function is called from the proximity monitor profile when a Tx Power
 * notification is received
 * @param data_value Tx Power value notified
 * @retval None
 */
extern void PXM_TxPowerNotificationReceived(int8_t data_value);

/**
 * @brief This function is called from the proximity monitor profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the PXM_SendPinCode() function to set the pin code.
 */
extern void PXM_PinCodeRequired_CB(void);

/**
 * @brief This callack returns the status of the Tx Power enable notification procedure on the
 * proximity reporter peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the PXM_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void PXM_EnableNotification_CB(uint8_t status);
/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the PXM_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void PXM_FullConfError_CB(uint8_t error_type, uint8_t code);

#endif //_PROXIMITY_MONITOR_H_
