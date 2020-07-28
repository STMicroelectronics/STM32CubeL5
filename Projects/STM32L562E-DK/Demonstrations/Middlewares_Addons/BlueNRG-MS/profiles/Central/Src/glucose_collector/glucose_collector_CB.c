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
*   FILENAME        -  glucose_collector_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    glucose collector callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  glucose collector callbacks handler.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include <host_config.h>
#include <hci_le.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>
#include "hal.h"
#include <master_basic_profile.h>

#if (BLE_CURRENT_PROFILE_ROLES & GLUCOSE_COLLECTOR)

#include <glucose_service.h>
#include <glucose_collector.h>
#include <glucose_collector_config.h>
#include <glucose_collector_racp_CB.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#ifndef GL_COLLECTOR_TEST
#define GL_COLLECTOR_TEST 1 //TBR 
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

    /* Glucose Service Characteristics */
    if ((uuid_searched == GLUCOSE_SERVICE_UUID) && 
	(uuid_charac == GLUCOSE_MEASUREMENT_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&glucoseCollectorContext.MeasurementCharValHandle, &charac[index+4], 2);
    } 
    
    else if ((uuid_searched == GLUCOSE_SERVICE_UUID) && 
        (uuid_charac == GLUCOSE_MEASUREMENT_CONTEXT_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&glucoseCollectorContext.MeasurementContextCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == GLUCOSE_SERVICE_UUID) && 
         (uuid_charac == GLUCOSE_FEATURE_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&glucoseCollectorContext.FeatureCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == GLUCOSE_SERVICE_UUID) && 
        (uuid_charac == GLUCOSE_RACP_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&glucoseCollectorContext.RACPCharValHandle, &charac[index+4], 2);
    } 

    /* Device Information Service Characteristics */
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
	     (uuid_charac == MANUFACTURER_NAME_UUID)) 
    {
      BLUENRG_memcpy(&glucoseCollectorContext.ManufacturerNameCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
              (uuid_charac == MODEL_NUMBER_UUID)) 
    {
      BLUENRG_memcpy(&glucoseCollectorContext.ModelNumberCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
             (uuid_charac == SYSTEM_ID_UUID)) 
    {
      BLUENRG_memcpy(&glucoseCollectorContext.SystemIDCharValHandle, &charac[index+4], 2);
    } 
    else 
    {
      ret = HCI_INVALID_PARAMETERS;
    }
    index += sizeElement;
  }

  return ret ;
}
/*******************************************************************************
* exported CallBacks
*******************************************************************************/  

void Master_GC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
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
    if (glucoseCollectorContext.state != GLUCOSE_COLLECTOR_START_CONNECTION)
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    glucoseCollectorContext.state = GLUCOSE_COLLECTOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  GL_Collector_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_GC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_INITIALIZED;
      glucoseCollectorContext.number_of_measurement_notifications = 0; //TBR
      glucoseCollectorContext.number_of_measurement_context_notifications = 0; //TBR
      
      GL_Collector_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	glucoseCollectorContext.connHandle = *connection_handle;  
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_CONNECTED;
	GL_Collector_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_INITIALIZED;
      if (glucoseCollectorContext.fullConf)
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_CONNECTION, *status);
      else
	GL_Collector_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_CONN_PARAM_UPDATE_REQ;
      GL_Collector_ConnectionParameterUpdateReq_CB((glcConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_GC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Glucose Service and Device Info  Service are both present */
      if (GL_Collector_findHandles(GLUCOSE_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.glucoseServicePresent = TRUE;
      } else {
	glucoseCollectorContext.glucoseServicePresent = FALSE;
      }
      if (GL_Collector_findHandles(DEVICE_INFORMATION_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.DeviceInfoServicePresent = TRUE;
      } else {
	glucoseCollectorContext.DeviceInfoServicePresent = FALSE;
      }

      if (glucoseCollectorContext.fullConf) {
        /* start glucose measurement characteristic discovery */
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY;
#if GL_COLLECTOR_TEST
	GL_Collector_ServicesDiscovery_CB(*status, glucoseCollectorContext.numpeerServices, 
				          glucoseCollectorContext.peerServices);
#endif
      } else {
	GL_Collector_ServicesDiscovery_CB(*status, glucoseCollectorContext.numpeerServices, 
				           glucoseCollectorContext.peerServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      GL_INFO_MSG(profiledbgfile,"------ *status = 0x%04X, uuid_searched = 0x%04X, numCharac = 0x%04X\n", *status,glucoseCollectorContext.uuid_searched, glucoseCollectorContext.numCharac); //TBR???
      if (*status != BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(glucoseCollectorContext.uuid_searched, 
			 glucoseCollectorContext.numCharac, 
			 glucoseCollectorContext.charac);
	if (glucoseCollectorContext.fullConf) {   //TBR 
	  switch (glucoseCollectorContext.uuid_searched) {
	  case GLUCOSE_SERVICE_UUID:
	    if (glucoseCollectorContext.glucoseServicePresent && 
                (glucoseCollectorContext.state == GLUCOSE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY))
            {
	      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_READ_FEATURE;
	    }
	    break;
           
	  }
#if GL_COLLECTOR_TEST
          if (glucoseCollectorContext.fullConf)
          {
            GL_Collector_CharacOfService_CB(*status, glucoseCollectorContext.numCharac, glucoseCollectorContext.charac);
          }
#endif
	} else {
	  GL_Collector_CharacOfService_CB(*status, glucoseCollectorContext.numCharac, glucoseCollectorContext.charac);
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
      GL_Collector_CharacDesc_CB(*status, glucoseCollectorContext.numCharac, glucoseCollectorContext.charac);
    }
    break;
  }
}

void Master_GC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				uint16_t *connection_handle, dataReceivedType *data)
{

  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    if (glucoseCollectorContext.fullConf && 
        (glucoseCollectorContext.state == GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_ENABLE_NOTIFICATION, *status);
      }
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_ENABLE_MEASUREMENT_CONTEXT_NOTIFICATION;
    } 
    else if (glucoseCollectorContext.fullConf && 
        (glucoseCollectorContext.state == GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_CONTEXT_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_ENABLE_NOTIFICATION, *status);
      }
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_ENABLE_RACP_INDICATION;
    } 
     else if (glucoseCollectorContext.fullConf && 
        (glucoseCollectorContext.state == GLUCOSE_COLLECTOR_WAIT_ENABLE_RACP_INDICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_ENABLE_INDICATION, *status);
      }
      glucoseCollectorContext.fullConf = FALSE; //TBR
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_CONNECTED_IDLE; //TBR
    } 
    
    GL_Collector_EnableNotificationIndication_CB(*status);
    //CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Indication Change Status (0x%02x)\r\n", *status);
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    
    if (*status != BLE_STATUS_SUCCESS) 
    {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_READ, *status);
    } 
    else 
    {
      if (glucoseCollectorContext.dataLen == GLUCOSE_READ_SIZE)
      {
        /* Store read glucose feature characteristic value */
        glucoseCollectorContext.glucose_feature = LE_TO_HOST_16(&(glucoseCollectorContext.data[0]));
        //CALLBACKS_MESG_DBG(profiledbgfile,"Master_PeerDataExchange_CB(), READ_VALUE_STATUS,glucose feature = 0x%02x\n",glucoseCollectorContext.glucose_feature);
      }
      GL_Collector_DataValueRead_CB(*status, glucoseCollectorContext.dataLen, glucoseCollectorContext.data); 
      
      if ((glucoseCollectorContext.fullConf) && (glucoseCollectorContext.state == GLUCOSE_COLLECTOR_WAIT_READ_FEATURE))
      {
        /* Start glucose measurement characteristic enabling */
        glucoseCollectorContext.state = GLUCOSE_COLLECTOR_ENABLE_MEASUREMENT_NOTIFICATION;
      } 
      else 
        glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION; //TBR
    }
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
  
    /* check write response status to the performed write request (RACP request sent through GL_Collector_Send_RACP())*/
    GL_Collector_RACP_Check_Write_Response(*status);
    break;
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      /* Call user callback (display received notification value) */
      GL_Collector_NotificationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      
      /* Store the received glucose measurement/context related to the latest RACP procedure */
      GL_Collector_RACP_Notifications_Storage(data->attr_handle,data->data_length,&(data->data_value[0])); 
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile,"INDICATION_DATA_RECEIVED (0x%02x): handle=0x%04X, data length=%d\n", *status, data->attr_handle,data->data_length);
    /* Call Clucose Collector RACP Indication Handler */
    GL_Collector_RACP_Indications(data->attr_handle,data->data_length, &(data->data_value[0]));
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }

}

void Master_GC_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  GL_Collector_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
    GL_Collector_PinCodeRequired_CB();
  }
}

void Master_GC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data)
{
  // Nothing to do 
}
 
/****************** glucose collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void GL_Collector_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void GL_Collector_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void GL_Collector_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void GL_Collector_ConnectionParameterUpdateReq_CB(glcConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void GL_Collector_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void GL_Collector_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void GL_Collector_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void GL_Collector_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void GL_Collector_EnableNotificationIndication_CB(uint8_t status))
{
}

WEAK_FUNCTION(void GL_Collector_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

WEAK_FUNCTION(void GL_Collector_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & GLUCOSE_COLLECTOR) */
