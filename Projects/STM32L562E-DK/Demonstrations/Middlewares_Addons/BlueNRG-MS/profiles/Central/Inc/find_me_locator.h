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
*   FILENAME        -  find_me_locator.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/26/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    find me locator central role header file 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the find me locator profile role
* 
******************************************************************************/


/******************************************************************************
 * Include Files
******************************************************************************/

#ifndef _FIND_ME_LOCATOR_H_
#define _FIND_ME_LOCATOR_H_

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
/* Alert levels */
#define NO_ALERT		        (0x00)
#define MILD_ALERT                      (0x01)
#define HIGH_ALERT                      (0x02)

/* Profiles States */
#define FIND_ME_LOCATOR_UNINITIALIZED                                  0x0000
#define FIND_ME_LOCATOR_CONNECTED_IDLE                                 0x0001
#define FIND_ME_LOCATOR_INITIALIZED                                    0x0002
#define FIND_ME_LOCATOR_DEVICE_DISCOVERY                               0x0003
#define FIND_ME_LOCATOR_START_CONNECTION                               0x0004
#define FIND_ME_LOCATOR_CONNECTED                                      0x0005

#define FIND_ME_LOCATOR_SERVICE_DISCOVERY                              0x0006
#define FIND_ME_LOCATOR_ALERT_LEVEL_CHARAC_DISCOVERY                   0x0007  
#define FIND_ME_LOCATOR_WAIT_ALERT_LEVEL_CHARAC_DISCOVERY              0x0008  

#define FIND_ME_LOCATOR_CONN_PARAM_UPDATE_REQ                          0x0009
#define FIND_ME_LOCATOR_DISCONNECTION                                  0x000A

#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 50
/******************************************************************************
* Type definitions
******************************************************************************/

/* Profile Error Condition */

/**
 * @brief Error during the service discovery procedure
 */
#define FML_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define FML_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define FML_ERROR_IN_CHARAC_DISCOVERY        0x03
   
/**
 * @brief Error during the connection procedure procedure
 */
#define FML_ERROR_IN_CONNECTION              0x04

/******************************************************************************
* Type definitions
******************************************************************************/

/* Profile Central Init Information */
typedef struct fmlInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
} fmlInitDevType;

/* Profile Central Security Information */
typedef struct fmlSecurityS {
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
} fmlSecurityType;

/* Profile Central Connection Information */
typedef struct fmlConnDevS {
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
} fmlConnDevType;

/* Profile Central Device Discovery Information */
typedef struct fmlDevDiscS {
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
} fmlDevDiscType;

/* Profile Central Connection Update Information */
typedef struct fmlConnUpdateParamS {
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
} fmlConnUpdateParamType;

/* Profile Context variable */
typedef struct FindMeLocatorContextS {
  
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numpeerServices;
  
  uint8_t IASServicePresent; 
  
  uint8_t peerServices[PRIMARY_SERVICES_ARRAY_SIZE];
  /**
   * Alert Level characteristic
   * handle
   */ 
  uint16_t AlertLevelCharValHandle; //AlertLevelCharHandle --> AlertLevelCharValHandle

  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
  
  uint8_t fullConf;
  
  /**
  * flags 
  */
  uint8_t requested_alert_level;
  
} FindMeLocatorContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* find me locator context */
extern FindMeLocatorContext_Type findMeLocatorContext; 

/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref fmlInitDevType)
 * @retval Status of call
 */
tBleStatus FML_Init(fmlInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref fmlDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the FML_DeviceDiscovery_CB()
 */
tBleStatus FML_DeviceDiscovery(fmlDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref fmlSecurityType)
 * @retval Status of call
 */
tBleStatus FML_SecuritySet(fmlSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * FML_SecuritySet()  
 * @retval Status of call
 */
tBleStatus FML_StartPairing(void);

/**
  * @brief Clear Glucose Collector Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus FML_Clear_Security_Database(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback FML_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus FML_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref fmlConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * FML_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the FML_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus FML_DeviceConnection(fmlConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the FML_ConnectionStatus_CB()
 */
tBleStatus FML_DeviceDisconnection(void);

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
tBleStatus FML_ConnectionParameterUpdateRsp(uint8_t accepted, fmlConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the FML_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the FML_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus FML_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the FML_CharacOfService_CB()
 * This function MUST not be called if the application uses the FML_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus FML_DiscCharacServ(uint16_t uuid_service);


/**
 * @brief Run a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref fmlConnDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * FML_StateMachine() to run the Glucose Collectorinternal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * FML_FullConfError_CB().
 */
tBleStatus FML_ConnConf(fmlConnDevType connParam);

/**
 * @brief Run the Glucose Collector internal state machine. This function is
 * required when the application executes the FML_ConnConf()
 * @retval None
 */
void FML_StateMachine(void);

/**
 * @brief Find the device services
 * @param uuid_service  service uuid to be searched
 * @param [out] *start_handle pointer to service start handle
 * @param [out] *end_handle pointer to service end handle
 * @retval Status of call
 */
tBleStatus FML_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                           uint16_t *end_handle);

/**
  * @brief It causes an alert the Find Me Target device 
  * @param [in] alertLevel: alert level value 
  * @retval tBleStatus: BLE status  
  */
tBleStatus FML_ALert_Target(uint8_t alertLevel);

/***************  Glucose CollectorPublic Function callback ******************/

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
extern void FML_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
 * @note This callback MUST not be called if the application uses the FML_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void FML_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void FML_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the Glucose Collector receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * FML_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref fmlConnUpdateParamType for more details)
 * @retval None
 */
extern void FML_ConnectionParameterUpdateReq_CB(fmlConnUpdateParamType *param);

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
 * @note This callback MUST not be called if the application uses the FML_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void FML_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This function is called from the Glucose Collectorprofile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the FML_SendPinCode() function to set the pin code.
 */
extern void FML_PinCodeRequired_CB(void);

/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the FML_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void FML_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void FML_Pairing_CB(uint16_t conn_handle, uint8_t status);

#endif /* _FIND_ME_LOCATOR_H_ */
