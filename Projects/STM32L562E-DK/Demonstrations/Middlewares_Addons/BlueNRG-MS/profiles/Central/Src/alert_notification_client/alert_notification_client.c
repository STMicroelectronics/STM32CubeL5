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
*   FILENAME        -  alert_notification_client.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      02/02/2015
*   $Revision$:  First version
*   $Author$:    
*   Comments:     Alert Notification Client Profile Central role 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  source code for the alert notification Client profile Central role 
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include "hci.h"
#include "hci_le.h"
#include "bluenrg_aci.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"
#include <stdio.h>

#include <host_config.h>
#include <debug.h>
#include <timer.h>
#include <uuid.h>

#if (BLE_CURRENT_PROFILE_ROLES & ALERT_NOTIFICATION_CLIENT)

#include <alert_notification_client.h>
#include <alert_notification_client_config.h>
/******************************************************************************
* Macro Declarations
******************************************************************************/
#define WEAK_FUNCTION(x) __weak x

/* stores the alerts sent by the application
 * these will be sent to server only when the
 * client enables notification by writing to the
 * control point
 */ 
uint8_t anc_new_alert_status[10]=
{
	0x00, /*CATEGORY_ID_SIMPLE_ALERT*/
	0x00, /*CATEGORY_ID_EMAIL*/
	0x00, /*CATEGORY_ID_NEWS*/
	0x00, /*CATEGORY_ID_CALL*/
	0x00, /*CATEGORY_ID_MISSED_CALL*/
	0x00, /*CATEGORY_ID_SMS_MMS*/
	0x00, /*CATEGORY_ID_VOICE_MAIL*/
	0x00, /*CATEGORY_ID_SCHEDULE*/
	0x00, /*CATEGORY_ID_HIGH_PRIORITIZED_ALERT*/
	0x00  /*CATEGORY_ID_INSTANT_MESSAGE*/
};

uint8_t anc_unread_alert_status[10]=
{
	0x00, /*CATEGORY_ID_SIMPLE_ALERT*/
	0x00, /*CATEGORY_ID_EMAIL*/
	0x00, /*CATEGORY_ID_NEWS*/
	0x00, /*CATEGORY_ID_CALL*/
	0x00, /*CATEGORY_ID_MISSED_CALL*/
	0x00, /*CATEGORY_ID_SMS_MMS*/
	0x00, /*CATEGORY_ID_VOICE_MAIL*/
	0x00, /*CATEGORY_ID_SCHEDULE*/
	0x00, /*CATEGORY_ID_HIGH_PRIORITIZED_ALERT*/
	0x00  /*CATEGORY_ID_INSTANT_MESSAGE*/
};

const uint8_t ANC_Category_ID_To_Bitmask[10]=
{
	CATEGORY_SIMPLE_ALERT,
	CATEGORY_EMAIL,
	CATEGORY_NEWS,
	CATEGORY_CALL,
	CATEGORY_MISSED_CALL,
	CATEGORY_SMS_MMS,
	CATEGORY_VOICE_MAIL,
	CATEGORY_SCHEDULE,
	CATEGORY_HIGH_PRIORITIZED_ALERT,
	CATEGORY_INSTANT_MESSAGE
};

/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;
extern initDevType initParam;

alertNotificationClientContext_Type alertNotificationClientContext; 
/******************************************************************************
 * Function Prototypes
******************************************************************************/


/*******************************************************************************
* SAP
*******************************************************************************/
tBleStatus ANC_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<alertNotificationClientContext.numpeerServices; i++) {
    if (alertNotificationClientContext.peerServices[j] == UUID_TYPE_16) {
      BLUENRG_memcpy(&uuid, &alertNotificationClientContext.peerServices[j+5], 2);
      j++;
      BLUENRG_memcpy(start_handle, &alertNotificationClientContext.peerServices[j], 2);
      j += 2;
      BLUENRG_memcpy(end_handle, &alertNotificationClientContext.peerServices[j], 2);
      j += 2;
      if (alertNotificationClientContext.peerServices[j-5] == UUID_TYPE_16) {
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


tBleStatus ANC_Init(ancInitDevType param)
{
  //initDevType initParam;
  tBleStatus ret = BLE_STATUS_FAILED;
  
  BLUENRG_memcpy(&alertNotificationClientContext, 0, sizeof(alertNotificationClientContext_Type));
  
  BLUENRG_memcpy(alertNotificationClientContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  initParam.Master_DeviceDiscovery_CB = Master_ANC_DeviceDiscovery_CB;
  initParam.Master_Connection_CB = Master_ANC_Connection_CB;
  initParam.Master_Pairing_CB = Master_ANC_Pairing_CB;
  initParam.Master_PeerDataExchange_CB = Master_ANC_PeerDataExchange_CB;
  initParam.Master_PeerDataPermitExchange_CB = Master_ANC_PeerDataPermitExchange_CB;
  initParam.Master_ServiceCharacPeerDiscovery_CB = Master_ANC_ServiceCharacPeerDiscovery_CB;

  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    alertNotificationClientContext.state = ANC_INITIALIZED;
    alertNotificationClientContext.fullConf = FALSE; //TBR
  } else {
    alertNotificationClientContext.state = ANC_UNINITIALIZED;
  }
  
  return ret;
}/* end ANC_Init() */

tBleStatus ANC_DeviceDiscovery(ancDevDiscType param)
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
    alertNotificationClientContext.state = ANC_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus ANC_SecuritySet(ancSecurityType param)
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

tBleStatus ANC_DeviceConnection(ancConnDevType param)
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
    alertNotificationClientContext.state = ANC_START_CONNECTION;
  }

  return ret;
}

tBleStatus ANC_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(alertNotificationClientContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    alertNotificationClientContext.fullConf = FALSE;//TBR
  }
  return ret;
}

tBleStatus ANC_ConnectionParameterUpdateRsp(uint8_t accepted, ancConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(alertNotificationClientContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus ANC_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(alertNotificationClientContext.connHandle, FALSE);
  return ret;
}

tBleStatus ANC_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end ANC_Clear_Security_Database() */

tBleStatus ANC_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(alertNotificationClientContext.connHandle, pinCode);
  
  return ret;
}

tBleStatus ANC_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(alertNotificationClientContext.connHandle, 
				  &alertNotificationClientContext.numpeerServices, 
				  alertNotificationClientContext.peerServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS)
  {
    alertNotificationClientContext.state = ANC_SERVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus ANC_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if (uuid_service == ALERT_NOTIFICATION_SERVICE_UUID) {
    if (!(alertNotificationClientContext.ANSPresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = ANC_findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(alertNotificationClientContext.connHandle,
				    start_handle,
				    end_handle,
				    &alertNotificationClientContext.numCharac,
				    alertNotificationClientContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS)
    {
      alertNotificationClientContext.uuid_searched = uuid_service;
    }
  }
  return ret;
}

/**
  * @brief Discovery New Alert Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Start_New_Alert_Client_Char_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

  /* It discovers the characteristic descriptors of the connected Temperature Measurement Characteristic */
  ble_status = Master_GetCharacDescriptors(alertNotificationClientContext.connHandle,
                                           alertNotificationClientContext.newAlertValueHandle,   /* New Alert chararacteristic: Value Handle */
                                           alertNotificationClientContext.newAlertValueHandle+1, /* New Alert chararacteristic: End Handle */
                                           &alertNotificationClientContext.numCharac, 
                                           alertNotificationClientContext.charac,
                                           CHARAC_OF_SERVICE_ARRAY_SIZE); 
  return(ble_status);
}

/**
  * @brief Discovery Unread Alert Status Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Start_Unread_Alert_Status_Client_Char_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

  /* It discovers the characteristic descriptors of the connected Intermediate Temperature Characteristic */
  ble_status = Master_GetCharacDescriptors(alertNotificationClientContext.connHandle,
                                           alertNotificationClientContext.unreadAlertStatusValueHandle,   /* Unread Alert Status chararacteristic: Value Handle */
                                           alertNotificationClientContext.unreadAlertStatusValueHandle+1, /* Unread Alert Status chararacteristic: End Handle */
                                           &alertNotificationClientContext.numCharac, 
                                           alertNotificationClientContext.charac,
                                           CHARAC_OF_SERVICE_ARRAY_SIZE); 

  return(ble_status);
}


/**
 * ANC_Enable_Disable_New_Alert_Notification
 * 
 * @param[in] enable : enables the notifications for the
 * new alert characteristic. After enabling this the control
 * point characteristic should also be written with the command
 * and category in order to get alerts from the peer
 * 
 * @return BLE_STATUS_SUCCESS if the input parameter is valid
 * and the command is sent else BLE_STATUS_INVALID_PARAMS
 */         
tBleStatus ANC_Enable_Disable_New_Alert_Notification(uint8_t enable)
{
  tBleStatus ble_status = ERR_COMMAND_DISALLOWED;
  
  ANC_DBG_MSG(profiledbgfile,"ANC_Enable_Disable_New_Alert_Notification \n");
  if((enable != 0) && (enable != 1))
  {
    ble_status = BLE_STATUS_INVALID_PARAMS;
  }
  else
  {  
    /* Enable New Alert Char Descriptor for Notification */
    ble_status = Master_NotifIndic_Status(alertNotificationClientContext.connHandle,
                                          alertNotificationClientContext.newAlertValueHandle + 1, /* New Alert Char Descriptor Handle */
                                          enable, /* enable-disable notification */
                                          FALSE /* no indication */);
  }
  return(ble_status); 
}/* end ANC_Enable_Disable_New_Alert_Notification() */
        
        
/**
 * ANC_Enable_Disable_Unread_Alert_Status_Notification
 * 
 * @param[in] enable : enables the notifications for the
 * unread alert status characteristic. After enabling this the control
 * point characteristic should also be written with the command
 * and category in order to get alerts from the peer
 * 
 * @return BLE_STATUS_SUCCESS if the input parameter is valid
 * and the command is sent else BLE_STATUS_INVALID_PARAMS
 */         
tBleStatus ANC_Enable_Disable_Unread_Alert_Status_Notification(uint8_t enable)        
{
  tBleStatus ble_status = ERR_COMMAND_DISALLOWED;
  
  ANC_DBG_MSG(profiledbgfile,"ANC_Enable_Disable_Unread_Alert_Status_Notification \n");
  if((enable != 0) && (enable != 1))
  {
    ble_status = BLE_STATUS_INVALID_PARAMS;
  }
  else
  {  
    /* Enable Unread Alert Status Char Descriptor for Notification */
    ble_status = Master_NotifIndic_Status(alertNotificationClientContext.connHandle,
                                          alertNotificationClientContext.unreadAlertStatusValueHandle + 1, /* Unread Alert Status Char Descriptor Handle */
                                          enable, /* enable-disable notification */
                                          FALSE /* no indication */);
  }
    return(ble_status); 
}/* end ANC_Enable_Disable_Unread_Alert_Status_Notification() */
       
        
/**
  * @brief Read Supported New Alert Category characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Read_New_Alert_Category(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Measurement Interval Characteristic */
  ble_status = Master_Read_Value(alertNotificationClientContext.connHandle,
                                 alertNotificationClientContext.newAlertCategoryValueHandle,  
                                 &alertNotificationClientContext.dataLen,  
                                 alertNotificationClientContext.data,  
                                 SUPPORTED_NEW_ALERT_CATEGORY_CHAR_SIZE);
  
  return(ble_status);  
}/* end ANC_Read_New_Alert_Category() */        
        
/**
  * @brief Read Supported Unread Alert Status Category characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Read_Unread_Alert_Status_Category(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Measurement Interval Characteristic */
  ble_status = Master_Read_Value(alertNotificationClientContext.connHandle,
                                 alertNotificationClientContext.unreadAlertCategoryValueHandle,  
                                 &alertNotificationClientContext.dataLen,  
                                 alertNotificationClientContext.data,  
                                 SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_SIZE);
  
  return(ble_status);  
}/* end ANC_Read_Unread_Alert_Status_Category() */     
    
        
/**
 * ANC_Write_Control_Point
 * 
 * @param[in] command : ID of the command to be sent
 * @param[in] category : ID of the category which has to be
 *            affected by the command. A value of 0xFF 
 *            will affect all categories 
 * 
 * Writes the control point characteristic with the command
 * and category specified. 
 * 
 * @return if the category ID specified is a category not
 * supported by the peer server or the categry ID or command ID
 * is out of range, then BLE_STATUS_INVALID_PARAMS is returned.
 * On successful write, BLE_STATUS_SUCCESS is returned
 */         
tBleStatus ANC_Write_Control_Point(tCommandID command, tCategoryID category)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS; 
  uint8_t idx = 0;
  uint8_t charVal[2];
  uint8_t ANC_Category_ID_To_Bitmask[10]=
    {
      CATEGORY_SIMPLE_ALERT,
      CATEGORY_EMAIL,
      CATEGORY_NEWS,
      CATEGORY_CALL,
      CATEGORY_MISSED_CALL,
      CATEGORY_SMS_MMS,
      CATEGORY_VOICE_MAIL,
      CATEGORY_SCHEDULE,
      CATEGORY_HIGH_PRIORITIZED_ALERT,
      CATEGORY_INSTANT_MESSAGE
    };
  
  if (alertNotificationClientContext.ctrlPointCharValueHandle != 0) 
  {
    ANC_DBG_MSG(profiledbgfile,"ANC_Write_Control_Point\n");
    if((command > NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY) ||
       ((category != 0xFF) && (category > CATEGORY_ID_INSTANT_MESSAGE)))
    {
      return BLE_STATUS_INVALID_PARAMS;
    }
		
    /* if the category is 0xFF, then we need not
     * check for the categories supported by the peer
     */ 
    if(category != 0xFF)
    {
      if(category > CATEGORY_ID_SCHEDULE)
      {
	idx = 1;
      }			
      if((command == ENABLE_NEW_ALERT_NOTIFICATION) ||
	 (command == DISABLE_NEW_ALERT_NOTIFICATION) ||
	 (command == NOTIFY_NEW_ALERT_IMMEDIATELY))
      {
	if(!(alertNotificationClientContext.alertCategory[idx] & ANC_Category_ID_To_Bitmask[category]))
	{
	  return BLE_STATUS_INVALID_PARAMS;
	}
      }
      else
      {
	if(!(alertNotificationClientContext.unreadAlertCategory[idx] & ANC_Category_ID_To_Bitmask[category]))
	{
	  return BLE_STATUS_INVALID_PARAMS;
	}
      }
    }
    charVal[0] = command;
    charVal[1] = category;
    
    /* Write Alert Notification Control Point Characteristic */
    ble_status =  Master_Write_Value(alertNotificationClientContext.connHandle,  /* connection handle */
                                     alertNotificationClientContext.ctrlPointCharValueHandle, /* attribute value handle */
                                     2, /* charValueLen */
                                     charVal);
    
    if (ble_status == BLE_STATUS_SUCCESS) 
      alertNotificationClientContext.state = ANC_WAIT_WRITE_CONTROL_POINT; 
    else
      alertNotificationClientContext.state = ANC_CONNECTED_IDLE;
    
    return(ble_status);
  }
  
  return ERR_COMMAND_DISALLOWED; 
}        
        
        
/* This function start the connection and the configuration of the device */
uint8_t ANC_ConnConf(ancConnDevType connParam)
{
  uint8_t ret;

  ret = ANC_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    alertNotificationClientContext.fullConf = TRUE;
  }

  return ret;
}        
        
/**
 * ANC_StateMachine
 * 
 * @param[in] None
 * 
 * ANS profile's state machine: to be called on application main loop. 
 */ 
void ANC_StateMachine(void)
{
  uint8_t ret;

  switch(alertNotificationClientContext.state) {
  case ANC_UNINITIALIZED:
    /* Nothing to do */
    break;
  case ANC_CONNECTED_IDLE:
    /* Nothing to do */
    break;
  case ANC_INITIALIZED:
    /* Nothing to do */
    break;
  case ANC_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case ANC_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case ANC_CONNECTED:
    {
      ret = ANC_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.state = ANC_SERVICE_DISCOVERY;
      } else {
	ANC_FullConfError_CB(ANC_ERROR_IN_SERVICE_DISCOVERY, ret);
	alertNotificationClientContext.state = ANC_DISCONNECTION;
      }
    }
    break;
  case ANC_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case ANC_CHARAC_DISCOVERY:
    {
      ret = ANC_DiscCharacServ(ALERT_NOTIFICATION_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.state = ANC_WAIT_CHARAC_DISCOVERY;
      } else {
	ANC_FullConfError_CB(ANC_ERROR_IN_CHARAC_DISCOVERY, ret);
	alertNotificationClientContext.state = ANC_DISCONNECTION;
      }
    }
    break;
  case ANC_WAIT_CHARAC_DISCOVERY:
    /* Waiting the end of the Alert Notification Characteristics discovery */ 
    break;
       
  case ANC_ENABLE_NEW_ALERT_NOTIFICATION:                 
   {
      ret = ANC_Enable_Disable_New_Alert_Notification(1);
      if (ret == BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.state = ANC_WAIT_ENABLE_NEW_ALERT_NOTIFICATION;
      } else {
	ANC_FullConfError_CB(ANC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	alertNotificationClientContext.state = ANC_DISCONNECTION;
      }
    }
    break; 
  case ANC_WAIT_ENABLE_NEW_ALERT_NOTIFICATION:             
     break;
     
  case ANC_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION:       
    {
      ret = ANC_Enable_Disable_Unread_Alert_Status_Notification(1);
      if (ret == BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.state = ANC_WAIT_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION;
      } else {
	ANC_FullConfError_CB(ANC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	alertNotificationClientContext.state = ANC_DISCONNECTION;
      }
    }
    break; 
  case ANC_WAIT_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION:       
     break;
       
  case ANC_READ_NEW_ALERT_CATEGORY:
    {
      ret = ANC_Read_New_Alert_Category();
      if (ret == BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.state = ANC_WAIT_READ_NEW_ALERT_CATEGORY;
      } else {
	ANC_FullConfError_CB(ANC_ERROR_IN_READ, ret);
	alertNotificationClientContext.state = ANC_DISCONNECTION;
      }
    }
    break;
  case ANC_WAIT_READ_NEW_ALERT_CATEGORY:
    /* Waiting the end of the read  procedure */
    break;
    
  case ANC_READ_UNREAD_ALERT_STATUS_CATEGORY:
    {
      ret = ANC_Read_Unread_Alert_Status_Category();
      if (ret == BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.state = ANC_WAIT_READ_UNREAD_ALERT_STATUS_CATEGORY;
      } else {
	ANC_FullConfError_CB(ANC_ERROR_IN_READ, ret);
	alertNotificationClientContext.state = ANC_DISCONNECTION;
      }
    }
    break;
  case ANC_WAIT_READ_UNREAD_ALERT_STATUS_CATEGORY:
    /* Waiting the end of the read  procedure */
    break; 
    
  case ANC_DISCONNECTION:
    {
      ret = ANC_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	ANC_FullConfError_CB(ANC_ERROR_IN_DISCONNECTION, ret);
      }
      alertNotificationClientContext.state = ANC_INITIALIZED;
    }
    break;
  }
  
}/* end ANC_StateMachine() */

        
/**
 * @brief  It checks the write response status and error related to the 
 *         Alert Notification Control Point (ANCP) write request to reset energy expended      
 *         It should be called within the Profile event handler, on EVT_BLUE_GATT_ERROR_RESP, 
 *         EVT_BLUE_GATT_PROCEDURE_COMPLETE (TBR?)  
 * @param  err_code:  HRCP write response error code (from Heart Rate Sensor) 
 */
void ANC_CP_Check_Write_Response_Handler(uint8_t err_code)
{
  ANC_INFO_MSG(profiledbgfile,"ANC_CP_Check_Write_Response_Handler().\n");
  
  if (err_code == 0)
  {
    ANC_INFO_MSG(profiledbgfile, "Alert Notification Control Point Value Write procedure completed with success!\n");
  }
  else if (err_code == COMMAND_NOT_SUPPORTED) 
  {
    ANC_INFO_MSG(profiledbgfile,"Command not supported!\n");
  } 
  else 
  {
    ANC_INFO_MSG(profiledbgfile, "Error during the Write Characteristic Value Procedure!\n");
  } 
  
  ANC_CP_Write_Response_CB(err_code);
}/* end ANC_CP_Check_Write_Response_Handler() */        

        
/**
  * @brief  Store & analyse the Characteristic Value for Supported New Alert Category Char
  * @param [in] None
  * @retval Error Code
  */
void ANC_Read_New_Alert_Category_CB(void)
{
  alertNotificationClientContext.alertCategory[0] = alertNotificationClientContext.data[0];
  if(alertNotificationClientContext.dataLen == 2)
  {
    alertNotificationClientContext.alertCategory[1] = alertNotificationClientContext.data[1];
  }
  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
  ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category BitMask[0] 0x%02x\n", alertNotificationClientContext.alertCategory[0]);
  ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category BitMask[1] 0x%02x\n", alertNotificationClientContext.alertCategory[1]);
  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
}        
        
/**
  * @brief  Store & analyse the Characteristic Value for Supported Unread Alert Status Category Char
  * @param [in] None
  * @retval Error Code
  */
void ANC_Read_Unread_Alert_Status_Category_CB(void)        
{
  alertNotificationClientContext.unreadAlertCategory[0] = alertNotificationClientContext.data[0];
  if(alertNotificationClientContext.dataLen == 2)
  {
    alertNotificationClientContext.unreadAlertCategory[1] = alertNotificationClientContext.data[1];
  }
  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
  ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Status Category BitMask[0] 0x%02x\n", alertNotificationClientContext.unreadAlertCategory[0]);
  ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Status Category BitMask[1] 0x%02x\n", alertNotificationClientContext.unreadAlertCategory[1]);
  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
}         
        

/**
* @brief  It stores the New Alert Characteristics Notification
* @param  attr_handle: New Alert characteristic handle
* @param  data_lenght: New Alert characteristic value lenght
* @param  value: New Alert characteristic value 
*/
void ANC_New_Alert_Notification_CB(uint16_t attr_handle, uint8_t data_length, uint8_t * value)
{
  uint8_t i;
  
  /* Check indication value handle: New Alert characteristic notification */ 
  if (attr_handle == alertNotificationClientContext.newAlertValueHandle)
  {  
     /* notify the application only if the alert for the category
     * is different from the previous notification data
     */ 
    if((value[0] < CATEGORY_ID_INSTANT_MESSAGE) && (anc_new_alert_status[value[0]] != value[1]))
    {
      anc_new_alert_status[value[0]] = value[1];
    
      /* the event buffer will have the data as follows:
       * category ID - 1 byte
       * number of alerts for that category - 1 byte
       * text string information - optional(varying number of bytes : the rest
       * of the available bytes as indicated by length has to be considered
       * as the string
       */
      ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
      ANC_DBG_MSG(profiledbgfile,"New Alert Char: CategoryId       0x%02x\n", value[0]);
      ANC_DBG_MSG(profiledbgfile,"New Alert Char: Number New Alert 0x%02x\n", value[1]);
      ANC_DBG_MSG(profiledbgfile,"New Alert Char: Text             ");
      
      alertNotificationClientContext.newAlert.categoryID = value[0];
      alertNotificationClientContext.newAlert.numNewAlert = value[1];
      alertNotificationClientContext.newAlert.textStringInfo.len = data_length-2; 
      
      uint8_t j=0;
      //for (i=2; i<data_length-2; i++)
      for (i=data_length-3; i>=2; i--)  //little endian
      {  
        ANC_DBG_MSG(profiledbgfile,"%c", value[i]);
        alertNotificationClientContext.newAlert.textStringInfo.str[j] = value[i];
        j++;
      }
      ANC_DBG_MSG(profiledbgfile,"\n***************************************************************\n");
    }                    
  }
}

/**
* @brief  It stores the Unread Alert Status Characteristics Notification
* @param  attr_handle: Unread Alert Status characteristic handle
* @param  data_lenght: Unread Alert Status characteristic value lenght
* @param  value: Unread Alert Status characteristic value 
*/
void ANC_Unread_Alert_Status_Notification_CB(uint16_t attr_handle, uint8_t data_length, uint8_t * value)
{
  /* Check indication value handle: New Alert characteristic notification */ 
  if (attr_handle == alertNotificationClientContext.unreadAlertStatusValueHandle)
  {          
    /* the event buffer will have the data as follows:
     * category ID - 1 byte
     * number of unread alerts for that category - 1 byte
     */ 
    if((value[0] < CATEGORY_ID_INSTANT_MESSAGE) && (anc_unread_alert_status[value[0]] != value[1]))
    {
      anc_unread_alert_status[value[0]] = value[1];
      /* the event buffer will have the data as follows:
       * category ID - 1 byte
       * number of unread alerts for that category - 1 byte 
       */
      ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
      ANC_DBG_MSG(profiledbgfile,"Unread Alert Status Char: CategoryId           0x%02x\n", value[0]);
      ANC_DBG_MSG(profiledbgfile,"Unread Alert Status Char: Number Unread Alert  0x%02x\n", value[1]);
      ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
      
      alertNotificationClientContext.unreadAlertStatus.categoryID = value[0];
      alertNotificationClientContext.unreadAlertStatus.unreadCount = value[1];
    }
  }
}
        
        
WEAK_FUNCTION(void ANC_CP_Write_Response_CB(uint8_t err_code))
{
}

WEAK_FUNCTION(void ANC_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & ALERT_NOTIFICATION_CLIENT) */
