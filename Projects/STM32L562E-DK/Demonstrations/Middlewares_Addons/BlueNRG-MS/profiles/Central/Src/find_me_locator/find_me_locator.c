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
*   FILENAME        -  find_me_locator.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/26/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    find me locator central role APIs(send RACP, ...)
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  find me locator profile central role.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include <host_config.h>
#include <hci.h>
#include <hci_le.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>

#if (BLE_CURRENT_PROFILE_ROLES & FIND_ME_LOCATOR)

#include <find_me_locator.h>
#include <find_me_locator_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#define WEAK_FUNCTION(x) __weak x
/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/
/* Profile Central role: context */
FindMeLocatorContext_Type findMeLocatorContext;

/******************************************************************************
* Imported Variable
******************************************************************************/
extern initDevType initParam;

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported SAP
*******************************************************************************/

tBleStatus FML_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<findMeLocatorContext.numpeerServices; i++) {
    if (findMeLocatorContext.peerServices[j] == UUID_TYPE_16) {
      BLUENRG_memcpy(&uuid, &findMeLocatorContext.peerServices[j+5], 2);
      j++;
      BLUENRG_memcpy(start_handle, &findMeLocatorContext.peerServices[j], 2);
      j += 2;
      BLUENRG_memcpy(end_handle, &findMeLocatorContext.peerServices[j], 2);
      j += 2;
      if (findMeLocatorContext.peerServices[j-5] == UUID_TYPE_16) {
	j += 2;
      } else {
	j += 16;
      }
      if (uuid == uuid_service) {
	return BLE_STATUS_SUCCESS;
      }
    }
  }
  
  return HCI_INVALID_PARAMETERS;
}

tBleStatus FML_Init(fmlInitDevType param)
{
  //initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&findMeLocatorContext, 0, sizeof(FindMeLocatorContext_Type));//TBR
  
  BLUENRG_memcpy(findMeLocatorContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  initParam.Master_DeviceDiscovery_CB = Master_FML_DeviceDiscovery_CB;
  initParam.Master_Connection_CB = Master_FML_Connection_CB;
  initParam.Master_Pairing_CB = Master_FML_Pairing_CB;
  initParam.Master_PeerDataExchange_CB = Master_FML_PeerDataExchange_CB;
  initParam.Master_PeerDataPermitExchange_CB = Master_FML_PeerDataPermitExchange_CB;
  initParam.Master_ServiceCharacPeerDiscovery_CB = Master_FML_ServiceCharacPeerDiscovery_CB;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    findMeLocatorContext.state = FIND_ME_LOCATOR_INITIALIZED;
    findMeLocatorContext.fullConf = FALSE;
  } else {
    findMeLocatorContext.state = FIND_ME_LOCATOR_UNINITIALIZED;
  }

  return ret;
}

tBleStatus FML_DeviceDiscovery(fmlDevDiscType param)
{
  devDiscoveryType devDiscParam;
  uint8_t ret;
  
  devDiscParam.procedure = LIMITED_DISCOVERY_PROCEDURE;
  devDiscParam.scanInterval = param.scanInterval;
  devDiscParam.scanWindow = param.scanWindow;
  devDiscParam.own_address_type = param.own_addr_type;
  ret = Master_DeviceDiscovery(&devDiscParam);
  if (ret == BLE_STATUS_SUCCESS)
  {
    findMeLocatorContext.state = FIND_ME_LOCATOR_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus FML_SecuritySet(fmlSecurityType param)
{
  securitySetType secParam;
  uint8_t ret;

  secParam.ioCapability = param.ioCapability;
  secParam.mitm_mode = param.mitm_mode;
  secParam.oob_enable = param.oob_enable;
  if (secParam.oob_enable == OOB_AUTH_DATA_PRESENT)
    BLUENRG_memcpy(secParam.oob_data, param.oob_data, 16);
  secParam.use_fixed_pin = param.use_fixed_pin;
  if (secParam.use_fixed_pin == USE_FIXED_PIN_FOR_PAIRING)
    secParam.fixed_pin = param.fixed_pin;
  else 
    secParam.fixed_pin = 0;
  secParam.bonding_mode = param.bonding_mode;
  ret = Master_SecuritySet(&secParam);

  return ret;
}

tBleStatus FML_DeviceConnection(fmlConnDevType param)
{
  connDevType master_param;
  uint8_t ret;

  master_param.procedure = DIRECT_CONNECTION_PROCEDURE;
  master_param.fastScanDuration = param.fastScanDuration;
  master_param.fastScanInterval = param.fastScanInterval;
  master_param.fastScanWindow = param.fastScanWindow;
  master_param.reducedPowerScanInterval = param.reducedPowerScanInterval;
  master_param.reducedPowerScanWindow = param.reducedPowerScanWindow;
  master_param.peer_addr_type = param.peer_addr_type;
  BLUENRG_memcpy(master_param.peer_addr, param.peer_addr, 6);
  master_param.own_addr_type = param.own_addr_type;
  master_param.conn_min_interval = param.conn_min_interval;
  master_param.conn_max_interval = param.conn_max_interval;
  master_param.conn_latency = param.conn_latency;
  master_param.supervision_timeout = param.supervision_timeout;
  master_param.min_conn_length = param.min_conn_length;
  master_param.max_conn_length = param.max_conn_length;
  ret = Master_DeviceConnection(&master_param);
  if (ret == BLE_STATUS_SUCCESS) {
    findMeLocatorContext.state = FIND_ME_LOCATOR_START_CONNECTION;
  }

  return ret;
}

tBleStatus FML_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(findMeLocatorContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    findMeLocatorContext.fullConf = FALSE;//TBR-----------
  }
  return ret;
}

tBleStatus FML_ConnectionParameterUpdateRsp(uint8_t accepted, fmlConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(findMeLocatorContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus FML_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(findMeLocatorContext.connHandle, FALSE);
  return ret;
}

tBleStatus FML_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end FML_Clear_Security_Database() */

tBleStatus FML_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(findMeLocatorContext.connHandle, pinCode);
  
  return ret;
}

tBleStatus FML_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(findMeLocatorContext.connHandle, 
				  &findMeLocatorContext.numpeerServices, 
				  findMeLocatorContext.peerServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS)
  {
    findMeLocatorContext.state = FIND_ME_LOCATOR_SERVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus FML_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if (uuid_service == IMMEDIATE_ALERT_SERVICE_UUID) {
    if (!(findMeLocatorContext.IASServicePresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = FML_findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(findMeLocatorContext.connHandle,
				    start_handle,
				    end_handle,
				    &findMeLocatorContext.numCharac,
				    findMeLocatorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS)
    {
      findMeLocatorContext.uuid_searched = uuid_service;
    }
  }
  return ret;
}


/* This function start the connection and the configuration of the device */
uint8_t FML_ConnConf(fmlConnDevType connParam)
{
  uint8_t ret;

  ret = FML_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    findMeLocatorContext.fullConf = TRUE;
  }

  return ret;
}

void FML_StateMachine(void)
{
  uint8_t ret;

  switch(findMeLocatorContext.state) {
  case FIND_ME_LOCATOR_UNINITIALIZED:
    /* Nothing to do */
    break;
  case FIND_ME_LOCATOR_CONNECTED_IDLE:
    /* Nothing to do */
    break;
  case FIND_ME_LOCATOR_INITIALIZED:
    /* Nothing to do */
    break;
  case FIND_ME_LOCATOR_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case FIND_ME_LOCATOR_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case FIND_ME_LOCATOR_CONNECTED:
    {
      ret = FML_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	findMeLocatorContext.state = FIND_ME_LOCATOR_SERVICE_DISCOVERY;
      } else {
	FML_FullConfError_CB(FML_ERROR_IN_SERVICE_DISCOVERY, ret);
	findMeLocatorContext.state = FIND_ME_LOCATOR_DISCONNECTION;
      }
    }
    break;
  case FIND_ME_LOCATOR_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case FIND_ME_LOCATOR_ALERT_LEVEL_CHARAC_DISCOVERY:
    {
      ret = FML_DiscCharacServ(IMMEDIATE_ALERT_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	findMeLocatorContext.state = FIND_ME_LOCATOR_WAIT_ALERT_LEVEL_CHARAC_DISCOVERY;
      } else {
	FML_FullConfError_CB(FML_ERROR_IN_CHARAC_DISCOVERY, ret);
	findMeLocatorContext.state = FIND_ME_LOCATOR_DISCONNECTION;
      }
    }
    break;
  case FIND_ME_LOCATOR_WAIT_ALERT_LEVEL_CHARAC_DISCOVERY:
    /* Waiting the end of the Glucose Measurement Characteristic discovery */ 
    break;
    
  case FIND_ME_LOCATOR_DISCONNECTION:
    {
      ret = FML_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	FML_FullConfError_CB(FML_ERROR_IN_DISCONNECTION, ret);
      }
      findMeLocatorContext.state = FIND_ME_LOCATOR_INITIALIZED;
    }
    break;
  }
}

/**
  * @brief It causes an alert the Find Me Target device 
  * @param [in] alertLevel: alert level value 
  * @retval tBleStatus: BLE status  
  */
tBleStatus FML_ALert_Target(uint8_t alertLevel)
{
  /* When the Find Me Locator device wishes to cause an alert on the Find Me Target device, 
     it shall write the specific Alert Level in the Alert Level characteristic.
  */
  
  tBleStatus ble_status = BLE_STATUS_FAILED;
  
  if((alertLevel != NO_ALERT) && (alertLevel != MILD_ALERT) && (alertLevel != HIGH_ALERT))
  {
    return HCI_INVALID_PARAMETERS;
  }
  
  /* Check if Device is connected */
  if ((findMeLocatorContext.IASServicePresent) && (findMeLocatorContext.AlertLevelCharValHandle !=0))
  {	
    findMeLocatorContext.requested_alert_level = alertLevel;
    //FML_DBG_MSG(profiledbgfile,"--- Connection Handle: 0x%04X , AlertLevelCharValHandle: 0x%04X, requested_alert_level: 0x%02X \n",findMeLocatorContext.peer.connHandle,findMeLocatorContext.AlertLevelCharValHandle,findMeLocatorContext.requested_alert_level);
   
    ble_status = Master_WriteWithoutResponse_Value(findMeLocatorContext.connHandle, /* connection handle */
                                                   findMeLocatorContext.AlertLevelCharValHandle, /* attribute value handle */
                                                   1, /* charValueLen */
                                                   (uint8_t *) &(findMeLocatorContext.requested_alert_level));
  }
  return ble_status; 
}

/****************** Weak Callbacks definition ***************************/

WEAK_FUNCTION(void FML_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & FIND_ME_LOCATOR) */
