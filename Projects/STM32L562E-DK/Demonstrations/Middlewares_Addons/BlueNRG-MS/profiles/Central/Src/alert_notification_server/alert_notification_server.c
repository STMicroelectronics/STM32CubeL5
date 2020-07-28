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
*   FILENAME        -  alert_notification_server.c
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
*   Comments:     Alert Notification Server Profile Central role 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  source code for the alert notification Server profile Central role 
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include "hci.h"
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

#if (BLE_CURRENT_PROFILE_ROLES & ALERT_NOTIFICATION_SERVER)

#include <alert_notification_server.h>
#include <alert_notification_server_config.h>
/******************************************************************************
* Macro Declarations
******************************************************************************/

/* stores the alerts sent by the application
 * these will be sent to server only when the
 * client enables notification by writing to the
 * control point
 */ 
uint8_t ans_new_alert_status[10]=
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

uint8_t ans_unread_alert_status[10]=
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

const uint8_t ANS_Category_ID_To_Bitmask[10]=
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

alertNotificationServerContext_Type alertNotificationServerContext; 
/******************************************************************************
 * Function Prototypes
******************************************************************************/

static void EnDis_All_Supp_Categories(uint8_t* category,uint8_t* notify,uint8_t enable);
static tBleStatus Update_Alert(void);
/*******************************************************************************
* SAP
*******************************************************************************/

tBleStatus ANS_Init(ansInitDevType param)
{
  initDevType initParam;
  tBleStatus ret = BLE_STATUS_FAILED;
  
  if(((!param.alertCategory[0]) && (!param.alertCategory[1])) || 
     ((!param.unreadAlertCategory[0]) && (!param.unreadAlertCategory[1])) ||
     (param.alertCategory[1] == 0xFF) || (param.unreadAlertCategory[1] == 0xFF))
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
	
  BLUENRG_memcpy(&alertNotificationServerContext, 0, sizeof(alertNotificationServerContext_Type));
  
  BLUENRG_memcpy(alertNotificationServerContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  
  alertNotificationServerContext.unreadAlertCategory[0] = param.unreadAlertCategory[0];
  alertNotificationServerContext.unreadAlertCategory[1] = param.unreadAlertCategory[1];
  alertNotificationServerContext.alertCategory[0] = param.alertCategory[0];
  alertNotificationServerContext.alertCategory[1] = param.alertCategory[1];
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    alertNotificationServerContext.state = ANS_INITIALIZED;
    //alertNotificationServerContext.fullConf = FALSE; //TBR
  } else {
    alertNotificationServerContext.state = ANS_UNINITIALIZED;
  }
  
  return ret;
}/* end ANS_Init() */

tBleStatus ANS_DeviceDiscovery(ansDevDiscType param)
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
    alertNotificationServerContext.state = ANS_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus ANS_SecuritySet(ansSecurityType param)
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

tBleStatus ANS_DeviceConnection(ansConnDevType param)
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
    alertNotificationServerContext.state = ANS_START_CONNECTION;
  }

  return ret;
}

tBleStatus ANS_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(alertNotificationServerContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    //alertNotificationServerContext.fullConf = FALSE;//TBR
  }
  return ret;
}

tBleStatus ANS_ConnectionParameterUpdateRsp(uint8_t accepted, ansConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(alertNotificationServerContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus ANS_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(alertNotificationServerContext.connHandle, FALSE);
  return ret;
}

tBleStatus ANS_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end ANS_Clear_Security_Database() */

tBleStatus ANS_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(alertNotificationServerContext.connHandle, pinCode);
  
  return ret;
}

/**
 * ANS_StateMachine
 * 
 * @param[in] None
 * 
 * ANS profile's state machine: to be called on application main loop. 
 */ 
void ANS_StateMachine(void)
{ 
  switch(alertNotificationServerContext.state) //TBR
  {
    case ANS_NOTIFY_NEW_ALERT_IMMEDIATELY:
    case ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY: 
    {
      ANS_DBG_MSG(profiledbgfile,"Call Update_Alert()\n");
      Update_Alert();
    }
    break;
  }
}/* end ANS_StateMachine() */

/**
 * ANS_Add_Services_Characteristics
 * 
 * @param[in]: None
 *            
 * It adds Alert Notiifcation Server service & related characteristics
 */ 
tBleStatus ANS_Add_Services_Characteristics(void)
{
  tBleStatus status = BLE_STATUS_FAILED;
  //uint8_t numAttribRec = 0;
  uint16_t uuid = 0;
  
  uuid = ALERT_NOTIFICATION_SERVICE_UUID;
  
  status = aci_gatt_add_serv(UUID_TYPE_16,
                             (uint8_t *)&uuid,
                             PRIMARY_SERVICE,
                             13,
                             &alertNotificationServerContext.alertServiceHandle);
  if (status == BLE_STATUS_SUCCESS)
  {
    ANS_DBG_MSG (profiledbgfile, "ANS_Add_Services_Characteristics(), Alert Notification Service is added Successfully %04X\n", alertNotificationServerContext.alertServiceHandle);
  }
  else 
  {
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to add Alert Notification Service, Error: %02X !!\n", status);
    return status;
  }
  
  uuid = SUPPORTED_NEW_ALERT_CATEGORY_CHAR_UUID;
  status = aci_gatt_add_char(alertNotificationServerContext.alertServiceHandle,
                             UUID_TYPE_16,
                             (uint8_t *)&uuid ,
                             SUPPORTED_NEW_ALERT_CATEGORY_CHAR_SIZE, 
                             CHAR_PROP_READ,
                             ATTR_PERMISSION_NONE,
                             GATT_DONT_NOTIFY_EVENTS,
                             MIN_ENCRY_KEY_SIZE,
                             CHAR_VALUE_LEN_VARIABLE,
                             &alertNotificationServerContext.newAlertCategoryHandle);
  if (status == BLE_STATUS_SUCCESS)
  {
    ANS_DBG_MSG (profiledbgfile, "ANS_Add_Services_Characteristics(), New Alert Category Char is added Successfully %04X\n", alertNotificationServerContext.newAlertCategoryHandle);
  }
  else 
  {
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to add  New Alert Category Char, Error: %02X !!\n", status);
    return status;
  }
    
  uuid = NEW_ALERT_CHAR_UUID;
  status = aci_gatt_add_char(alertNotificationServerContext.alertServiceHandle,
                             UUID_TYPE_16,
                             (uint8_t *)&uuid ,
                             NEW_ALERT_CHAR_SIZE, //20
                             CHAR_PROP_NOTIFY,
                             ATTR_PERMISSION_NONE,
                             GATT_DONT_NOTIFY_EVENTS,
                             MIN_ENCRY_KEY_SIZE,
                             CHAR_VALUE_LEN_VARIABLE,
                             &alertNotificationServerContext.newAlertHandle);
  if (status == BLE_STATUS_SUCCESS)
  {
    ANS_DBG_MSG (profiledbgfile, "ANS_Add_Services_Characteristics(), New Alert Char is added Successfully %04X\n", alertNotificationServerContext.newAlertHandle);
  }
  else 
  {
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to add New Alert Char, Error: %02X !!\n", status);
    return status;
  }
  uuid = SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_UUID;
  status = aci_gatt_add_char(alertNotificationServerContext.alertServiceHandle,
                             UUID_TYPE_16,
                             (uint8_t *)&uuid ,
                             SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_SIZE, 
                             CHAR_PROP_READ,
                             ATTR_PERMISSION_NONE,
                             GATT_DONT_NOTIFY_EVENTS,
                             MIN_ENCRY_KEY_SIZE,
                             CHAR_VALUE_LEN_VARIABLE,
                             &alertNotificationServerContext.UnreadAlertCategoryHandle);
  if (status == BLE_STATUS_SUCCESS)
  {
    ANS_DBG_MSG (profiledbgfile, "ANS_Add_Services_Characteristics(), Unread Alert Category Char is added Successfully %04X\n", alertNotificationServerContext.UnreadAlertCategoryHandle);
  }
  else 
  {
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to add Unread Alert Category Char, Error: %02X !!\n", status);
    return status;
  }
  uuid = UNREAD_ALERT_STATUS_CHAR_UUID;
  status = aci_gatt_add_char(alertNotificationServerContext.alertServiceHandle,
                             UUID_TYPE_16,
                             (uint8_t *)&uuid ,
                             UNREAD_ALERT_STATUS_CHAR_SIZE, //2,
                             CHAR_PROP_NOTIFY,
                             ATTR_PERMISSION_NONE,
                             GATT_DONT_NOTIFY_EVENTS,
                             MIN_ENCRY_KEY_SIZE,
                             CHAR_VALUE_LEN_CONSTANT,
                             &alertNotificationServerContext.unreadAlertStatusHandle);
  if (status == BLE_STATUS_SUCCESS)
  {
    ANS_DBG_MSG (profiledbgfile, "ANS_Add_Services_Characteristics(), Unread Alert Status Char is added Successfully %04X\n", alertNotificationServerContext.unreadAlertStatusHandle);
  }
  else 
  {
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to add Unread Alert Status Char, Error: %02X !!\n", status);
    return status;
  }
  uuid = ALERT_NOTIFICATION_CONTROL_POINT_CHAR_UUID;
  status = aci_gatt_add_char(alertNotificationServerContext.alertServiceHandle,
                             UUID_TYPE_16,
                             (uint8_t *)&uuid ,
                             ALERT_NOTIFICATION_CONTROL_POINT_CHAR_SIZE, //2,
                             CHAR_PROP_WRITE,
                             ATTR_PERMISSION_NONE,
                             GATT_NOTIFY_ATTRIBUTE_WRITE|GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                             MIN_ENCRY_KEY_SIZE,
                             CHAR_VALUE_LEN_CONSTANT,
                             &alertNotificationServerContext.ctrlPointCharHandle);
  if (status == BLE_STATUS_SUCCESS)
  {
    ANS_DBG_MSG (profiledbgfile, "ANS_Add_Services_Characteristics(), Control Point Char is added Successfully %04X\n", alertNotificationServerContext.ctrlPointCharHandle);
  }
  else 
  {
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to add Control Point Char, Error: %02X !!\n", status);
    return status;
  }
  
  status = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
                                      alertNotificationServerContext.newAlertCategoryHandle,
                                      0,
                                      SUPPORTED_NEW_ALERT_CATEGORY_CHAR_SIZE, //2,
                                      alertNotificationServerContext.alertCategory);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* initialization error inform the application and exit */
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to update new Alert Category Char: 0x%02X !!\n", status);
    return status;
  }
   
  status = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
                                      alertNotificationServerContext.UnreadAlertCategoryHandle,
                                      0,
                                      SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_SIZE,//2,
                                      alertNotificationServerContext.unreadAlertCategory);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* initialization error inform the application and exit */
    ANS_DBG_MSG (profiledbgfile,"ANS_Add_Services_Characteristics(),FAILED to update Unread Alert Category Char: 0x%02X !!\n", status);
    return status;
  }

  return status;
}/* end ANS_Add_Services_Characteristics() */

/**
 * ANS_Update_New_Alert_Category
 * 
 * @param[in] len : length of the category field. It has to be 0 or 1.
 * @param[in] category : bitmask of the categories supported. The bitmasks
 * are split across 2 octets and have the meaning as described in the
 * bluetooth assigned numbers documentation
 * 
 * updates the alert category characteristic with the new bitmask.
 * The supported categories have to be constant when in connection
 * 
 * @return BLE_STATUS_SUCCESS if the update is successfully started
 * BLE_STATUS_INVALID_PARAMS if bitmask for non existent category is set
 */ 
tBleStatus ANS_Update_New_Alert_Category(uint8_t len,uint8_t* category)
{
  tBleStatus retval = ERR_COMMAND_DISALLOWED;
	
  /* the value of this characteristic should not be changed
   * while in connection. So donot allow an update during connection
   */ 
  if((alertNotificationServerContext.state != ANS_CONNECTED) &&
     (alertNotificationServerContext.state >= ANS_INITIALIZED))
  {
    if(((len != 1) || (len != 2)) ||
       ((len == 2) && (category[0] == 0) && 
	(!(category[1] & (CATEGORY_HIGH_PRIORITIZED_ALERT|CATEGORY_INSTANT_MESSAGE)))) ||
       ((len == 1) && (category[0] == 0)))
    {
      retval = BLE_STATUS_INVALID_PARAMS;
    }

    retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
					alertNotificationServerContext.newAlertCategoryHandle,
					0,
					len,
					category);
    if(retval == BLE_STATUS_SUCCESS)
    {
      alertNotificationServerContext.alertCategory[0] = 0x00;
      alertNotificationServerContext.alertCategory[1] = 0x00;
      BLUENRG_memcpy(alertNotificationServerContext.alertCategory,category,len);
      ANS_DBG_MSG(profiledbgfile,"Updated new alert category\n");
    }
    else
    {
      ANS_DBG_MSG (profiledbgfile,"ANS_Update_New_Alert_Category(),FAILED to update new Alert Category Char: 0x%02X !!\n", retval);
    }
  }
	
  return retval;
}/* end ANS_Update_New_Alert_Category() */

/**
 * ANS_Update_Unread_Alert_Category
 * 
 * @param[in] len : length of the category field. It has to be 0 or 1.
 * @param[in] category : bitmask of the categories supported. The bitmasks
 * are split across 2 octets and hav the meaning as described in the
 * bluetooth assigned numbers documentation
 * 
 * updates the unread alert status category characteristic with the new bitmask.
 * The supported categories have to be constant when in connection
 * 
 * @return BLE_STATUS_SUCCESS if the update is successfully started
 * BLE_STATUS_INVALID_PARAMS if bitmask for non existent category is set
 */ 
tBleStatus ANS_Update_Unread_Alert_Category(uint8_t len,uint8_t* category)
{
  tBleStatus retval = ERR_COMMAND_DISALLOWED;
	
  /* the value of this characteristic should not be changed
   * while in connection. So donot allow an update during connection
   */ 
  if((alertNotificationServerContext.state != ANS_CONNECTED) &&
     (alertNotificationServerContext.state >= ANS_INITIALIZED))
  {
    if(((len != 1) || (len != 2)) ||
       ((len == 2) && (category[0] == 0) && 
	(!(category[1] & (CATEGORY_HIGH_PRIORITIZED_ALERT|CATEGORY_INSTANT_MESSAGE)))) ||
       ((len == 1) && (category[0] == 0)))
    {
      retval = BLE_STATUS_INVALID_PARAMS;
    }

    retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
					alertNotificationServerContext.UnreadAlertCategoryHandle,
					0,
					len,
					category);
    if(retval == BLE_STATUS_SUCCESS)
    {
      alertNotificationServerContext.unreadAlertCategory[0] = 0x00;
      alertNotificationServerContext.unreadAlertCategory[1] = 0x00;
      BLUENRG_memcpy(alertNotificationServerContext.unreadAlertCategory,category,len);
      ANS_DBG_MSG(profiledbgfile,"Updated unread alert category\n");
    }
    else
    {
      ANS_DBG_MSG (profiledbgfile,"ANS_Update_Unread_Alert_Category(),FAILED to update Unread Alert Category Char: 0x%02X !!\n", retval);
    }
  }
  return retval;
}/* end ANS_Update_Unread_Alert_Category() */

/**
 * ANS_Update_New_Alert
 * 
 * @param[in] categoryID : ID of the category for which 
 *            the alert status has to be updated. 
 * @param[in] alertCount : alert count for the category specified.
 * The application has to maintain the count of new alerts. When
 * this function is called, the previous value of the alert will
 * be overwritten. 
 * @param[in] textInfo : Textual information corresponding to the alert.
 * For Example, in case of missed call category, the textual information
 * could be the name of the caller. If the alert count is more than 1, then
 * the textual information should correspond to the newest alert
 * 
 * updates the number of new alerts for the category specified
 * in the new alert characteristic
 * 
 * @return if the category ID specified is not valid, or the text
 * information is longer than18 octets, then BLE_STATUS_INVALID_PARAMS 
 * is returned. On successful write, BLE_STATUS_SUCCESS is returned
 */ 
tBleStatus ANS_Update_New_Alert(tCategoryID categoryID,uint8_t alertCount,tTextInfo textInfo)
{
  uint8_t charVal[ALERT_CHAR_MAX_TEXT_FIELD_LEN+2];
  tBleStatus retval;

  if((categoryID > CATEGORY_ID_INSTANT_MESSAGE) ||
     (textInfo.len > ALERT_CHAR_MAX_TEXT_FIELD_LEN))
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
	
  ANS_DBG_MSG(profiledbgfile,"ANS_Update_New_Alert\n");
  /* The application calling this API has to maintain the count of new alerts.
     When this function is called, the previous value of the alert will
     be overwritten. 
  */
  ans_new_alert_status[categoryID] = alertCount;
	
  /* if the notification for that particular category
   * is enabled, then update
   */ 
  if(alertNotificationServerContext.notifyNewAlert[categoryID])
  {
    ANS_DBG_MSG(profiledbgfile,"New alert notify\n");
    charVal[0] = categoryID;
    charVal[1] = alertCount;
    BLUENRG_memcpy(&charVal[2],textInfo.str,textInfo.len);
    retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
					alertNotificationServerContext.newAlertHandle,
					0,
					textInfo.len+2,
					charVal);
    if (retval != BLE_STATUS_SUCCESS)
    {
      ANS_DBG_MSG (profiledbgfile,"ANS_Update_New_Alert(),FAILED to update new Alert Char: 0x%02X !!\n", retval);
    }
  }
	
  return retval;
}/* end ANS_Update_New_Alert() */

/**
 * ANS_Update_Unread_Alert_Status
 * 
 * @param[in] categoryID : ID of the category for which 
 *            the alert status has to be updated. 
 * @param[in] alertCount : alert count for the category specified.
 * The application has to maintain the count of unread alerts. When
 * this function is called, the previous value of the alert will
 * be overwritten. 
 * 
 * updates the number of unread alerts for the category specified
 * in the unread alert status characteristic
 * 
 * @return if the category ID specified is not valid,
 * then BLE_STATUS_INVALID_PARAMS is returned.
 * On successful write, BLE_STATUS_SUCCESS is returned
 */ 
tBleStatus ANS_Update_Unread_Alert_Status(tCategoryID categoryID,uint8_t alertCount)
{
  uint8_t charVal[2];
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  ANS_DBG_MSG(profiledbgfile,"ANS_Update_Unread_Alert_Status\n");
  if(categoryID > CATEGORY_ID_INSTANT_MESSAGE)
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
  /* The application calling this API has to maintain the count of unread alerts.
     When this function is called, the previous value of the alert will
     be overwritten. 
  */	
  ans_unread_alert_status[categoryID] = alertCount;
	
  /* if the notification for that particular category
   * is enabled, then update
   */ 
  if(alertNotificationServerContext.notifyUnreadAlert[categoryID])
  {
    charVal[0] = categoryID;
    charVal[1] = alertCount;
    ANS_DBG_MSG(profiledbgfile,"Unread alert status notify\n");
    retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
					alertNotificationServerContext.unreadAlertStatusHandle,
					0,
					2,
					charVal);
    if (retval != BLE_STATUS_SUCCESS)
    {
      ANS_DBG_MSG (profiledbgfile,"ANS_Update_Unread_Alert_Status(),FAILED to update unread Alert Status Handle Char: 0x%02X !!\n", retval);
    }
   }
	
  return retval;
}/* end ANS_Update_Unread_Alert_Status() */


/**
* ANC_Handle_ControlPoint_Write: It write the alert control point
 * 
 * @param[in] *attVal : pointer to alert control point value 
 *
 * @return:  status
 */ 
tBleStatus ANC_Handle_ControlPoint_Write(uint8_t *attVal)
{
  uint8_t idx;
  tBleStatus retval = BLE_STATUS_SUCCESS;
	
  ANS_DBG_MSG(profiledbgfile,"ANC_Handle_ControlPoint_Write(): 0x%02x 0x%02x\n",attVal[0],attVal[1]); 
	
  if((attVal[1] != 0xFF) && (attVal[1] > CATEGORY_ID_INSTANT_MESSAGE))
  {
    /* invalid category */
    ANS_DBG_MSG(profiledbgfile,"Invalid category\n");
    return BLE_STATUS_INVALID_PARAMS;
  }
	
  switch(attVal[0])
  {
  case ENABLE_NEW_ALERT_NOTIFICATION:
    {
      if(attVal[1] == 0xFF)
      {
        ANS_INFO_MSG(profiledbgfile,"ENABLE_NEW_ALERT_NOTIFICATION 0xFF\n");
	/* enable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServerContext.alertCategory,
				  alertNotificationServerContext.notifyNewAlert,
				  0x01);
      }
      else if(alertNotificationServerContext.alertCategory[attVal[1]/8] & ANS_Category_ID_To_Bitmask[attVal[1]])
      {
        ANS_INFO_MSG(profiledbgfile,"ENABLE_NEW_ALERT_NOTIFICATION, 0x%02x\n",attVal[1]);
	/* if the enabling of notification is for a category supported,
	 * then enable the notifications
	 */ 
	alertNotificationServerContext.notifyNewAlert[attVal[1]] = 0x01;
	//ANS_DBG_MSG(profiledbgfile,"Set\n");
      }
    }
    break;
  case ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION:
    {
      if(attVal[1] == 0xFF)
      {
        ANS_INFO_MSG(profiledbgfile,"ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION 0xFF\n");
	/* enable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServerContext.unreadAlertCategory,
				  alertNotificationServerContext.notifyUnreadAlert,
				  0x01);
      }
      else if(alertNotificationServerContext.unreadAlertCategory[attVal[1]/8] & ANS_Category_ID_To_Bitmask[attVal[1]])
      {
	/* if the enabling of notification is for a category supported,
	 * then enable the notifications
	 */ 
        ANS_INFO_MSG(profiledbgfile,"ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION 0x%02x\n",attVal[1]);
	alertNotificationServerContext.notifyUnreadAlert[attVal[1]] = 0x01;
	ANS_DBG_MSG(profiledbgfile,"Set\n");
      }
    }
    break;
  case DISABLE_NEW_ALERT_NOTIFICATION:
    {
      if(attVal[1] == 0xFF)
      {
        ANS_INFO_MSG(profiledbgfile,"DISABLE_NEW_ALERT_NOTIFICATION for ALL supported Categories (0xFF)");
	/* disable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServerContext.alertCategory,
				  alertNotificationServerContext.notifyNewAlert,
				  0x00);
      }
      else if(alertNotificationServerContext.alertCategory[attVal[1]/8] & ANS_Category_ID_To_Bitmask[attVal[1]])
      {
	/* if the disabling of notification is for a category supported,
	 * then disable the notifications
	 */ 
        ANS_INFO_MSG(profiledbgfile,"DISABLE_NEW_ALERT_NOTIFICATION 0x%02x\n",attVal[1])
	alertNotificationServerContext.notifyNewAlert[attVal[1]] = 0x00;
      }
    }
    break;
  case DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION:
    {
      if(attVal[1] == 0xFF)
      {
        ANS_INFO_MSG(profiledbgfile,"DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION for ALL supported Catagories (0xFF)");
	/* disable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServerContext.unreadAlertCategory,
				  alertNotificationServerContext.notifyUnreadAlert,
				  0x00);
      }
      else if(alertNotificationServerContext.unreadAlertCategory[attVal[1]/8] & ANS_Category_ID_To_Bitmask[attVal[1]])
      {
        ANS_INFO_MSG(profiledbgfile,"DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION for Supported Category 0x%02x\n",attVal[1]/8);
	/* if the disabling of notification is for a category supported,
	 * then disable the notifications
	 */ 
	alertNotificationServerContext.notifyUnreadAlert[attVal[1]] = 0x00;
      }
    }
    break;
  case NOTIFY_NEW_ALERT_IMMEDIATELY:
    {
      if(attVal[1] == 0xFF)
      {
        ANS_DBG_MSG(profiledbgfile,"NOTIFY_NEW_ALERT_IMMEDIATELY 0xFF\n");
	/* notify all supported categories for which
	 * the notification has been previously enabled
	 */
	for(idx=0;idx<=CATEGORY_ID_INSTANT_MESSAGE;idx++)
	{
	  if(alertNotificationServerContext.notifyNewAlert[idx] == 0x01)
	  {
	    alertNotificationServerContext.notifyNewAlert[idx] = 0x02;
	  }
	}
	
	//ANS_DBG_DATA(profiledbgfile,alertNotificationServerContext.notifyNewAlert,10); //TBR
      }
      else if((alertNotificationServerContext.alertCategory[attVal[1]/8] & ANS_Category_ID_To_Bitmask[attVal[1]]) &&
	      (alertNotificationServerContext.notifyNewAlert[attVal[1]] == 0x01))
      {
        ANS_INFO_MSG(profiledbgfile,"NOTIFY_NEW_ALERT_IMMEDIATELY for Supported Category 0x%02x\n",attVal[1]);
	/* if the enabling of notification is for a category supported,
	 * and that category has been previously enabled,
	 * then enable the notifications
	 */ 
	alertNotificationServerContext.notifyNewAlert[attVal[1]] = 0x02;
      }
      else
      {
	break;
      }
			
      if(alertNotificationServerContext.state != ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY)
      {
	alertNotificationServerContext.state = ANS_NOTIFY_NEW_ALERT_IMMEDIATELY;
	/* update the characteristic so that it will be
	 * notified
	 */ 
        ANS_INFO_MSG(profiledbgfile,"ANS_NOTIFY_NEW_ALERT_IMMEDIATELY: call Update_Alert()\n",);
	Update_Alert();
	//ANS_DBG_DATA(profiledbgfile,alertNotificationServerContext.notifyNewAlert,10); //TBR
      }
    }
    break;
  case NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY:
    {
      if(attVal[1] == 0xFF)
      {
        ANS_DBG_MSG(profiledbgfile,"NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY 0xFF\n");
	/* notify all supported categories for which
	 * the notification has been previously enabled
	 */
	for(idx=0;idx<=CATEGORY_ID_INSTANT_MESSAGE;idx++)
	{
	  if(alertNotificationServerContext.notifyUnreadAlert[idx] == 0x01)
	  {
	    alertNotificationServerContext.notifyUnreadAlert[idx] = 0x02;
	  }
	}
	//ANS_DBG_DATA(profiledbgfile,alertNotificationServerContext.notifyUnreadAlert,10);//TBR
      }
      else if(alertNotificationServerContext.unreadAlertCategory[attVal[1]/8] & ANS_Category_ID_To_Bitmask[attVal[1]])
      {
        ANS_INFO_MSG(profiledbgfile,"NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY for Supported Category 0x%02x\n",attVal[1]);
	/* if the enabling of notification is for a category supported,
	 * then enable the notifications
	 */ 
	alertNotificationServerContext.notifyUnreadAlert[attVal[1]] = 0x02;
      }
      else
      {
	break;
      }
      
      if(alertNotificationServerContext.state != ANS_NOTIFY_NEW_ALERT_IMMEDIATELY)
      {
	alertNotificationServerContext.state = ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY;
	
	/* update the characteristic so that it will be
	 * notified
	 */ 
        ANS_INFO_MSG(profiledbgfile,"ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY: call Update_Alert()\n",);
	retval = Update_Alert();
      }
    }
    break;
  }
  return retval;
}/* end ANC_Handle_ControlPoint_Write() */

/*******************************************************************************
* Local Functions
*******************************************************************************/
static void EnDis_All_Supp_Categories(uint8_t* category,uint8_t* notify,uint8_t enable)
{
  uint8_t idx;
  uint8_t index;
	
  ANS_DBG_MSG(profiledbgfile,"EnDis_All_Supp_Categories %x %x\n",category[0],category[1]);
	
  for(idx=1,index=0; idx<=0x08; idx=idx<<1,index++)
  {
    if(category[0] & idx)
    {
      notify[index] = enable;
    }
  }
  if(category[1] & 0x01)
  {
    notify[8] = enable;
  }
  if(category[1] & 0x02)
  {
    notify[9] = enable;
  }
	
  //ANS_DBG_DATA(profiledbgfile,notify,10); //TBR
}/* end EnDis_All_Supp_Categories() */

/**
 * starts an update procedure to
 * update the new alert or unread
 * alert status characteristic
 */ 
static tBleStatus Update_Alert(void)
{
  uint8_t idx;
  uint8_t charVal[2];
  tBleStatus retval = BLE_STATUS_SUCCESS;
	
  ANS_DBG_MSG(profiledbgfile,"Update_Alert\n");
  if(alertNotificationServerContext.state == ANS_NOTIFY_NEW_ALERT_IMMEDIATELY)
  {
    //ANS_DBG_DATA(profiledbgfile,alertNotificationServerContext.notifyNewAlert,10); //TBR
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServerContext.notifyNewAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = ans_new_alert_status[idx];
        
        retval = BLE_STATUS_INSUFFICIENT_RESOURCES;
        while (retval == BLE_STATUS_INSUFFICIENT_RESOURCES)
        {
          retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
                                              alertNotificationServerContext.newAlertHandle,
                                              0,
                                              2,   
                                              charVal);
        }
        if (retval != BLE_STATUS_SUCCESS)
	{
	  ANS_DBG_MSG (profiledbgfile,"Update_Alert(),FAILED to update new Alert Handle Char: 0x%02X !!\n", retval);
	} 
	else 
	{
	  alertNotificationServerContext.notifyNewAlert[idx] = 0x01;
          ANS_INFO_MSG(profiledbgfile,"Update New Alert, Category ID: 0x%02x, New Alert Status: 0x%02x\n",charVal[0],charVal[1] );
	}
	return retval;
      }
    }
		
    /* need not notify any alerts. check for
     * unread alert status
     */ 
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServerContext.notifyUnreadAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = ans_unread_alert_status[idx];
        retval = BLE_STATUS_INSUFFICIENT_RESOURCES;
        while (retval == BLE_STATUS_INSUFFICIENT_RESOURCES)
        {
          retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
                                              alertNotificationServerContext.unreadAlertStatusHandle,
                                              0,
                                              2,
                                              charVal);
        }
        if (retval != BLE_STATUS_SUCCESS)
	{
          ANS_DBG_MSG (profiledbgfile,"Update_Alert(),FAILED to update unread alert status Char: 0x%02X !!\n", retval);
	}
	else
	{
	  alertNotificationServerContext.notifyUnreadAlert[idx] = 0x01;
	  alertNotificationServerContext.state = ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY;
          ANS_INFO_MSG(profiledbgfile,"Update Unread Alert Status, Category ID: 0x%02x, Unread Alert Status: 0x%02x\n",charVal[0],charVal[1] );
	}
	return retval;
      }
    }
		
    /* no new alerts or unread alerts, so change the
     * state
     */ 
    alertNotificationServerContext.state = ANS_CONNECTED;
  }
  else if(alertNotificationServerContext.state == ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY)
  {
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServerContext.notifyUnreadAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = ans_unread_alert_status[idx];
        retval = BLE_STATUS_INSUFFICIENT_RESOURCES;
        while (retval == BLE_STATUS_INSUFFICIENT_RESOURCES)
        {
          retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
                                              alertNotificationServerContext.unreadAlertStatusHandle,
                                              0,
                                              2,
                                              charVal);
        }
        if (retval != BLE_STATUS_SUCCESS)
	{
	  ANS_DBG_MSG(profiledbgfile,"Error in Update char value for unread alert status characteristic!!!!!\n");
	}
	else
	{
	  alertNotificationServerContext.notifyUnreadAlert[idx] = 0x01;
          ANS_INFO_MSG(profiledbgfile,"Update Unread Alert Status, Category ID: 0x%02x, Unread Alert Status: 0x%02x\n",charVal[0],charVal[1] );
	}
	return retval;
      }
    }
    
    /* need not notify any unread alerts. Check for
     * new alert notification
     */ 
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServerContext.notifyNewAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = ans_new_alert_status[idx];
        retval = BLE_STATUS_INSUFFICIENT_RESOURCES;
        while (retval == BLE_STATUS_INSUFFICIENT_RESOURCES)
        {
          retval = aci_gatt_update_char_value(alertNotificationServerContext.alertServiceHandle,
                                              alertNotificationServerContext.newAlertHandle,
                                              0,
                                              2,
                                              charVal);
        }
	if (retval != BLE_STATUS_SUCCESS)
	{
          ANS_DBG_MSG (profiledbgfile,"Update_Alert(),FAILED to update new alert Char: 0x%02X !!\n", retval);
	}
	else
	{
	  alertNotificationServerContext.notifyNewAlert[idx] = 0x01;
	  alertNotificationServerContext.state = ANS_NOTIFY_NEW_ALERT_IMMEDIATELY;
          ANS_INFO_MSG(profiledbgfile,"Update New Alert, Category ID: 0x%02x, New Alert Status: 0x%02x\n",charVal[0],charVal[1] );
	}
        return retval; //TBR
      }
    }
		
    /* no new alerts or unread alerts, so change the
     * state
     */ 
    alertNotificationServerContext.state = ANS_CONNECTED; //TBR
  }
  return retval; 
}/* end Update_Alert() */

#endif /* (BLE_CURRENT_PROFILE_ROLES & ALERT_NOTIFICATION_SERVER) */
