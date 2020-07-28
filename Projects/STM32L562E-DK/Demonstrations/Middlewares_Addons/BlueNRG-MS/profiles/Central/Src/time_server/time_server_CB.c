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
*   FILENAME        - timer_server_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      02/23/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    timer server central role  callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  timer server central role callbacks handler.
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
#include <master_basic_profile.h>

#if (BLE_CURRENT_PROFILE_ROLES & TIME_SERVER)

#include <time_server.h>
#include <time_server_config.h>

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

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported CallBacks
*******************************************************************************/  

void Master_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
			       uint8_t *data_length, uint8_t *data, 
			       uint8_t *RSSI)
{
  uint8_t i;

  if (*status == DEVICE_DISCOVERED) {
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Device Discovered ****\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "Addr Type = %d\r\n", *addr_type);
    CALLBACKS_MESG_DBG(profiledbgfile, "Addr = 0x");
    for (i=0; i<6; i++)
      CALLBACKS_MESG_DBG(profiledbgfile, "%02x", addr[i]);
    CALLBACKS_MESG_DBG(profiledbgfile, "\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "Data (Length=%d):\r\n", *data_length);
    for (i=0; i<*data_length; i++)
      CALLBACKS_MESG_DBG(profiledbgfile, "%02x ", data[i]);
    CALLBACKS_MESG_DBG(profiledbgfile, "\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "RSSI = 0x%02x\r\n", *RSSI);
    CALLBACKS_MESG_DBG(profiledbgfile, "**************************\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_ENDED) {
    if (TimeServerContext.state != TIME_SERVER_STATE_START_CONNECTION)
      TimeServerContext.state = TIME_SERVER_STATE_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    TimeServerContext.state = TIME_SERVER_STATE_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  TimeServer_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      TimeServerContext.state = TIME_SERVER_STATE_INITIALIZED;
      
      TimeServer_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	TimeServerContext.connHandle = *connection_handle;  
	TimeServerContext.state = TIME_SERVER_STATE_CONNECTED;
	TimeServer_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      TimeServerContext.state = TIME_SERVER_STATE_INITIALIZED;
#if 0 //TBR??
      if (TimeServerContext.fullConf)
	TimeServer_FullConfError_CB(TIME_SERVER_ERROR_IN_CONNECTION, *status);
      else
#endif
	TimeServer_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      TimeServerContext.state = TIME_SERVER_STATE_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      TimeServerContext.state = TIME_SERVER_STATE_CONN_PARAM_UPDATE_REQ;
      TimeServer_ConnectionParameterUpdateReq_CB((tipsConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
}

void Master_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle, dataReceivedType *data)
{ 
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x Ended with status = 0x%02x\n", *connection_handle, *procedure, *status);
  
  if (*procedure == NOTIFICATION_INDICATION_CHANGE_STATUS) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"Master_PeerDataExchange_CB(), event: NOTIFICATION_INDICATION_CHANGE_STATUS\r\n");
  }

  if ((*procedure == READ_VALUE_STATUS) ||  
      (*procedure == READ_LONG_VALUE_STATUS)) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"Read Procedure %d: \r\n", *procedure);
  }

  if (*procedure == WRITE_VALUE_STATUS) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"WRITE_VALUE_STATUS, Write Status = 0x%02x\r\n", *status);
  }

  if (*procedure == NOTIFICATION_DATA_RECEIVED) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"NOTIFICATION_DATA_RECEIVED: handle=0x%04x, data length=%d, data:\r\n",
	   data->attr_handle, data->data_length);      
  }
  
  if (*procedure == INDICATION_DATA_RECEIVED ) 
  {
    
    CALLBACKS_MESG_DBG(profiledbgfile,"INDICATION_DATA_RECEIVED: handle=0x%04X, data length=%d\n",data->attr_handle,data->data_length);
  }
  
  if (*procedure == ATTRIBUTE_MODIFICATION_RECEIVED)
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"ATTRIBUTE_MODIFICATION_RECEIVED: handle=0x%04X, data length=%d\n",data->attr_handle,data->data_length);
    for (uint8_t i=0; i<data->data_length; i++) {
      CALLBACKS_MESG_DBG(profiledbgfile,"%02x ", data->data_value[i]);
    }
    CALLBACKS_MESG_DBG(profiledbgfile,"\r\n");
    if(data->attr_handle == (TimeServerContext.timeUpdateControlPointCharHandle + 1))
    {
      /* this event is received only when the client writes to the Update 
         Control Point characteristic */ 
      
      /* Call the Control Point characteristic write handler */
      TimeServer_Update_Control_Point_Handler((tTimeUpdateControlPoint) data->data_value[0]);
    }
    /* TBR: new code (ctCharNotificationConfig should be set to 1 if the Current Time Configuration Descriptor has been configured for notification) */
    else if ((data->attr_handle == (TimeServerContext.currentTimeCharHandle + 2)) && (data->data_value[0] == 0x01)) /* : Notify value: 0x01 */ 
    {
      /* Current Time Configuration Descriptor has been configured for notification */
      TimeServerContext.ctCharNotificationConfig = 0x01;
      CALLBACKS_MESG_DBG ( profiledbgfile, "attrHandle %02X, attrValue %02X\n", data->attr_handle, data->data_value[0]);
    }
    /* TBR: new code (ctCharNotificationConfig should be set to 0 if the Current Time Configuration Descriptor has not been configured for notification) */
    else if ((data->attr_handle == (TimeServerContext.currentTimeCharHandle + 2)) && (data->data_value[0] == 0x00))  
    {
      /* Current Time Configuration Descriptor has been set to 0 (no notification) */
      CALLBACKS_MESG_DBG ( profiledbgfile, "attrHandle %02X, attrValue %02X\n", data->attr_handle, data->data_value[0]);
      TimeServerContext.ctCharNotificationConfig = 0x00;
    }
    /* TBR: end new code */
  }
  
}/* end Master_PeerDataExchange_CB() */

void Master_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  TimeServer_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
     TimeServer_PinCodeRequired_CB();
  }
}/* end Master_Pairing_CB() */

void Master_PeerDataPermitExchange_CB(uint8_t * procedure, uint16_t * connection_handle, dataPermitExchangeType * data) 
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x \n", *connection_handle, *procedure);
  if (* procedure == READ_PERMIT_REQUEST)
  {
   if(data->attr_handle == (TimeServerContext.currentTimeCharHandle + 1))
   {
      /* send the allow read command */			
      TimeServerContext.timeServerAllowRead = 1;
      /* let the application know that the current time is
       * being requested for read. there is no event data
       * for this event
       */
      TimeServer_Notify_State_To_User_Application_CB(EVT_TS_CURTIME_READ_REQ);
   }
  }
}/* end Master_PeerDataPermitExchange_CB() */


/****************** phone alert status server Weak Callbacks definition ***************************/

WEAK_FUNCTION(void TimeServer_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void TimeServer_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void TimeServer_ConnectionParameterUpdateReq_CB(tipsConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void TimeServer_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void TimeServer_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & TIME_SERVER) */
