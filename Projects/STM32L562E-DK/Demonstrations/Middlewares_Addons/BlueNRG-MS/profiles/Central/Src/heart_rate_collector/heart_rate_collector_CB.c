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
*   FILENAME        -  heart_rate_collector_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/16/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    heart rate collector callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  heart Rate collector callbacks handler.
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
#include <master_basic_profile.h>

#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_COLLECTOR)

#include <heart_rate_service.h>
#include <heart_rate_collector.h>
#include <heart_rate_collector_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#ifndef HRC_TEST
#define HRC_TEST 1 //TBR 
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

    /* Heart Rate Service Characteristics */
    if ((uuid_searched == HEART_RATE_SERVICE_UUID) && 
	(uuid_charac == HEART_RATE_MEASURMENT_UUID)) 
    {
      BLUENRG_memcpy(&heartRateCollectorContext.HRMeasurementCharValHandle, &charac[index+4], 2);
    } 
    
    else if ((uuid_searched == HEART_RATE_SERVICE_UUID) && 
        (uuid_charac == SENSOR_LOCATION_UUID)) 
    {
      BLUENRG_memcpy(&heartRateCollectorContext.BodySensorLocValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == HEART_RATE_SERVICE_UUID) && 
        (uuid_charac == CONTROL_POINT_UUID)) 
    {
      BLUENRG_memcpy(&heartRateCollectorContext.HRControlPointValHandle, &charac[index+4], 2);
    } 

    /* Device Information Service Characteristics */
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
	     (uuid_charac == MANUFACTURER_NAME_UUID)) 
    {
      BLUENRG_memcpy(&heartRateCollectorContext.ManufacturerNameCharValHandle, &charac[index+4], 2);
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

void Master_HRC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
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
    if (heartRateCollectorContext.state != HEART_RATE_COLLECTOR_START_CONNECTION)
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    heartRateCollectorContext.state = HEART_RATE_COLLECTOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  HRC_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_HRC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_INITIALIZED;
      HRC_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	heartRateCollectorContext.connHandle = *connection_handle;  
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_CONNECTED;
	HRC_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_INITIALIZED;
      if (heartRateCollectorContext.fullConf)
	HRC_FullConfError_CB(HRC_ERROR_IN_CONNECTION, *status);
      else
	HRC_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_CONN_PARAM_UPDATE_REQ;
      HRC_ConnectionParameterUpdateReq_CB((hrcConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_HRC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Heart Rate Service and Device Info  Service are both present */
      if (HRC_findHandles(HEART_RATE_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	heartRateCollectorContext.HeartRateServicePresent = TRUE;
      } else {
	heartRateCollectorContext.HeartRateServicePresent = FALSE;
      }
      if (HRC_findHandles(DEVICE_INFORMATION_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	heartRateCollectorContext.DeviceInfoServicePresent = TRUE;
      } else {
	heartRateCollectorContext.DeviceInfoServicePresent = FALSE;
      }

      if (heartRateCollectorContext.fullConf) {
        /* start HR measurement characteristic discovery */
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY;
#if HRC_TEST
	HRC_ServicesDiscovery_CB(*status, heartRateCollectorContext.numpeerServices, 
				          heartRateCollectorContext.peerServices);
#endif
      } else {
	HRC_ServicesDiscovery_CB(*status, heartRateCollectorContext.numpeerServices, 
				           heartRateCollectorContext.peerServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      if (*status != BLE_STATUS_SUCCESS) {
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_DISCONNECTION;
	HRC_FullConfError_CB(HRC_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(heartRateCollectorContext.uuid_searched, 
			 heartRateCollectorContext.numCharac, 
			 heartRateCollectorContext.charac);
	if (heartRateCollectorContext.fullConf) {   //TBR 
	  switch (heartRateCollectorContext.uuid_searched) {
	  case HEART_RATE_SERVICE_UUID:
	    if (heartRateCollectorContext.HeartRateServicePresent && 
                (heartRateCollectorContext.state == HEART_RATE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY))
            {
	      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_READ_BODY_SENSOR;
	    }
	    break;
	  }
#if HRC_TEST
          if (heartRateCollectorContext.fullConf)
          {
            HRC_CharacOfService_CB(*status, heartRateCollectorContext.numCharac, heartRateCollectorContext.charac);
          }
#endif
	} else {
	  HRC_CharacOfService_CB(*status, heartRateCollectorContext.numCharac, heartRateCollectorContext.charac);
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
      HRC_CharacDesc_CB(*status, heartRateCollectorContext.numCharac, heartRateCollectorContext.charac);
    }
    break;
  }
}

void Master_HRC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				uint16_t *connection_handle, dataReceivedType *data)
{

  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Indication Change Status (0x%02x)\r\n", *status);
    if (heartRateCollectorContext.fullConf && 
        (heartRateCollectorContext.state == HEART_RATE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	HRC_FullConfError_CB(HRC_ERROR_IN_ENABLE_INDICATION, *status);
      }
      heartRateCollectorContext.fullConf = FALSE; //TBR
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_CONNECTED_IDLE; //TBR
    } 
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    
    if (*status != BLE_STATUS_SUCCESS) 
    {
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_DISCONNECTION;
	HRC_FullConfError_CB(HRC_ERROR_IN_READ, *status);
    } 
    else 
    {
      if (heartRateCollectorContext.dataLen == HR_READ_SIZE)
      {
        /* Store and analyse read Body Sensor Location characteristic value */
        HRC_Read_Body_Sensor_Location_Handler();
      }
      HRC_DataValueRead_CB(*status, heartRateCollectorContext.dataLen, heartRateCollectorContext.data); 
      
      if ((heartRateCollectorContext.fullConf) && (heartRateCollectorContext.state == HEART_RATE_COLLECTOR_WAIT_READ_BODY_SENSOR))
      {
        /* We enter here as consequence of heartRateCollectorContext.state = HEART_RATE_COLLECTOR_READ_BODY_SENSOR;*/
        /* enable hr measurement notification */
        heartRateCollectorContext.state = HEART_RATE_COLLECTOR_ENABLE_MEASUREMENT_NOTIFICATION;
      } 
      else 
        heartRateCollectorContext.state = HEART_RATE_COLLECTOR_CONNECTED_IDLE; //TBR
    }
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
  
    /* check write response status to the performed write request (APPL_WRITE_HR_CTRL_POINT_CHAR)*/
    HRC_CP_Check_Write_Response_Handler(*status);
    
    break;
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      /* Call user callback (display received notification value) */
      HRC_NotificationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      
      /* Store the received heart Rate measurement/context related to the latest control point procedure */
      HRC_Notification_Handler(data->attr_handle, data->data_length, &(data->data_value[0]));   
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile,"INDICATION_DATA_RECEIVED (0x%02x): handle=0x%04X, data length=%d\n", *status, data->attr_handle,data->data_length);
   
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }

}

void Master_HRC_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  HRC_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
    HRC_PinCodeRequired_CB();
  }
}
    
void Master_HRC_PeerDataPermitExchange_CB(uint8_t * procedure, uint16_t * connection_handle, dataPermitExchangeType * data) 
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x \n", *connection_handle, *procedure);
  
}/* end Master_PeerDataPermitExchange_CB() */


/****************** glucose collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void HRC_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void HRC_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void HRC_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void HRC_ConnectionParameterUpdateReq_CB(hrcConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void HRC_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void HRC_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void HRC_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void HRC_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void HRC_EnableNotificationIndication_CB(uint8_t status))
{
}

WEAK_FUNCTION(void HRC_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

WEAK_FUNCTION(void HRC_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_COLLECTOR) */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
