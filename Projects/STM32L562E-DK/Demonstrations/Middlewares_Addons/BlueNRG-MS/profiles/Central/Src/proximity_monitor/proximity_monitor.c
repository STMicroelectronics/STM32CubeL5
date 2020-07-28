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
*   FILENAME        -  proximity_monitor.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/01/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    proximity monitor APIs(connect, config charac, enable notification...)
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the proximity monitor profile central role.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include "debug.h"
#include "timer.h"
#include "uuid.h"
#include "hci.h"
#include "hci_le.h"
#include "hci_tl.h"
#include "hal.h"
#include "master_basic_profile.h"

#if (BLE_CURRENT_PROFILE_ROLES & PROXIMITY_MONITOR)

#include "string.h"
#include "bluenrg_gatt_server.h"
#include "proximity_monitor.h"

#define WEAK_FUNCTION(x) __weak x

#ifndef PXM_TEST
#define PXM_TEST 1
#endif

/******************************************************************************
* Macro Declarations
******************************************************************************/

/* Proximity Monitor States */
#define PROXIMITY_MONITOR_UNINITIALIZED                  0x0000
#define PROXIMITY_MONITOR_CONNECTED_IDLE                 0x0001
#define PROXIMITY_MONITOR_INITIALIZED                    0x0002
#define PROXIMITY_MONITOR_DEVICE_DISCOVERY               0x0003
#define PROXIMITY_MONITOR_START_CONNECTION               0x0004
#define PROXIMITY_MONITOR_CONNECTED                      0x0005
#define PROXIMITY_MONITOR_SERVICE_DISCOVERY              0x0006
#define PROXIMITY_MONITOR_LL_CHARAC_DISCOVERY            0x0007
#define PROXIMITY_MONITOR_WAIT_LL_CHARAC_DISCOVERY       0x0008
#define PROXIMITY_MONITOR_IA_CHARAC_DISCOVERY            0x0009
#define PROXIMITY_MONITOR_WAIT_IA_CHARAC_DISCOVERY       0x000A
#define PROXIMITY_MONITOR_TP_CHARAC_DISCOVERY            0x000B
#define PROXIMITY_MONITOR_WAIT_TP_CHARAC_DISCOVERY       0x000C
#define PROXIMITY_MONITOR_TP_CHARAC_DESC_DISCOVERY       0x000D
#define PROXIMITY_MONITOR_WAIT_TP_CHARAC_DESC_DISCOVERY  0x000E
#define PROXIMITY_MONITOR_ENABLE_TP_NOTIFICATION         0x000F
#define PROXIMITY_MONITOR_WAIT_ENABLE_TP_NOTIFICATION    0x0010
#define PROXIMITY_MONITOR_CONFIGURATION_REPORTER         0x0011
#define PROXIMITY_MONITOR_WAIT_LL_ALERT_LEVEL_CONFIG     0x0012
#define PROXIMITY_MONITOR_IA_ALERT_LEVEL_CONFIG          0x0013
#define PROXIMITY_MONITOR_CONN_PARAM_UPDATE_REQ          0x0014
#define PROXIMITY_MONITOR_DISCONNECTION                  0x0015
#define PROXIMITY_MONITOR_READ_TX_POWER                  0x0016
#define PROXIMITY_MONITOR_WAIT_READ_TX_POWER             0x0017

#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 50

/******************************************************************************
* type definitions
******************************************************************************/

typedef struct presentFormatS {
  uint8_t format;
  int8_t exponent;
  uint16_t uint;
  uint8_t namespace;
  uint16_t description;
} presentFormatType;

/* Proximity Monitor Context variable */
typedef struct proximityMonitorContextS {
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numReporterServices;
  uint8_t immediateAlertPresent;
  uint8_t txPowerPresent;
  uint8_t reporterServices[PRIMARY_SERVICES_ARRAY_SIZE];
  uint16_t llAlertLevelValHandle;
  uint16_t iaAlertLevelValHandle;
  uint16_t txPowerLevelValHandle;
  uint16_t txPowerLevelEndCharHandle;
  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
  presentFormatType txPwrLvlDesc;
  uint8_t data[20];
  uint16_t dataLen;
  uint8_t iaAlertLevel;
  uint8_t llAlertLevel;
  uint8_t fullConf;
  uint8_t alertPathLossEnabled;
  int8_t pathLossTheshold;
  uint32_t pathLossTimeout;
  uint32_t pathLossStartTime;
  int8_t txPower;
  uint8_t iaAlertSent;
  uint8_t iaAlertLevelAppl;
} proximityMonitorContext_Type;

/******************************************************************************
* Variable Declarations
******************************************************************************/

/* proximity monitor context */
proximityMonitorContext_Type proximityMonitorContext;

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* Static Functions
*******************************************************************************/
static uint8_t findHandles(uint16_t uuid_service, uint16_t *start_handle, 
			   uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<proximityMonitorContext.numReporterServices; i++) {
    if (proximityMonitorContext.reporterServices[j] == UUID_TYPE_16) {
      uuid = LE_TO_HOST_16(&proximityMonitorContext.reporterServices[j+5]);
      j++;
      *start_handle = LE_TO_HOST_16(&proximityMonitorContext.reporterServices[j]);
      j += 2;
      *end_handle = LE_TO_HOST_16(&proximityMonitorContext.reporterServices[j]);
      j += 2;
      if (proximityMonitorContext.reporterServices[j-5] == UUID_TYPE_16) {
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

static uint8_t findCharacHandle(uint16_t uuid_searched, uint8_t numCharac, uint8_t *charac)
{
  uint8_t ret, i, index, sizeElement;
  uint16_t uuid_charac;

  ret = BLE_STATUS_SUCCESS;

  index = 0;
  for (i=0; i<numCharac; i++) { 
    if (charac[index] == 1) {
      sizeElement = 8;
      uuid_charac = LE_TO_HOST_16(&charac[index+6]);
    } else {
      sizeElement = 22;
    }

    /* Link Loss Service Alert Level Charac */
    if ((uuid_searched == LINK_LOSS_SERVICE_UUID) && 
	(uuid_charac == ALERT_LEVEL_CHARACTERISTIC_UUID)) {
      proximityMonitorContext.llAlertLevelValHandle = LE_TO_HOST_16(&charac[index+4]);
    } else {
      ret = HCI_INVALID_PARAMETERS;
    }

    /* Immediate Alert Service Alert Level Charac */
    if ((uuid_searched == IMMEDIATE_ALERT_SERVICE_UUID) && 
	(uuid_charac == ALERT_LEVEL_CHARACTERISTIC_UUID)) {
      proximityMonitorContext.iaAlertLevelValHandle = LE_TO_HOST_16(&charac[index+4]);
    } else {
      ret = HCI_INVALID_PARAMETERS;
    }

    /* TX Power Service TX Power Level Charac */
    if ((uuid_searched == TX_POWER_SERVICE_UUID) && 
	(uuid_charac == TX_POWER_LEVEL_CHARACTERISTIC_UUID)) {
      proximityMonitorContext.txPowerLevelValHandle = LE_TO_HOST_16(&charac[index+4]);
    } else {
      ret = HCI_INVALID_PARAMETERS;
    }

    index += sizeElement;
  }

  return ret ;
}

/*******************************************************************************
* exported SAP
*******************************************************************************/

uint8_t PXM_Init(pxmInitDevType param)
{
  initDevType initParam;
  uint8_t ret;

  memcpy(proximityMonitorContext.public_addr, param.public_addr, 6);
  memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    proximityMonitorContext.state = PROXIMITY_MONITOR_INITIALIZED;
    proximityMonitorContext.fullConf = FALSE;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device initialized\r\n");
  } else {
    proximityMonitorContext.state = PROXIMITY_MONITOR_UNINITIALIZED;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device not initialized\r\n");
  }

  return ret;
}

uint8_t PXM_DeviceDiscovery(pxmDevDiscType param)
{
  devDiscoveryType devDiscParam;
  uint8_t ret;
  
  devDiscParam.procedure = LIMITED_DISCOVERY_PROCEDURE;
  devDiscParam.scanInterval = param.scanInterval;
  devDiscParam.scanWindow = param.scanWindow;
  devDiscParam.own_address_type = param.own_addr_type;
  ret = Master_DeviceDiscovery(&devDiscParam);
  if (ret != BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure failed\n");
  } else {
    proximityMonitorContext.state = PROXIMITY_MONITOR_DEVICE_DISCOVERY;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure started\n");
  }
  return ret;
}

uint8_t PXM_SecuritySet(pxmSecurityType param)
{
  securitySetType secParam;
  uint8_t ret;

  secParam.ioCapability = param.ioCapability;
  secParam.mitm_mode = param.mitm_mode;
  secParam.oob_enable = param.oob_enable;
  if (secParam.oob_enable == OOB_AUTH_DATA_PRESENT)
    memcpy(secParam.oob_data, param.oob_data, 16);
  secParam.use_fixed_pin = param.use_fixed_pin;
  if (secParam.use_fixed_pin == USE_FIXED_PIN_FOR_PAIRING)
    secParam.fixed_pin = param.fixed_pin;
  else 
    secParam.fixed_pin = 0;
  secParam.bonding_mode = param.bonding_mode;
  ret = Master_SecuritySet(&secParam);
  if (ret != BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Set Security failed\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Set Security success\r\n");
  }

  return ret;
}

uint8_t PXM_DeviceConnection(pxmConnDevType param)
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
  memcpy(master_param.peer_addr, param.peer_addr, 6);
  master_param.own_addr_type = param.own_addr_type;
  master_param.conn_min_interval = param.conn_min_interval;
  master_param.conn_max_interval = param.conn_max_interval;
  master_param.conn_latency = param.conn_latency;
  master_param.supervision_timeout = param.supervision_timeout;
  master_param.min_conn_length = param.min_conn_length;
  master_param.max_conn_length = param.max_conn_length;
  ret = Master_DeviceConnection(&master_param);
  if (ret == BLE_STATUS_SUCCESS) {
    proximityMonitorContext.state = PROXIMITY_MONITOR_START_CONNECTION;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Connection success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Connection failed\r\n");
  }

  return ret;
}

uint8_t PXM_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(proximityMonitorContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Disconnection success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Disconnection failed\r\n");
  }
  return ret;
}

uint8_t PXM_ConnectionParameterUpdateRsp(uint8_t accepted, pxmConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(proximityMonitorContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Connection Update Parameter Response success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Connection Update Parameter Response failed\r\n");
  }

  return ret;
}

uint8_t PXM_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(proximityMonitorContext.connHandle, FALSE);
  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Pairing Procedure success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Pairing Procedure failed\r\n");
  }
  return ret;
}

uint8_t PXM_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(proximityMonitorContext.connHandle, pinCode);
  
  return ret;
}

uint8_t PXM_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(proximityMonitorContext.connHandle, 
				  &proximityMonitorContext.numReporterServices, 
				  proximityMonitorContext.reporterServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret != BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Service Discovery Procedure failed\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Service Discovery Procedure success\r\n");
    proximityMonitorContext.state = PROXIMITY_MONITOR_SERVICE_DISCOVERY;
  }

  
  return ret;
}

uint8_t PXM_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if ((uuid_service == IMMEDIATE_ALERT_SERVICE_UUID) || (uuid_service == TX_POWER_SERVICE_UUID)) {
    if (!(proximityMonitorContext.immediateAlertPresent && proximityMonitorContext.txPowerPresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(proximityMonitorContext.connHandle,
				    start_handle,
				    end_handle,
				    &proximityMonitorContext.numCharac,
				    proximityMonitorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS) {
      proximityMonitorContext.uuid_searched = uuid_service;
      PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic of a Service success\r\n");
    } else {
      PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic of a Service failed\r\n");
    }
  }

  return ret;
}

uint8_t PXM_TxPwrLvl_DiscCharacDesc(void)
{
  uint8_t ret;

  ret = Master_GetCharacDescriptors(proximityMonitorContext.connHandle,
				    proximityMonitorContext.txPowerLevelValHandle,
				    proximityMonitorContext.txPowerLevelEndCharHandle,
				    &proximityMonitorContext.numCharac,
				    proximityMonitorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic Descriptors of TX Power Level success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic Descriptors of TX Power Level failed\r\n");
  }

  return ret;
}

uint8_t PXM_ConfigureLinkLossAlert(uint8_t level)
{
  uint8_t ret;

  ret = Master_Write_Value(proximityMonitorContext.connHandle, 
			   proximityMonitorContext.llAlertLevelValHandle, 1, &level);

  if (ret == BLE_STATUS_SUCCESS) {
    proximityMonitorContext.llAlertLevel = level;
    PROFILE_MESG_DBG(profiledbgfile,"**** Write Alert Level success\r\n");    
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Write Alert Level failed\r\n");    
  }

  return ret;
}

uint8_t PXM_ReadTxPower(void)
{
  uint8_t ret;

  ret = Master_Read_Value(proximityMonitorContext.connHandle, 
			  proximityMonitorContext.txPowerLevelValHandle,
			  &proximityMonitorContext.dataLen,
			  proximityMonitorContext.data,
			  20);
  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Read Tx Power success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Read Tx Power failed\r\n");
  }

  return ret;
}

uint8_t PXM_EnableTxPowerNotification(void)
{
  uint8_t ret;

  ret = Master_NotifIndic_Status(proximityMonitorContext.connHandle, 
				 proximityMonitorContext.txPowerLevelValHandle+1,
				 TRUE, FALSE);

  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Enable Tx Power Notification success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Enable Tx Power Notification failed\r\n");
  }

  return ret;
}

uint8_t PXM_ConfigureImmediateAlert(uint8_t level)
{
  uint8_t ret;

  if (!(proximityMonitorContext.immediateAlertPresent && proximityMonitorContext.txPowerPresent)) {
      PROFILE_MESG_DBG(profiledbgfile,"**** Immediate Alert Configuration failed\r\n");
      return HCI_INVALID_PARAMETERS;
  }

  ret = Master_WriteWithoutResponse_Value(proximityMonitorContext.connHandle,  
					  proximityMonitorContext.iaAlertLevelValHandle,
					  1, &level);
  if (ret == BLE_STATUS_SUCCESS) {
    proximityMonitorContext.iaAlertLevel = level;
    PROFILE_MESG_DBG(profiledbgfile,"**** Immediate Alert Configuration success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Immediate Alert Configuration failed\r\n");
  }

  return ret;
}

uint8_t PXM_GetRSSI(int8_t *value)
{
  uint8_t ret;

  ret = hci_read_rssi(&proximityMonitorContext.connHandle, value);
  if(ret != BLE_STATUS_SUCCESS) {
    *value = 0;
    PROFILE_MESG_DBG(profiledbgfile,"**** Get RSSI procedure failed\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Get RSSI procedure success\r\n");
  }

  return ret;
}

/* This function start the connection and the configuration of the proximity reported device */
uint8_t PXM_ConnConf(pxmConnDevType connParam, pxmConfDevType confParam)
{
  uint8_t ret;

  ret = PXM_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    proximityMonitorContext.fullConf = TRUE;
    proximityMonitorContext.llAlertLevel = confParam.llAlertLevel;
    proximityMonitorContext.iaAlertLevelAppl = confParam.iaAlertLevel;
    proximityMonitorContext.alertPathLossEnabled = confParam.alertPathLossEnabled;
    proximityMonitorContext.pathLossTheshold = confParam.pathLossTheshold;
    proximityMonitorContext.pathLossTimeout = confParam.pathLossTimeout;
  }

  return ret;
}

void PXM_StateMachine(void)
{
  uint8_t ret;

  switch(proximityMonitorContext.state) {
  case PROXIMITY_MONITOR_UNINITIALIZED:
    /* Nothing to do */
    break;
  case PROXIMITY_MONITOR_CONNECTED_IDLE:
    {
      int8_t rssi;
      uint8_t pathLoss;

      if ((proximityMonitorContext.immediateAlertPresent && proximityMonitorContext.txPowerPresent) && 
	  proximityMonitorContext.alertPathLossEnabled) {
	if ((HAL_GetTick() - proximityMonitorContext.pathLossStartTime) > proximityMonitorContext.pathLossTimeout) {
	  proximityMonitorContext.pathLossStartTime = HAL_GetTick();
	  PXM_GetRSSI(&rssi);
	  pathLoss = proximityMonitorContext.txPower - rssi;
	  if (pathLoss > proximityMonitorContext.pathLossTheshold) {
	    proximityMonitorContext.iaAlertSent = TRUE;
	    proximityMonitorContext.iaAlertLevel = proximityMonitorContext.iaAlertLevelAppl;
	    PXM_PathLossAlert(proximityMonitorContext.iaAlertLevelAppl);
	  } else {
	    if (proximityMonitorContext.iaAlertSent) {
	      proximityMonitorContext.state = PROXIMITY_MONITOR_IA_ALERT_LEVEL_CONFIG;
	      proximityMonitorContext.iaAlertLevel = 0;
	      PXM_PathLossAlert(NO_ALERT);
	    }
	  }
	}
      }
    }
    break;
  case PROXIMITY_MONITOR_INITIALIZED:
    /* Nothing to do */
    break;
  case PROXIMITY_MONITOR_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case PROXIMITY_MONITOR_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case PROXIMITY_MONITOR_CONNECTED:
    {
      ret = PXM_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_SERVICE_DISCOVERY;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_SERVICE_DISCOVERY, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }
    }
    break;
  case PROXIMITY_MONITOR_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case PROXIMITY_MONITOR_LL_CHARAC_DISCOVERY:
    {
      ret = PXM_DiscCharacServ(LINK_LOSS_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_WAIT_LL_CHARAC_DISCOVERY;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_CHARAC_DISCOVERY, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }
    }
    break;
  case PROXIMITY_MONITOR_WAIT_LL_CHARAC_DISCOVERY:
    /* Waiting the end of the Link Loss Characteristic discovery */ 
    break;
  case PROXIMITY_MONITOR_IA_CHARAC_DISCOVERY:
    {
      ret = PXM_DiscCharacServ(IMMEDIATE_ALERT_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_WAIT_IA_CHARAC_DISCOVERY;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_CHARAC_DISCOVERY, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }      
    }
    break;
  case PROXIMITY_MONITOR_WAIT_IA_CHARAC_DISCOVERY:
    /* Waiting the end of the Immediate Alert Characteristic discovery */ 
    break;
  case PROXIMITY_MONITOR_TP_CHARAC_DISCOVERY:
    {
      ret = PXM_DiscCharacServ(TX_POWER_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_WAIT_TP_CHARAC_DISCOVERY;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_CHARAC_DISCOVERY, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }      
    }
    break;
  case PROXIMITY_MONITOR_WAIT_TP_CHARAC_DISCOVERY:
    /* Waiting the end of the Tx Power Characteristic discovery */ 
    break;
  case PROXIMITY_MONITOR_TP_CHARAC_DESC_DISCOVERY:
    {
      ret = PXM_TxPwrLvl_DiscCharacDesc();
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_WAIT_TP_CHARAC_DESC_DISCOVERY;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }
    }
    break;
  case PROXIMITY_MONITOR_WAIT_TP_CHARAC_DESC_DISCOVERY:
    /* Waiting the end of the Tx Power Characteristic Descriptor discovery */
    break;
  case PROXIMITY_MONITOR_ENABLE_TP_NOTIFICATION:
    {
      ret = PXM_EnableTxPowerNotification();
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_WAIT_ENABLE_TP_NOTIFICATION;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_ENABLE_TP_NOTIFICATION, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }
    }
    break;
  case PROXIMITY_MONITOR_WAIT_ENABLE_TP_NOTIFICATION:
    /* Waiting the end of the Tx Power Enable Notification */
    break;
  case PROXIMITY_MONITOR_CONFIGURATION_REPORTER:
    {
      ret = PXM_ConfigureLinkLossAlert(proximityMonitorContext.llAlertLevel);
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_WAIT_LL_ALERT_LEVEL_CONFIG;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_CONFIG_LL_ALERT_LEVEL, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }
    }
    break;
  case PROXIMITY_MONITOR_WAIT_LL_ALERT_LEVEL_CONFIG:
    /* Waiting the end of the Link Loss Alert Level Configuration */
    break;
  case PROXIMITY_MONITOR_READ_TX_POWER:
    {
      ret = PXM_ReadTxPower();
      if (ret == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_WAIT_READ_TX_POWER;
	proximityMonitorContext.iaAlertSent = FALSE;
      } else {
	PXM_FullConfError_CB(PXM_ERROR_IN_READ_TX_POWER, ret);
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }
    }
    break;
  case PROXIMITY_MONITOR_WAIT_READ_TX_POWER:
    /* Waiting the end of the read Tx power proceduere */
    break;
  case PROXIMITY_MONITOR_IA_ALERT_LEVEL_CONFIG:
    {
      ret = PXM_ConfigureImmediateAlert(proximityMonitorContext.iaAlertLevel); 
      if (ret != BLE_STATUS_SUCCESS) {
	PXM_FullConfError_CB(PXM_ERROR_IN_CONFIG_IA_ALERT_LEVEL, ret);
      }
      if (proximityMonitorContext.iaAlertLevel == 0) {
	proximityMonitorContext.iaAlertSent = FALSE;
      }
      proximityMonitorContext.state = PROXIMITY_MONITOR_CONNECTED_IDLE;	
    }
    break;
  case PROXIMITY_MONITOR_DISCONNECTION:
    {
      ret = PXM_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	PXM_FullConfError_CB(PXM_ERROR_IN_DISCONNECTION, ret);
      }
      proximityMonitorContext.state = PROXIMITY_MONITOR_INITIALIZED;
    }
    break;
  }
}

/****************** Proximity Monitor Callbacks ***************************/
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
    if (proximityMonitorContext.state != PROXIMITY_MONITOR_START_CONNECTION)
      proximityMonitorContext.state = PROXIMITY_MONITOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    proximityMonitorContext.state = PROXIMITY_MONITOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  PXM_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      proximityMonitorContext.state = PROXIMITY_MONITOR_INITIALIZED;
      PXM_LinkLossAlert(proximityMonitorContext.llAlertLevel);
      PXM_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	proximityMonitorContext.connHandle = *connection_handle;  
	proximityMonitorContext.state = PROXIMITY_MONITOR_CONNECTED;
	PXM_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      proximityMonitorContext.state = PROXIMITY_MONITOR_INITIALIZED;
      if (proximityMonitorContext.fullConf)
	PXM_FullConfError_CB(PXM_ERROR_IN_CONNECTION, *status);
      else
	PXM_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      proximityMonitorContext.state = PROXIMITY_MONITOR_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      proximityMonitorContext.state = PROXIMITY_MONITOR_CONN_PARAM_UPDATE_REQ;
      PXM_ConnectionParameterUpdateReq_CB((pxmConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Immediate Alert Service and TX power Service are both present */
      if (findHandles(IMMEDIATE_ALERT_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.immediateAlertPresent = TRUE;
      } else {
	proximityMonitorContext.immediateAlertPresent = FALSE;
      }
      if (findHandles(TX_POWER_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.txPowerLevelEndCharHandle = end_handle;
	proximityMonitorContext.txPowerPresent = TRUE;
      } else {
	proximityMonitorContext.txPowerPresent = FALSE;
      }

      if (proximityMonitorContext.fullConf) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_LL_CHARAC_DISCOVERY;
#if PXM_TEST
	PXM_ServicesDiscovery_CB(*status, proximityMonitorContext.numReporterServices, 
				 proximityMonitorContext.reporterServices);
#endif
      } else {
	PXM_ServicesDiscovery_CB(*status, proximityMonitorContext.numReporterServices, 
				 proximityMonitorContext.reporterServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      if (*status != BLE_STATUS_SUCCESS) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
	PXM_FullConfError_CB(PXM_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(proximityMonitorContext.uuid_searched, 
			 proximityMonitorContext.numCharac, 
			 proximityMonitorContext.charac);
	if (proximityMonitorContext.fullConf) {
	  switch (proximityMonitorContext.uuid_searched) {
	  case LINK_LOSS_SERVICE_UUID:
	    if (proximityMonitorContext.immediateAlertPresent && proximityMonitorContext.txPowerPresent) {
	      proximityMonitorContext.state = PROXIMITY_MONITOR_IA_CHARAC_DISCOVERY;
	    } else {
	      proximityMonitorContext.state = PROXIMITY_MONITOR_CONFIGURATION_REPORTER;
	    }
	    break;
	  case IMMEDIATE_ALERT_SERVICE_UUID:
	    proximityMonitorContext.state = PROXIMITY_MONITOR_TP_CHARAC_DISCOVERY;
	    break;
	  case TX_POWER_SERVICE_UUID:
	    proximityMonitorContext.state = PROXIMITY_MONITOR_TP_CHARAC_DESC_DISCOVERY;
	    break;
	  }
#if PXM_TEST
	  PXM_CharacOfService_CB(*status, proximityMonitorContext.numCharac, proximityMonitorContext.charac);
#endif
	} else {
	  PXM_CharacOfService_CB(*status, proximityMonitorContext.numCharac, proximityMonitorContext.charac);
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
      if (proximityMonitorContext.fullConf) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_ENABLE_TP_NOTIFICATION;;
#if PXM_TEST
	PXM_CharacDesc_CB(*status, proximityMonitorContext.numCharac, proximityMonitorContext.charac);
#endif
      } else {
	PXM_CharacDesc_CB(*status, proximityMonitorContext.numCharac, proximityMonitorContext.charac);
      }
    }
    break;
  }
}

void Master_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				uint16_t *connection_handle, dataReceivedType *data)
{

  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    if (proximityMonitorContext.fullConf && 
	((proximityMonitorContext.state == PROXIMITY_MONITOR_WAIT_ENABLE_TP_NOTIFICATION) ||
	 (proximityMonitorContext.state == PROXIMITY_MONITOR_ENABLE_TP_NOTIFICATION))) {
      if (*status != BLE_STATUS_SUCCESS) {
	PXM_FullConfError_CB(PXM_ERROR_IN_ENABLE_TP_NOTIFICATION, *status);
      }
      proximityMonitorContext.state = PROXIMITY_MONITOR_CONFIGURATION_REPORTER;
    } else {
      PXM_EnableNotification_CB(*status);
    }
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Indication Change Status (0x%02x)\r\n", *status);
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    PXM_DataValueRead_CB(*status, proximityMonitorContext.dataLen, proximityMonitorContext.data);
    if (proximityMonitorContext.fullConf) {
      proximityMonitorContext.fullConf = FALSE;
      if (*status == BLE_STATUS_SUCCESS) {
	proximityMonitorContext.txPower = (int8_t)proximityMonitorContext.data[0];
	proximityMonitorContext.state = PROXIMITY_MONITOR_CONNECTED_IDLE;
	proximityMonitorContext.pathLossStartTime = HAL_GetTick();
      } else {
	proximityMonitorContext.state = PROXIMITY_MONITOR_DISCONNECTION;
      }
    } 
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
    if (proximityMonitorContext.fullConf) {
      if (*status != BLE_STATUS_SUCCESS) {
	PXM_FullConfError_CB(PXM_ERROR_IN_WRITE_PROCEDURE, *status);
      }
      if (proximityMonitorContext.immediateAlertPresent && proximityMonitorContext.txPowerPresent) {
	proximityMonitorContext.state = PROXIMITY_MONITOR_READ_TX_POWER; 
      } else {
	proximityMonitorContext.state = PROXIMITY_MONITOR_CONNECTED_IDLE;
      }
    } 
    PXM_ConfigureAlert_CB(*status);
    break;
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    if (data->attr_handle == proximityMonitorContext.txPowerLevelValHandle) {
      PXM_TxPowerNotificationReceived((int8_t)data->data_value[0]);
      proximityMonitorContext.txPower = (int8_t)data->data_value[0];
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Indication Data Received (0x%02x)\r\n", *status);
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }

}

void Master_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  if (*status == PASS_KEY_REQUEST) {
    PXM_PinCodeRequired_CB();
  }
}

void Master_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data)
{
  // Nothing to do 
}

/****************** Proximity Monitor Weak Callbacks definition ***************************/

WEAK_FUNCTION(void PXM_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
				   uint8_t data_length, uint8_t *data, 
					  uint8_t RSSI))
{
}

WEAK_FUNCTION(void PXM_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void PXM_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void PXM_ConnectionParameterUpdateReq_CB(pxmConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void PXM_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void PXM_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void PXM_ConfigureAlert_CB(uint8_t status))
{
}

WEAK_FUNCTION(void PXM_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void PXM_LinkLossAlert(uint8_t level))
{
}

WEAK_FUNCTION(void PXM_PathLossAlert(uint8_t level))
{
}

WEAK_FUNCTION(void PXM_TxPowerNotificationReceived(int8_t data_value))
{
}

WEAK_FUNCTION(void PXM_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void PXM_EnableNotification_CB(uint8_t status))
{
}

WEAK_FUNCTION(void PXM_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & PROXIMITY_MONITOR) */
