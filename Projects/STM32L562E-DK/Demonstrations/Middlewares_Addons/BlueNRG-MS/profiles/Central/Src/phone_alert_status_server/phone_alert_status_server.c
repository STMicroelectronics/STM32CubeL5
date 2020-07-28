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
*   FILENAME        -  phone_alert_server.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/16/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    phone alert server central role APIs
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the phone alert server profile.
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

#if (BLE_CURRENT_PROFILE_ROLES & PHONE_ALERT_STATUS_SERVER)

#include <phone_alert_status_service.h>
#include <phone_alert_status_server.h>
#include <phone_alert_status_server_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#define ALERT_STATUS_CHAR_LENGHT         1
#define RINGER_SETTING_CHAR_LENGHT       1
#define RINGER_CONTROL_POINT_CHAR_LENGHT 1

#define ENCRIPTION_KEY_SIZE 10 //TBR

#define WEAK_FUNCTION(x) __weak x
/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/

/* phone alert status server context */
PhoneAlertStatusServerContext_Type phone_alert_status_server_context;

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported SAP
*******************************************************************************/
tBleStatus PASS_Init(passInitDevType param)
{
  initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&phone_alert_status_server_context, 0, sizeof(PhoneAlertStatusServerContext_Type));//TBR
  
  phone_alert_status_server_context.alert_status_value = param.alert_status_init_value; 
  phone_alert_status_server_context.ringer_setting_value = param.ringer_setting_init_value;
  phone_alert_status_server_context.mute_once_silence = FALSE;
  
  BLUENRG_memcpy(phone_alert_status_server_context.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    phone_alert_status_server_context.state = PHONE_ALERT_STATUS_SERVER_INITIALIZED;
    //phone_alert_status_server_context.fullConf = FALSE; //TBR
  } else {
    phone_alert_status_server_context.state = PHONE_ALERT_STATUS_SERVER_UNINITIALIZED;
  }
  
  return ret;
}

tBleStatus PASS_DeviceDiscovery(passDevDiscType param)
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
    phone_alert_status_server_context.state = PHONE_ALERT_STATUS_SERVER_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus PASS_SecuritySet(passSecurityType param)
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

tBleStatus PASS_DeviceConnection(passConnDevType param)
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
    phone_alert_status_server_context.state = PHONE_ALERT_STATUS_SERVER_START_CONNECTION;
  }

  return ret;
}

tBleStatus PASS_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(phone_alert_status_server_context.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    //phone_alert_status_server_context.fullConf = FALSE;//TBR
  }
  return ret;
}

tBleStatus PASS_ConnectionParameterUpdateRsp(uint8_t accepted, passConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(phone_alert_status_server_context.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus PASS_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(phone_alert_status_server_context.connHandle, FALSE);
  return ret;
}

tBleStatus PASS_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end PASS_Clear_Security_Database() */

tBleStatus PASS_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(phone_alert_status_server_context.connHandle, pinCode);
  
  return ret;
}

void PASS_StateMachine(void)//TBR: Is It needed?
{

  switch(phone_alert_status_server_context.state) {
   case PHONE_ALERT_STATUS_SERVER_UNINITIALIZED:
    /* Nothing to do */
    break;
  case PHONE_ALERT_STATUS_SERVER_CONNECTED_IDLE:
    {
    }
    break;
  case PHONE_ALERT_STATUS_SERVER_INITIALIZED:
    /* Nothing to do */
    break;
  case PHONE_ALERT_STATUS_SERVER_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case PHONE_ALERT_STATUS_SERVER_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case PHONE_ALERT_STATUS_SERVER_CONNECTED:
    {
      phone_alert_status_server_context.state = PHONE_ALERT_STATUS_SERVER_CONNECTED_IDLE;
    }
    break;
  }
    
}/* end PASS_StateMachine() */

void PASS_Alert_Control_Point_Handler(uint8_t alert_control_point_value)
{
  switch(alert_control_point_value)
  {
    case(ALERT_STATUS_RINGER_CONTROL_POINT_SILENT_MODE):
    {
      //PAS_DBG_MSG(profiledbgfile,"Received ALERT_STATUS_RINGER_CONTROL_POINT_SILENT_MODE \n");
      /* set the ringer setting state value to silent and related ringer setting characteristic */
      PASS_Set_RingerSetting_Value(ALERT_STATUS_RINGER_SETTING_SILENT);
      
      /* reset the phone_alert_status_server_context.mute_once_silence *///TBR---
      phone_alert_status_server_context.mute_once_silence = FALSE;
      
    }
    break;
    
    case(ALERT_STATUS_RINGER_CONTROL_POINT_MUTE_ONCE):
    {
      /* check if the ringer is active (if not active the mute once command has no effect) */
      if (phone_alert_status_server_context.ringer_setting_value)
      {
        //PAS_DBG_MSG(profiledbgfile,"Received ALERT_STATUS_RINGER_CONTROL_POINT_MUTE_ONCE \n");
        /* The command “Mute Once” shall silence the server device’s ringer. 
           The ringer state is not affected by the “Mute Once” command :
           no call to PASS_Set_RingerSetting_Value to update the related ringer setting characteristic */ //TBR---
         phone_alert_status_server_context.mute_once_silence = TRUE;
      }
    }
    break;
    
    case(ALERT_STATUS_RINGER_CONTROL_POINT_CANCEL_SILENT_MODE):
    {
      //PAS_DBG_MSG(profiledbgfile,"Received ALERT_STATUS_RINGER_CONTROL_POINT_CANCEL_SILENT_MODE \n");
      /* set the ringer setting state value to normal and related ringer setting characteristic */
      PASS_Set_RingerSetting_Value(ALERT_STATUS_RINGER_SETTING_NORMAL);
      
      /* reset the phone_alert_status_server_context.mute_once_silence */
      phone_alert_status_server_context.mute_once_silence = FALSE;
    }
    break;
    
    default:
    {
      //PAS_DBG_MSG(profiledbgfile,"Received Unsupported Ringer Control Point Command \n");
      /* reset the phone_alert_status_server_context.mute_once_silence */
      phone_alert_status_server_context.mute_once_silence = FALSE; //TBR------------- 
    }
    break;
  }
  
  /* Notify user application for implementation specific action */
  PASS_Ringer_State_CB(alert_control_point_value);
  
}/* end PASS_Alert_Control_Point_Handler() */

BOOL PASS_Get_Mute_Once_Silence_Ringer_Status(void)
{
  /* When Ringer is active, the command “Mute Once” shall silence the server device’s ringer. 
     The ringer state is not affected by the “Mute Once” command.
     This variable should allows to user application to know if there is "Mute Once: silence the device ringer"
     condition happen and to act accordingly:  actually silence real ringer */ //TBR---
  return(phone_alert_status_server_context.mute_once_silence);
} /* end PASS_Get_Mute_Once_Silence_Ringer_Status */

tBleStatus PASS_Set_AlertStatus_Value(uint8_t value)
{
  tBleStatus retval = BLE_STATUS_SUCCESS; 
        
  if ((phone_alert_status_server_context.PASServiceHandle != 0) && 
      (phone_alert_status_server_context.AlertStatusCharHandle != 0)) //TBR---
  {
    if (value != phone_alert_status_server_context.alert_status_value)
    {
      phone_alert_status_server_context.alert_status_value = value;
      retval =  aci_gatt_update_char_value(phone_alert_status_server_context.PASServiceHandle, 
                                           phone_alert_status_server_context.AlertStatusCharHandle,
                                           0, /* charValOffset */
                                           ALERT_STATUS_CHAR_LENGHT, /* charValueLen */
                                           (const uint8_t *) &(phone_alert_status_server_context.alert_status_value));    
    }
    
  }
  else
  {
    retval = BLE_STATUS_NOT_ALLOWED; 
  }
  return (retval);
}/* end PASS_Set_AlertStatus_Value() */

tBleStatus PASS_Set_RingerSetting_Value(uint8_t value)
{
  tBleStatus retval = BLE_STATUS_SUCCESS; 
  
  if ((phone_alert_status_server_context.PASServiceHandle != 0) && 
      (phone_alert_status_server_context.RingerSettingCharHandle != 0)) //TBR---
  {
    //if (value !=  phone_alert_status_server_context.ringer_setting_value)
    //{
      phone_alert_status_server_context.ringer_setting_value = value;
      retval =  aci_gatt_update_char_value(phone_alert_status_server_context.PASServiceHandle, 
                                           phone_alert_status_server_context.RingerSettingCharHandle,
                                           0, /* charValOffset */
                                           RINGER_SETTING_CHAR_LENGHT, /* charValueLen */
                                           (const uint8_t *) &(phone_alert_status_server_context.ringer_setting_value));    
    //}
  }
  else
  {
    retval = BLE_STATUS_NOT_ALLOWED; 
  }
  return (retval);
}/* end PASS_Set_RingerSetting_Value() */

tBleStatus PASS_Add_Services_Characteristics(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  uint8_t numAttribRec = 0;
  uint16_t uuid = 0;
  
  //PAS_DBG_MSG(profiledbgfile,"Main profile initialized, Adding Phone Alert Status Service\n");
  
  uuid = PHONE_ALERT_SERVICE_UUID;
  numAttribRec = 1; /* service attribute */
  
  /* Add num Attributes records for following characteristics:
  * 
  * Alert Status: 3 (we have to support the client configuration
  *                         descriptor for that characteristic as well);
  * Ringer Setting: 3 (we have to support the client configuration
  *                    descriptor for that characteristic as well);
  * Record Access Control Point (RACP): 2
  *
  */
  numAttribRec = numAttribRec +3 +3 +2; 
  
  hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                   (const uint8_t *) &uuid, 
                                   PRIMARY_SERVICE, 
                                   numAttribRec,
                                   &(phone_alert_status_server_context.PASServiceHandle));
  
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    PAS_DBG_MSG (profiledbgfile, "PASS_Add_Services_Characteristics(), Phone Alert Status Service is added Successfully %04X\n", phone_alert_status_server_context.PASServiceHandle);
  }
  else 
  {
    PAS_DBG_MSG (profiledbgfile,"PASS_Add_Services_Characteristics(),FAILED to add Phone Alert Status Service, Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }
  
  /* Add Phone Alert Status Characteristic */
  //PAS_DBG_MSG (profiledbgfile,"PAS_Add_Services_Characteristics(),Adding Alert Status Characteristic. \n");
  uuid = PHONE_ALERT_STATUS_CHARAC_UUID;
  
  hciCmdResult = aci_gatt_add_char(phone_alert_status_server_context.PASServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   ALERT_STATUS_CHAR_LENGHT, 
                                   CHAR_PROP_READ|CHAR_PROP_NOTIFY, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   ENCRIPTION_KEY_SIZE,  /* encryKeySize */ 
                                   CHAR_VALUE_LEN_CONSTANT, /* isConstant */
                                   &(phone_alert_status_server_context.AlertStatusCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    PAS_DBG_MSG (profiledbgfile, "PASS_Add_Services_Characteristics(),Alert Status Characteristic Added Successfully  %04X \n", phone_alert_status_server_context.AlertStatusCharHandle);
  }
  else 
  {
    PAS_DBG_MSG (profiledbgfile,"PASS_Add_Services_Characteristics(),FAILED to add Alert Status Characteristic,  Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }          
  
  /* Add Ringer Setting Characteristic */
  //PAS_DBG_MSG (profiledbgfile,"PASS_Add_Services_Characteristics(),Adding Ringer Setting Characteristic. \n");
  uuid = RINGER_SETTING_CHARAC_UUID;

  hciCmdResult = aci_gatt_add_char(phone_alert_status_server_context.PASServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   RINGER_SETTING_CHAR_LENGHT, 
                                   CHAR_PROP_READ|CHAR_PROP_NOTIFY, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   ENCRIPTION_KEY_SIZE,  /* encryKeySize */ 
                                   CHAR_VALUE_LEN_CONSTANT, /* isConstant */
                                   &(phone_alert_status_server_context.RingerSettingCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    PAS_DBG_MSG (profiledbgfile, "PASS_Add_Services_Characteristics(),Ringer Setting Characteristic Added Successfully  %04X \n", phone_alert_status_server_context.RingerSettingCharHandle);
  }
  else 
  {
    PAS_DBG_MSG (profiledbgfile,"PASS_Add_Services_Characteristics(),FAILED to add Ringer Setting Characteristic,  Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }          
  
  /* Add Ringer Control Point Characteristic */
  //PAS_DBG_MSG (profiledbgfile,"PASS_Add_Services_Characteristics(),Adding Ringer Control Point Characteristic. \n");
  uuid = RINGER_CNTRL_POINT_CHARAC_UUID;

  hciCmdResult = aci_gatt_add_char(phone_alert_status_server_context.PASServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   RINGER_CONTROL_POINT_CHAR_LENGHT, 
                                   CHAR_PROP_WRITE_WITHOUT_RESP, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_NOTIFY_ATTRIBUTE_WRITE, /* gattEvtMask */
                                   ENCRIPTION_KEY_SIZE,  /* encryKeySize */ 
                                   CHAR_VALUE_LEN_CONSTANT, /* isConstant */
                                   &(phone_alert_status_server_context.RingerControlPointCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    PAS_DBG_MSG (profiledbgfile, "PASS_Add_Services_Characteristics(),Ringer Control Point Characteristic Added Successfully  %04X \n", phone_alert_status_server_context.RingerControlPointCharHandle);
  }
  else 
  {
    PAS_DBG_MSG (profiledbgfile,"PASS_Add_Services_Characteristics(),FAILED to add Ringer Control Point Characteristic,  Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }    
  
  /* Update characteristics alert_status_value, ringer_setting_value with init values*/ //TBR 
  aci_gatt_update_char_value(phone_alert_status_server_context.PASServiceHandle, 
                             phone_alert_status_server_context.AlertStatusCharHandle,
                             0, /* charValOffset */
                             ALERT_STATUS_CHAR_LENGHT, /* charValueLen */
                             (const uint8_t *) &(phone_alert_status_server_context.alert_status_value));   
  
  aci_gatt_update_char_value(phone_alert_status_server_context.PASServiceHandle, 
                             phone_alert_status_server_context.RingerSettingCharHandle,
                             0, /* charValOffset */
                             RINGER_SETTING_CHAR_LENGHT, /* charValueLen */
                             (const uint8_t *) &(phone_alert_status_server_context.ringer_setting_value));  
  
  return hciCmdResult;
}/* end PASS_Add_Services_Characteristics() */

/****************** phone alert status server Weak Callbacks definition ***************************/

WEAK_FUNCTION(void PASS_Ringer_State_CB(uint8_t alert_control_point_value))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & PHONE_ALERT_STATUS_SERVER) */
