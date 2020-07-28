/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : healtt_thermometer_collector.c
* Author             : AMS - AAS, RF Application Team
* Version            : V1.0.0
* Date               : 28-January-2015
* Description        : This file manages all the HTP Master API
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include "math.h"

#include <host_config.h>
#include <hci.h>
#include <hci_le.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>

#if (BLE_CURRENT_PROFILE_ROLES & HEALTH_THERMOMETER_COLLECTOR)

#include <health_thermometer_service.h>
#include <health_thermometer_collector.h>
#include <health_thermometer_collector_config.h>

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

/* Health Thermometer collector context */
healthThermometerCollectorContext_Type healthThermometerCollectorContext; 

/* Private macros ------------------------------------------------------------*/

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
tBleStatus HT_Collector_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<healthThermometerCollectorContext.numpeerServices; i++) {
    if (healthThermometerCollectorContext.peerServices[j] == UUID_TYPE_16) {
      BLUENRG_memcpy(&uuid, &healthThermometerCollectorContext.peerServices[j+5], 2);
      j++;
      BLUENRG_memcpy(start_handle, &healthThermometerCollectorContext.peerServices[j], 2);
      j += 2;
      BLUENRG_memcpy(end_handle, &healthThermometerCollectorContext.peerServices[j], 2);
      j += 2;
      if (healthThermometerCollectorContext.peerServices[j-5] == UUID_TYPE_16) {
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

tBleStatus HT_Collector_Init(htcInitDevType param)
{
  //initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&healthThermometerCollectorContext, 0, sizeof(healthThermometerCollectorContext_Type));//TBR
  BLUENRG_memcpy(healthThermometerCollectorContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  initParam.Master_DeviceDiscovery_CB = Master_HTC_DeviceDiscovery_CB;
  initParam.Master_Connection_CB = Master_HTC_Connection_CB;
  initParam.Master_Pairing_CB = Master_HTC_Pairing_CB;
  initParam.Master_PeerDataExchange_CB = Master_HTC_PeerDataExchange_CB;
  initParam.Master_PeerDataPermitExchange_CB = Master_HTC_PeerDataPermitExchange_CB;
  initParam.Master_ServiceCharacPeerDiscovery_CB = Master_HTC_ServiceCharacPeerDiscovery_CB;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_INITIALIZED;
    healthThermometerCollectorContext.fullConf = FALSE;
  } else {
    healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_UNINITIALIZED;
  }

  return ret;
}

tBleStatus HT_Collector_DeviceDiscovery(htcDevDiscType param)
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
    healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus HT_Collector_SecuritySet(htcSecurityType param)
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

tBleStatus HT_Collector_DeviceConnection(htcConnDevType param)
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
    healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_START_CONNECTION;
  }

  return ret;
}

tBleStatus HT_Collector_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(healthThermometerCollectorContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    healthThermometerCollectorContext.fullConf = FALSE;//TBR-----------
  }
  return ret;
}

tBleStatus HT_Collector_ConnectionParameterUpdateRsp(uint8_t accepted, htcConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(healthThermometerCollectorContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus HT_Collector_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(healthThermometerCollectorContext.connHandle, FALSE);
  return ret;
}

tBleStatus HT_Collector_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end HT_Collector_Clear_Security_Database() */

tBleStatus HT_Collector_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(healthThermometerCollectorContext.connHandle, pinCode);
  
  return ret;
}

tBleStatus HT_Collector_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(healthThermometerCollectorContext.connHandle, 
				  &healthThermometerCollectorContext.numpeerServices, 
				  healthThermometerCollectorContext.peerServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS)
  {
    healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_SERVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus HT_Collector_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if ((uuid_service == HEALTH_THERMOMETER_SERVICE_UUID) || (uuid_service == DEVICE_INFORMATION_SERVICE_UUID)) {
    if (!(healthThermometerCollectorContext.HT_ServicePresent && healthThermometerCollectorContext.DeviceInfoServicePresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = HT_Collector_findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(healthThermometerCollectorContext.connHandle,
				    start_handle,
				    end_handle,
				    &healthThermometerCollectorContext.numCharac,
				    healthThermometerCollectorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS)
    {
      healthThermometerCollectorContext.uuid_searched = uuid_service;
    }
  }
  return ret;
}

/**
  * @brief Discovery Temperature Measurement characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Start_Temperature_Measurement_Client_Char_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

  /* It discovers the characteristic descriptors of the connected Temperature Measurement Characteristic */
  ble_status = Master_GetCharacDescriptors(healthThermometerCollectorContext.connHandle,
                                           healthThermometerCollectorContext.TempMeasurementCharValHandle, /* Temperature measurement chararacteristic: Value Handle */
                                           healthThermometerCollectorContext.TempMeasurementCharValHandle+1, /* Temperature measurement chararacteristic: End Handle */
                                           &healthThermometerCollectorContext.numCharac, 
                                           healthThermometerCollectorContext.charac,
                                           CHARAC_OF_SERVICE_ARRAY_SIZE); 
  return(ble_status);
}

/**
  * @brief Discovery Intermediate Temperature characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Start_Intermediate_Temperature_Client_Char_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

  /* It discovers the characteristic descriptors of the connected Intermediate Temperature Characteristic */
  ble_status = Master_GetCharacDescriptors(healthThermometerCollectorContext.connHandle,
                                           healthThermometerCollectorContext.IntermediateTempCharValHandle, /* Intermediate Temperature chararacteristic: Value Handle */
                                           healthThermometerCollectorContext.IntermediateTempCharValHandle+1, /* Intermediate Temperature chararacteristic: End Handle */
                                           &healthThermometerCollectorContext.numCharac, 
                                           healthThermometerCollectorContext.charac,
                                           CHARAC_OF_SERVICE_ARRAY_SIZE); 

  return(ble_status);
}


/**
  * @brief Discovery Measurement Interval characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Start_Measurement_Interval_Client_Char_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

    /* It discovers the characteristic descriptors of the connected Measurement Interval Characteristic */
    ble_status = Master_GetCharacDescriptors(healthThermometerCollectorContext.connHandle,
                                             healthThermometerCollectorContext.MeasurementIntervalCharValHandle, /* Measurement Interval chararacteristic: Value Handle */
                                             healthThermometerCollectorContext.MeasurementIntervalCharValHandle+2, /* Measurement Interval chararacteristic: End Handle is +2 since there is the valid range descriptor */
                                             &healthThermometerCollectorContext.numCharac, 
                                             healthThermometerCollectorContext.charac,
                                             CHARAC_OF_SERVICE_ARRAY_SIZE); 
  return(ble_status);
}


tBleStatus HT_Collector_ReadDISManufacturerNameChar(void)
{
  tBleStatus ret;

  /* Read Device Info Manufacturer Name Characteristic */
  ret = Master_Read_Value(healthThermometerCollectorContext.connHandle,  
                          healthThermometerCollectorContext.ManufacturerNameCharValHandle,  
                          &healthThermometerCollectorContext.dataLen,  
                          healthThermometerCollectorContext.data,  
                          HT_DIS_MANUFACTURER_NAME_SIZE
                          );
  
  return ret;
}/* end HT_Collector_ReadDISManufacturerNameChar() */

tBleStatus HT_Collector_ReadDISModelNumberChar(void)
{
  tBleStatus ret;
  
  /* Read Device Info Model Number Characteristic */
  ret = Master_Read_Value(healthThermometerCollectorContext.connHandle,  
                          healthThermometerCollectorContext.ModelNumberCharValHandle,  
                          &healthThermometerCollectorContext.dataLen,  
                          healthThermometerCollectorContext.data,  
                          HT_DIS_MODEL_NUMBER_SIZE 
                          );
 return ret; 
}/* end HT_Collector_ReadDISModelNumberChar() */

tBleStatus HT_Collector_ReadDISSystemIDChar(void)
{
  tBleStatus ret;
  
  /* Read Device Info System ID Characteristic */
  ret = Master_Read_Value(healthThermometerCollectorContext.connHandle,  
                          healthThermometerCollectorContext.SystemIDCharValHandle,  
                          &healthThermometerCollectorContext.dataLen,  
                          healthThermometerCollectorContext.data,  
                          HT_DIS_SYSTEM_ID_SIZE 
                          );
  return ret; 
}/* end HT_Collector_ReadDISSystemIDChar() */

/**
  * @brief Enable Temperature Measurement Characteristic for Indication
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Enable_Temperature_Measurement_Char_Indication(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Enable Temperature Measurement Char Descriptor for Indication */
 
  ble_status = Master_NotifIndic_Status(healthThermometerCollectorContext.connHandle,
                                        healthThermometerCollectorContext.TempMeasurementCharValHandle + 1, /* Temperature Measurement Char Descriptor Handle */
                                        FALSE, /* no notification */
                                        TRUE /* enable indication */);
  return(ble_status); 
}/* end HT_Collector_Enable_Temperature_Measurement_Char_Indication() */

/**
  * @brief Enable Intermediate Temperature Characteristic for Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Enable_Intermediate_Temperature_Char_Notification(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Enable Intermediate Temperature Char Descriptor for Notification */
  ble_status = Master_NotifIndic_Status(healthThermometerCollectorContext.connHandle,
                                        healthThermometerCollectorContext.IntermediateTempCharValHandle + 1, /* Intermediate Temperature Char Descriptor Handle */
                                        TRUE, /* enable notification */
                                        FALSE /* no indication */);
  return(ble_status); 
}/* end HT_Collector_Enable_Intermediate_Temperature_Char_Notification() */


/**
  * @brief Enable Measurement Interval Characteristic for Indication
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Enable_Measurement_Interval_Char_Indication(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Enable Measurement Interval Char Descriptor for Indication */
  
  ble_status = Master_NotifIndic_Status(healthThermometerCollectorContext.connHandle,
                                        healthThermometerCollectorContext.MeasurementIntervalCharValHandle + 1, /* Measurement Interval Char Descriptor Handle */
                                        FALSE, /* no notification */
                                        TRUE /* enable indication */);
 
  return(ble_status); 
}/* end HT_Collector_Enable_Measurement_Interval_Char_Indication() */

/**
  * @brief Read Measurement Interval characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Read_Measurement_Interval(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Measurement Interval Characteristic */
  ble_status = Master_Read_Value(healthThermometerCollectorContext.connHandle,  
                                 healthThermometerCollectorContext.MeasurementIntervalCharValHandle,  
                                 &healthThermometerCollectorContext.dataLen,  
                                 healthThermometerCollectorContext.data,  
                                 MEAS_INTER_READ_SIZE);
  
  return(ble_status);  
}/* end HT_Collector_Read_Measurement_Interval() */


/**
  * @brief Write Measurement Interval characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Write_Measurement_Interval(uint16_t writeValue)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
 
  /* Write Measurement Interval Characteristic */
  ble_status =  Master_Write_Value(healthThermometerCollectorContext.connHandle,  /* connection handle */
                                   healthThermometerCollectorContext.MeasurementIntervalCharValHandle , /* attribute value handle */
                                   2, /* charValueLen */
                                   (uint8_t *) &writeValue);
  return(ble_status);  
} /* end HT_Collector_Write_Measurement_Interval() */


/**
  * @brief Read Measurement Interval Valid Range descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Read_Measurement_Interval_Valid_Range_Descr(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Measurement Interval Valid Range Descriptor */
  ble_status = Master_Read_Value(healthThermometerCollectorContext.connHandle,  
                                 healthThermometerCollectorContext.MeasurementIntervalCharValHandle+2,  
                                 &healthThermometerCollectorContext.dataLen,  
                                 healthThermometerCollectorContext.data,  
                                 MEAS_INTER_VALID_RANGE_READ_SIZE);
  
  return(ble_status);   
}

/**
  * @brief Read Temperature Type characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Read_Temperature_Type(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Body Sensor Location Characteristic */
  ble_status = Master_Read_Value(healthThermometerCollectorContext.connHandle,  
                                 healthThermometerCollectorContext.TempTypeCharValHandle,  
                                 &healthThermometerCollectorContext.dataLen,  
                                 healthThermometerCollectorContext.data,  
                                 TEMP_TYPE_READ_SIZE);
  return(ble_status);  
}/* end HT_Collector_Read_Temperature_Type */

/* This function start the connection and the configuration of the health thermometer collector device */
uint8_t HT_Collector_ConnConf(htcConnDevType connParam)
{
  uint8_t ret;

  ret = HT_Collector_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    healthThermometerCollectorContext.fullConf = TRUE;
  }

  return ret;
}


void HT_Collector_StateMachine(void)
{
  uint8_t ret;

  switch(healthThermometerCollectorContext.state) {
  case HEALTH_THERMOMETER_COLLECTOR_UNINITIALIZED:
    /* Nothing to do */
    break;
  case HEALTH_THERMOMETER_COLLECTOR_CONNECTED_IDLE:
    {
    }
    break;
  case HEALTH_THERMOMETER_COLLECTOR_INITIALIZED:
    /* Nothing to do */
    break;
  case HEALTH_THERMOMETER_COLLECTOR_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case HEALTH_THERMOMETER_COLLECTOR_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case HEALTH_THERMOMETER_COLLECTOR_CONNECTED:
    {
      ret = HT_Collector_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_SERVICE_DISCOVERY;
      } else {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_SERVICE_DISCOVERY, ret);
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEALTH_THERMOMETER_COLLECTOR_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case HEALTH_THERMOMETER_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY:
    {
      ret = HT_Collector_DiscCharacServ(HEALTH_THERMOMETER_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY;
      } else {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_CHARAC_DISCOVERY, ret);
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEALTH_THERMOMETER_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY:
    /* Waiting the end of the Health Thermometer Measurement Characteristic discovery */ 
    break;
       
  case HEALTH_THERMOMETER_COLLECTOR_ENABLE_TEMPERATURE_MEASUREMENT_INDICATION:                 
   {
      ret = HT_Collector_Enable_Temperature_Measurement_Char_Indication();
      if (ret == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_TEMPERATURE_MEASUREMENT_INDICATION;
      } else {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
      }
    }
    break; 
  case HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_TEMPERATURE_MEASUREMENT_INDICATION:             
     break;
     
  case HEALTH_THERMOMETER_COLLECTOR_ENABLE_INTERMEDIATE_TEMPERATURE_NOTIFICATION:       
    {
      ret = HT_Collector_Enable_Intermediate_Temperature_Char_Notification();
      if (ret == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_INTERMEDIATE_TEMPERATURE_NOTIFICATION;
      } else {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
      }
    }
    break; 
  case HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_INTERMEDIATE_TEMPERATURE_NOTIFICATION:       
     break;
    
  case HEALTH_THERMOMETER_COLLECTOR_ENABLE_MEASUREMENT_INTERVAL_INDICATION:
    {
      ret = HT_Collector_Enable_Measurement_Interval_Char_Indication();
      if (ret == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_MEASUREMENT_INTERVAL_INDICATION;
      } else {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_ENABLE_INDICATION, ret);
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_MEASUREMENT_INTERVAL_INDICATION:
    /* Waiting the end of the Tx Power Enable Notification */
    break;
    
  case HEALTH_THERMOMETER_COLLECTOR_READ_TEMPERATURE_TYPE:
    {
      ret = HT_Collector_Read_Temperature_Type();
      if (ret == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_TEMPERATURE_TYPE;
      } else {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_READ, ret);
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_TEMPERATURE_TYPE:
    /* Waiting the end of the read  procedure */
    break;
    
  case HEALTH_THERMOMETER_COLLECTOR_READ_MEASUREMENT_INTERVAL:
    {
      ret = HT_Collector_Read_Measurement_Interval();
      if (ret == BLE_STATUS_SUCCESS) {
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_MEASUREMENT_INTERVAL;
      } else {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_READ, ret);
	healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_MEASUREMENT_INTERVAL:
    /* Waiting the end of the read  procedure */
    break; 
    
  case HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION:
    {
      ret = HT_Collector_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	HT_Collector_FullConfError_CB(HTC_ERROR_IN_DISCONNECTION, ret);
      }
      healthThermometerCollectorContext.state = HEALTH_THERMOMETER_COLLECTOR_INITIALIZED;
    }
    break;
  }
}


/**
  * @brief  Read the Characteristic Value for Measurement Interval Char
  * @param [in] None
  * @retval Error Code
  */
void HT_Collector_Read_Measurement_Interval_CB(void)
{  
  healthThermometerCollectorContext.MeasurementIntervalValue = (uint16_t)(healthThermometerCollectorContext.data[1] <<8)
                                                        | healthThermometerCollectorContext.data[0];
  if (healthThermometerCollectorContext.MeasurementIntervalValue == 0)
  {
    HT_INFO_MSG(profiledbgfile, "No Periodic Measurement.\n");    
  }
  else  
  {
    HT_INFO_MSG(profiledbgfile, "Measurement Interval Char value: %d s\n", healthThermometerCollectorContext.MeasurementIntervalValue);
  }
}

/**
 * @brief  It checks the write response status and error related to the 
 *         Measurement_Interval characteristic write request     
 *         It should be called within the Profile event handler, on EVT_BLUE_GATT_ERROR_RESP, 
 *         EVT_BLUE_GATT_PROCEDURE_COMPLETE (TBR?)  
 * @param  err_code: write response error code (from Health Thermometer Sensor) 
 */
void HT_Collector_Measurement_Interval_Check_Write_Response_CB(uint8_t err_code)
{ 
  HT_INFO_MSG(profiledbgfile,"HT_Collector_Measurement_Interval_Check_Write_Response_CB().\n");
  
  if (err_code == 0)
  {
    HT_INFO_MSG(profiledbgfile, "Measurement Interval Value Write procedure completed with success!\n");
  }
  else if (err_code == OUT_OF_RANGE) 
  {
    HT_INFO_MSG(profiledbgfile,"Measurement Interval Value Out of Range!\n");
  }
  else 
  {
    HT_INFO_MSG(profiledbgfile, "Error during the Write Characteristic Value Procedure!\n");
  }

}/* end HT_Collector_Measurement_Interval_Check_Write_Response_CB() */

/**
  * @brief  Read the Value for Measurement Interval Valid Range Descriptor
  * @param [in] None
  * @retval Error Code
  */
void HT_Collector_Read_Measurement_Interval_Valid_Range_CB(void)
{  
  uint8_t index;
  tValidRange validRange;
  
  index = 1;
  validRange.minValidInterval = (uint16_t)(healthThermometerCollectorContext.data[index] <<8) | 
                                healthThermometerCollectorContext.data[index-1];  index+=2;
  validRange.maxValidInterval = (uint16_t)(healthThermometerCollectorContext.data[index] <<8) | 
                                healthThermometerCollectorContext.data[index-1];  
  HT_INFO_MSG(profiledbgfile, "Valid Range Descriptor - MIN Value: %d s\n", validRange.minValidInterval);
  HT_INFO_MSG(profiledbgfile, "Valid Range Descriptor - MAX Value: %d s\n", validRange.maxValidInterval);  
  
  healthThermometerCollectorContext.MeasurementIntervalValidRangeValue = validRange;     
    
} /* end HT_Collector_Read_Measurement_Interval_Valid_Range_CB() */

/**
  * @brief  Parse and print the value of the Temperature Type Characteristic
  * @param [in] tempType: the Temperature type value
  * @retval None
  */
static void printTemperatureType(uint8_t tempType) 
{ 
  switch(tempType) 
  {
    case TEMPERATURE_TYPE_ARMPIT: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Armpit\n");
      break;
    }  
    case TEMPERATURE_TYPE_BODY: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Body (general)\n");
      break;
    } 
    case TEMPERATURE_TYPE_EAR: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Ear (usually ear lobe)\n");
      break;
    } 
    case TEMPERATURE_TYPE_FINGER: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Finger\n");
      break;
    } 
    case TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Gastro-Intestinal Tract\n");
      break;
    } 
    case TEMPERATURE_TYPE_MOUTH: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Mouth\n");
      break;
    } 
    case TEMPERATURE_TYPE_RECTUM: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Rectum\n");
      break;
    } 
    case TEMPERATURE_TYPE_TOE: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Toe\n");
      break;
    } 
    case TEMPERATURE_TYPE_TYMPANUM: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: Tympanum (ear drum)\n");
      break;
    } 
    default: {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Value: RFU\n");
      break;
    }
  }
}

/**
  * @brief  Read the Characteristic Value for Temperature Type Char
  * @param [in] None
  * @retval Error Code
  */
void HT_Collector_Read_Temperature_Type_CB(void)
{
  healthThermometerCollectorContext.TempTypeValue = healthThermometerCollectorContext.data[0];
  printTemperatureType(healthThermometerCollectorContext.TempTypeValue);
} /* end HT_Collector_Read_Temperature_Type_CB() */


static tTimestamp printTimeStamp(uint8_t index, uint8_t *value) 
{
  tTimestamp TimeStamp;
  
  index += 2;
  TimeStamp.year = (uint16_t)(value[index]<<8) | value[index-1];
  index++;
  TimeStamp.month = value[index];
  index++;
  TimeStamp.day = value[index];
  index++;
  TimeStamp.hours = value[index];
  index++;
  TimeStamp.minutes = value[index];
  index++;
  TimeStamp.seconds = value[index];  
  HT_INFO_MSG(profiledbgfile,"Time Stamp: %04d-%02d-%02d, %02d:%02d:%02d\n",  TimeStamp.year, 
                     TimeStamp.month, TimeStamp.day, TimeStamp.hours, TimeStamp.minutes, TimeStamp.seconds);  
  return (TimeStamp);
}
/*
float convert_Temperature_Measurement(uint32_t mantissa, int8_t exponent)
{
  float app;
  
 // if (exponent == 0xFF)
 //   exponent = -1;
  app = mantissa * pow(10.0f, exponent);
  return app; //(mantissa * pow(10.0f, exponent));
}
*/

/**
* @brief  It stores the Temperature Measurement Characteristics Indication
* @param  attr_handle: Temperature Measurement characteristic handle
* @param  data_lenght: Temperature Measurement characteristic value lenght
* @param  value: Temperature Measurement characteristic value 
*/
void HT_Collector_Temperature_Measurement_Indication_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value)
{
  uint8_t index;
  int8_t exponent; 
  int32_t mantissa;
  double TempMeasFinal;
  tBleStatus retval = BLE_STATUS_SUCCESS;
  /* Temperature measurement characteristic */
  TemperatureMeasurementVal TempMeasurement;
  
  /* Check indication value handle: Temperature measurement characteristic indication */ 
  if (attr_handle == healthThermometerCollectorContext.TempMeasurementCharValHandle)
  {
    HT_INFO_MSG(profiledbgfile, "HT_Collector_Temperature_Measurement_Indication_CB()\n");
    BLUENRG_memset(&TempMeasurement, 0 , sizeof(TempMeasurement)); 
    
    /* To check the Flags value field to determine the contents of  
       Temperature Measurement and the format of Char value field. */
    index = 0; 
    TempMeasurement.flags = value[index];  
   
    /** 
    * The Temperature Measurement Field is a FLOAT-Type 32-bit data type that contains
    * an 8-bit signed exponent to base 10, followed by a 24-bit signed integer (mantissa).
    */
    index += 4;
    exponent = (int8_t) value[index];
    mantissa = (uint32_t) ((value[index-1]<<16) | (value[index-2]<<8) | value[index-3]); 
    TempMeasurement.Temp_Meas_Mantissa = mantissa;
    TempMeasurement.Temp_Meas_Exponent = exponent;
    HT_INFO_MSG(profiledbgfile, "Temperature Measurement Value Mantissa: 0x%08x (%d)\n", mantissa, mantissa);
    HT_INFO_MSG(profiledbgfile, "Temperature Measurement Value Exponent: 0x%02x (%d)\n", exponent, exponent);
     
    if (mantissa == 0x007FFFFF)
    {
      HT_INFO_MSG(profiledbgfile, "Temperature Measurement Value: NaN.\n");
    }
    else   
    {               
      /* Temperature Measurement Value = Temp_Meas_Mantissa * pow(10, Temp_Meas_Exponent)*/
      TempMeasFinal = (double)(mantissa * pow(10, exponent)); 
      /* If the first bit is 1 the Temperature Meas Value is in units 
         Fahrenheit else it is in units Celsius */
      if (TempMeasurement.flags & FLAG_TEMPERATURE_UNITS_FARENHEIT)
      {  
        HT_INFO_MSG(profiledbgfile, "Temperature Measurement Value (Fahrenheit): %g F\n", TempMeasFinal);
        TempMeasurement.Temp_Meas_Unit = UNIT_TEMP_FAHRENHEIT;
      } 
      else 
      { /* Temperature Measurement in Celsius */
        HT_INFO_MSG(profiledbgfile, "Temperature Measurement Value (Celsius): %g C\n", TempMeasFinal);
        TempMeasurement.Temp_Meas_Unit = UNIT_TEMP_CELSIUS;
      }   
    }  
    /* Time Stamp is present */
    if ((TempMeasurement.flags & FLAG_TIMESTAMP_PRESENT ))  
    {  
      TempMeasurement.Time_Stamp = printTimeStamp(index, value); 
      index += 7;
    }
    else   
      HT_INFO_MSG(profiledbgfile, "Time Stamp is not present\n");  

    /* Temperature Type is present */
    if ((TempMeasurement.flags & FLAG_TEMPERATURE_TYPE)){    
      index++;
      TempMeasurement.Temp_Type = value[index];
      printTemperatureType(TempMeasurement.Temp_Type);  
    } else {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Field is not present.\n");
    }  
    BLUENRG_memcpy(&healthThermometerCollectorContext.TempMeasurementValue, &TempMeasurement, sizeof(TempMeasurement));
    
    /* Send confirm for indication */
    retval =  aci_gatt_confirm_indication(healthThermometerCollectorContext.connHandle);  /* connection handle */  
    if(retval == BLE_STATUS_SUCCESS)
    {
      HT_INFO_MSG(profiledbgfile,"HT_Collector_Temperature_Measurement_Indication_CB(), aci_gatt_confirm_indication() OK\n");
    }
    else
    {
      HT_INFO_MSG(profiledbgfile,"HT_Collector_Temperature_Measurement_Indication_CB(), aci_gatt_confirm_indication() Failed %04X \n", retval);
    }
  }

} /* end HT_Collector_Temperature_Measurement_Indication_CB() */

/**
* @brief  It stores the Intermediate Temperature Characteristics Notification
* @param  attr_handle: Intermediate Temperature characteristic handle
* @param  data_lenght: Intermediate Temperature characteristic value lenght
* @param  value: Intermediate Temperature characteristic value 
*/
void HT_Collector_Intermediate_Temperature_Notification_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value)
{
  uint8_t index;
  int8_t exponent; 
  int32_t mantissa;
  double TempMeasFinal;
  /* Intermediate Temperature characteristic */
  TemperatureMeasurementVal TempMeasurement;
  
  /* Check notification value handle: Intermediate Temperature characteristic Notification */ 
  if (attr_handle == healthThermometerCollectorContext.IntermediateTempCharValHandle)
  {
    HT_INFO_MSG(profiledbgfile, "HT_Collector_Intermediate_Temperature_Notification_CB()\n");
    BLUENRG_memset(&TempMeasurement, 0 , sizeof(TempMeasurement)); 
    
    /* To check the Flags value field to determine the contents of  
       Intermediate Temperature and the format of Char value field. */
    index = 0; 
    TempMeasurement.flags = value[index];  
    
    /** 
    * The Temperature Measurement Field is a FLOAT-Type 32-bit data type that contains
    * an 8-bit signed exponent to base 10, followed by a 24-bit signed integer (mantissa).
    */
    index += 4;
    exponent = (int8_t) value[index];
    mantissa = (uint32_t) ((value[index-1]<<16) | (value[index-2]<<8) | value[index-3]); 
    TempMeasurement.Temp_Meas_Mantissa = mantissa;
    TempMeasurement.Temp_Meas_Exponent = exponent;
    HT_INFO_MSG(profiledbgfile, "Intermediate Temperature Value Mantissa: 0x%08x (%d)\n", mantissa, mantissa);
    HT_INFO_MSG(profiledbgfile, "Intermediate Temperature Value Exponent: 0x%02x (%d)\n", exponent, exponent);
     
    if (mantissa == 0x007FFFFF)
    {
      HT_INFO_MSG(profiledbgfile, "Intermediate Temperature Value: NaN.\n");
    }
    else   
    {               
      /* Temperature Measurement Value = Temp_Meas_Mantissa * pow(10, Temp_Meas_Exponent)*/
      TempMeasFinal = (double)(mantissa * pow(10, exponent)); 
      /* If the first bit is 1 the Temperature Meas Value is in units 
         Fahrenheit else it is in units Celsius */
      if (TempMeasurement.flags & FLAG_TEMPERATURE_UNITS_FARENHEIT)
      {  
        HT_INFO_MSG(profiledbgfile, "Intermediate Temperature Value (Fahrenheit): %g F\n", TempMeasFinal);
        TempMeasurement.Temp_Meas_Unit = UNIT_TEMP_FAHRENHEIT;
      } 
      else 
      { /* Temperature Measurement in Celsius */
        HT_INFO_MSG(profiledbgfile, "Intermediate Temperature Value (Celsius): %g C\n", TempMeasFinal);
        TempMeasurement.Temp_Meas_Unit = UNIT_TEMP_CELSIUS;
      }   
    }
    
    /* Time Stamp is present */
    if ((TempMeasurement.flags & FLAG_TIMESTAMP_PRESENT ))   
    { 
      TempMeasurement.Time_Stamp = printTimeStamp(index, value); 
      index += 7;
    }  
    else   
    {
      HT_INFO_MSG(profiledbgfile, "Time Stamp is not present\n");  
    }

    /* Temperature Type is present */
    if ((TempMeasurement.flags & FLAG_TEMPERATURE_TYPE)){    
      index++;
      TempMeasurement.Temp_Type = value[index];
      printTemperatureType(TempMeasurement.Temp_Type);  
    } else {
      HT_INFO_MSG(profiledbgfile, "Temperature Type Field is not present.\n");
    }  
    BLUENRG_memcpy(&healthThermometerCollectorContext.IntermediateTempValue, &TempMeasurement, sizeof(TempMeasurement));
  }

} /* end HT_Collector_Intermediate_Temperature_Notification_CB() */


/**
* @brief  It stores the Measurement Interval Characteristics Indication
* @param  attr_handle: Measurement Interval characteristic handle
* @param  data_lenght: Measurement Interval characteristic value lenght
* @param  value: Measurement Interval characteristic value 
*/
void HT_Collector_Measurement_Interval_Indication_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value)
{ 
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  if (attr_handle == healthThermometerCollectorContext.MeasurementIntervalCharValHandle)
  {  
    healthThermometerCollectorContext.MeasurementIntervalValue = (uint16_t)(value[1]<<8) | value[0];
    if (healthThermometerCollectorContext.MeasurementIntervalValue == 0)
    {
      HT_INFO_MSG(profiledbgfile, "No Periodic Measurement.\n");
    }
    else  
    {
      HT_INFO_MSG(profiledbgfile, "Measurement Interval Char value: %d s\n", healthThermometerCollectorContext.MeasurementIntervalValue);  
    }
    
    /* Send confirm for indication */
    retval =  aci_gatt_confirm_indication(healthThermometerCollectorContext.connHandle);  /* connection handle */  
    if(retval == BLE_STATUS_SUCCESS)
    {
      HT_INFO_MSG(profiledbgfile,"HT_Collector_Measurement_Interval_Indication_CB(), aci_gatt_confirm_indication() OK\n");
    }
    else
    {
      HT_INFO_MSG(profiledbgfile,"HT_Collector_Measurement_Interval_Indication_CB(), aci_gatt_confirm_indication() Failed %04X \n", retval);
    }    
  }
} /* end HT_Collector_Measurement_Interval_Indication_CB() */


/****************** health thermometer collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void HT_Collector_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & HEALTH_THERMOMETER_COLLECTOR) */
