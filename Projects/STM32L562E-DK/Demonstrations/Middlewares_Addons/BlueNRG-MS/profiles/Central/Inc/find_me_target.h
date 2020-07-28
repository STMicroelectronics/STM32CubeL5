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
*   FILENAME        -  find_me_target.h
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
*   Comments:    find me target central role  header file
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the find me target central profile role
* 
******************************************************************************/


/******************************************************************************
 * Include Files
******************************************************************************/

#ifndef _FIND_ME_TARGET_H_
#define _FIND_ME_TARGET_H_

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
#define FIND_ME_TARGET_UNINITIALIZED           0x0000
#define FIND_ME_TARGET_CONNECTED_IDLE          0x0001
#define FIND_ME_TARGET_INITIALIZED             0x0002
#define FIND_ME_TARGET_DEVICE_DISCOVERY        0x0003
#define FIND_ME_TARGET_START_CONNECTION        0x0004
#define FIND_ME_TARGET_CONNECTED               0x0005
#define FIND_ME_TARGET_CONN_PARAM_UPDATE_REQ   0x0006
/******************************************************************************
* Type definitions
******************************************************************************/

/* Profile Central Init Information */
typedef struct fmtInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
} fmtInitDevType;

/* Profile Central Security Information */
typedef struct fmtSecurityS {
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
} fmtSecurityType;

/* Profile Central Connection  Information */
typedef struct fmtConnDevS {
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
} fmtConnDevType;

/* Profile Central Device Discovery Information */
typedef struct fmtDevDiscS {
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
} fmtDevDiscType;

/* Profile Central Connection Update Information */
typedef struct fmtConnUpdateParamS {
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
} fmtConnUpdateParamType;


/* Profile context: it contains 
   characteristics state values,  handles */ 
typedef struct FindMeTargetContextS {
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  
  uint8_t alert_status_value; 
  uint8_t ringer_setting_value;
  BOOL mute_once_silence; 
  uint8_t alert_level_value; 
  
  /**
   * the Immediate alert service handle
   */ 
  uint16_t IASServiceHandle;
	
  /**
   * Alert level characteristic
   * handle
   */ 
  uint16_t AlertLevelCharHandle;
  
  //uint8_t fullConf;

} FindMeTargetContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* phone alert status profile context */
extern FindMeTargetContext_Type find_me_target_context; 

/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref fmtInitDevType)
 * @retval Status of call
 */
tBleStatus FMT_Init(fmtInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref fmtDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the FMT_DeviceDiscovery_CB()
 */
tBleStatus FMT_DeviceDiscovery(fmtDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref fmtSecurityType)
 * @retval Status of call
 */
tBleStatus FMT_SecuritySet(fmtSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * FMT_SecuritySet()  
 * @retval Status of call
 */
tBleStatus FMT_StartPairing(void);

/**
  * @brief Clear Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus FMT_Clear_Security_Database();

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback FMT_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus FMT_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref fmtConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * FMT_ConnectionStatus_CB() 
 */
tBleStatus FMT_DeviceConnection(fmtConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the FMT_ConnectionStatus_CB()
 */
tBleStatus FMT_DeviceDisconnection(void);

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
tBleStatus FMT_ConnectionParameterUpdateRsp(uint8_t accepted, fmtConnUpdateParamType *param);

/**
 * @brief Add the Profile service & related characteristics
 * @param[in]: None
 * @retval Status of the call.
 */ 
tBleStatus FMT_Add_Services_Characteristics(void);

/**
 * @brief Run the profile central state machine. Is it needed? TBR???
 * @retval None
 */
void FMT_StateMachine(void);

/**
  * @brief  Set the alert level characteristic value 
  * @param  value: alert level value
  * @retval tBleStatus: function status
  */
tBleStatus FMT_Set_Alert_Level_Value(uint8_t value); //TBR Is it needed at init time?

/**
  * @brief  Get the currente alert level value 
  * @param  None
  * @retval alert level value
  */
uint8_t FMT_Get_Alert_Level_Value(void);

/***************  Public Function callback ******************/

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
extern void FMT_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
extern void FMT_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void FMT_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * FMT_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref passConnUpdateParamType for more details)
 * @retval None
 */
extern void FMT_ConnectionParameterUpdateReq_CB(fmtConnUpdateParamType *param);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the FMT_SendPinCode() function to set the pin code.
 */
extern void FMT_PinCodeRequired_CB(void);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void FMT_Pairing_CB(uint16_t conn_handle, uint8_t status);

/**
  * @brief  User Callback which is called each time alert level 
  *         value is received: user specific implementation 
  *         action should be done accordingly.
  * @param  alert_level value of received alert level
  * @retval None
  */
extern void FMT_Alert_Level_Value_CB(uint8_t alert_level);

#endif /* _FIND_ME_TARGET_H_ */
