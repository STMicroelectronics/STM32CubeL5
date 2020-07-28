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
*   FILENAME        -  alert_notification_client_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      26/02/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    Alert Notification Client Profile Central role callbacks for profile 
*                device, services, characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  Alert Notification Client Profile Central role callbacks handler.
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
#include "bluenrg_aci.h"
#include <master_basic_profile.h>

#if (BLE_CURRENT_PROFILE_ROLES & ALERT_NOTIFICATION_CLIENT)

#include <alert_notification_client.h>
#include <alert_notification_client_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#ifndef ANC_TEST
#define ANC_TEST 1 //TBR 
#endif

#define WEAK_FUNCTION(x) __weak x
/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported CallBacks
*******************************************************************************/  

static uint8_t findCharacHandle(uint16_t uuid_searched, uint8_t numCharac, uint8_t *charac)
{
  uint8_t ret, i, index, sizeElement;
  uint16_t uuid_charac;

  ret = BLE_STATUS_SUCCESS;

  index = 0;
  for (i=0; i<numCharac; i++) 
  { 
    if (charac[index] == 1) 
    {
      sizeElement = 8;
      BLUENRG_memcpy(&uuid_charac, &charac[index+6], 2);
    } 
    else 
    {
      sizeElement = 22;
    }
    
    /* Alert Notification Service Characteristics */
    if ((uuid_searched == ALERT_NOTIFICATION_SERVICE_UUID) && 
	(uuid_charac == SUPPORTED_NEW_ALERT_CATEGORY_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&alertNotificationClientContext.newAlertCategoryValueHandle, &charac[index+4], 2);
    }     
    else if ((uuid_searched == ALERT_NOTIFICATION_SERVICE_UUID) && 
        (uuid_charac == NEW_ALERT_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&alertNotificationClientContext.newAlertValueHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == ALERT_NOTIFICATION_SERVICE_UUID) && 
         (uuid_charac == SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&alertNotificationClientContext.unreadAlertCategoryValueHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == ALERT_NOTIFICATION_SERVICE_UUID) && 
        (uuid_charac == UNREAD_ALERT_STATUS_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&alertNotificationClientContext.unreadAlertStatusValueHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == ALERT_NOTIFICATION_SERVICE_UUID) && 
         (uuid_charac == ALERT_NOTIFICATION_CONTROL_POINT_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&alertNotificationClientContext.ctrlPointCharValueHandle, &charac[index+4], 2);
    }  
    else 
    {
      ret = HCI_INVALID_PARAMETERS;
    }
    index += sizeElement;
  }

  return ret ;
}

void Master_ANC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
		                   uint8_t *data_length, uint8_t *data, 
			           uint8_t *RSSI)
{
  uint8_t i;

  if (*status == DEVICE_DISCOVERED) {
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Device Discovered ****\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "Addr Type = %d\r\n", *addr_type);
    CALLBACKS_MESG_DBG(profiledbgfile, "Addr = 0x");
    /*
    for (i=0; i<6; i++)
      CALLBACKS_MESG_DBG(profiledbgfile, "%02x", addr[i]);
    CALLBACKS_MESG_DBG(profiledbgfile, "\r\n");
    */
    for (i=5; i>0; i--){
      CALLBACKS_MESG_DBG(profiledbgfile,"%02x-", addr[i]);
    }
    CALLBACKS_MESG_DBG(profiledbgfile,"%02X\r\n", addr[0]);
    
    CALLBACKS_MESG_DBG(profiledbgfile, "Data (Length=%d):\r\n", *data_length);
    for (i=0; i<*data_length; i++)
      CALLBACKS_MESG_DBG(profiledbgfile, "%02x ", data[i]);
    CALLBACKS_MESG_DBG(profiledbgfile, "\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "RSSI = 0x%02x\r\n", *RSSI);
    CALLBACKS_MESG_DBG(profiledbgfile, "**************************\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_ENDED) {
    if (alertNotificationClientContext.state != ANC_START_CONNECTION)
      alertNotificationClientContext.state = ANC_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    alertNotificationClientContext.state = ANC_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  ANC_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_ANC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      alertNotificationClientContext.state = ANC_INITIALIZED;
      //to test the recovery from connection lost (4.8.1 and 4.8.2)
      //alertNotificationClientContext.disconnected = 0x01; 
      
      ANC_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	alertNotificationClientContext.connHandle = *connection_handle;  
	alertNotificationClientContext.state = ANC_CONNECTED;
	ANC_ConnectionStatus_CB(*connection_evt, *status);
        /*
        if (alertNotificationClientContext.disconnected)
        {
          ANC_Write_Control_Point(ENABLE_NEW_ALERT_NOTIFICATION,0xFF);
          ANC_Write_Control_Point(ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION,0xFF);
          ANC_Write_Control_Point(NOTIFY_NEW_ALERT_IMMEDIATELY,0xFF);
          ANC_Write_Control_Point(NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY,0xFF);
        }
        */
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      alertNotificationClientContext.state = ANC_INITIALIZED;
      if (alertNotificationClientContext.fullConf)
	ANC_FullConfError_CB(ANC_ERROR_IN_CONNECTION, *status);
      else
	ANC_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      alertNotificationClientContext.state = ANC_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      alertNotificationClientContext.state = ANC_CONN_PARAM_UPDATE_REQ;
      ANC_ConnectionParameterUpdateReq_CB((ancConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_ANC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Alert Notification Service is present */
      if (ANC_findHandles(ALERT_NOTIFICATION_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.ANSPresent = TRUE;
      } else {
	alertNotificationClientContext.ANSPresent = FALSE;
      }

      if (alertNotificationClientContext.fullConf) {
        /* start Alert Notification characteristics discovery */
	alertNotificationClientContext.state = ANC_CHARAC_DISCOVERY;
#if ANC_TEST
	ANC_ServicesDiscovery_CB(*status, alertNotificationClientContext.numpeerServices, 
				          alertNotificationClientContext.peerServices);
#endif
      } else {
	ANC_ServicesDiscovery_CB(*status, alertNotificationClientContext.numpeerServices, 
				           alertNotificationClientContext.peerServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      CALLBACKS_MESG_DBG(profiledbgfile,"------ *status = 0x%04X, uuid_searched = 0x%04X, numCharac = 0x%04X\n", *status, alertNotificationClientContext.uuid_searched, alertNotificationClientContext.numCharac); //TBR???
      if (*status != BLE_STATUS_SUCCESS) {
	alertNotificationClientContext.state = ANC_DISCONNECTION;
	ANC_FullConfError_CB(ANC_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(alertNotificationClientContext.uuid_searched, 
			 alertNotificationClientContext.numCharac, 
			 alertNotificationClientContext.charac);
	if (alertNotificationClientContext.fullConf) 
        {   //TBR 
	  if (alertNotificationClientContext.uuid_searched == ALERT_NOTIFICATION_SERVICE_UUID)
	    if (alertNotificationClientContext.ANSPresent && 
                (alertNotificationClientContext.state == ANC_WAIT_CHARAC_DISCOVERY))
            {
	      alertNotificationClientContext.state = ANC_READ_NEW_ALERT_CATEGORY;
	    }           
	 }
#if ANC_TEST
         if (alertNotificationClientContext.fullConf)
         {
           ANC_CharacOfService_CB(*status, alertNotificationClientContext.numCharac, alertNotificationClientContext.charac);
         }
#endif
	 else {
	  ANC_CharacOfService_CB(*status, alertNotificationClientContext.numCharac, alertNotificationClientContext.charac);
	}
      }
    }
    break;
  case FIND_INCLUDED_SERVICES:
    {
    }
    break;
  case FIND_CHARAC_DESCRIPTORS:
    {
      ANC_CharacDesc_CB(*status, alertNotificationClientContext.numCharac, alertNotificationClientContext.charac);
    }
    break;
  }
}/* end Master_ServiceCharacPeerDiscovery_CB() */



void Master_ANC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle, dataReceivedType *data)
{ 
  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    if (alertNotificationClientContext.fullConf && 
        (alertNotificationClientContext.state == ANC_WAIT_ENABLE_NEW_ALERT_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	ANC_FullConfError_CB(ANC_ERROR_IN_ENABLE_NOTIFICATION, *status);
      }
      ANC_EnableNotification_CB(*status);
      alertNotificationClientContext.state = ANC_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION;
    } 
    else if (alertNotificationClientContext.fullConf && 
        (alertNotificationClientContext.state == ANC_WAIT_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	ANC_FullConfError_CB(ANC_ERROR_IN_ENABLE_NOTIFICATION, *status);
      }
      ANC_EnableNotification_CB(*status);
      alertNotificationClientContext.fullConf = FALSE; //TBR
      alertNotificationClientContext.state = ANC_CONNECTED_IDLE; //TBR
    } 
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    
    if (*status != BLE_STATUS_SUCCESS) 
    {
	alertNotificationClientContext.state = ANC_DISCONNECTION;
	ANC_FullConfError_CB(ANC_ERROR_IN_READ, *status);
    } 
    else 
    { 
      if (alertNotificationClientContext.dataLen == SUPPORTED_NEW_ALERT_CATEGORY_CHAR_SIZE)
      {
        /* Store read Supported New Alert Category Characteristic value */      
        ANC_Read_New_Alert_Category_CB();
      } 
      else if ((alertNotificationClientContext.dataLen == SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_SIZE) &&
               (bReadSupportedUnreadAlert))
      {
        bReadSupportedUnreadAlert = FALSE;
        /* Store read Supported Unread Alert Category Characteristic value */
        ANC_Read_Unread_Alert_Status_Category_CB();
      }  
      ANC_DataValueRead_CB(*status, alertNotificationClientContext.dataLen, alertNotificationClientContext.data); 

      if ((alertNotificationClientContext.fullConf) && (alertNotificationClientContext.state == ANC_WAIT_READ_NEW_ALERT_CATEGORY))
      {
        alertNotificationClientContext.state = ANC_READ_UNREAD_ALERT_STATUS_CATEGORY;
      }
      else if ((alertNotificationClientContext.fullConf) && (alertNotificationClientContext.state == ANC_WAIT_READ_UNREAD_ALERT_STATUS_CATEGORY))
      {
        alertNotificationClientContext.state = ANC_ENABLE_NEW_ALERT_NOTIFICATION;
      } 
      else 
        alertNotificationClientContext.state = ANC_DISCONNECTION; 
    }
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
    // check write response status to the performed write request (APPL_WRITE_HR_CTRL_POINT_CHAR)
    ANC_CP_Check_Write_Response_Handler(*status);
    break;
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      /* Call user callback (display received notification value) */
      ANC_NotificationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      if (data->attr_handle == alertNotificationClientContext.newAlertValueHandle)
      {  
        /* Store the received New Alert notification*/
        ANC_New_Alert_Notification_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      }    
      else if (data->attr_handle == alertNotificationClientContext.unreadAlertStatusValueHandle)
      {  
        /* Store the received Unread Alert Status notification*/
        ANC_Unread_Alert_Status_Notification_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      } 
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Indication Data Received (0x%02x)\r\n", *status);
    CALLBACKS_MESG_DBG(profiledbgfile,"INDICATION_DATA_RECEIVED: handle=0x%04X, data length=%d\n",data->attr_handle,data->data_length);
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    CALLBACKS_MESG_DBG(profiledbgfile,"ATTRIBUTE_MODIFICATION_RECEIVED: handle=0x%04X, data length=%d\n",data->attr_handle,data->data_length);
    /*
    for (uint8_t i=0; i<data->data_length; i++) {
      CALLBACKS_MESG_DBG(profiledbgfile,"%02x ", data->data_value[i]);
    }
    CALLBACKS_MESG_DBG(profiledbgfile,"\r\n");
    if(data->attr_handle == (alertNotificationClientContext.ctrlPointCharHandle + 1))
    {
      ANC_Handle_ControlPoint_Write(data->data_value);
    }
    else
    {
      ANC_Attribute_Modification_Received_CB(data->attr_handle,data->data_length,&(data->data_value[0]));
    }
    */
    break;
  }  
  
}/* end Master_PeerDataExchange_CB() */

void Master_ANC_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  ANC_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
     ANC_PinCodeRequired_CB();
  }
}/* end Master_Pairing_CB() */

void Master_ANC_PeerDataPermitExchange_CB(uint8_t * procedure, uint16_t * connection_handle, dataPermitExchangeType * data) 
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x \n", *connection_handle, *procedure);
  
}/* end Master_PeerDataPermitExchange_CB() */




/****************** alert notification client Weak Callbacks definition ***************************/

WEAK_FUNCTION(void ANC_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void ANC_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void ANC_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void ANC_ConnectionParameterUpdateReq_CB(ancConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void ANC_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void ANC_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void ANC_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void ANC_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void ANC_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

WEAK_FUNCTION(void ANC_EnableNotification_CB(uint8_t status))
{
}

WEAK_FUNCTION(void ANC_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & ALERT_NOTIFICATION_CLIENT) */
