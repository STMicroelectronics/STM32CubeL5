/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
* File Name          : master_basic_profile_sm.h
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.0
* Date               : 15-October-2014
* Description        : Header for master_basic_profile_sm.c module
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MASTER_BASIC_PROFILE_SM_H
#define __MASTER_BASIC_PROFILE_SM_H

/* Includes ------------------------------------------------------------------*/
#include "bluenrg_aci.h"
#include "master_basic_profile.h"

/* Public macros ------------------------------------------------------------*/

#define GET_CURRENT_STATE() masterState.state
#define GET_CURRENT_SUBSTATE() masterState.subState

/* Defines -------------------------------------------------------------------*/

/* INIT states */
#define STATE_INIT 0x00000000

/* IDLE states */
#define STATE_IDLE 0x00010000

/* DEVICE DISCOVERY states */
#define STATE_DEVICE_DISCOVERY                            0x00020000
#define SUBSTATE_DEVICE_DISCOVERY_INIT                    (STATE_DEVICE_DISCOVERY | 0x0001)
#define SUBSTATE_DEVICE_DISCOVERY_WAITING                 (STATE_DEVICE_DISCOVERY | 0x0002)
#define SUBSTATE_DEVICE_DISCOVERY_DONE                    (STATE_DEVICE_DISCOVERY | 0x0003)

/* CONNECTION states */
#define STATE_CONNECTION                                  0x00030000
#define SUBSTATE_CONNECTION_INIT                          (STATE_CONNECTION | 0x0001)
#define SUBSTATE_CONNECTION_WAITING_END_DISCOVERY         (STATE_CONNECTION | 0x0002)
#define SUBSTATE_CONNECTION_FAST_SCAN_WAITING             (STATE_CONNECTION | 0x0003)
#define SUBSTATE_CONNECTION_WAITING_END_PROC_FAST_SCAN    (STATE_CONNECTION | 0x0004)
#define SUBSTATE_CONNECTION_RED_PWR_SCAN_WAITING          (STATE_CONNECTION | 0x0005)
#define SUBSTATE_CONNECTION_DONE                          (STATE_CONNECTION | 0x0006)

/* CONNECTED IDLE states */
#define STATE_CONNECTED_IDLE                               0x00040000
#define SUBSTATE_CONNECTED_IDLE_INIT                      (STATE_CONNECTED_IDLE | 0x0001)
#define SUBSTATE_CONNECTED_IDLE_WAIT_DATA_EXCHANGE_STATUS (STATE_CONNECTED_IDLE | 0x0002)
#define SUBSTATE_CONNECTED_IDLE_DONE                      (STATE_CONNECTED_IDLE | 0x0003)

/* SERVICE DISCOVERY states */
#define STATE_SERVICE_DISCOVERY                           0x00050000
#define SUBSTATE_SERVICE_DISCOVERY_INIT                   (STATE_SERVICE_DISCOVERY | 0x0001)
#define SUBSTATE_SERVICE_DISCOVERY_ALL_PRIMARY            (STATE_SERVICE_DISCOVERY | 0x0002)
#define SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_PRIMARY       (STATE_SERVICE_DISCOVERY | 0x0003)
#define SUBSTATE_SERVICE_DISCOVERY_VERIFY                 (STATE_SERVICE_DISCOVERY | 0x0004)
#define SUBSTATE_SERVICE_DISCOVERY_ALL_INCLUDE            (STATE_SERVICE_DISCOVERY | 0x0005)
#define SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_INCLUDE       (STATE_SERVICE_DISCOVERY | 0x0006)
#define SUBSTATE_SERVICE_DISCOVERY_ALL_CHARAC             (STATE_SERVICE_DISCOVERY | 0x0007)
#define SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_CHARAC        (STATE_SERVICE_DISCOVERY | 0x0008)
#define SUBSTATE_SERVICE_DISCOVERY_ALL_CHARAC_DESC        (STATE_SERVICE_DISCOVERY | 0x0009)
#define SUBSTATE_SERVICE_DISCOVERY_WAIT_ALL_CHARAC_DESC   (STATE_SERVICE_DISCOVERY | 0x000A)
#define SUBSTATE_SERVICE_DISCOVERY_DONE                   (STATE_SERVICE_DISCOVERY | 0x000B)

/* BONDING states */
#define STATE_BONDING                                     0x00060000
#define SUBSTATE_BONDING_INIT                             (STATE_BONDING | 0x0001)
#define SUBSTATE_BONDING_WAIT                             (STATE_BONDING | 0x0002)
#define SUBSTATE_BONDING_RUN_LAST_PROCEDURE               (STATE_BONDING | 0x0003)
#define SUBSTATE_BONDING_DONE                             (STATE_BONDING | 0x0004)

/* Global State machine Information */
typedef struct StateS {
  /** Macro State */
  uint32_t state;
  /** Substate */
  uint32_t subState;
  /** 
   * Power Save bitmask 
   *  - 1 LSB flag to signal if the micro can enter in power save or not
   *  - 1 bit flag to signal if the micro needs to enable the power save with a timeout
   *  - 30 MSB power save timeout
   */
  uint32_t powerSave_bitMask;
} StateType;

/* Device Discovery Context Type */
typedef struct devDiscoveryContextS {
  uint8_t procedureComplete;
  uint8_t addr_type;
  uint8_t addr[6]; 
  uint8_t procedureUsed;
  uint8_t end_reason;
  uint32_t powerSave_enable;
} devDiscoveryContextType;

/* Connection Context Type */
typedef struct connectionContextS {
  uint8_t discProcedureComplete;  
  uint8_t connProcedureComplete;
  uint8_t connectionCompleted;
  uint16_t connHandle;
  uint32_t scanStartTime;
  connDevType userParam;
  uint32_t powerSave_enable;
  uint8_t hostExit_TimeoutPowerSave;
} connectionContextType;

/* Bonding Context Type */
typedef struct bondingContextS {
  uint8_t procedure;
  uint16_t *data_length;
  uint8_t *data_array;
  uint8_t write_len;
  uint8_t write_data[22];
  uint16_t size;
  uint8_t procedureComplete;
  uint8_t procedureStatus;
  uint16_t conn_handle;
  uint32_t powerSave_enable;
  uint8_t passKeyProc;
} bondingContextType;

/* Service Discovery Type */
typedef struct serDiscContextS {
  uint8_t procedure;
  uint8_t procedureComplete;
  uint8_t procedureStatus;
  uint16_t size;
  uint16_t index;
  uint8_t *numInfoServer;
  uint8_t *arrayInfoServer;
  uint16_t conn_handle;
  uint16_t start_handle;
  uint16_t end_handle;
  uint32_t powerSave_enable;
} serDiscContextType;

/* Connected Idle Context Type */
typedef struct connIdleContextS {
  uint8_t procedure;
  uint16_t *data_length;
  uint8_t *data_array;
  uint8_t write_len;
  uint8_t write_data[22];
  uint16_t size;
  uint8_t procedureComplete;
  uint8_t procedureStatus;
  uint16_t conn_handle;
  uint32_t powerSave_enable;
} connIdleContextType;

/** Dummy type just to compute the largest of all contexts */
typedef union masterGlobalContextU {
  devDiscoveryContextType a;
  connectionContextType b;
  bondingContextType c;
  serDiscContextType d;
  connIdleContextType e;
} masterGlobalContextType;

extern masterGlobalContextType *masterGlobalContext;
extern StateType masterState;
extern void (*BLE_LowLevel_ACI_Event_CB)(void *pckt);

StateType masterStateMachine(StateType state, initDevType *param);
void DEBUG_MESSAGE(uint32_t prefix, uint32_t next);

#endif /*__MASTER_BASIC_PROFILE_SM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
