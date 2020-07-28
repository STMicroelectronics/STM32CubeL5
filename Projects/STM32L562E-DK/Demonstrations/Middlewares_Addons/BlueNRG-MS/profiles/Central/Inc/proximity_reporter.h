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
*   FILENAME        -  proximity_reporter.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      03/05/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    proximity reporter central role header file
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the proximity reporter profile central role
* 
******************************************************************************/


/******************************************************************************
 * Include Files
******************************************************************************/

#ifndef _PROXIMITY_REPORTER_H_
#define _PROXIMITY_REPORTER_H_

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
/* alert levels values */
#define NO_ALERT	       (0x00)
#define MILD_ALERT             (0x01)
#define HIGH_ALERT             (0x02)

/* alert levels types */
#define EVT_PR_PATH_LOSS_ALERT (0x00)
#define EVT_PR_LINK_LOSS_ALERT (0x01)

/* Proximity Reporter States */
#define PROXIMITY_REPORTER_UNINITIALIZED           0x0000
#define PROXIMITY_REPORTER_CONNECTED_IDLE          0x0001
#define PROXIMITY_REPORTER_INITIALIZED             0x0002
#define PROXIMITY_REPORTER_DEVICE_DISCOVERY        0x0003
#define PROXIMITY_REPORTER_START_CONNECTION        0x0004
#define PROXIMITY_REPORTER_CONNECTED               0x0005
#define PROXIMITY_REPORTER_CONN_PARAM_UPDATE_REQ   0x0006

#define CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH  (7)
#define READ_CURR_TRANSMIT_POWER_LEVEL   (0x00)
/******************************************************************************
* Type definitions
******************************************************************************/
/* Profile Central Information */
typedef struct pxrInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
  /* Immediate Alert and TX Power Services are both supported (1) or not (0) */
  uint8_t immAlertTxPowerServSupport;
} pxrInitDevType;

typedef struct pxrSecurityS {
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
} pxrSecurityType;

typedef struct pxrConnDevS {
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
} pxrConnDevType;

typedef struct pxrDevDiscS {
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
} pxrDevDiscType;

typedef struct pxrConnUpdateParamS {
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
} pxrConnUpdateParamType;

/* Proximity reporter Context variable */
typedef struct ProximityReporterContextS {
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  
  /**
   * this is a flag set during initialization
   * based on the application's value to tell the
   * profile whether the TX power level and
   * immediate alert services have to be exposed
   */ 
  uint8_t immAlertTxPowerServSupport;
    
  /**
   * handle of link loss service
   */ 
  uint16_t linkLossServiceHandle;
	
  /**
   * handle of the alert level charecteristic of
   * link loss service
   */ 
  uint16_t linkLossAlertLevelCharHandle;
	
  /**
   * handle of the immediate alert service
   */ 
  uint16_t immdiateAlertServiceHandle;
	
  /**
   * handle of the alert level characteristic
   * in the immediate alert service
   */ 
  uint16_t immediateAlertLevelCharHandle;
	
  /**
   * handle of the TX power level service
   */ 
  uint16_t txPowerServiceHandle;
	
  /**
   * handle of the TX power level characteristic 
   */ 
  uint16_t txPowerLevelCharHandle;

  /**
   * the alert level that has to be raised
   * on detecting a link loss
   */ 
  uint8_t linkLossAlertLevel;
  
  //uint8_t fullConf;

} ProximityReporterContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* profile context */
extern ProximityReporterContext_Type proximity_reporter_context; 

/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref pxrInitDevType)
 * @retval Status of call
 */
tBleStatus PXR_Init(pxrInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref pxrDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the PXR_DeviceDiscovery_CB()
 */
tBleStatus PXR_DeviceDiscovery(pxrDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref pxrSecurityType)
 * @retval Status of call
 */
tBleStatus PXR_SecuritySet(pxrSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * PXR_SecuritySet()  
 * @retval Status of call
 */
tBleStatus PXR_StartPairing(void);

/**
  * @brief Clear Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus PXR_Clear_Security_Database();

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback PXR_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus PXR_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref pxrConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * PXR_ConnectionStatus_CB() 
 */
tBleStatus PXR_DeviceConnection(pxrConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the PXR_ConnectionStatus_CB()
 */
tBleStatus PXR_DeviceDisconnection(void);

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
tBleStatus PXR_ConnectionParameterUpdateRsp(uint8_t accepted, pxrConnUpdateParamType *param);

/**
 * @brief Add the Proximity reporter service & related characteristics
 * @param[in]: None
 * @retval Status of the call.
 */ 
tBleStatus PXR_Add_Services_Characteristics(void);

/**
 * @brief Run the profile central state machine. Is it needed? TBR???
 * @param  None
 * @retval None
 */
void PXR_StateMachine(void);

/**
* @brief  It is called on when an alert level characteristic is written
* @param  attrHandle: handle of the alert level characteristic
* @param  attValue: alert value  
* @retval None
*/
void PXR_Received_Alert_Handler(uint16_t attrHandle,uint8_t attValue);

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
extern void PXR_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
extern void PXR_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void PXR_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * PXR_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref pxrConnUpdateParamType for more details)
 * @retval None
 */
extern void PXR_ConnectionParameterUpdateReq_CB(pxrConnUpdateParamType *param);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the PXR_SendPinCode() function to set the pin code.
 */
extern void PXR_PinCodeRequired_CB(void);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void PXR_Pairing_CB(uint16_t conn_handle, uint8_t status);

/**
* @brief  User defined callback which is called each time an 
*         alert is raised: user specific implementation 
*         action should be done accordingly.
* @param  alert_type: alert type (path loss or link loss)
* @param  alert_value: alert value  
* @retval None
*/
extern void PXR_Alert_CB(uint8_t alert_type, uint8_t alert_value);

#endif /* _PROXIMITY_REPORTER_H_ */
