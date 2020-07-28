/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
* File Name          : master_basic_profile.c
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.0
* Date               : 15-October-2014
* Description        : This file manages all the General Master APIs
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "bluenrg_gap.h"
#include "master_basic_profile.h"
#include "master_basic_profile_sm.h"
#include "hci_le.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#ifdef DEBUG_BASIC_PROFILE
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define STATE_TRANSITION(prefix, prevState, nextState) {    \
    masterState.state = prefix##_##nextState;	            \
    masterState.subState = prefix##_##nextState | 0x0001;   \
    DEBUG_MESSAGE(prefix##_##prevState, masterState.state); \
}

uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */

initDevType initParam;

uint8_t getBlueNRGVersion(uint8_t *hwVersion, uint16_t *fwVersion);

/* Public functions ----------------------------------------------------------*/

uint8_t Master_Init(initDevType *param)
{ 
  uint8_t  hwVersion;
  uint16_t fwVersion;
  uint8_t ret = BLE_STATUS_SUCCESS;
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;


  /* ACI, HCI application command callback */
  BLE_LowLevel_ACI_Event_CB = param->BLE_HCI_Event_CB;

  /* Reset the device */
  hci_reset();
	
  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  /* 
   * Reset BlueNRG again otherwise we won't
   * be able to change its MAC address.
   * aci_hal_write_config_data() must be the first
   * command after reset otherwise it will fail.
   */
  hci_reset();
  
  PRINTF("HWver %d, FWver %d", hwVersion, fwVersion);
  
  if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
    bnrg_expansion_board = IDB05A1; 
    /*
     * Change the MAC address to avoid issues with Android cache:
     * if different boards have the same MAC address, Android
     * applications do not properly work unless you restart Bluetooth
     * on tablet/phone
     */
    param->public_addr[5] = 0x03;
  }

  /* Set the public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, 
				  CONFIG_DATA_PUBADDR_LEN, param->public_addr);
  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("Setting BD_ADDR failed.\n");
    return ret;
  }
    
  /* GATT Init */
  ret = aci_gatt_init();    
  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GATT_Init failed.\n");
    return ret;
  }
    
  /* GAP Init */
  if (bnrg_expansion_board == IDB04A1) {
    ret = aci_gap_init_IDB04A1(GAP_CENTRAL_ROLE_IDB04A1, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }
  else {
    ret = aci_gap_init_IDB05A1(GAP_CENTRAL_ROLE_IDB05A1, 0,
                     param->device_name_len,
                     &service_handle,
                     &dev_name_char_handle,
                     &appearance_char_handle);
  }
  
  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GAP_Init failed.\n");
    return ret;
  }

  /* Set the device name */
  if (param->device_name_len != 0) {
    if (bnrg_expansion_board == IDB04A1) {
      if (param->device_name_len > 8)
        param->device_name_len = 8;
    }
    ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, param->device_name_len, param->device_name);
    if(ret){
      PRINTF("Set device name failed.\n");
    }
  }

  /* Set the tx output power */
  ret = aci_hal_set_tx_power_level(1, param->txPowerLevel);    
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("Set Output Power failed.\n");
    return ret;
  }

  PRINTF("BLE Stack Initialized.\n");

  /* Run Master state machine */
  Master_Process(param); // Reset the peer context
  STATE_TRANSITION(STATE, INIT, IDLE); // Init complete

  return ret;
}

uint8_t Master_SecuritySet(securitySetType *param)
{
  uint8_t ret;

  /* Set the IO device capability */
  ret = aci_gap_set_io_capability(param->ioCapability);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Set IO Capability failed. \n");
    return ret;
  }

  /* Set security param for the master */
  ret = aci_gap_set_auth_requirement(param->mitm_mode, param->oob_enable,
				     param->oob_data, 7, 16,
				     param->use_fixed_pin, param->fixed_pin,
				     param->bonding_mode);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Set auth requirement failed. \n");
    return ret;
  }

  return ret;
}

uint8_t Master_DeviceDiscovery(devDiscoveryType *param)
{
  uint8_t ret = BLE_STATUS_SUCCESS;
  devDiscoveryContextType *devDisContext = (devDiscoveryContextType *) masterGlobalContext; 

  if ((GET_CURRENT_STATE() != STATE_IDLE) &&
      (GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_DEVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    if (param->procedure == LIMITED_DISCOVERY_PROCEDURE) {
      ret = aci_gap_start_limited_discovery_proc(param->scanInterval, param->scanWindow, 
						 param->own_address_type, 1);
    } else {
      ret = aci_gap_start_general_discovery_proc(param->scanInterval, param->scanWindow, 
						 param->own_address_type, 1);
    }
    if (ret != BLE_STATUS_SUCCESS) {
      PRINTF("Start Discovery Procedure failed\n");
    } else {
      /* During all the device discovery procedure the host micro can enter in power save */
      devDisContext->powerSave_enable = 1;
      devDisContext->end_reason = 0;
      devDisContext->procedureComplete = FALSE;
      devDisContext->procedureUsed = param->procedure;
      STATE_TRANSITION(STATE, IDLE, DEVICE_DISCOVERY); // Start Device Discovery Procedure
    }
  }

  return ret;
}

uint8_t Master_TerminateDiscovery(void)
{
  uint8_t ret = BLE_STATUS_SUCCESS;
  devDiscoveryContextType *devDisContext = (devDiscoveryContextType *) masterGlobalContext; 

  if (devDisContext->procedureUsed == LIMITED_DISCOVERY_PROCEDURE) {
    ret = aci_gap_terminate_gap_procedure(LIMITED_DISCOVERY_PROCEDURE);
  } else {
    ret = aci_gap_terminate_gap_procedure(GENERAL_DISCOVERY_PROCEDURE);
  }

  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("GAP Terminate Gap Procedure failed\n");
  } else {
    devDisContext->end_reason = DEVICE_DISCOVERY_PROCEDURE_ENDED;
  }

  return ret;
}

uint32_t Master_Process(initDevType *param)
{
  masterState = masterStateMachine(masterState, param);

  return masterState.powerSave_bitMask;
}

uint8_t Master_DeviceConnection(connDevType *param)
{
  uint8_t ret=BLE_STATUS_SUCCESS;
  connectionContextType *connectionContext = (connectionContextType *) masterGlobalContext;

  memcpy(&connectionContext->userParam, param, sizeof(connDevType));

  connectionContext->hostExit_TimeoutPowerSave = 0;

  if (GET_CURRENT_STATE() != STATE_DEVICE_DISCOVERY) {
    connectionContext->discProcedureComplete = TRUE;
    STATE_TRANSITION(STATE, IDLE, CONNECTION); // Start Connection Procedure
  } else {
    /* If the device is in discovery procedure 
     before to start the connection procedure we need to stop it */
    connectionContext->discProcedureComplete = FALSE;
    connectionContext->powerSave_enable = 1;
    ret = Master_TerminateDiscovery();
    STATE_TRANSITION(STATE, DEVICE_DISCOVERY, CONNECTION); // Start Connection Procedure
  }

  return ret;
}

uint8_t Master_TerminateGAPConnectionProcedure(void)
{
  uint8_t ret=BLE_STATUS_SUCCESS;
  uint8_t procedure;
  connectionContextType *connectionContext = (connectionContextType *) masterGlobalContext;

  if (connectionContext->userParam.procedure == DIRECT_CONNECTION_PROCEDURE)
    procedure = GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC;
  else
    procedure = GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC;
  ret = aci_gap_terminate_gap_procedure(procedure);

  return ret;
}

uint8_t Master_CloseConnection(uint16_t handle)
{
  return aci_gap_terminate(handle, HCI_OE_USER_ENDED_CONNECTION);
}

uint8_t Master_GetPrimaryServices(uint16_t conn_handle, uint8_t *numPrimaryService, 
				 uint8_t *primaryService, uint16_t size)
{
  uint8_t ret=BLE_STATUS_SUCCESS;
  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

  /* The device shall be in the state connected idle
     before to start the service discovery procedure */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) && 
      (GET_CURRENT_STATE() != STATE_CONNECTION)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    serDiscContext->procedure = PRIMARY_SERVICE_DISCOVERY;
    serDiscContext->conn_handle = conn_handle;
    serDiscContext->numInfoServer = numPrimaryService;
    serDiscContext->arrayInfoServer = primaryService;
    serDiscContext->size = size;
    *serDiscContext->numInfoServer = 0;
    STATE_TRANSITION(STATE, CONNECTED_IDLE, SERVICE_DISCOVERY); // Start Service Discovery Procedure
  }
  
  return ret;
}

uint8_t Master_GetCharacOfService(uint16_t conn_handle, uint16_t start_service_handle, uint16_t end_service_handle,
				  uint8_t *numCharac, uint8_t *charac, uint16_t size)
{
  uint8_t ret=BLE_STATUS_SUCCESS;
  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

  /* The device shall be in the state connected idle or service discovery
     before to start the get characteristics of a service procedure */
  if ((GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY) &&
      (GET_CURRENT_STATE() != STATE_CONNECTED_IDLE)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    serDiscContext->procedure = GET_CHARACTERISTICS_OF_A_SERVICE;
    serDiscContext->conn_handle = conn_handle;
    serDiscContext->numInfoServer = numCharac;
    serDiscContext->arrayInfoServer = charac;
    serDiscContext->size = size;
    serDiscContext->start_handle = start_service_handle;
    serDiscContext->end_handle = end_service_handle;
    *serDiscContext->numInfoServer = 0;
    STATE_TRANSITION(STATE, CONNECTED_IDLE, SERVICE_DISCOVERY); // Read all Characteriscis of a Service Procedure
  }
  
  return ret;  
}

uint8_t Master_GetIncludeServices(uint16_t conn_handle, uint16_t start_service_handle, uint16_t end_service_handle,
				  uint8_t *numIncludedServices, uint8_t *includedServices, uint16_t size)
{
  uint8_t ret=BLE_STATUS_SUCCESS;
  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

  /* The device shall be in the state connected idle or service discovery
     before to start the get include services procedure */
  if ((GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY) &&
      (GET_CURRENT_STATE() != STATE_CONNECTED_IDLE)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    serDiscContext->procedure = FIND_INCLUDED_SERVICES;
    serDiscContext->conn_handle = conn_handle;
    serDiscContext->numInfoServer = numIncludedServices;
    serDiscContext->arrayInfoServer = includedServices;
    serDiscContext->size = size;
    serDiscContext->start_handle = start_service_handle;
    serDiscContext->end_handle = end_service_handle;
    *serDiscContext->numInfoServer = 0;
    STATE_TRANSITION(STATE, CONNECTED_IDLE, SERVICE_DISCOVERY); // Find all Included Services Procedure
  }
  
  return ret;  
}

uint8_t Master_GetCharacDescriptors(uint16_t conn_handle, uint16_t start_value_charac_handle, uint16_t end_charac_handle,
				    uint8_t *numCharacDesc, uint8_t *characDesc, uint16_t size)
{
  uint8_t ret=BLE_STATUS_SUCCESS;
  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

  /* The device shall be in the state connected idle or service discovery
     before to start the get characteristic descriptors procedure */
  if ((GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY) &&
      (GET_CURRENT_STATE() != STATE_CONNECTED_IDLE)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    serDiscContext->procedure = FIND_CHARAC_DESCRIPTORS;
    serDiscContext->conn_handle = conn_handle;
    serDiscContext->numInfoServer = numCharacDesc;
    serDiscContext->arrayInfoServer = characDesc;
    serDiscContext->size = size;
    serDiscContext->start_handle = start_value_charac_handle;
    serDiscContext->end_handle = end_charac_handle;
    *serDiscContext->numInfoServer = 0;
    STATE_TRANSITION(STATE, CONNECTED_IDLE, SERVICE_DISCOVERY); // Find all Characteristic Descriptor
  }
  
  return ret;  
}

uint8_t Master_NotifIndic_Status(uint16_t conn_handle, uint16_t config_desc_handle, 
				 uint8_t notification_enable, uint8_t indication_enable)
{
  uint8_t ret;
  uint8_t attr_value[2]={0,0};
  connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;

  /* The device shall be in the state connected idle or in the bonding state
     before to start the change notification/indication status */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_BONDING) &&
      (GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    attr_value[0] = (indication_enable<<1) | notification_enable;
    ret = aci_gatt_write_charac_descriptor(conn_handle, config_desc_handle, 2, attr_value);
    if (ret == BLE_STATUS_SUCCESS) {
      connIdleContext->procedureComplete = FALSE;
      connIdleContext->procedure = NOTIFICATION_INDICATION_CHANGE_STATUS;
      connIdleContext->conn_handle = conn_handle;
      connIdleContext->procedureStatus = BLE_STATUS_SUCCESS;
      memcpy(connIdleContext->write_data, &config_desc_handle, 2);
      connIdleContext->write_data[2] = notification_enable;
      connIdleContext->write_data[3] = indication_enable;
      connIdleContext->write_len = 2;
      connIdleContext->powerSave_enable = 1;
      STATE_TRANSITION(STATE, CONNECTED_IDLE, CONNECTED_IDLE); // Start the Notification Indication change status procedure
    }
  }
  
  return ret;  
}

uint8_t Master_Read_Value(uint16_t conn_handle, uint16_t attr_value_handle, 
			  uint16_t *data_length, uint8_t *data, uint16_t size)
{
  uint8_t ret;
  connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;

  /* The device shall be in the state connected idle or in the bonding state
     before to start the read procedure */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_BONDING) &&
      (GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    ret = aci_gatt_read_charac_val(conn_handle, attr_value_handle);
    if (ret == BLE_STATUS_SUCCESS) {
      connIdleContext->procedureComplete = FALSE;
      connIdleContext->procedure = READ_VALUE_STATUS;
      connIdleContext->conn_handle = conn_handle;
      connIdleContext->data_length = data_length;
      connIdleContext->data_array = data;
      connIdleContext->size = size;
      memcpy(connIdleContext->write_data, &attr_value_handle, 2);
      connIdleContext->procedureStatus = BLE_STATUS_SUCCESS;
      connIdleContext->powerSave_enable = 1;
      STATE_TRANSITION(STATE, CONNECTED_IDLE, CONNECTED_IDLE); // Start the read characteristic procedure
    }
  }

  return ret;
}

uint8_t Master_Read_Long_Value(uint16_t conn_handle, uint16_t attr_value_handle, 
			      uint16_t *data_length, uint8_t *data, 
			      uint8_t offset, uint16_t size)
{
  uint8_t ret;
  connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;

  /* The device shall be in the state connected idle or in the bonding state
     before to start the read procedure */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_BONDING) &&
      (GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    ret = aci_gatt_read_long_charac_val(conn_handle, attr_value_handle, offset);
    if (ret == BLE_STATUS_SUCCESS) {
      connIdleContext->procedureComplete = FALSE;
      connIdleContext->procedure = READ_LONG_VALUE_STATUS;
      connIdleContext->conn_handle = conn_handle;
      connIdleContext->data_length = data_length;
      connIdleContext->data_array = data;
      connIdleContext->size = size;
      memcpy(connIdleContext->write_data, &attr_value_handle, 2);
      connIdleContext->write_data[2] = offset;
      connIdleContext->procedureStatus = BLE_STATUS_SUCCESS;
      connIdleContext->powerSave_enable = 1;
      STATE_TRANSITION(STATE, CONNECTED_IDLE, CONNECTED_IDLE); // Start the read characteristic procedure
    }
  }

  return ret;
}

uint8_t Master_Write_Value(uint16_t conn_handle, uint16_t attr_value_handle, 
			   uint8_t value_len, uint8_t *attr_value)
{
  uint8_t ret;
  connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;

  if (value_len > (ATT_MTU - 3)) {
    return HCI_INVALID_PARAMETERS;
  }

  /* The device shall be in the state connected idle or in the bonding state
     before to start the read procedure */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_BONDING) &&
      (GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    ret = aci_gatt_write_charac_value(conn_handle, attr_value_handle, value_len, attr_value);
     if (ret == BLE_STATUS_SUCCESS) {
      connIdleContext->procedureComplete = FALSE;
      connIdleContext->procedure = WRITE_VALUE_STATUS;
      connIdleContext->conn_handle = conn_handle;
      connIdleContext->procedureStatus = BLE_STATUS_SUCCESS;
      memcpy(connIdleContext->write_data, &attr_value_handle, 2);
      connIdleContext->write_len = value_len;
      memcpy(&connIdleContext->write_data[2], attr_value, value_len);
      connIdleContext->powerSave_enable = 1;
      STATE_TRANSITION(STATE, CONNECTED_IDLE, CONNECTED_IDLE); // Start the read characteristic procedure
     }
  }

  return ret;
}

uint8_t Master_WriteWithoutResponse_Value(uint16_t conn_handle,  uint16_t attr_value_handle,
					  uint8_t value_len, uint8_t* attr_value)
{
  uint8_t ret;

  if (value_len > (ATT_MTU - 3)) {
    return HCI_INVALID_PARAMETERS;
  }

  /* The device shall be in the state connected idle or in the bonding state
     before to start the write without response procedure */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_BONDING) &&
      (GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {  
    ret = aci_gatt_write_without_response(conn_handle, attr_value_handle,
					  value_len, attr_value);
  }

  return ret;
}

uint8_t Master_AllowRead(uint16_t conn_handle)
{
  uint8_t ret;

  /* The device shall be in the state connected idle or in the bonding state
     before to start the allow read procedure */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_BONDING) &&
      (GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {  
    ret = aci_gatt_allow_read(conn_handle);
  }

  return ret;
}

uint8_t Master_WriteResponse(uint16_t conn_handle, uint16_t attr_value_handle, uint8_t write_status, uint8_t err_code,
			     uint8_t attr_value_len, uint8_t *attr_value)
{
  uint8_t ret;

  /* The device shall be in the state connected idle or in the bonding state
     before to start the write response procedure */
  if ((GET_CURRENT_STATE() != STATE_CONNECTED_IDLE) &&
      (GET_CURRENT_STATE() != STATE_BONDING) &&
      (GET_CURRENT_STATE() != STATE_SERVICE_DISCOVERY)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    ret = aci_gatt_write_response(conn_handle, attr_value_handle,
				  write_status, err_code,
				  attr_value_len, attr_value);
  }

  return ret;
}

uint8_t Master_Start_Pairing_Procedure(uint16_t conn_handle, uint8_t force_rebond)
{
  uint8_t ret;
  uint32_t current_state;
  bondingContextType *bondingContext = (bondingContextType *)masterGlobalContext;


  /* The device shall be in the state connected, conencted idle or
     discovery service before to start the bonding procedure */
  current_state = GET_CURRENT_STATE();
  if ((current_state != STATE_CONNECTED_IDLE) &&
      (current_state != STATE_SERVICE_DISCOVERY) &&
      (current_state != STATE_CONNECTION)) {
    ret = ERR_COMMAND_DISALLOWED;
  } else {
    ret = aci_gap_send_pairing_request(conn_handle, force_rebond);
    if (ret == BLE_STATUS_SUCCESS) {
      bondingContext->powerSave_enable = 1;
      bondingContext->procedure = 0;
      bondingContext->procedureComplete = FALSE;
      bondingContext->conn_handle = conn_handle;
      bondingContext->procedureStatus = BLE_STATUS_SUCCESS;
      bondingContext->passKeyProc = FALSE;
      if (current_state == STATE_CONNECTED_IDLE) {
	STATE_TRANSITION(STATE, CONNECTED_IDLE, BONDING); // Start the bonding & pairing procedure
      } else {
	if (current_state == STATE_SERVICE_DISCOVERY) {
	  STATE_TRANSITION(STATE, SERVICE_DISCOVERY, BONDING); // Start the bonding & pairing procedure
	} else {
	  STATE_TRANSITION(STATE, CONNECTION, BONDING); // Start the bonding & pairing procedure
	}
      }
    }
  }
  
  return ret;
}

uint8_t Master_Passkey_Response(uint16_t conn_handle, uint32_t key)
{
  uint8_t ret;
  bondingContextType *bondingContext = (bondingContextType *)masterGlobalContext;

  bondingContext->passKeyProc = TRUE;
  bondingContext->procedureComplete = FALSE;
  bondingContext->conn_handle = conn_handle;
  ret = aci_gap_pass_key_response(conn_handle, key);
  STATE_TRANSITION(STATE, BONDING, BONDING); // Start the pass key procedure

  return ret;
}

uint8_t Master_ClearSecurityDatabase(void)
{
  return aci_gap_clear_security_database();
}

uint8_t Master_SetTxPower(uint8_t level)
{
  uint8_t ret;

  if (level > 7)
    return HCI_INVALID_PARAMETERS;

  ret = aci_hal_set_tx_power_level(1, level);    
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("Set Output Power failed.\n");
  }
  return ret;

}

uint8_t Master_HostExitTimeoutPowerSave(void)
{
  connectionContextType *connectionContext = (connectionContextType *) masterGlobalContext;
  
  if (GET_CURRENT_STATE() == STATE_CONNECTION) {
    connectionContext->hostExit_TimeoutPowerSave = 1;
  }
	  
  return BLE_STATUS_SUCCESS;
}

uint8_t Master_ConnectionUpdateParamResponse(uint16_t conn_handle, uint8_t accepted, connUpdateParamType *param)
{
  uint8_t ret;

  if (bnrg_expansion_board == IDB04A1) {
    ret = aci_l2cap_connection_parameter_update_response_IDB04A1(conn_handle, param->interval_min, param->interval_max, 
                                                                 param->slave_latency, param->timeout_mult, 
                                                                 param->identifier, accepted);
  }
  else {
    ret = aci_l2cap_connection_parameter_update_response_IDB05A1(conn_handle, param->interval_min, param->interval_max, 
                                                                 param->slave_latency, param->timeout_mult,
                                                                 param->min_conn_length, param->max_conn_length,
                                                                 param->identifier, accepted);
  }

  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF(" Master_ConnectionUpdateParamResponse() failed.\n");
  }

  return ret;
}

uint8_t getBlueNRGVersion(uint8_t *hwVersion, uint16_t *fwVersion)
{
  uint8_t status;
  uint8_t hci_version, lmp_pal_version;
  uint16_t hci_revision, manufacturer_name, lmp_pal_subversion;

  status = hci_le_read_local_version(&hci_version, &hci_revision, &lmp_pal_version, 
				     &manufacturer_name, &lmp_pal_subversion);

  if (status == BLE_STATUS_SUCCESS) {
    *hwVersion = hci_revision >> 8;
    *fwVersion = (hci_revision & 0xFF) << 8;              // Major Version Number
    *fwVersion |= ((lmp_pal_subversion >> 4) & 0xF) << 4; // Minor Version Number
    *fwVersion |= lmp_pal_subversion & 0xF;               // Patch Version Number
  }

  //hci_user_evt_proc(); // To receive the BlueNRG EVT

  return status;
}
