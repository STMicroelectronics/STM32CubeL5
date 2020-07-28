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
*   Comments:    Healt Thermometer collector callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  Healt Thermometer collector callbacks handler.
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

#if (BLE_CURRENT_PROFILE_ROLES & HEALTH_THERMOMETER_COLLECTOR)

#include <health_thermometer_service.h>
#include <health_thermometer_collector.h>
#include <health_thermometer_collector_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#ifndef HT_COLLECTOR_TEST
#define HT_COLLECTOR_TEST 1 //TBR 
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

    /* Glucose Service Characteristics */
    if ((uuid_searched == HEALTH_THERMOMETER_SERVICE_UUID) && 
	(uuid_charac == TEMPERATURE_MEASUREMENT_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&healthThermometerCollectorContext.TempMeasurementCharValHandle, &charac[index+4], 2);
    } 
    
    else if ((uuid_searched == HEALTH_THERMOMETER_SERVICE_UUID) && 
        (uuid_charac == TEMPERATURE_TYPE_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&healthThermometerCollectorContext.TempTypeCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == HEALTH_THERMOMETER_SERVICE_UUID) && 
         (uuid_charac == INTERMEDIATE_TEMPERATURE_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&healthThermometerCollectorContext.IntermediateTempCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == HEALTH_THERMOMETER_SERVICE_UUID) && 
        (uuid_charac == MEASUREMENT_INTERVAL_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&healthThermometerCollectorContext.MeasurementIntervalCharValHandle, &charac[index+4], 2);
    } 
    
    /* Device Information Service Characteristics */
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
	     (uuid_charac == MANUFACTURER_NAME_UUID)) 
    {
      BLUENRG_memcpy(&healthThermometerCollectorContext.ManufacturerNameCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
              (uuid_charac == MODEL_NUMBER_UUID)) 
    {
      BLUENRG_memcpy(&healthThermometerCollectorContext.ModelNumberCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
             (uuid_charac == SYSTEM_ID_UUID)) 
    {
      BLUENRG_memcpy(&healthThermometerCollectorContext.SystemIDCharValHandle, &charac[index+4], 2);
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

void Master_HTC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
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
    if (healthThermometerCollectorContext.state != HEALTH_THERMOMETER_COLLECTOR_START_CONNECTION)
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  HT_Collector_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_HTC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_INITIALIZED;
      HT_Collector_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	healthThermometerCollectorContext.connHandle = *connection_handle;  
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_CONNECTED;
	HT_Collector_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_INITIALIZED;
      if (healthThermometerCollectorContext.fullConf)
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_CONNECTION, *status);
      else
	HT_Collector_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_CONN_PARAM_UPDATE_REQ;
      HT_Collector_ConnectionParameterUpdateReq_CB((htcConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_HTC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Glucose Service and Device Info  Service are both present */
      if (HT_Collector_findHandles(HEALTH_THERMOMETER_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.HT_ServicePresent = TRUE;
      } else {
	healthThermometerCollectorContext.HT_ServicePresent = FALSE;
      }
      if (HT_Collector_findHandles(DEVICE_INFORMATION_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.DeviceInfoServicePresent = TRUE;
      } else {
	healthThermometerCollectorContext.DeviceInfoServicePresent = FALSE;
      }

      if (healthThermometerCollectorContext.fullConf) {
        /* start glucose measurement characteristic discovery */
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY;
#if HT_COLLECTOR_TEST
	HT_Collector_ServicesDiscovery_CB(*status, healthThermometerCollectorContext.numpeerServices, 
				          healthThermometerCollectorContext.peerServices);
#endif
      } else {
	HT_Collector_ServicesDiscovery_CB(*status, healthThermometerCollectorContext.numpeerServices, 
				           healthThermometerCollectorContext.peerServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      CALLBACKS_MESG_DBG(profiledbgfile,"------ *status = 0x%04X, uuid_searched = 0x%04X, numCharac = 0x%04X\n", *status,healthThermometerCollectorContext.uuid_searched, healthThermometerCollectorContext.numCharac); //TBR???
      if (*status != BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(healthThermometerCollectorContext.uuid_searched, 
			 healthThermometerCollectorContext.numCharac, 
			 healthThermometerCollectorContext.charac);
	if (healthThermometerCollectorContext.fullConf) {   //TBR 
	  switch (healthThermometerCollectorContext.uuid_searched) {
	  case HEALTH_THERMOMETER_SERVICE_UUID:
	    if (healthThermometerCollectorContext.HT_ServicePresent && 
                (healthThermometerCollectorContext.state == HEALTH_THERMOMETER_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY))
            {
	      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_READ_TEMPERATURE_TYPE;
	    }
	    break;
           
	  }
#if HT_COLLECTOR_TEST
          if (healthThermometerCollectorContext.fullConf)
          {
            HT_Collector_CharacOfService_CB(*status, healthThermometerCollectorContext.numCharac, healthThermometerCollectorContext.charac);
          }
#endif
	} else {
	  HT_Collector_CharacOfService_CB(*status, healthThermometerCollectorContext.numCharac, healthThermometerCollectorContext.charac);
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
      HT_Collector_CharacDesc_CB(*status, healthThermometerCollectorContext.numCharac, healthThermometerCollectorContext.charac);
    }
    break;
  }
}

void Master_HTC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				uint16_t *connection_handle, dataReceivedType *data)
{

  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    if (healthThermometerCollectorContext.fullConf && 
        (healthThermometerCollectorContext.state == HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_TEMPERATURE_MEASUREMENT_INDICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_ENABLE_NOTIFICATION, *status);
      }
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_ENABLE_INTERMEDIATE_TEMPERATURE_NOTIFICATION;
    } 
    else if (healthThermometerCollectorContext.fullConf && 
        (healthThermometerCollectorContext.state == HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_INTERMEDIATE_TEMPERATURE_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_ENABLE_NOTIFICATION, *status);
      }
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_ENABLE_MEASUREMENT_INTERVAL_INDICATION;
    } 
    else if (healthThermometerCollectorContext.fullConf && 
        (healthThermometerCollectorContext.state == HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_MEASUREMENT_INTERVAL_INDICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_ENABLE_INDICATION, *status);
      }
      healthThermometerCollectorContext.fullConf = FALSE; //TBR
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_CONNECTED_IDLE; //TBR
    } 
    
    HT_Collector_EnableNotificationIndication_CB(*status);
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    
    if (*status != BLE_STATUS_SUCCESS) 
    {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_READ, *status);
    } 
    else 
    { 
      if (healthThermometerCollectorContext.dataLen == TEMP_TYPE_READ_SIZE)
      {
        /* Store read Temperature Type characteristic value */      
        HT_Collector_Read_Temperature_Type_CB();
      }
      else if (healthThermometerCollectorContext.dataLen == MEAS_INTER_READ_SIZE)
      {
        /* Store read Measurement Interval characteristic value */
        HT_Collector_Read_Measurement_Interval_CB();
      } 
      else if ((healthThermometerCollectorContext.dataLen == MEAS_INTER_VALID_RANGE_READ_SIZE) &&
               (bReadValidRangeDescr))
      {
        bReadValidRangeDescr = FALSE;
        /* Store read Measurement Interval Valid Range value */
        HT_Collector_Read_Measurement_Interval_Valid_Range_CB();
      }  
      HT_Collector_DataValueRead_CB(*status, healthThermometerCollectorContext.dataLen, healthThermometerCollectorContext.data); 
      
      if ((healthThermometerCollectorContext.fullConf) && (healthThermometerCollectorContext.state == HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_TEMPERATURE_TYPE))
      {
        /* Start read measurement interval characteristic */
        healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_READ_MEASUREMENT_INTERVAL;
      } 
      else if ((healthThermometerCollectorContext.fullConf) && (healthThermometerCollectorContext.state == HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_MEASUREMENT_INTERVAL))
      {
        /* Start enable temparature measurement indication */
        healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_ENABLE_TEMPERATURE_MEASUREMENT_INDICATION;
      } 
      else 
        healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION; //TBR
    }
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
     /* check write response status to the performed write request (APPL_WRITE_HR_CTRL_POINT_CHAR)*/
    HT_Collector_Measurement_Interval_Check_Write_Response_CB(*status);
    break;
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      /* Call user callback (display received notification value) */
      HT_Collector_NotificationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      
      /* Store the received Intermediate Temperature */
      HT_Collector_Intermediate_Temperature_Notification_CB(data->attr_handle, data->data_length, &(data->data_value[0]));         
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Indication Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      HT_Collector_IndicationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      
      if (data->attr_handle == healthThermometerCollectorContext.TempMeasurementCharValHandle)
      {
        /* Store the received Temperature Measurement */
        HT_Collector_Temperature_Measurement_Indication_CB(data->attr_handle, data->data_length, &(data->data_value[0]));      
      }
      else if (data->attr_handle == healthThermometerCollectorContext.MeasurementIntervalCharValHandle)
      {
        /* Store the received Measurement Interval */
        HT_Collector_Measurement_Interval_Indication_CB(data->attr_handle, data->data_length, &(data->data_value[0]));         
      }
    }
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }

}

void Master_HTC_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  HT_Collector_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
    HT_Collector_PinCodeRequired_CB();
  }
}
 
void Master_HTC_PeerDataPermitExchange_CB(uint8_t * procedure, uint16_t * connection_handle, dataPermitExchangeType * data) 
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x \n", *connection_handle, *procedure);
  
}/* end Master_PeerDataPermitExchange_CB() */


/****************** health thermometer collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void HT_Collector_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void HT_Collector_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void HT_Collector_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void HT_Collector_ConnectionParameterUpdateReq_CB(htcConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void HT_Collector_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void HT_Collector_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void HT_Collector_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void HT_Collector_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void HT_Collector_EnableNotificationIndication_CB(uint8_t status))
{
}

WEAK_FUNCTION(void HT_Collector_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

WEAK_FUNCTION(void HT_Collector_IndicationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

WEAK_FUNCTION(void HT_Collector_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & HEALTH_THERMOMETER_COLLECTOR) */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
