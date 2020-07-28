/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
* File Name          : master_basic_profile_sm.c
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.0
* Date               : 15-October-2014
* Description        : This file manages all the Master state machines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "bluenrg_gap.h"
#include "master_basic_profile_sm.h"

/* External variables --------------------------------------------------------*/
extern initDevType initParam;

/* External functions --------------------------------------------------------*/
//extern void Master_DeviceDiscovery_CB(uint8_t *addr_type, uint8_t *addr, 
//				      uint8_t *data_length, uint8_t *data,
//				      uint8_t *RSSI);
//extern void Master_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, 
//						 uint8_t *status);

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static masterGlobalContextType globalContextMemory;
masterGlobalContextType *masterGlobalContext = &globalContextMemory;
StateType masterState = {STATE_INIT, 0};

extern uint8_t bnrg_expansion_board;

/* Private macros ------------------------------------------------------------*/
#if DEBUG_BASIC_PROFILE
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void DEBUG_MESSAGE(uint32_t prefix, uint32_t next)
{
  PRINTF("State transition from %08x to %08x\n", prefix, next);
}

#define SUBSTATE_TRANSITION(prefix, prevState, nextState)  \
  { if  (prefix##_##prevState != prefix##_##nextState)	   \
      { \
	DEBUG_MESSAGE(prefix##_##prevState, prefix##_##nextState); \
      } \
      newState = prefix##_##nextState;			   \
  }

#define STATE_TRANSITION(prefix, prevState, nextState) \
  { if  (prefix##_##prevState != prefix##_##nextState)			\
      { \
	DEBUG_MESSAGE(prefix##_##prevState, prefix##_##nextState);	\
      } \
    newState.state = prefix##_##nextState;				\
    if (prefix##_##prevState != prefix##_##nextState) {			\
      newState.subState = prefix##_##nextState | 0x0001;		\
    }									\
  }

/* Public function prototypes ------------------------------------------------*/
void (*BLE_LowLevel_ACI_Event_CB)(void *pckt);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint8_t connectionProcedure(connDevType *param, uint8_t fast_conn, connectionContextType *connectionContext)
{
  uint8_t ret;
  uint8_t addr[7], dummy=0;
  uint16_t scanInt, scanWind;

  if (fast_conn) {
    scanInt = param->fastScanInterval;
    scanWind = param->fastScanWindow;
  } else {
    scanInt = param->reducedPowerScanInterval;
    scanWind = param->reducedPowerScanWindow;
  }

  if (param->procedure == DIRECT_CONNECTION_PROCEDURE) {
    ret = aci_gap_create_connection(scanInt, scanWind,
				    param->peer_addr_type, param->peer_addr,	
				    param->own_addr_type, param->conn_min_interval,	
				    param->conn_max_interval, param->conn_latency,	
				    param->supervision_timeout, param->min_conn_length, 
				    param->max_conn_length);
  } else {
    addr[0] = param->peer_addr_type;
    memcpy(&addr[1], param->peer_addr, 6);
    if (bnrg_expansion_board == IDB04A1) {
      ret =  aci_gap_start_auto_conn_establish_proc_IDB04A1(scanInt, scanWind,
                                                            param->own_addr_type, param->conn_min_interval,	
                                                            param->conn_max_interval, param->conn_latency,	
                                                            param->supervision_timeout, param->min_conn_length, 
                                                            param->max_conn_length, 0, &dummy,
                                                            1, addr);
    }
    else {
      dummy++; // To avoid a warning
      ret =  aci_gap_start_auto_conn_establish_proc_IDB05A1(scanInt, scanWind,
                                                            param->own_addr_type, param->conn_min_interval,	
                                                            param->conn_max_interval, param->conn_latency,	
                                                            param->supervision_timeout, param->min_conn_length, 
                                                            param->max_conn_length, 1, addr); 
    }
  }

  if (ret == BLE_STATUS_SUCCESS) {
    connectionContext->connProcedureComplete = FALSE;
    connectionContext->scanStartTime = HAL_GetTick();
  } else {
    PRINTF("Gap Start Connection procedure failed\n");
  }

  return ret;
}

static uint8_t primaryServiceCollector(uint8_t event_data_length, uint8_t attribute_data_length, uint8_t *attribute_data_list)
{
  uint8_t i, offset, numSer;
  serDiscContextType *servDiscContext = (serDiscContextType *) masterGlobalContext;

  numSer = (event_data_length - 1) / attribute_data_length;

  if ((servDiscContext->index + (event_data_length - 1) + numSer) > servDiscContext->size) {
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }

  *servDiscContext->numInfoServer += numSer;

  offset = 0;
  for (i=0; i<numSer; i++) {
    // UUID Type
    if (attribute_data_length == 6) {
      servDiscContext->arrayInfoServer[servDiscContext->index] = UUID_TYPE_16;
    } else {
      servDiscContext->arrayInfoServer[servDiscContext->index] = UUID_TYPE_128;
    }
    servDiscContext->index++;
    // Service Start Handle, Service End Handle, Service UUID
    memcpy(&servDiscContext->arrayInfoServer[servDiscContext->index], &attribute_data_list[offset], attribute_data_length);    
    servDiscContext->index += attribute_data_length;
    offset += attribute_data_length;
  }

  return BLE_STATUS_SUCCESS;
}

static uint8_t characServiceCollector(uint8_t event_data_length, uint8_t handle_value_pair_length,
				   uint8_t *handle_value_pair)
{
  uint8_t i, numChar, offset;
  serDiscContextType *servDiscContext = (serDiscContextType *) masterGlobalContext;

  numChar = (event_data_length - 1) / handle_value_pair_length;

  if ((servDiscContext->index + (event_data_length - 1) + numChar) > servDiscContext->size) {
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }

  *servDiscContext->numInfoServer += numChar;
  
  offset = 0;
  for (i=0; i<numChar; i++) {
    // UUID Type
    if (handle_value_pair_length == 7) {
      servDiscContext->arrayInfoServer[servDiscContext->index] = UUID_TYPE_16;
    } else {
      servDiscContext->arrayInfoServer[servDiscContext->index] = UUID_TYPE_128;
    }
    servDiscContext->index++;
    // Charac Handle, Charac Properties, Charac Value Handle, Charac UUID
    memcpy(&servDiscContext->arrayInfoServer[servDiscContext->index], &handle_value_pair[offset], handle_value_pair_length);    
    servDiscContext->index += handle_value_pair_length;
    offset += handle_value_pair_length;
  }

  return BLE_STATUS_SUCCESS;
}

static uint8_t includeServiceCollector(uint8_t event_data_length, uint8_t handle_value_pair_length,
				       uint8_t *handle_value_pair)
{
  uint8_t i, numIncludeSer, offset;
  serDiscContextType *servDiscContext = (serDiscContextType *) masterGlobalContext;

  numIncludeSer = (event_data_length - 1) / handle_value_pair_length;

  if ((servDiscContext->index + (event_data_length - 1) + numIncludeSer) > servDiscContext->size) {
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }

  *servDiscContext->numInfoServer += numIncludeSer;
  
  offset = 0;
  for (i=0; i<numIncludeSer; i++) {
    // Handle value pair length
    servDiscContext->arrayInfoServer[servDiscContext->index] = handle_value_pair_length;
    servDiscContext->index++;
    // Charac Handle, Included Handle
    memcpy(&servDiscContext->arrayInfoServer[servDiscContext->index], &handle_value_pair[offset], handle_value_pair_length);    
    servDiscContext->index += handle_value_pair_length;
    offset += handle_value_pair_length;
  }

  return BLE_STATUS_SUCCESS;
}

static uint8_t discAllCharacDescCollector(uint8_t event_data_length, uint8_t format, uint8_t *handle_uuid_pair)
{
  uint8_t i, numCharacDesc, offset, uuid_length;
  serDiscContextType *servDiscContext = (serDiscContextType *) masterGlobalContext;

  uuid_length = 4; //Handle + UUID_16
  if (format == 2)
    uuid_length = 18; //Handle + UUID_128

  numCharacDesc = (event_data_length - 1) / uuid_length;

  if ((servDiscContext->index + (event_data_length - 1) + numCharacDesc) > servDiscContext->size) {
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }

  *servDiscContext->numInfoServer += numCharacDesc;
  
  offset = 0;
  for (i=0; i<numCharacDesc; i++) {
    // Format of the information data
    servDiscContext->arrayInfoServer[servDiscContext->index] = format;
    servDiscContext->index++;
    // Charac Descriptor Handle and UUID
    memcpy(&servDiscContext->arrayInfoServer[servDiscContext->index], &handle_uuid_pair[offset], uuid_length);    
    servDiscContext->index += uuid_length;
    offset += uuid_length;
  }

  return BLE_STATUS_SUCCESS;
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void MBProfileSM_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  uint8_t procedure, status;
  dataReceivedType param;
  uint16_t conn_handle;
  
  procedure = ATTRIBUTE_MODIFICATION_RECEIVED;
  status = BLE_STATUS_SUCCESS;
  conn_handle = handle;
  param.attr_handle = handle;
  param.data_length = data_length;
  param.data_value = att_data;
  initParam.Master_PeerDataExchange_CB(&procedure, &status, &conn_handle, &param);  
}

/***** Master Device Discovery State machine *****/
uint32_t deviceDiscoveryStateMachine(uint32_t subState)
{
  uint32_t newState = subState;
  devDiscoveryContextType *devDisContext = (devDiscoveryContextType *) masterGlobalContext;

  switch(subState) {
  case  SUBSTATE_DEVICE_DISCOVERY_INIT:
    {
      SUBSTATE_TRANSITION(SUBSTATE_DEVICE_DISCOVERY, INIT, WAITING); // Procedure Run
    }
    break;
  case SUBSTATE_DEVICE_DISCOVERY_WAITING:
    {
      if (devDisContext->procedureComplete) {
	SUBSTATE_TRANSITION(SUBSTATE_DEVICE_DISCOVERY, WAITING, DONE); // Procedure Complete
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_DEVICE_DISCOVERY, WAITING, WAITING); // Wait the timeout or terminate procedure command
      } 
    }
    break;
  case SUBSTATE_DEVICE_DISCOVERY_DONE:
    /* Nothing to be done */
  default:
    /* Invalid sub state */
    break;
  }

  return newState;
}

/***** Master Connection State machine *****/
uint32_t connectionStateMachine(uint32_t subState, initDevType *param)
{
  uint32_t newState = subState;
  connectionContextType *connectionContext = (connectionContextType *) masterGlobalContext;
  uint8_t ret, procedure;
  connUpdateParamType dummy;

  switch(subState) {
  case SUBSTATE_CONNECTION_INIT:
    {
      connectionContext->connectionCompleted = FALSE;
      SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, INIT, WAITING_END_DISCOVERY); // Wait the procedure end
    }
    break;
  case SUBSTATE_CONNECTION_WAITING_END_DISCOVERY:
    {
      if (!connectionContext->discProcedureComplete) {
	SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, WAITING_END_DISCOVERY, WAITING_END_DISCOVERY); // Wait the discovery procedure end
      } else {
	HAL_Delay(10); //???
	ret = connectionProcedure(&connectionContext->userParam, TRUE, connectionContext);
	if (ret == BLE_STATUS_SUCCESS) {
	  // Signal to host micro that can enable the power save with timeout 
	  connectionContext->hostExit_TimeoutPowerSave = 0;
	  connectionContext->powerSave_enable = (connectionContext->userParam.fastScanDuration<<2) | 3;
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, WAITING_END_DISCOVERY, FAST_SCAN_WAITING); // Connection procedure ongoing
	} else {
	  procedure = CONNECTION_FAILED_EVT;
	  param->Master_Connection_CB(&procedure, &ret, &connectionContext->connHandle, &dummy);
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, WAITING_END_DISCOVERY, DONE); // Error during start the connection
	}
      }
    }
    break;
  case SUBSTATE_CONNECTION_FAST_SCAN_WAITING:
    {
      if ((HAL_GetTick() - connectionContext->scanStartTime > connectionContext->userParam.fastScanDuration) ||
	  connectionContext->hostExit_TimeoutPowerSave){
	if (connectionContext->userParam.procedure == DIRECT_CONNECTION_PROCEDURE)
	  procedure = GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC;
	else
	  procedure = GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC;
	ret = aci_gap_terminate_gap_procedure(procedure);
	if (ret == BLE_STATUS_SUCCESS) {
	  connectionContext->powerSave_enable = 1;
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, FAST_SCAN_WAITING, WAITING_END_PROC_FAST_SCAN); // End procedure ongoing
	} else {
	  procedure = STOP_CONNECTION_PROCEDURE_EVT;
	  param->Master_Connection_CB(&procedure, &ret, &connectionContext->connHandle, &dummy);
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, FAST_SCAN_WAITING, DONE); // Error in terminate procedure call
	}
      } else {
	if (connectionContext->connectionCompleted) {
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, FAST_SCAN_WAITING, DONE); // Connection procedure completed
	} else {
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, FAST_SCAN_WAITING, FAST_SCAN_WAITING); // Connection procedure ongoing
	}
      }
    }
    break;
  case SUBSTATE_CONNECTION_WAITING_END_PROC_FAST_SCAN:
    {
      if (!connectionContext->connProcedureComplete) {
	SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, WAITING_END_PROC_FAST_SCAN, WAITING_END_PROC_FAST_SCAN); // End procedure ongoing
      } else {
	HAL_Delay(15); //???
	ret = connectionProcedure(&connectionContext->userParam, FALSE, connectionContext);
	if (ret == BLE_STATUS_SUCCESS) {
	  connectionContext->powerSave_enable = 1;
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, WAITING_END_PROC_FAST_SCAN, RED_PWR_SCAN_WAITING); // Start reduced power scan 
	} else {
	  procedure = CONNECTION_FAILED_EVT;
	  param->Master_Connection_CB(&procedure, &ret, &connectionContext->connHandle, &dummy);
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, WAITING_END_PROC_FAST_SCAN, DONE); // Error in connection procedure call
	}
      }
    }
    break;
  case SUBSTATE_CONNECTION_RED_PWR_SCAN_WAITING:
    {
      if ((connectionContext->connectionCompleted) || (connectionContext->connProcedureComplete)) {
	SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, RED_PWR_SCAN_WAITING, DONE); // Connection procedure ended
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_CONNECTION, RED_PWR_SCAN_WAITING, RED_PWR_SCAN_WAITING); // Procedure ongoing
      }
    }
    break;
  case SUBSTATE_CONNECTION_DONE:
    /* Nothing to be done */
  default:
    /* Invalid sub state */
    break;
  }

  return newState;
}

/***** Master Bonding State machine *****/
uint32_t bondingStateMachine(uint32_t subState)
{
  uint32_t newState = subState;
  bondingContextType *bondingContext = (bondingContextType *) masterGlobalContext;
  uint8_t ret;
  uint16_t attr_handle;

  switch(subState) {
  case SUBSTATE_BONDING_INIT:
    {
      SUBSTATE_TRANSITION(SUBSTATE_BONDING, INIT, WAIT); // Procedure ongoing
    }
    break;
  case SUBSTATE_BONDING_WAIT:
    {
      if (bondingContext->procedureComplete) {
	initParam.Master_Pairing_CB(&bondingContext->conn_handle, &bondingContext->procedureStatus);
	if (bondingContext->procedure == 0) {
	  SUBSTATE_TRANSITION(SUBSTATE_BONDING, WAIT, DONE); // Procedure Complete
	} else {
	  SUBSTATE_TRANSITION(SUBSTATE_BONDING, WAIT, RUN_LAST_PROCEDURE); // Re-run the last procedure
	}
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_BONDING, WAIT, WAIT); // Wait the timeout or terminate procedure command
      } 
    }
    break;
  case SUBSTATE_BONDING_RUN_LAST_PROCEDURE:
    {
      ret = BLE_STATUS_SUCCESS;
      switch (bondingContext->procedure) {
      case NOTIFICATION_INDICATION_CHANGE_STATUS:
	{
	  memcpy(&attr_handle, bondingContext->write_data, 2);
	  ret = Master_NotifIndic_Status(bondingContext->conn_handle, attr_handle,
					 bondingContext->write_data[2], bondingContext->write_data[3]);
	}
	break;
      case READ_VALUE_STATUS:
	{
	  memcpy(&attr_handle, bondingContext->write_data, 2);
	  ret = Master_Read_Value(bondingContext->conn_handle, attr_handle, 
				  bondingContext->data_length, bondingContext->data_array, 
				  bondingContext->size);
	}
	break;
      case WRITE_VALUE_STATUS:
	{
	  memcpy(&attr_handle, bondingContext->write_data, 2);
	  ret = Master_Write_Value(bondingContext->conn_handle, attr_handle, 
				   bondingContext->write_len, &bondingContext->write_data[2]);
	}
	break;
      case READ_LONG_VALUE_STATUS:
	{
	  memcpy(&attr_handle, bondingContext->write_data, 2);
	  ret = Master_Read_Long_Value(bondingContext->conn_handle, attr_handle, 
				       bondingContext->data_length, bondingContext->data_array, 
				       bondingContext->write_data[2], bondingContext->size);
	}
	break;
      }
      if (ret != BLE_STATUS_SUCCESS) {
	initParam.Master_PeerDataExchange_CB(&bondingContext->procedure, &ret, &bondingContext->conn_handle, NULL);
      }
      SUBSTATE_TRANSITION(SUBSTATE_BONDING, RUN_LAST_PROCEDURE, DONE); // Procedure Complete
    }
    break;
  case SUBSTATE_BONDING_DONE:
    /* Nothing to be done */
  default:
    /* Invalid sub state */
    break;
  }

  return newState;
}

/***** Master Service Discovery State machine *****/
uint32_t serviceDiscoveryStateMachine(uint32_t subState)
{
  uint32_t newState = subState;
  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;
  uint8_t status;

  switch(subState) {
  case SUBSTATE_SERVICE_DISCOVERY_INIT:
    {
      serDiscContext->procedureComplete = FALSE;
      serDiscContext->index = 0;
      if (serDiscContext->procedure == PRIMARY_SERVICE_DISCOVERY) {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, INIT, ALL_PRIMARY); // Start all primary services discovery
      }
      if (serDiscContext->procedure == GET_CHARACTERISTICS_OF_A_SERVICE) {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, INIT, ALL_CHARAC); // Start read characteriscs of a service procedure
      }
      if (serDiscContext->procedure == FIND_INCLUDED_SERVICES) {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, INIT, ALL_INCLUDE); // Find all included service procedure start
      }
      if (serDiscContext->procedure == FIND_CHARAC_DESCRIPTORS) {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, INIT, ALL_CHARAC_DESC); // Find all characteristic descriptor procedure start
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_ALL_PRIMARY:
    {
      status = aci_gatt_disc_all_prim_services(serDiscContext->conn_handle);
      if (status != BLE_STATUS_SUCCESS) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &status, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_PRIMARY, DONE); // Error
      } else {
	serDiscContext->powerSave_enable = 1;
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_PRIMARY, WAIT_ALL_PRIMARY); // Procedure started
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_PRIMARY:
    {
      if (serDiscContext->procedureComplete) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &serDiscContext->procedureStatus, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_PRIMARY, DONE); // Procedure completed
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_PRIMARY, WAIT_ALL_PRIMARY); // Procedure ongoing
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_VERIFY:
    {
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_ALL_INCLUDE:
    {
      status = aci_gatt_find_included_services(serDiscContext->conn_handle, 
					       serDiscContext->start_handle, 
					       serDiscContext->end_handle);
      if (status != BLE_STATUS_SUCCESS) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &status, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_INCLUDE, DONE); // Error
      } else {
	serDiscContext->powerSave_enable = 1;
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_INCLUDE, WAIT_ALL_INCLUDE); // Procedure started
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_INCLUDE:
    {
      if (serDiscContext->procedureComplete) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &serDiscContext->procedureStatus, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_INCLUDE, DONE); // Procedure completed
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_INCLUDE, WAIT_ALL_INCLUDE); // Procedure ongoing
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_ALL_CHARAC:
    {
      status = aci_gatt_disc_all_charac_of_serv(serDiscContext->conn_handle, 
						serDiscContext->start_handle, 
						serDiscContext->end_handle);
      if (status != BLE_STATUS_SUCCESS) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &status, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_CHARAC, DONE); // Error
      } else {
	serDiscContext->powerSave_enable = 1;
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_CHARAC, WAIT_ALL_CHARAC); // Procedure started
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_CHARAC:
    {
      if (serDiscContext->procedureComplete) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &serDiscContext->procedureStatus, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_CHARAC, DONE); // Procedure completed
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_CHARAC, WAIT_ALL_CHARAC); // Procedure ongoing
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_ALL_CHARAC_DESC:
    {
      status = aci_gatt_disc_all_charac_descriptors(serDiscContext->conn_handle, 
						    serDiscContext->start_handle, 
						    serDiscContext->end_handle);
      if (status != BLE_STATUS_SUCCESS) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &status, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_CHARAC_DESC, DONE); // Error
      } else {
	serDiscContext->powerSave_enable = 1;
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, ALL_CHARAC_DESC, WAIT_ALL_CHARAC_DESC); // Procedure started
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_CHARAC_DESC:
    {
      if (serDiscContext->procedureComplete) {
	initParam.Master_ServiceCharacPeerDiscovery_CB(&serDiscContext->procedure, &serDiscContext->procedureStatus, &serDiscContext->conn_handle);
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_CHARAC_DESC, DONE); // Procedure completed
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_SERVICE_DISCOVERY, WAIT_ALL_CHARAC_DESC, WAIT_ALL_CHARAC_DESC); // Procedure ongoing
      }
    }
    break;
  case SUBSTATE_SERVICE_DISCOVERY_DONE:
    /* Nothing to be done */
  default:
    /* Invalid sub state */
    break;
  }

  return newState;
}

/***** Master Connected Idle State machine *****/
uint32_t connectedIdleStateMachine(uint32_t subState)
{
  uint32_t newState = subState;
  connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;

  switch(subState) {
  case SUBSTATE_CONNECTED_IDLE_INIT:
    {
      switch (connIdleContext->procedure) {
      case NOTIFICATION_INDICATION_CHANGE_STATUS:
      case READ_VALUE_STATUS:
      case READ_LONG_VALUE_STATUS:
      case WRITE_VALUE_STATUS:
	{
	  SUBSTATE_TRANSITION(SUBSTATE_CONNECTED_IDLE, INIT, WAIT_DATA_EXCHANGE_STATUS); // Procedure ongoing	
	}
	break;
      default:
	SUBSTATE_TRANSITION(SUBSTATE_CONNECTED_IDLE, INIT, DONE); // Nothing to do
      }
      
    }
    break;
  case SUBSTATE_CONNECTED_IDLE_WAIT_DATA_EXCHANGE_STATUS:
    {
      if (connIdleContext->procedureComplete) {
      	SUBSTATE_TRANSITION(SUBSTATE_CONNECTED_IDLE, WAIT_DATA_EXCHANGE_STATUS, DONE); // Procedure ended
      } else {
	SUBSTATE_TRANSITION(SUBSTATE_CONNECTED_IDLE, WAIT_DATA_EXCHANGE_STATUS, WAIT_DATA_EXCHANGE_STATUS); // Procedure ongoing
      }
    }
    break;
  case SUBSTATE_CONNECTED_IDLE_DONE:
    /* Nothing to be done */
  default:
    /* Invalid sub state */
    break;
  }

  return newState;
}

/***** Master Global State machine *****/
StateType masterStateMachine(StateType state, initDevType *param)
{
  StateType newState = state;

  switch (state.state) {
  case STATE_INIT:
    {
      newState.powerSave_bitMask = 0;
    }
    break;
  case STATE_DEVICE_DISCOVERY:
    {
      devDiscoveryContextType *devDisContext = (devDiscoveryContextType *) masterGlobalContext;

      newState.subState = deviceDiscoveryStateMachine(state.subState);
      if (newState.subState == SUBSTATE_DEVICE_DISCOVERY_DONE)
	STATE_TRANSITION(STATE, DEVICE_DISCOVERY, IDLE); // Device Discovery Procedure completed
      newState.powerSave_bitMask = devDisContext->powerSave_enable;
    }
    break;
  case STATE_CONNECTION:
    {
      connectionContextType *connectionContext = (connectionContextType *) masterGlobalContext;
    
      newState.subState = connectionStateMachine(state.subState, param);
      if (newState.subState == SUBSTATE_CONNECTION_DONE) {
	connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;
	uint16_t conn_handle;
	conn_handle = connectionContext->connHandle;
	connIdleContext->conn_handle = conn_handle;
	STATE_TRANSITION(STATE, CONNECTION, CONNECTED_IDLE); // Connection Procedure completed
      }
      newState.powerSave_bitMask = connectionContext->powerSave_enable;
    }
    break;
  case STATE_SERVICE_DISCOVERY:
    {
      serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

      newState.subState = serviceDiscoveryStateMachine(state.subState);
      if (newState.subState == SUBSTATE_SERVICE_DISCOVERY_DONE) {
	STATE_TRANSITION(STATE, SERVICE_DISCOVERY, CONNECTED_IDLE); // Service Discovery Procedure completed
      }
      newState.powerSave_bitMask = serDiscContext->powerSave_enable;
    }
    break;
  case STATE_CONNECTED_IDLE:
    {
      connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;

      newState.subState = connectedIdleStateMachine(state.subState);
      if (newState.subState == SUBSTATE_CONNECTED_IDLE_DONE) {
	connIdleContext->powerSave_enable = 1;
	STATE_TRANSITION(STATE, CONNECTED_IDLE, CONNECTED_IDLE); // Connected Idle Procedure completed
      }
      newState.powerSave_bitMask = connIdleContext->powerSave_enable;
    }
    break;
  case STATE_BONDING:
    {
      bondingContextType *bondingContext = (bondingContextType *) masterGlobalContext;

      newState.subState = bondingStateMachine(state.subState);
      if (newState.subState == SUBSTATE_BONDING_DONE)
	STATE_TRANSITION(STATE, BONDING, CONNECTED_IDLE); // Bonding Procedure completed      
      newState.powerSave_bitMask = bondingContext->powerSave_enable;
    }
    break;
  case STATE_IDLE:
    {
      /* Nothing to do */
    }
    break;
  default:
    /* Error state not defined */
    break;
  }
  return newState;
}

/* BlueNRG Event Callback */
void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
    
  if(hci_pckt->type != HCI_EVENT_PKT)
    return;
    
  switch(event_pckt->evt){
  case EVT_ENCRYPT_CHANGE:
    {
      evt_encrypt_change *evt = (void*)event_pckt->data;
      uint8_t ret, procedure;
      uint16_t conn_handle;
      bondingContextType *bondingContext = (bondingContextType *)masterGlobalContext;

      if (evt->status == ERR_UNSUPP_RMT_FEATURE) {
	procedure = bondingContext->procedure;
	ret = Master_Start_Pairing_Procedure(evt->handle, 1);
	bondingContext->procedure = procedure;
	if (ret != BLE_STATUS_SUCCESS) {
	  ret = evt->status;
	  conn_handle = evt->handle;
	  initParam.Master_Pairing_CB(&conn_handle, &ret);
	}
      }
    }
    break;
  case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *evt = (void*)event_pckt->data;
      uint8_t type_evt;
      uint16_t handle;
      connUpdateParamType dummy;
      
      type_evt = DISCONNECTION_EVT;
      handle = evt->handle;
      initParam.Master_Connection_CB(&type_evt, &evt->reason, &handle, &dummy);
    }
    break;
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
	{
	  evt_le_connection_complete *cc = (void *)evt->data;
	  connectionContextType *connContext = (connectionContextType *) masterGlobalContext;
	  uint8_t type_evt;
	  uint8_t dummy_addr[6]={0,0,0,0,0,0};
	  connUpdateParamType dummy;
	  
	  if (cc->status == BLE_STATUS_SUCCESS) {
	    connContext->connHandle = cc->handle;	      
	    type_evt = CONNECTION_ESTABLISHED_EVT;
	  } else {
	    type_evt = CONNECTION_FAILED_EVT;
	  }
	  // Workaround for bug #968
	  if ((cc->status != ERR_UNKNOWN_CONN_IDENTIFIER) || memcmp(cc->peer_bdaddr, dummy_addr, 6)) {
	    initParam.Master_Connection_CB(&type_evt, &cc->status, &connContext->connHandle, &dummy);
	    connContext->powerSave_enable = 0;
	    connContext->connectionCompleted = TRUE;
	  }
	}
	break;
      case EVT_LE_ADVERTISING_REPORT:
        {
          le_advertising_info *pr = (void *)(evt->data+1); /* evt->data[0] is number of reports (On BlueNRG-MS is always 1) */
	  uint8_t reason;
	  
	  reason = DEVICE_DISCOVERED;
	  initParam.Master_DeviceDiscovery_CB(&reason, &pr->bdaddr_type, pr->bdaddr, &pr->data_length, &pr->data_RSSI[0],
				    &pr->data_RSSI[pr->data_length]);
        }
        break;
      }
    }
    break;

  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      switch(blue_evt->ecode){
      case EVT_BLUE_GAP_DEVICE_FOUND:
	{
	  evt_gap_device_found *pr = (void*)blue_evt->data;
	  uint8_t reason;
	  
	  reason = DEVICE_DISCOVERED;
	  initParam.Master_DeviceDiscovery_CB(&reason, &pr->bdaddr_type, pr->bdaddr, &pr->data_length, &pr->data_RSSI[0],
				    &pr->data_RSSI[pr->data_length]);
	}
	break;
      case EVT_BLUE_GAP_PROCEDURE_COMPLETE:
	{
	  evt_gap_procedure_complete *pr = (void*)blue_evt->data;
	  switch(pr->procedure_code) {
	  case GAP_LIMITED_DISCOVERY_PROC:
	  case GAP_GENERAL_DISCOVERY_PROC:
	    {
	      if (GET_CURRENT_STATE() == STATE_DEVICE_DISCOVERY) {
		devDiscoveryContextType *devDisContext = (devDiscoveryContextType *) masterGlobalContext;
		devDisContext->procedureComplete = TRUE;
		devDisContext->powerSave_enable = 0;
		if (devDisContext->end_reason != DEVICE_DISCOVERY_PROCEDURE_ENDED)
		  devDisContext->end_reason = DEVICE_DISCOVERY_PROCEDURE_TIMEOUT;
		HAL_Delay(10); //???
		initParam.Master_DeviceDiscovery_CB(&devDisContext->end_reason, NULL, NULL, NULL, NULL, NULL);

	      }
	      if (GET_CURRENT_STATE() == STATE_CONNECTION) {
		connectionContextType *connContext = (connectionContextType *) masterGlobalContext;
		connContext->discProcedureComplete = TRUE;
		connContext->powerSave_enable = 0;
	      }
	      
	    }
	    break;
	  case GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC:
	  case GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC:
	    {
	      connectionContextType *connContext = (connectionContextType *) masterGlobalContext;
	      connContext->connProcedureComplete = TRUE;
	      connContext->powerSave_enable = 0;
	    }
	    break;
	  }
	}
	break;
      case EVT_BLUE_GATT_READ_PERMIT_REQ:
	{
	  evt_gatt_read_permit_req *req = (void*)blue_evt->data;
	  uint8_t procedure;
	  uint16_t connHandle;
	  dataPermitExchangeType data;

	  procedure = READ_PERMIT_REQUEST;
	  connHandle = req->conn_handle;
	  data.attr_handle = req->attr_handle;
	  data.data_length = 1;
	  data.offset = req->offset;
	  initParam.Master_PeerDataPermitExchange_CB(&procedure, &connHandle, &data);
	}
	break;
      case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
	{
	  evt_gatt_write_permit_req *req = (void*)blue_evt->data;
	  uint8_t procedure;
	  uint16_t connHandle;
	  dataPermitExchangeType data;

	  procedure = WRITE_PERMIT_REQUEST;
	  connHandle = req->conn_handle;
	  data.attr_handle = req->attr_handle;
	  data.data_length = req->data_length;
	  data.offset = 0;
	  data.data = req->data;	  
	  initParam.Master_PeerDataPermitExchange_CB(&procedure, &connHandle, &data);	  
	}
	break;
      case EVT_BLUE_ATT_FIND_INFORMATION_RESP:
	{
	  evt_att_find_information_resp *pr = (void*)blue_evt->data;
	  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

	  if ((serDiscContext->conn_handle == pr->conn_handle) &&
	      (serDiscContext->procedure == FIND_CHARAC_DESCRIPTORS)) {
	    serDiscContext->procedureStatus = discAllCharacDescCollector(pr->event_data_length,
									 pr->format, 
									 pr->handle_uuid_pair);
	    if (serDiscContext->procedureStatus != BLE_STATUS_SUCCESS) {	      
	      serDiscContext->procedureComplete = TRUE;
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
	{
	  evt_att_read_by_group_resp *pr = (void*)blue_evt->data;
	  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

	  if ((serDiscContext->conn_handle == pr->conn_handle) &&
	      (serDiscContext->procedure == PRIMARY_SERVICE_DISCOVERY)) {
	    serDiscContext->procedureStatus = primaryServiceCollector(pr->event_data_length, 
								      pr->attribute_data_length, 
								      pr->attribute_data_list);
	    if (serDiscContext->procedureStatus != BLE_STATUS_SUCCESS) {	      
	      serDiscContext->procedureComplete = TRUE;
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
	{
	  evt_att_read_by_type_resp *pr = (void*)blue_evt->data;
	  serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

	  if ((serDiscContext->conn_handle == pr->conn_handle) &&
	      (serDiscContext->procedure == GET_CHARACTERISTICS_OF_A_SERVICE)) {
	    serDiscContext->procedureStatus = characServiceCollector(pr->event_data_length, 
								     pr->handle_value_pair_length, 
								     pr->handle_value_pair);
	  }
	  if ((serDiscContext->conn_handle == pr->conn_handle) &&
	      (serDiscContext->procedure == FIND_INCLUDED_SERVICES)) {
	    serDiscContext->procedureStatus = includeServiceCollector(pr->event_data_length, 
								      pr->handle_value_pair_length, 
								      pr->handle_value_pair); 
	  }
	  if (serDiscContext->procedureStatus != BLE_STATUS_SUCCESS) {	      
	    serDiscContext->procedureComplete = TRUE;
	  }
	}
	break;
      case EVT_BLUE_GATT_NOTIFICATION:
	{
	  evt_gatt_attr_notification *evt = (void*)blue_evt->data;
	  uint8_t procedure, status;
	  dataReceivedType param;
	  uint16_t conn_handle;

	  procedure = NOTIFICATION_DATA_RECEIVED;
	  status = BLE_STATUS_SUCCESS;
	  conn_handle = evt->conn_handle;
	  param.attr_handle = evt->attr_handle;
	  param.data_length = evt->event_data_length - 2;
	  param.data_value = evt->attr_value;
	  initParam.Master_PeerDataExchange_CB(&procedure, &status, &conn_handle, &param);
	}
	break;
      case EVT_BLUE_GATT_INDICATION:
	{
	  evt_gatt_indication *evt = (void*)blue_evt->data;
	  uint8_t procedure, status;
	  dataReceivedType param;
	  uint16_t conn_handle;

	  procedure = INDICATION_DATA_RECEIVED;
	  status = BLE_STATUS_SUCCESS;
	  conn_handle = evt->conn_handle;
	  param.attr_handle = evt->attr_handle;
	  param.data_length = evt->event_data_length - 2;
	  param.data_value = evt->attr_value;
	  initParam.Master_PeerDataExchange_CB(&procedure, &status, &conn_handle, &param);

	}
	break;
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
	{
          /* Check for attribute handle */
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            MBProfileSM_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            MBProfileSM_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          } 
          
	  
	}
	break;
      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
	{
	  evt_gatt_procedure_complete *pr = (void*)blue_evt->data;

	  if (GET_CURRENT_STATE() == STATE_SERVICE_DISCOVERY) {
	    serDiscContextType *serDiscContext = (serDiscContextType *) masterGlobalContext;

	    if ((serDiscContext->conn_handle == pr->conn_handle) && 
		((serDiscContext->procedure == PRIMARY_SERVICE_DISCOVERY) ||
		 (serDiscContext->procedure == GET_CHARACTERISTICS_OF_A_SERVICE) ||
		 (serDiscContext->procedure == FIND_INCLUDED_SERVICES) ||
		 (serDiscContext->procedure == FIND_CHARAC_DESCRIPTORS))) {
	      serDiscContext->procedureComplete = TRUE;
	      serDiscContext->procedureStatus = pr->error_code;
	      serDiscContext->powerSave_enable = 0;
	    }
	  }

	  if (GET_CURRENT_STATE() == STATE_CONNECTED_IDLE) {
	    connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;
	    if ((connIdleContext->conn_handle == pr->conn_handle) &&
		((connIdleContext->procedure == NOTIFICATION_INDICATION_CHANGE_STATUS) ||
		 (connIdleContext->procedure == READ_VALUE_STATUS) ||
		 (connIdleContext->procedure == READ_LONG_VALUE_STATUS) ||
		 (connIdleContext->procedure == WRITE_VALUE_STATUS))) {
	      connIdleContext->powerSave_enable = 0;
	      connIdleContext->procedureComplete = TRUE;
	      if (connIdleContext->procedureStatus == BLE_STATUS_SUCCESS)
		connIdleContext->procedureStatus = pr->error_code;
	      initParam.Master_PeerDataExchange_CB(&connIdleContext->procedure, &connIdleContext->procedureStatus, 
					 &connIdleContext->conn_handle, NULL);

	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_RESP:
	{
	  evt_att_read_resp *pr = (void*)blue_evt->data;
	  
	  if (GET_CURRENT_STATE() == STATE_CONNECTED_IDLE) {
	    connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;
	    if ((connIdleContext->conn_handle == pr->conn_handle) &&
		(connIdleContext->procedure == READ_VALUE_STATUS)) {
	      *connIdleContext->data_length = pr->event_data_length;
	      if (pr->event_data_length > connIdleContext->size)
		connIdleContext->procedureStatus = BLE_STATUS_INSUFFICIENT_RESOURCES;
	      else
		memcpy(connIdleContext->data_array, pr->attribute_value, *connIdleContext->data_length);
	    }
	  }	  
	}
	break;
      case EVT_BLUE_ATT_READ_BLOB_RESP:
	{
	  evt_att_read_blob_resp *pr = (void*)blue_evt->data;

	  if (GET_CURRENT_STATE() == STATE_CONNECTED_IDLE) {
	    connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;
	    if ((connIdleContext->conn_handle == pr->conn_handle) &&
		(connIdleContext->procedure == READ_LONG_VALUE_STATUS)) {
	      if ((*connIdleContext->data_length + pr->event_data_length) > connIdleContext->size) {
		connIdleContext->procedureStatus = BLE_STATUS_INSUFFICIENT_RESOURCES;
	      } else {
		memcpy(&connIdleContext->data_array[*connIdleContext->data_length], 
		       pr->part_attribute_value, pr->event_data_length);
		*connIdleContext->data_length += pr->event_data_length;
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_GAP_PAIRING_CMPLT:
	{
	  evt_gap_pairing_cmplt *pr = (void*)blue_evt->data;

	  if (GET_CURRENT_STATE() == STATE_BONDING) {
	    bondingContextType *bondingContext = (bondingContextType *)masterGlobalContext;
	    if (bondingContext->conn_handle == pr->conn_handle) {
	      bondingContext->procedureComplete = TRUE;
	      bondingContext->procedureStatus = pr->status;
	      bondingContext->powerSave_enable = 0;
	    }
	  }
	}
	break;
      case EVT_BLUE_GATT_ERROR_RESP:
	{
	  evt_gatt_error_resp *evt = (void*)blue_evt->data;
	  bondingContextType *bondingContext = (bondingContextType *)masterGlobalContext;
	  uint8_t ret, procedure;
	  uint16_t conn_handle;
	  
	  ret = evt->error_code;
	  if ((evt->error_code == ERROR_INSUFFICIENT_AUTHENTICATION) ||
	      (evt->error_code == ERROR_INSUFFICIENT_AUTHORIZATION) ||
	      (evt->error_code == ERROR_INSUFFICIENT_ENCRYPTION)) {
	    if (GET_CURRENT_STATE() == STATE_CONNECTED_IDLE) {
	      procedure = bondingContext->procedure;
	      bondingContext->conn_handle = evt->conn_handle;
	      ret = Master_Start_Pairing_Procedure(evt->conn_handle, 0);
	      bondingContext->procedure = procedure;
	    }
	    if (ret != BLE_STATUS_SUCCESS) {
	      conn_handle = evt->conn_handle;
	      initParam.Master_Pairing_CB(&conn_handle, &evt->error_code);
	    }
	  } else {
	    if (GET_CURRENT_STATE() == STATE_CONNECTED_IDLE) {
	      procedure = bondingContext->procedure;
	      conn_handle = evt->conn_handle;
	      bondingContext->procedureComplete = TRUE;
	      initParam.Master_PeerDataExchange_CB(&procedure, &evt->error_code, 
					 &conn_handle, NULL);
	    }
	  }
	}
	break;
      case EVT_BLUE_GAP_PASS_KEY_REQUEST:
	{
	  evt_gap_pass_key_req *evt = (void*)blue_evt->data;
	  bondingContextType *bondingContext = (bondingContextType *)masterGlobalContext;
	  uint8_t status;

	  status = PASS_KEY_REQUEST;
	  bondingContext->conn_handle = evt->conn_handle;
	  initParam.Master_Pairing_CB(&bondingContext->conn_handle, &status);
	}
	break;
      case EVT_BLUE_L2CAP_CONN_UPD_REQ:
	{
	  evt_l2cap_conn_upd_req *evt = (void*)blue_evt->data;
	  uint8_t procedure_evt, status;
	  uint16_t conn_handle;
	  connUpdateParamType param;

	  procedure_evt = CONNECTION_UPDATE_PARAMETER_REQUEST_EVT;
	  status = BLE_STATUS_SUCCESS;
	  param.identifier = evt->identifier;
	  param.interval_min = evt->interval_min;
	  param.interval_max = evt->interval_max;
	  param.slave_latency = evt->slave_latency;
	  param.timeout_mult = evt->timeout_mult;
	  if ((GET_CURRENT_STATE() == STATE_CONNECTED_IDLE) || 
	      (GET_CURRENT_STATE() == STATE_CONNECTION)) {
	    if (GET_CURRENT_STATE() == STATE_CONNECTED_IDLE) {
	      connIdleContextType *connIdleContext = (connIdleContextType *) masterGlobalContext;  
	      conn_handle = connIdleContext->conn_handle;
	    } else {
	      connectionContextType *connContext = (connectionContextType *) masterGlobalContext;
	      conn_handle = connContext->connHandle;
	    }
	    initParam.Master_Connection_CB(&procedure_evt, &status, &conn_handle, &param);
	  } else {
            if (bnrg_expansion_board == IDB04A1) {
	      aci_l2cap_connection_parameter_update_response_IDB04A1(conn_handle, param.interval_min,
                                                                     param.interval_max, param.slave_latency,
                                                                     param.timeout_mult, param.identifier, 0);
            }
            else {
              aci_l2cap_connection_parameter_update_response_IDB05A1(conn_handle, param.interval_min,
                                                                     param.interval_max, param.slave_latency,
                                                                     param.timeout_mult, 0, 0, param.identifier, 0);
            }
	  }
	}
	break;
      }
    }
    break;
  }

  if (BLE_LowLevel_ACI_Event_CB != NULL)
    BLE_LowLevel_ACI_Event_CB(pckt);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
