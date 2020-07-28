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
*   FILENAME        -  find_me_target.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/26/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    find me target central role APIs
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the find me target central rol profile.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include <host_config.h>
#include <hci.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>

#if (BLE_CURRENT_PROFILE_ROLES & FIND_ME_TARGET)

#include <find_me_target.h>
#include <find_me_target_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#define ALERT_STATUS_CHAR_LENGHT         1
#define RINGER_SETTING_CHAR_LENGHT       1
#define RINGER_CONTROL_POINT_CHAR_LENGHT 1

#define ENCRIPTION_KEY_SIZE 10 //TBR
/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/

/* Find me target profile context */
FindMeTargetContext_Type find_me_target_context; 

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported SAP
*******************************************************************************/

tBleStatus FMT_Init(fmtInitDevType param)
{
  initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&find_me_target_context, 0, sizeof(FindMeTargetContext_Type));//TBR
  
  BLUENRG_memcpy(find_me_target_context.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  
  find_me_target_context.alert_level_value = 0; //TBR
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    find_me_target_context.state = FIND_ME_TARGET_INITIALIZED;
    //find_me_target_context.fullConf = FALSE; //TBR
  } else {
    find_me_target_context.state = FIND_ME_TARGET_UNINITIALIZED;
  }
  
  return ret;
}

tBleStatus FMT_DeviceDiscovery(fmtDevDiscType param)
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
    find_me_target_context.state = FIND_ME_TARGET_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus FMT_SecuritySet(fmtSecurityType param)
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

tBleStatus FMT_DeviceConnection(fmtConnDevType param)
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
    find_me_target_context.state = FIND_ME_TARGET_START_CONNECTION;
  }

  return ret;
}

tBleStatus FMT_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(find_me_target_context.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    //find_me_target_context.fullConf = FALSE;//TBR
  }
  return ret;
}

tBleStatus FMT_ConnectionParameterUpdateRsp(uint8_t accepted, fmtConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(find_me_target_context.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus FMT_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(find_me_target_context.connHandle, FALSE);
  return ret;
}

tBleStatus FMT_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end FMT_Clear_Security_Database() */

tBleStatus FMT_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(find_me_target_context.connHandle, pinCode);
  
  return ret;
}

void FMT_StateMachine(void)//TBR: Is it needed?
{
  //uint8_t ret;

  switch(find_me_target_context.state) {
   case FIND_ME_TARGET_UNINITIALIZED:
    /* Nothing to do */
    break;
  case FIND_ME_TARGET_CONNECTED_IDLE:
    {
    }
    break;
  case FIND_ME_TARGET_INITIALIZED:
    /* Nothing to do */
    break;
  case FIND_ME_TARGET_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case FIND_ME_TARGET_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case FIND_ME_TARGET_CONNECTED:
    {
      find_me_target_context.state = FIND_ME_TARGET_CONNECTED_IDLE;
    }
    break;
  }
    
}/* end FMT_StateMachine() */

/**
  * @brief  Set the alert level characteristic value 
  * @param  value: alert level value
  * @retval tBleStatus: function status
  */
tBleStatus FMT_Set_Alert_Level_Value(uint8_t value) //TBR Is it needed at inti time?
{
  tBleStatus retval = BLE_STATUS_SUCCESS; 
        
  find_me_target_context.alert_level_value = value;
  retval =  aci_gatt_update_char_value(find_me_target_context.IASServiceHandle, 
                                       find_me_target_context.AlertLevelCharHandle,
                                       0, /* charValOffset */
                                       ALERT_STATUS_CHAR_LENGHT, /* charValueLen */
                                       (const uint8_t *) &(find_me_target_context.alert_level_value));  
  return (retval);
}/* end FMT_Set_AlertStatus_Value() */

/**
  * @brief  Get the currente alert level value 
  * @param  None
  * @retval alert level value
  */
uint8_t FMT_Get_Alert_Level_Value(void) 
{
  return (find_me_target_context.alert_level_value);  
}/* end FMT_Get_Alert_Level_Value() */

/**
 * FMT_Add_Services_Characteristics
 * 
 * @param[in]: None
 *            
 * It adds Immediate Alert service & related characteristic
 */ 
tBleStatus FMT_Add_Services_Characteristics(void)
{
  tBleStatus status = BLE_STATUS_FAILED;
  uint16_t uuid;
	
  /* the main profile has completed its initializations
   * so start the profile specific initialization
   * add the immediate alert service
   */
  FMT_DBG_MSG(profiledbgfile,"FMT_Add_Services_Characteristics(), Adding immediate alert service\n");
  uuid = IMMEDIATE_ALERT_SERVICE_UUID;
  status = aci_gatt_add_serv(UUID_TYPE_16, 
			     (const uint8_t *) &uuid, 
			     PRIMARY_SERVICE, 
			     3,
			     &(find_me_target_context.IASServiceHandle));
												  
  if (status == BLE_STATUS_SUCCESS)
  {
    FMT_DBG_MSG(profiledbgfile,"FMT_Add_Services_Characteristics(), immediate alert service OK %x\n",find_me_target_context.IASServiceHandle);
  } 
  else 
  {
    FMT_DBG_MSG(profiledbgfile,"FMT_Add_Services_Characteristics(),immediate alert service adding failed: 0x%02X\n", status);
    return status;
  }

  /* add the alert characteristic */  
  FMT_DBG_MSG(profiledbgfile,"Adding immediate alert characteristic\n");
  uuid = ALERT_LEVEL_CHARACTERISTIC_UUID;
  status = aci_gatt_add_char(find_me_target_context.IASServiceHandle, 
			     UUID_TYPE_16, 
			     (const uint8_t *) &uuid , 
			     2, /* value len */ 
			     CHAR_PROP_WRITE_WITHOUT_RESP,/* char properties */ 
			     ATTR_PERMISSION_NONE, /* security permission */
			     GATT_NOTIFY_ATTRIBUTE_WRITE, /* events to be notified */ //TBR
			     10, /* encryKeySize */
			     CHAR_VALUE_LEN_VARIABLE, /* isVariable */
			     &(find_me_target_context.AlertLevelCharHandle));

  if (status == BLE_STATUS_SUCCESS)
  {
    FMT_DBG_MSG(profiledbgfile,"FMT_Add_Services_Characteristics(),alert level characteristic OK %x\n", find_me_target_context.AlertLevelCharHandle);
  }
  else 
  {
    FMT_DBG_MSG(profiledbgfile,"FMT_Add_Services_Characteristics(),alert level characteristic adding failed: 0x%02X\n", status);
  }

  return status;
}/* end FMT_Add_Services_Characteristics() */

#endif /* (BLE_CURRENT_PROFILE_ROLES & FIND_ME_TARGET) */
