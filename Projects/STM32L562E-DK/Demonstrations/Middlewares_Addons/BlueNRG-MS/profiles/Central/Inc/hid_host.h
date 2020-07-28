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
*   FILENAME        -  hid_host.h
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
*   Comments:    hid host
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the hid host application profile central role
* 
******************************************************************************/

#ifndef _HID_HOST_H_
#define _HID_HOST_H_

/* HID Report Type */

/**
 * @brief Input report type 
 */
#define INPUT_REPORT 0x01

/**
 * @brief Output report type 
 */
#define OUTPUT_REPORT 0x02

/**
 * @brief Boot Keyboard input report type
 */
#define BOOT_KEYBOARD_INPUT_REPORT  0x03

/**
 * @brief Boot Keyboard output report type
 */
#define BOOT_KEYBOARD_OUTPUT_REPORT 0x04

/**
 * @brief Boot Mouse input report type
 */
#define BOOT_MOUSE_INPUT_REPORT     0x05

/**
 * @brief Feature report type 
 */
#define FEATURE_REPORT 0x03

/* HID Protocolo Mode */

/**
 * @brief Boot Protocol Mode
 */
#define BOOT_PROTOCOL_MODE 0x00

/**
 * @brief Report Protocol Mode
 */
#define REPORT_PROTOCOL_MODE 0x01

/* HID Host Error Condition */

/**
 * @brief Error during the connection procedure procedure
 */
#define HID_ERROR_IN_CONNECTION                0x01

/**
 * @brief Error during the service discovery procedure
 */
#define HID_ERROR_IN_SERVICE_DISCOVERY         0x02
/**
 * @brief Error during the include service discovery procedure
 */
#define HID_ERROR_IN_INCLUDE_SERVICE_DISCOVERY 0x03

/**
 * @brief Error during the characteristic discovery procedure
 */
#define HID_ERROR_IN_CHARAC_DISCOVERY          0x04

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define HID_ERROR_IN_CHARAC_DESC_DISCOVERY     0x05

/**
 * @brief Error during the disconnection procedure
 */
#define HID_ERROR_IN_DISCONNECTION             0x06

/**
 * @brief Error during the read report descriptor procedure
 */
#define HID_ERROR_IN_READ_REPORT_DESCRIPTOR    0x07

/**
 * @brief Error during the read HID information procedure
 */
#define HID_ERROR_IN_READ_HID_INFORMATION      0x08

/**
 * @brief Error during the read Battery Characteristic 
 * Presentation Format 
 */
#define HID_ERROR_IN_READ_BATTERY_PRESENTATION_FORMAT 0x09

/**
 * @brief Error during the Battery Level read procedure 
 */
#define HID_ERROR_IN_READ_BATTERY_LEVEL 0x0A

/**
 * @brief Error durign the Battery Level Client Characteristic 
 * Descriptor read procedure
 */
#define HID_ERROR_IN_READ_BATTERY_LEVEL_CHARAC_DESC 0x0B

/**
 * @brief Error during the Pnp ID read procedure
 */
#define HID_ERROR_IN_READ_PNP_ID 0x0C

/**
 * @brief Error during the Boot Report read procedure
 */
#define HID_ERROR_IN_READ_BOOT_REPORT 0x0D

/**
 * @brief Error Boot Report Characteristic not present
 */
#define HID_ERROR_BOOT_REPORT_NOT_PRESENT 0x0E

/**
 * @brief Report not present
 */
#define HID_ERROR_REPORT_NOT_PRESENT 0x0F

/**
 * @brief Error during the enable notification procedure
 */
#define HID_ERROR_IN_ENABLE_NOTIFICATION 0x10

/**
 * @brief Error during the write procedure
 */
#define HID_ERROR_IN_WRITE_PROCEDURE 0x11

/* HID Host Structure definition */

/**
 * @brief HID Host init parameters 
 */
typedef struct hidInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** TX output power */
  uint8_t txPower;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** HID Host Mode:\n
   *  - BOOT_PROTOCOL_MODE\n
   *  - REPORT_PROTOCOL_MODE
   */
  uint8_t hostMode;
} hidInitDevType;

/**
 * @brief HID Host security parameters
 */
typedef struct hidSecurityS {
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
} hidSecurityType;

/**
 *@brief Parameter settings for the device discovery procedure
 */
typedef struct hidDevDiscS {
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
} hidDevDiscType;

/**
 * @brief Parameters used during the connection procedure
 */
typedef struct hidConnDevS {
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
} hidConnDevType;

/**
 * @brief Contains all the configuration variables to run the HID internal state machine.
 * This structure is used only if the HID_ConnConf() function is called
 */
typedef struct hidConfDevS {
  /** Max number of HID report descriptors supported */
  uint8_t maxNmbReportDesc;
  /** Max Size of the HID Report descriptor size */
  uint16_t maxReportDataSize;
  /** Returns the number of HID report descriptors present in the peer device */
  uint8_t *numReportDescPresent;
  /** Array to contain all the length of the HID report descriptors */
  uint16_t *reportDescLen;
  /** Array to contain all the HID report descriptors data */ 
  uint8_t *reportData;
} hidConfDevType;

/**
 * @brief Connection update paramters suggested from the HID peripheral device
 */
typedef struct hidConnUpdateParamS {
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
} hidConnUpdateParamType;

/* HID Host Public Function call */

/**
 * @brief Init the HID Host device
 * @param param Contains all the values to initialize the device
 * (see @ref hidInitDevType)
 * @retval Status of call
 */
uint8_t HID_Init(hidInitDevType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref hidSecurityType)
 * @retval Status of call
 */
uint8_t HID_SecuritySet(hidSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * HID_SecuritySet()  
 * @retval Status of call
 */
uint8_t HID_StartPairing(void);

/**
 * @brief Delete the information database of the bonded devices
 * @retval Status of call
 */
uint8_t HID_ClearBondedDevices(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function must be called if the callback HID_PinCodeRequired_CB()
 * is raised from the HID host profile.
 */
uint8_t HID_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref hidDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the HID_DeviceDiscovery_CB()
 */
uint8_t HID_DeviceDiscovery(hidDevDiscType param);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref hidConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * HID_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t HID_DeviceConnection(hidConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the HID_ConnectionStatus_CB()
 */
uint8_t HID_DeviceDisconnection(void);

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
uint8_t HID_ConnectionParameterUpdateRsp(uint8_t accepted, hidConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the HID_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t HID_ServicesDiscovery(void);

/**
 * @brief This function performs the relationship discovery to find included
 * services, to dicovery all Battery Services with characteristis described within
 * a HID Service Report Map characteristic value.
 * @retval Status of call
 * @note All the information of the included services will be returned
 * on the HID_IncludedServices_CB()
 * This function MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine
 */
uint8_t HID_GetIncludedBatterySerivces(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service Service UUID to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the HID_CharacOfService_CB()
 * This function MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t HID_DiscCharacServ(uint16_t uuid_service);

/**
 * @brief Discovery all the characteristic descriptors for the 
 * given characteristic
 * @param uuid_charac UUID of the characteristic to use to discover all the
 * descriptors. Possible value are:\n
 * - REPORT_MAP_CHAR_UUID\n
 * - REPORT_CHAR_UUID\n
 * - BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID\n
 * - BOOT_MOUSE_INPUT_REPORT_CHAR_UUID\n
 * - SCAN_REFRESH_CHAR_UUID
 * @retval Status of call
 * @note All the information of the characteristic descriptors 
 * will be returned in  HID_CharacDesc_CB()
 * This function MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
uint8_t HID_DiscCharacDesc(uint16_t uuid_charac);

/**
 * @brief Returns the number of USB HID report descriptors present in the peer device
 * @retval Number of the report descriptors present in the peer device.
 */
uint8_t HID_NumberOfReportDescriptor(void);

/**
 * @brief Reads the USB HID report descriptor for the specified HID service in the peer device.
 * @param reportToRead USB HID report descriptor to read the first report set this variable to 1. 
 * The know how many report descriptors are present in the peer device use 
 * the HID_NumberOfReportDescriptor() function call.
 * @param reportDataLen Length of the report descriptor of the peer device.
 * @param reportData Array with the USB Report Descriptor data
 * @param maxSize Max size of the report data array. If the length of the report descriptors exceeds
 * the maxSize a BLE_STATUS_INSUFFICIENT_RESOURCES error will be returned in the HID_ReadReportDescruiptor_CB().
 * @ertval Status of call
 * @note The report descriptors data and length will be available when the HID_ReadReportDescriptor_CB() is called.
 */
uint8_t HID_ReadReportDescriptor(uint8_t reportToRead, uint16_t *reportDataLen, uint8_t *reportData, uint16_t maxSize);

/**
 * @brief Reads all the Report values for each HID Services present in the peripheral
 * device.
 * @param characToRead Report Characteristic information to read. Valid values are:\n
 * - EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID\n
 * - REPORT_REFERENCE_DESCRIPTOR_UUID\n
 * - CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID\n
 * - INPUT_REPORT\n
 * - OUTPUT_REPORT\n
 * - FEATURE_REPORT\n
 * @retval Status of call
 * @note All the data read will be returned at the end of the procedure with the callback HID_DataValueRead_CB()
 */
uint8_t HID_ReadReportValue(uint16_t characToRead);

/**
 * @brief Reads the HID Information value for all HID Services in the peer HID Device.
 * @retval Status of call
 * @note All the data read will be returned at the end of the procedure with the callback HID_InformationData_CB()
 */
uint8_t HID_ReadHidInformation(void);

/**
 * @brief Reads the Battery Level values of the peer device.
 * @retval Status of call
 * @note All the data read will be returned at the end of the procedure with the callback HID_BetteryLevelData_CB()
 */
uint8_t HID_ReadBatteryLevel(void);

/**
 * @brief Reads the Battery Level Client Characteristic Descriptor
 * @retval Status of call
 * @note All the data read will be returned at the end of the procedure with the callback HID_BatteryClientCahracDesc_CB()
 */
uint8_t HID_ReadBatteryClientCharacDesc(void);

/**
 * @brief Reads the Pnp ID value of the Device Information Service.
 * @retval Status of call
 * @note All the data read will be returned at the end of the procedure with the callback HID_PnPID_CB()
 */
uint8_t HID_ReadPnPID(void);

/**
 * @brief Reads the Boot Report characteristic value
 * @param bootReportUUID Boot Report UUID to read. Valid values are:\n
 * - BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID\n
 * - BOOT_KEYBOARD_OUTPUT_REPORT_CHAR_UUID\n
 * - BOOT_MOUSE_INPUT_REPORT_CHAR_UUID
 * @retval Status of call
 * @note All the data read will be returned at the end of the procedure with the callback HID_BootReportValue_CB()
 */
uint8_t HID_ReadBootReport(uint16_t bootReportUUID);

/**
 * @brief Reads the Boot Report Client Characteristic Descriptor value
 * @param bootReportUUID Boot Report UUID to read. Valid values are:\n
 * - BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID\n
 * - BOOT_MOUSE_INPUT_REPORT_CHAR_UUID
 * @retval Status of call
 * @note All the data read will be returned at the end of the procedure with the callback HID_ReadBootReportClientCharacDesc_CB()
 */
uint8_t HID_ReadBootReportClientCharacDesc(uint16_t bootReportUUID);

/**
 * @brief Writes the Scan Interval Window parameters to the HID peer device
 * @param scanInterval Maximum scan interval the scan client intends to use while scanning
 * @param scanWindow Maximum scan window the scan client intends to use while scanning in
 * conjunction with the maximum scan interval written
 * @retval Status of call
 */
uint8_t HID_WriteScanIntervalWindowParam(uint16_t scanInterval, uint16_t scanWindow);

/**
 * @brief Sets the notification status for the Scan Refresh Characteristic
 * @param enabled TRUE to enable the notification, FALSE otherwise
 * @retval Stqatus of call
 */
uint8_t HID_ScanRefreshNotificationStatus(uint8_t enabled);

/**
 * @brief Runs a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref hidConnDevType)
 * @param confParam Contains all the variables used to configure and to manage report information
 * (see @ref hidConfDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * HID_StateMachine() to run the HID host internal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * HID_FullConfError_CB().
 */
uint8_t HID_ConnConf(hidConnDevType connParam, hidConfDevType confParam);

/**
 * @brief Runs the HID host internal state machine. This function is
 * required when the application executes the HID_ConnConf()
 * @retval None
 */
void HID_StateMachine(void);

/**
 * @brief Returns at the end of the configuration returns all the Report ID of the requested report Type
 * @param type Report Type to search
 * @param numReport Returns the number of specifeid reports discovered
 * @param ID Array with the Report ID
 * @retval None
 */
void HID_GetReportId(uint8_t type, uint8_t *numReport, uint8_t *ID);

/**
 * @brief Sets the report value in the peer device.
 * @brief noResponseFlag If TRUE the HID Host executes the BLE write without response procedure where allowed,
 * FALSE the BLE write procedure
 * @param type Report type:\n
 * - INPUT_REPORT\n
 * - OUTPUT_REPORT\n
 * - FEATURE_REPORT\n
 * @param ID Report ID
 * @param dataLen Length of the data to write
 * @param data Data to write
 * @retval Status of call
 * @note If the noResponseFlag is FALSE, the status of the procedure will be returned in the HID_SetProcedure_CB()
 */ 
uint8_t HID_SetReport(uint8_t noResponseFlag, uint8_t type, uint8_t ID, uint8_t dataLen, uint8_t *data);

/**
 * @brief Sets the report value in the peer device.
 * @param type Report type:\n
 * - INPUT_REPORT\n
 * - OUTPUT_REPORT\n
 * - FEATURE_REPORT\n
 * @param ID Report ID
 * @retval Status of call
 * @note The report data read will be returned at the end of the procedure with the callback HID_DataValueRead_CB()
 */ 
uint8_t HID_GetReport(uint8_t type, uint8_t ID);

/**
 * @brief Sets the HID Control Point, to inform the HID Device that the HID Host is entering/exiting
 * the suspended state
 * @param suspend TRUE the HID Host enters in suspend state, FALSE otherwise
 * @retval Status of call
 * @note The status of the procedure will be returned in the HID_SetProcedure_CB()
 */
uint8_t HID_SetControlPoint(uint8_t suspend);

/**
 * @brief Sets the HID Device Protocol Mode. 
 * @param mode Protocol mode to setup. Valid values are:\n
 * - BOOT_PROTOCOL_MODE\n
 * - REPORT_PROTOCOL_MODE
 * @retval Status of call
 * @note The status of the procedure will be returned in the HID_SetProcedure_CB()
 */
uint8_t HID_SetProtocol(uint8_t mode);

/**
 * @brief Gets the Protocol Mode:\n
 * - BOOT_PROTOCOL_MODE\n
 * - REPORT_PROTOCOL_MODE
 * @retval Status of call
 * @note The Protocol mode will be returned at the end of the procedure with the callback HID_ProtocolMode_CB()
 */
uint8_t HID_GetProtocol(void);

/**
 * @brief Sets the Boot Report value.
 * @param type Boot report type:\n
 * - BOOT_KEYBOARD_INPUT_REPORT\n
 * - BOOT_KEYBOARD_OUTPUT_REPORT\n
 * - BOOT_MOUSE_INPUT_REPORT
 * @param dataLen Length of the data to write
 * @param data Data to write
 * @retval Status of call
 * @note The status of the procedure will be returned in the HID_SetProcedure_CB()
 */
uint8_t HID_SetBootReport(uint8_t type, uint8_t dataLen, uint8_t *data);

/**
 * @brief Sets the Notification status enabled/disabled for the input report
 * in the HID Device.
 * @param type Report type:\n
 * - INPUT_REPORT\n
 * - BOOT_KEYBOARD_INPUT_REPORT\n
 * - BOOT_MOUSE_INPUT_REPORT
 * @param enabled TRUE if the notifications are enable, FALSE otherwise
 * @retval None
 * @note The status of the procedure will be returned in the HID_NotificationChageStatus_CB()
 */
uint8_t HID_SetNotificationStatus(uint8_t type, uint8_t enabled);

/**
 * @brief Sets the HID Host mode
 * @param mode HID Host mode:\n
 * - BOOT_PROTOCOL_MODE\n
 * - REPORT_PROTOCOL_MODE
 * @retval Status of call
 */
uint8_t HID_SetHostMode(uint8_t mode);

/* HID Host Public Function callback */

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
extern void HID_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
				   uint8_t data_length, uint8_t *data, 
				   uint8_t RSSI);

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
extern void HID_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the HID host receives a connection
 * parameter update request from the HID peripheral device connected.
 * The response from the HID host device may be ignored or sent by 
 * HID_ConnectionParameterUpdateRsp()
 * @param param Connection update parameters sent from HID peripheral device.
 * (see @ref hidConnUpdateParamType for more details)
 * @retval None
 */
extern void HID_ConnectionParameterUpdateReq_CB(hidConnUpdateParamType *param);

/**
 * @brief This function is called from the HID host profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the HID_SendPinCode() function to set the pin code.
 */
extern void HID_PinCodeRequired_CB(void);

/**
 * @brief This function is called from the HID host profile when during the reconnection the 
 * pairing procedure fails. So, the application needs repeat the pairing procedure.
 */
extern void HID_PairingFailed_CB(void);

/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the HID_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void HID_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callback contains all the information of the service discovery procedure
 * @param status Status of the service discovery procedure
 * @param numServices Number of services discovered
 * @param services Data of the services discovered.
 *  * The array will be a sequence of elements with the following format:
 * - 1 byte  UUID Type (1 = 16 bit, 2 = 128 bit)
 * - 2 bytes Service Start Handle
 * - 2 bytes Service End Handle
 * - n bytes (2 or 16) Service UUID
 * @retval None
 * @note This callback MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HID_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

/**
 * @brief This callback contains all the information of the included service discovery  procedure
 * @param status Status of the included service discovery procedure
 * @param numIncludedServices Number of included services discovered
 * @param includedServices Data of the included services discovered.
 * The array will be a sequence of elements with the following format:
 * - 1 byte size of the information (possible value: 6 or 8)
 * - 2 bytes Characteristic Handle
 * - 2 bytes Start Included Handle
 * - 2 bytes End Included Handle
 * - 2 bytes UUID only if it is a 16-bit Bluetooth UUID 
 * @retval None
 * @note This callback MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HID_IncludedServices_CB(uint8_t status, uint8_t numIncludedServices, uint8_t *includedServices);

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
 * @note This callback MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HID_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

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
 * @note This callback MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HID_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback returns the status of the USB HID report descriptor read procedure.
 * if the status is BLE_STATUS_SUCCESS the reportDataLen and reportData variable will contain
 * the HID report descriptor information.
 * @param status Status of the USB HID report descriptor read procedure
 * @retval None
 * @note This callback MUST not be called if the application uses the HID_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HID_ReadReportDescriptor_CB(uint8_t status);

/**
 * @brief This callback returns the data read from the peer device.
 * Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void HID_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This callback returns the HID Information data read for an HID Service.
 * This callback will be called for each HID Service present in the HID peer Device.
 * @param status Status of the read procedure
 * @param version Version number of the USB HID Specification implemented in the peer HID Device.
 * @param countryCode Country Code for the HID Device hardware. If 0x00 the HW ia not localized.
 * @param remoteWake TRUE if the peer HID Device is capable of sending a wake signal to a HID Host.
 * FALSE otherwise.
 * @param normallyConnectable TRUE if HID device will be advertising when bonded but not connected.
 * FALSE otherwise.
 * @retVal None
 */
extern void HID_InformationData_CB(uint8_t status, uint16_t version, 
                                   uint8_t countryCode, uint8_t remoteWake, 
                                   uint8_t normallyConnectable);

/**
 * @brief This callback returns the level of the Battery services present in the HID Device.
 * This callback will be called for each Battery Service present in the HID peer Device.
 * @param status Status of the read procedure
 * @param namespace If there are more than 1 battery service this value is unique for that instance of the Battery Serivce
 * @param description If there are more than 1 battery service this value is unique for that instance of the Battery Serivce
 * @param level Battery level as a percentage from 0% to 100%. 0% represents a battery that is 
 * full discharged, 100% represents a battery that is fully charged.
 * @retVal None
 */
extern void HID_BetteryLevelData_CB(uint8_t status, uint8_t namespace, uint16_t description, uint8_t level);

/**
 * @brief This callback returns the value of the Battery Level client characteristic descriptor for the peer HID Device.
 * @param status Status of the read procedure
 * @param notification If 1 the notification is enabled. If 0 is disabled.
 * @param indication If 1 the indication is enabled. If 0 is disabled.
 * @retVal None
 */
extern void HID_BatteryClientCahracDesc_CB(uint8_t status, uint8_t notification, uint8_t indication);

/**
 * @brief This callback returns the values of the PnP ID characteristic for the peer HID Device.
 * @param status Status of the read procedure
 * @param vendorIdSource Identifies the source of the Vendor ID parameter. 1 Bluetooth SIG source, 2 USB Implementer Forum assigned Vendor ID value
 * @param vendorId Identifies the product vendor from the namespace in the Vendor ID Source
 * @param productId Manufacturer managed identifier for this product
 * @param productVersion Manufacturer managed version for this product
 * @retVal None
 */
extern void HID_PnPID_CB(uint8_t status, uint8_t vendorIdSource, uint16_t vendorId, uint16_t productId, uint16_t productVersion);

/**
 * @brief This callback returns the report value for the Boot Keyboard and Mouse characteristics
 * @param status Status of the read procedure
 * @param dataLen Data length
 * @param data Boot report data
 * @retVal None
 */
extern void HID_BootReportValue_CB(uint8_t status, uint8_t dataLen, uint8_t *data);

/**
 * @brief This callback returns the client characteristic descriptor value for the Boot Keyboard and Mouse characteristics
 * @param status Status of the read procedure
 * @param notification If 1 the notification is enabled. If 0 is disabled.
 * @param indication If 1 the indication is enabled. If 0 is disabled.
 * @retVal None
 */
extern void HID_ReadBootReportClientCharacDesc_CB(uint8_t status, uint8_t notification, uint8_t indication);

/**
 * @brief This callback returns the protocol mode of the HID peer Device
 * @param status Status of the get protocol mode procedure
 * @param mode Protocol Mode:\n
 * - 0 Boot Protocol Mode\n
 * - 1 Report Protocol Mode\n
 * @retVal None
 */
extern void HID_ProtocolMode_CB(uint8_t status, uint8_t mode);

/**
 * @brief This callback returns the status of the set procedure
 * @param status Status of the set report procedure
 * @retVal None
 */
extern void HID_SetProcedure_CB(uint8_t status);

/**
 * @brief This callback reports the status of the enable/disable notification
 * @param status Status of the enable/disable procedure
 * @retVal None
 */
extern void HID_NotificationChageStatus_CB(uint8_t status);

/**
 * @brief This callbak reports the notification recevied from the HID Host
 * @param type Report Type
 * @param id Report ID
 * @param data_length Length of the data received
 * @param data_value Data received
 * @retval None
 */
extern void HID_ReportDataReceived_CB(uint8_t type, uint8_t id, uint8_t data_length, uint8_t *data_value);

#endif //_HID_HOST_H_
