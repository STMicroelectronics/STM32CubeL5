/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  proximity_monitor.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      19/08/2014
*   $Revision$:  second version
*   $Author$:   
*   Comments:    Redesigned the Proximity Monitor Profile  according 
*                to unified BlueNRG DK framework.
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  This file will have the implementation of Proximity Monitor Profile. 
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include "hci.h"
#include "hci_le.h"
#include "bluenrg_aci.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"
#include <stdio.h>
#include <host_config.h>
#include <debug.h>
#include <timer.h>
#include <ble_profile.h>
#include <ble_events.h>
#include <uuid.h>
#include <proximity_monitor.h>

/******************************************************************************
 * Macro Declarations
******************************************************************************/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
                (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
                (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))

/* GATT command Opcode */
#define HCI_CMD_NO_OPERATION				     (0x0000)
#define HCI_VENDOR_CMD_GATT_EXCHG_CONFIG                     (0xFD0B)
#define HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES        (0xFD12)
#define HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE     (0xFD15)
#define HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL                 (0xFD1C)
#define HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS        (0xFD17)
#define HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL          (0xFD18)
#define HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR          (0xFD21)
#define HCI_STATUS_PARAM_CMD_READ_RSSI                       (0x1405)
#define HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE           (0xFD23)
#define HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE        (0xFC8C)
#define HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE              (0xFC81)

/* bit masks to maintain track of the services found */
#define LINK_LOSS_SERVICE_MASK	      (0x01)
#define IMMEDIATE_ALERT_SERVICE_MASK  (0x02)
#define TX_POWER_LEVEL_SERVICE_MASK   (0x04)

/* advertising intervals in terms of 625 micro seconds */
#define PXP_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define PXP_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define PXP_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define PXP_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/* proximity monitor main states */
#define PROXIMITY_MONITOR_STATE_UNINITIALIZED                 (0x00)
#define PROXIMITY_MONITOR_STATE_WAITING_BLE_INIT              (0x10)
#define PROXIMITY_MONITOR_STATE_INITIALIZING                  (0x20)
#define PROXIMITY_MONITOR_STATE_INITIALIZED                   (0x30)
#define PROXIMITY_MONITOR_STATE_ADVERTIZE                     (0x40)
#define PROXIMITY_MONITOR_STATE_CONNECTED                     (0x60)
#define PROXIMITY_MONITOR_STATE_INVALID                       (0xFF)

/* Sub State for PROXIMITY_MONITOR_STATE_INITIALIZED main state */
#define PROXIMITY_MONITOR_STATE_WAITING_FOR_ADVERTISE_STATE           (0x31)

/* Sub States for PROXIMITY_MONITOR_STATE_ADVERTISE main state */
#define PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE                            (0x41)
#define PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START          (0x42)
#define PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING          (0x43)
#define PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST                   (0x44)
#define PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP   (0x45)
#define PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_STOPPING          (0x46)
#define PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_DO_START                   (0x47)
#define PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_STARTING                   (0x48)
#define PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE                            (0x49)
#define PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP            (0x4A)
#define PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_STOPPING                   (0x4B)
#define PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_DO_START              (0x4C)
#define PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_STARTING              (0x4D)
#define PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE                       (0x4E)
#define PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP       (0x4F)
#define PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_STOPPING              (0x50)
#define PROXIMITY_MONITOR_STATE_BLE_ADVERTISING                              (0x51)
#define PROXIMITY_MONITOR_STATE_PROCESSING_PAIRING                           (0x52)


/* Sub States for PROXIMITY_MONITOR_STATE_CONNECTED main state */
#define PROXIMITY_MONITOR_STATE_CONNECTED_IDLE                                       (0x61)
#define PROXIMITY_MONITOR_STATE_EXCHANGING_CONFIG                                    (0x62)
#define PROXIMITY_MONITOR_STATE_EXCHG_CONFIG_COMPLETE                                (0x63)
#define PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES                     (0x64)
#define PROXIMITY_MONITOR_STATE_DISCOVERED_ALL_PRIMARY_SERVICES                      (0x65)
#define PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_LINK_LOSS_SERV                (0x66)
#define PROXIMITY_MONITOR_STATE_DISCOVERED_CHARACS_OF_LINK_LOSS_SERV                 (0x67)
#define PROXIMITY_MONITOR_STATE_WRITING_LL_ALERT_CHARACTERISTIC                      (0x68)
#define PROXIMITY_MONITOR_STATE_WRITTEN_LL_ALERT_CHARACTERISTIC                      (0x69)
#define PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_IMMED_ALERT_SERV              (0x6A)
#define PROXIMITY_MONITOR_STATE_DISCOVERED_CHARACS_OF_IMMED_ALERT_SERV               (0x6B)
#define PROXIMITY_MONITOR_STATE_WRITING_IMM_ALERT_CHARACTERISTIC                     (0x6C)
#define PROXIMITY_MONITOR_STATE_WRITTEN_IMM_ALERT_CHARACTERISTIC                     (0x6D)
#define PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_TX_PWR_SERV                   (0x6E)
#define PROXIMITY_MONITOR_STATE_DISCOVERED_CHARACS_OF_TX_PWR_SERV                    (0x6F)
#define PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR      (0x70)
#define PROXIMITY_MONITOR_STATE_DISCOVERED_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR       (0x71)
#define PROXIMITY_MONITOR_STATE_READING_TX_POWER_LEVEL                               (0x72)
#define PROXIMITY_MONITOR_STATE_READ_TX_POWER_LEVEL_DONE                             (0x73)
#define PROXIMITY_MONITOR_STATE_ENABLING_TX_POWER_LEVEL_NOTIFICATION                 (0x74)
#define PROXIMITY_MONITOR_STATE_ENABLED_TX_POWER_LEVEL_NOTIFICATION                  (0x75)
#define PROXIMITY_MONITOR_STATE_MONITOR                                              (0x76)
#define PROXIMITY_MONITOR_STATE_PATH_LOSS_DETECTED                                   (0x77)
#define PROXIMITY_MONITOR_STATE_SENDING_IMMEDIATE_ALERT                              (0x78)
#define PROXIMITY_MONITOR_STATE_SENT_AN_IMMEDIATE_ALERT                              (0x79)
#define PROXIMITY_MONITOR_STATE_DO_DISCONNECT                                        (0x7A)
#define PROXIMITY_MONITOR_STATE_DISCONNECTING                                        (0x7B)
#define PROXIMITY_MONITOR_STATE_DISCONNECTED                                         (0x7C)
#define PROXIMITY_MONITOR_STATE_WAIT                                                 (0x77)

/*******************************************************************************
* type definitions
*******************************************************************************/
typedef struct _tProximityMonitorContext
{
  /**
   * main state of the heart rate
   * sensor state machine
   */ 
  tProfileState mainState;
	
  /**
   * sub state of the heart rate
   * sensor state machine
   */ 
  tProfileState subState;
   
  /**
   * callback to notify the applciation with
   * the profile specific events
   */ 
  BLE_CALLBACK_FUNCTION_TYPE applicationNotifyFunc;

  /**
   * bit mask of the services found during the
   * discovery process on the reporter
   */ 
  uint8_t servFound;
	
  /**
   * flag to keep track of whether we had
   * previously started the alert on the
   * reporter
   */ 
  uint8_t pathLossNotify;
	
  /**
   * connection handle
   */ 
  uint16_t connHandle;
	
  /**
   * handle of link loss service
   */ 
  uint16_t linkLossServHandle;
	
  /**
   * end handle of link loss service
   */ 
  uint16_t linkLossServEndHandle;
	
  /**
   * handle of the immediate alert service
   */ 
  uint16_t immAlertServHandle;
	
  /**
   * End handle of the immediate alert service
   */ 
  uint16_t immAlertServEndHandle;
	
  /**
   * handle of the TX power level service
   */ 
  uint16_t txPwrLvlServHandle;
	
  /**
   * End handle of the TX power level service
   */ 
  uint16_t txPwrLvlServEndHandle;
	
  /**
   * characteristic handle of the alert level
   * characteristic of the link loss service
   */ 
  uint16_t linkLossAlertLevelCharHandle;
	
  /**
   * characteristic handle of the alert level
   * characteristic of immediate alert service
   */
  uint16_t immAlertLevelCharHandle;
	
  /**
   * characteristic handle of the TX power
   * level characteristic
   */ 
  uint16_t txPowerLevelCharHandle;
	
  /**
   * characteristic descriptor handle of the
   * client configuration descriptor
   */ 
  uint16_t txPwrLevlCharConfigDescHandle;
	
  /**
   * characteristic descriptor handle of the
   * TX power level presentation format
   * descriptor
   */ 
  uint16_t txPwrLevlCharPresFormatDescHandle;
	
  /**
   * ID of the timer started by the
   * proximity monitor
   */ 
  tTimerID timerID;
 
  /**
   * TX power level of the reporter
   * as read from the TX power level service
   */ 
  int8_t  txPwrLvl;
	
  /**
   * profile interface context to be registered
   * with the main profile
   */ 
  tProfileInterfaceContext PMtoBLEInf;

  /**
   * BLE Procedure ongoing
   */
  uint16_t opCode;
}tProximityMonitorContext;

/******************************************************************************
* Variables
******************************************************************************/
tProximityMonitorContext proximityMonitor;

/******************************************************************************
 * Imported Variable
******************************************************************************/
extern void *profiledbgfile;

/******************************************************************************
 * Function Declarations
******************************************************************************/
static void ProximityMonitor_Rx_Event_Handler(void *pckt);
static void ProximityMonitor_GATT_Procedure_Complete_EvtHandler(void);
static void ProximityMonitor_Advertise_Period_Timeout_Handler(void);

static void PMProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState PMProfile_Read_MainStateMachine(void);
static void PMProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState PMProfile_Read_SubStateMachine(void);

/******************************************************************************
* SAP
* ******************************************************************************/
tBleStatus ProximityMonitor_Init(BLE_CALLBACK_FUNCTION_TYPE profileCallbackFunc)
{
  tBleStatus retval = BLE_STATUS_FAILED;

  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Proximity Monitor Profile Init \n" );
    
  if (Is_Profile_Present(PID_PROXIMITY_MONITOR) == BLE_STATUS_SUCCESS)
  {
    return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }

  if (profileCallbackFunc == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }

  /* Initialize Profile's Data Structures */
  BLUENRG_memset ( &proximityMonitor, 0, sizeof(tProximityMonitorContext) );

  /* Initialize Current Profile to BLE main Profile Interface data structure */
  BLUENRG_memset ( &proximityMonitor.PMtoBLEInf, 
		0, 
		sizeof(tProfileInterfaceContext) );

  proximityMonitor.PMtoBLEInf.profileID = PID_PROXIMITY_MONITOR;
  proximityMonitor.PMtoBLEInf.callback_func = ProximityMonitor_Rx_Event_Handler;
  proximityMonitor.PMtoBLEInf.voteForAdvertisableState = 0;

  proximityMonitor.applicationNotifyFunc = profileCallbackFunc;
  proximityMonitor.timerID = 0xFF;

  retval = BLE_Profile_Register_Profile ( &proximityMonitor.PMtoBLEInf );
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    return (retval);
  }

  /* Initialize profile's state machine's state variables */
  PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_WAITING_BLE_INIT);
  PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_WAITING_BLE_INIT);

  PROFILE_MESG_DBG ( profiledbgfile, "Proximity Monitor Profile is Initialized \n");
  return (BLE_STATUS_SUCCESS);
}

tBleStatus ProximityMonitor_Make_Discoverable(uint8_t useBoundedDeviceList)
{
  tBleStatus retval = BLE_STATUS_FAILED;

  if ((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_ADVERTIZE)  &&  
      (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE))
  {
    PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Proximity Monitor Set Discoverable mode: %d \n", useBoundedDeviceList );
    if ((useBoundedDeviceList) && (gBLEProfileContext.bleSecurityParam.bonding_mode))
    {
      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START);
    }
    else
    {
      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_DO_START);
    }
    retval = BLE_STATUS_SUCCESS;
  }
  else
  {
    PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Proximity Monitor Set Discoverable mode: Invalid Profile State \n");
  }
  
  return (retval);
}

tBleStatus ProximityMonitor_Notify_Path_Loss(uint8_t enableAlert)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  PROFILE_MESG_DBG ( profiledbgfile, "ProximityMonitor_Notify_Path_Loss(), enableAlert value...............%x \n",enableAlert);
  
  retval = aci_gatt_write_without_response(proximityMonitor.connHandle,
                                           proximityMonitor.immAlertLevelCharHandle, 
                                           (uint8_t)1, 
                                           (uint8_t *)&enableAlert);
                                                                                                                                            
  PROFILE_MESG_DBG ( profiledbgfile, "ProximityMonitor_Notify_Path_Loss() return value......%x \n",retval);													
  return (retval);
}
/******************************************************************************
* Local Functions
********************************************************************************/

static void PMProfile_Write_MainStateMachine(tProfileState localmainState)
{
  proximityMonitor.mainState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState PMProfile_Read_MainStateMachine(void)
{
  return(proximityMonitor.mainState);
}

static void PMProfile_Write_SubStateMachine(tProfileState localsubState)
{
  proximityMonitor.subState = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState PMProfile_Read_SubStateMachine(void)
{
  return(proximityMonitor.subState);
}

/**
 * ProximityMonitor_Rx_Event_Handler
 * 
 * @param[in] pckt : pointer to the event data
 * 
 * parses the events received from HCI according
 * to the profile's state machine
 */ 
static void ProximityMonitor_Rx_Event_Handler(void *pckt)
{
  uint8_t respDataLen = 0;
  uint8_t groupBaseIndx = 1;
  uint16_t startHandle = 0;
  uint16_t endHandle = 0;
  uint16_t charHandle = 0;
  uint16_t uuid = 0;
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
	
  PROXIMITY_MONITOR_MESG_DBG(profiledbgfile, "Proximity Monitor: MainState %02X, SubState %02X \n", 
			     PMProfile_Read_MainStateMachine(), PMProfile_Read_SubStateMachine());
  
  switch(event_pckt->evt)
  {
  case EVT_DISCONN_COMPLETE:
    {
      PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Received event EVT_DISCONN_COMPLETE \n");
      if (((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_ADVERTIZE) && 
	   (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_PROCESSING_PAIRING)) || 
	  (PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_CONNECTED))
      {
	PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_ADVERTIZE);
	PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE);
	respDataLen = LINK_LOSS_ALERT_LEVEL;
	proximityMonitor.applicationNotifyFunc(EVT_PM_LINK_LOSS_ALERT,1,&respDataLen);
      }
    }
    break;
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent)
      {
      case EVT_LE_CONN_COMPLETE:
	{
	  evt_le_connection_complete *cc = (void *)evt->data;

	  PROXIMITY_MONITOR_MESG_DBG(profiledbgfile, "Received event EVT_LE_CONN_COMPLETE \n");
                   
	  /* check event status */
	  if (cc->status == BLE_STATUS_SUCCESS)
	  {
	    proximityMonitor.connHandle = cc->handle;
	    /* if the main profile is in the connected state that means
	     * we need not wait for pairing
	     */ 
	    if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTED)
	    {
	      PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_CONNECTED);
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_EXCHANGING_CONFIG);
		
	      /* the MTU size has to be negotiated first */
	      BLE_Profile_Start_Configuration_Exchange();
	    }
	    else
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_PROCESSING_PAIRING);
	    }
	  }
	  else
	  {
	    /* connection was not successful, now we need to wait for
	     * the application to put the device in discoverable mode
	     */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE);
	  }
	  /* Here there is not need to do anything extra if this profile is not
	   * controlling the advertising process. Above if-else statement does
	   * the needful. */
	  /* Stop if any timer is running */
          
	}
	break; /* EVT_LE_CONN_COMPLETE */
      } /* switch(evt->subevent) */
    }
    break; /* EVT_LE_META_EVENT */
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;

      PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "VENDOR EVT CODE 0x%04x \n", blue_evt->ecode);

      switch (blue_evt->ecode)
      {
      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
	{
	  PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "Received EVT_BLUE_GAP_LIMITED_DISCOVERABLE \n");
	  
	  if ((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_ADVERTIZE) && 
	      ((PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE) ||
	       (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE)))
	  {
	    /* Put the main profile in connectable idle state */
	    BLE_Profile_Change_Advertise_Sub_State(&proximityMonitor.PMtoBLEInf, 
						   BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	    if(PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE)
	    {
	      /* Limited Discoverable mode has timed out */
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE);
	      
	      /* notify the application */
	      proximityMonitor.applicationNotifyFunc(EVT_MP_ADVERTISE_ERROR,0x00,NULL);
	    }
	    PROXIMITY_MONITOR_MESG_DBG(profiledbgfile, "Limited Discoverable mode has timed out \n");
	  }
	}
	break;
      case EVT_BLUE_GAP_PAIRING_CMPLT:
	{
	  evt_gap_pairing_cmplt *pairing = (void*)blue_evt->data;

	  PROXIMITY_MONITOR_MESG_DBG(profiledbgfile, "EVT_BLUE_GAP_PAIRING_CMPLT \n");

	  if ((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_ADVERTIZE) && 
	      (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_PROCESSING_PAIRING))
	  {
	    if (pairing->status == BLE_STATUS_SUCCESS)
	    {
	      PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_CONNECTED);
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_EXCHANGING_CONFIG);
              
	      /* the MTU size has to be negotiated first */
	      BLE_Profile_Start_Configuration_Exchange(); 
	    }
	  }
	}
	break; /* EVT_BLUE_GAP_PAIRING_CMPLT */
      case EVT_BLUE_GATT_PROCEDURE_TIMEOUT:
	{
	  PROXIMITY_MONITOR_MESG_DBG(profiledbgfile, "EVT_BLUE_GATT_PROCEDURE_TIMEOUT \n");
	}
	break;
      case EVT_BLUE_ATT_EXCHANGE_MTU_RESP:
	{
	  PROXIMITY_MONITOR_MESG_DBG(profiledbgfile, "EVT_BLUE_ATT_EXCHANGE_MTU_RESP \n");
	  PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_EXCHG_CONFIG_COMPLETE);
	}
	break;
      case EVT_BLUE_ATT_FIND_INFORMATION_RESP:
	{
	  evt_att_find_information_resp *infRsp = (void*)blue_evt->data;

	  PROXIMITY_MONITOR_MESG_DBG(profiledbgfile, "EVT_BLUE_ATT_FIND_INFORMATION_RESP \n");

	  if((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_CONNECTED) &&
	     (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR))
	  {
	    if (infRsp->event_data_length <= 1)
	    {
	      /* ?? */
	      break;
	    }
	    groupBaseIndx = 0;
                        
	    if (infRsp->format == 0x01) /* format = 01 :: handle with 16 bit UUID */
	    {
	      infRsp->event_data_length -= 1;
	      while (infRsp->event_data_length > 0)
	      {
		uint16_t handle = UNPACK_2_BYTE_PARAMETER (infRsp->handle_uuid_pair + groupBaseIndx);
		uint16_t uuid = UNPACK_2_BYTE_PARAMETER (infRsp->handle_uuid_pair + groupBaseIndx + 2);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"***************************************************************\n");
		PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Descriptor Handle: %04X \n", handle);
		PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Descriptor UUID:   %04X \n", uuid);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"***************************************************************\n");
		
		if (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID)
		{
		  proximityMonitor.txPwrLevlCharConfigDescHandle = handle;
		}
		
		infRsp->event_data_length -= 4;
		groupBaseIndx += 4;
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
	{
	  evt_att_read_by_type_resp *serRsp = (void*)blue_evt->data;
	  
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_BY_TYPE_RESP \n");
	  if ((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_CONNECTED) && 
	      ((PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_LINK_LOSS_SERV) || 
	       (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_TX_PWR_SERV) || 
	       (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_IMMED_ALERT_SERV)))
	  {
	    if (serRsp->event_data_length <= 1)
	    {
	      /* ?? */
	      break;
	    }
	    groupBaseIndx = 0;
	    /**
	     * Resp event group data fromat:
	     * uint16_t Start_Handle,
	     * uint8_t Char_Properties, 
	     * uint16_t Char_Handle, 
	     * uint16_t Char_UUID
	     */
	    if (serRsp->handle_value_pair_length == 7)
	    {
	      uint16_t startHandle;
	      uint8_t charProp;

	      serRsp->event_data_length -= 1;
	      while (serRsp->event_data_length > 0)
	      {
		startHandle =  UNPACK_2_BYTE_PARAMETER (serRsp->handle_value_pair + groupBaseIndx);
		charProp = *(serRsp->handle_value_pair + groupBaseIndx + 2);
		charHandle = UNPACK_2_BYTE_PARAMETER (serRsp->handle_value_pair + groupBaseIndx + 3);
		uuid = UNPACK_2_BYTE_PARAMETER (serRsp->handle_value_pair + groupBaseIndx + 5);
		
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"***************************************************************\n");
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"Char Start Handle 0x%04x\n", startHandle);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"Char Properties 0x%02x\n", charProp);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"Char Handle 0x%04x\n", charHandle);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"UUID 0x%04x\n", uuid);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile,"***************************************************************\n");
		if (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_LINK_LOSS_SERV)
		{
		  if (uuid == ALERT_LEVEL_CHARACTERISTIC_UUID)
		  {
		    PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "********Discovered Alert Level Char \n");
		    proximityMonitor.linkLossAlertLevelCharHandle = charHandle;
		  }
		}
		else if (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_TX_PWR_SERV)
		{
		  if (uuid == TX_POWER_LEVEL_CHARACTERISTIC_UUID)
		  {
		    PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "********Discovered Tx Power Level Char \n");
		    proximityMonitor.txPowerLevelCharHandle = charHandle;
		  }
		}
		else if (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_IMMED_ALERT_SERV)
		{
		  if (uuid == ALERT_LEVEL_CHARACTERISTIC_UUID)
		  {
		    PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "*********Discovered Alert Level Char \n");
		    proximityMonitor.immAlertLevelCharHandle = charHandle;
		  }
		}

		serRsp->event_data_length -= 7;
		groupBaseIndx += 7;
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_RESP:
	{
	  evt_att_read_resp *readRsp = (void*)blue_evt->data;

	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_RESP \n");
	  
	  if((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_CONNECTED) &&
	     (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_READING_TX_POWER_LEVEL))
	  {
	    /* the TX power level is an 8 bit integer */
	    if (readRsp->event_data_length == 1)
	    {
	      proximityMonitor.txPwrLvl = readRsp->attribute_value[0];
	      PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "********************Read Resp Tx Power Level %d \n", proximityMonitor.txPwrLvl );
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
	{
	  evt_att_read_by_group_resp *readGrpRsp = (void*)blue_evt->data;

	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP \n");
		
	  if ((PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_CONNECTED) && 
	      (PMProfile_Read_SubStateMachine() == PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES))
	  {
	    if (readGrpRsp->event_data_length <= 1)
	    {
	      /* ?? */
	      break;
	    }
	    groupBaseIndx = 0;
	    /**
	     * Resp event group data fromat:
	     * uint16_t Start_Handle,
	     * uint16_t End_Handle, 
	     * uint16_t UUID
	     */						
	    if (readGrpRsp->attribute_data_length == 6)
	    {
	      readGrpRsp->event_data_length -= 1;
	      PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "*************** PRIMARY SERVICE FOUND ***************************\n");
	      while (readGrpRsp->event_data_length > 0)
	      {
		startHandle = UNPACK_2_BYTE_PARAMETER (readGrpRsp->attribute_data_list + groupBaseIndx);
		endHandle   = UNPACK_2_BYTE_PARAMETER (readGrpRsp->attribute_data_list + groupBaseIndx + 2);
		uuid        = UNPACK_2_BYTE_PARAMETER (readGrpRsp->attribute_data_list + groupBaseIndx + 4);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "************************** UUID is 0x%04x\n",uuid);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "************************** StartHandle is 0x%02x\n",startHandle);
		PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "************************** EndHandle is 0x%02x\n",endHandle);
		
		if (uuid == LINK_LOSS_SERVICE_UUID)
		{
		  proximityMonitor.servFound |= LINK_LOSS_SERVICE_MASK;
		  proximityMonitor.linkLossServHandle = startHandle;
		  proximityMonitor.linkLossServEndHandle = endHandle;
		}
		else if (uuid == IMMEDIATE_ALERT_SERVICE_UUID)
		{
		  proximityMonitor.servFound |= IMMEDIATE_ALERT_SERVICE_MASK;
		  proximityMonitor.immAlertServHandle = startHandle;
		  proximityMonitor.immAlertServEndHandle = endHandle;
		}
		else if (uuid == TX_POWER_SERVICE_UUID)
		{
		  proximityMonitor.servFound |= TX_POWER_LEVEL_SERVICE_MASK;
		  proximityMonitor.txPwrLvlServHandle = startHandle;
		  proximityMonitor.txPwrLvlServEndHandle = endHandle;
		}

		readGrpRsp->event_data_length -= 6;
		groupBaseIndx += 6;
	      }
	    }
	  }
	}
	break;  /* EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP */
      case EVT_BLUE_GATT_NOTIFICATION:
	{
	  evt_gatt_attr_notification *notify = (void*)blue_evt->data;

	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_NOTIFICATION \n");
	  if (PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_CONNECTED)
	  {
	    if ((notify->attr_handle == proximityMonitor.txPowerLevelCharHandle) && 
		(notify->event_data_length == 2+1))
	    {
	      proximityMonitor.txPwrLvl = notify->attr_value[0];
	      PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "************Notification Tx Power Level %d Dbm \n", proximityMonitor.txPwrLvl );
	    }
	  }
	}
	break;
      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_PROCEDURE_COMPLETE \n");
	  /* we know the procedure has completed, so move the state machine to
	   * the next state
	   */ 
	  ProximityMonitor_GATT_Procedure_Complete_EvtHandler();
	}
	break;
      case EVT_BLUE_GATT_ERROR_RESP:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_ERROR_RESP \n");
	}
	break;
       }
    }
    break; /* EVT_VENDOR */
  }
}

/**
 * PM_Exit
 * 
 * @param[in] errCode : error code to be passed to the application
 */ 
void PM_Exit(uint8_t errCode)
{
  /* one of the requirements of the proximity monitor is not met
   * so unregister the profile with the main profile and notify the
   * application
   */ 
  PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_UNINITIALIZED);
  PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_UNINITIALIZED);
	
  BLE_Profile_Unregister_Profile(&proximityMonitor.PMtoBLEInf);
	
  proximityMonitor.applicationNotifyFunc(EVT_PM_DISCOVERY_CMPLT,1,&errCode);
}

/**
 * ProximityMonitor_GATT_Procedure_Complete_EvtHandler
 * 
 * @param[in] None
 */ 
void ProximityMonitor_GATT_Procedure_Complete_EvtHandler (void)
{
  uint8_t alertLevelCharValLen = 1;
  uint8_t alertLevelCharValue = HIGH_ALERT;
  tBleStatus retval = BLE_STATUS_FAILED;
  uint32_t doCheckForDisconnProcess = 1;
  uint16_t txPwrLevlCharDescValue = 0x0000;
	
  PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "GATT Procedure Complete Evt opcode %04X \n", 
			      proximityMonitor.opCode);

  /* The following gatt procedures have to be performed one after the other
   * 
   * 1. discover all primary services  
   * 2. discover all characteristics of link loss service 
   * 3. write required alert_level to the alert level characteristic of link loss service 
   * 4. discover all characteristics of immediate alert service 
   * 5. discover all characteristics of tx power service 
   * 6. discover all characteristic descriptors of tx power level characteristic 
   * 7. read the tx power level characteristic tx power service 
   * 8. periodically monitor RSSI of the connection with reporter 
   */
  switch (PMProfile_Read_MainStateMachine())
  {
  case PROXIMITY_MONITOR_STATE_CONNECTED:
    {
      switch (PMProfile_Read_SubStateMachine())
      {
      case PROXIMITY_MONITOR_STATE_CONNECTED_IDLE:
	break;
      case PROXIMITY_MONITOR_STATE_EXCHANGING_CONFIG:
	break;
      case PROXIMITY_MONITOR_STATE_EXCHG_CONFIG_COMPLETE:
	{
	  PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "Requesting to Discover all Primary services \n");

	  retval = aci_gatt_disc_all_prim_services(proximityMonitor.connHandle);
	  if (retval == BLE_STATUS_SUCCESS)
	  {
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES);
	    proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES;
	  }
	  else
	  {
	    /* Service discovery failed, retry */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_EXCHG_CONFIG_COMPLETE);
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES:
	{
	  PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES\n");
					
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES)
	  {
	    PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "proximityMonitor.servFound = %x\n",proximityMonitor.servFound);
	    if(proximityMonitor.servFound & LINK_LOSS_SERVICE_MASK)
	    {	      
	      /* Immediate Alert Service and Tx Power Service can be supporter only if both are supported  */
	      if ((!(proximityMonitor.servFound & IMMEDIATE_ALERT_SERVICE_MASK)) || 
		  (!(proximityMonitor.servFound & TX_POWER_LEVEL_SERVICE_MASK)))
	      {
		proximityMonitor.servFound &= ~(IMMEDIATE_ALERT_SERVICE_MASK | TX_POWER_LEVEL_SERVICE_MASK);
	      }
	      retval = aci_gatt_disc_all_charac_of_serv(proximityMonitor.connHandle,
							proximityMonitor.linkLossServHandle, 
							proximityMonitor.linkLossServEndHandle );
	      if (retval == BLE_STATUS_SUCCESS)
	      {
		PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_LINK_LOSS_SERV);
		proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE;
	      }
	      else
	      {
		PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES);
	      }
	    }
	    else
	    {
	      PM_Exit(PM_LINK_LOSS_SERVICE_NOT_FOUND);
	      break;
	    }
	  }
	  else
	  {
	    break;
	  }
	}  /* PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES */
	break;
      case PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_LINK_LOSS_SERV:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE)
	  {
	    alertLevelCharValue = HIGH_ALERT;
	    retval = aci_gatt_write_charac_value(proximityMonitor.connHandle,
						 proximityMonitor.linkLossAlertLevelCharHandle, 
						 alertLevelCharValLen, 
						 &alertLevelCharValue );
	    if (retval == BLE_STATUS_SUCCESS)
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_WRITING_LL_ALERT_CHARACTERISTIC);
	      proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL;
	    }
	    else
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_LINK_LOSS_SERV);
	    }
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_WRITING_LL_ALERT_CHARACTERISTIC:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL)
	  {
	    if (proximityMonitor.servFound & IMMEDIATE_ALERT_SERVICE_MASK)
	    {
	      retval = aci_gatt_disc_all_charac_of_serv(proximityMonitor.connHandle,
							proximityMonitor.immAlertServHandle, 
							proximityMonitor.immAlertServEndHandle);
	      if (retval == BLE_STATUS_SUCCESS)
	      {
		PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_IMMED_ALERT_SERV);
		proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE;
	      }
	      else
	      {
		PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_WRITING_LL_ALERT_CHARACTERISTIC);
	      }
	    }
	    else
	    {
	      /* we have to just enter a wait state. Since we did not discover
	       * the TX power level and immediate alert services we cannot
	       * not monitor the path loss since we donot know the  TX power
	       * level
	       */ 
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_WAIT);
	      /*ProximityMonitor_GATT_Procedure_Complete_EvtHandler ();*/
	    }
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_IMMED_ALERT_SERV:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE)
	  {
	    retval = aci_gatt_disc_all_charac_of_serv(proximityMonitor.connHandle,
						      proximityMonitor.txPwrLvlServHandle, 
						      proximityMonitor.txPwrLvlServEndHandle);
	    if (retval == BLE_STATUS_SUCCESS)
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_TX_PWR_SERV);
	      proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE;
	    }
	    else
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_IMMED_ALERT_SERV);
	    }
	  }
	  break;
	}
      case PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_TX_PWR_SERV:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE)
	  {
	    retval = aci_gatt_disc_all_charac_descriptors(proximityMonitor.connHandle,
							  proximityMonitor.txPowerLevelCharHandle, 
							  proximityMonitor.txPwrLvlServEndHandle);
	    if (retval == BLE_STATUS_SUCCESS)
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR);
	      proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS;
	    }
	    else
	    {
	      /* Service discovery failed, retry */
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_TX_PWR_SERV);
	    }
	  }
	  break;
	}
      case PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS)
	  {
	    /* Read Tx Power Level */
	    retval = aci_gatt_read_charac_val(proximityMonitor.connHandle,
					      proximityMonitor.txPowerLevelCharHandle);
	    if (retval == BLE_STATUS_SUCCESS)
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_READING_TX_POWER_LEVEL);
	      proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL;
	    }
	    else
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR);
	    }
	  }
	  break;
	}
      case PROXIMITY_MONITOR_STATE_READING_TX_POWER_LEVEL:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL)
	  {
	    PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "Enabling Tx Power Level Char Notification \n");
	  
	    txPwrLevlCharDescValue = 0x0001;
	    retval = aci_gatt_write_charac_descriptor(proximityMonitor.connHandle,
						      proximityMonitor.txPwrLevlCharConfigDescHandle, 
						      (uint8_t)2, 
						      (uint8_t *)&txPwrLevlCharDescValue );
	    if (retval == BLE_STATUS_SUCCESS)
	    {
	      PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ENABLING_TX_POWER_LEVEL_NOTIFICATION);
	      proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR;
	    }
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_ENABLING_TX_POWER_LEVEL_NOTIFICATION:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR)
	  {
	    PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Enabled Tx Power Level Char Notification \n" );
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_MONITOR);
	    proximityMonitor.opCode = HCI_STATUS_PARAM_CMD_READ_RSSI;
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_MONITOR:
	break;
      case PROXIMITY_MONITOR_STATE_PATH_LOSS_DETECTED:
	break;
      case PROXIMITY_MONITOR_STATE_SENDING_IMMEDIATE_ALERT:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE)
	  {
	    PROXIMITY_MONITOR_MESG_DBG (profiledbgfile, "Immediate Alert on Link Loss has been sent \n");
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_MONITOR);
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_DO_DISCONNECT:
      case PROXIMITY_MONITOR_STATE_DISCONNECTING:
      case PROXIMITY_MONITOR_STATE_DISCONNECTED:
	{
	  doCheckForDisconnProcess = 0;
	}
	break;
      } /* switch (PMProfile_Read_SubStateMachine()) */

      if (doCheckForDisconnProcess)
      {
	if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING)
	{
	  PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_CONNECTED);
	  PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DO_DISCONNECT);
	}
      }
    }
    break; /* PROXIMITY_MONITOR_STATE_CONNECTED */
  }

  return;
}

/**
 * ProximityMonitorProfile_StateMachine
 * 
 * @param[in] None
 * 
 * Proximity Monitor profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ProximityMonitorProfile_StateMachine(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint8_t localName[20] = {0};
  uint8_t pathLoss;
  uint8_t doCheckForDisconnectProcess = 0;
    
  //PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Profile_Parse_Received_Response %02X \n", proximityMonitor.opCode );

  switch (PMProfile_Read_MainStateMachine())
  {
  case PROXIMITY_MONITOR_STATE_WAITING_BLE_INIT:
    {
      /* chech whether the main profile has done with ble init or not */
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
	PROXIMITY_REPORTER_MESG_DBG( profiledbgfile, "Proximity Monitor Profile is in initialized State \n");
	
	/* As there is nothing to initialize by the Proximity Monitor Profile, 
	 * the profile will directly go to INITIALIZED IDLE state */
	PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_INITIALIZED);
	PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_INVALID);
	
	/* vote to the main profile to go to advertise state */
	(void) BLE_Profile_Vote_For_Advertisable_State (&proximityMonitor.PMtoBLEInf);
      }
    }
    break; /* PROXIMITY_MONITOR_STATE_WAITING_BLE_INIT */
  case PROXIMITY_MONITOR_STATE_INITIALIZED:
    {
      /* Proximity Reporter has completed its initialization process and voted the 
       * main profile for Advertisable state. Now its waiting for the main profile
       * to set to Advertisable state. */
      
      /* check main profile's state, if its in advertisable state, set the same for 
       * current profile as well */
      if ((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) && 
	  (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CONNECTABLE_IDLE))
      {
	PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_ADVERTIZE);
	PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE);
	pathLoss = BLE_STATUS_SUCCESS;
	/* inform the application that the profile is ready to advertize */
	proximityMonitor.applicationNotifyFunc(EVT_PM_INITIALIZED,1,&pathLoss);
      }
    }
    break; /* PROXIMITY_MONITOR_STATE_INITIALIZED */
  case PROXIMITY_MONITOR_STATE_ADVERTIZE:
    {
      switch (PMProfile_Read_SubStateMachine())
      {
      case PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE:
	// ??????????????????????? /
/* 	{ */
/* 	  if ((proximityMonitor.opCode == HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE) ||  */
/* 	      (proximityMonitor.opCode == HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE) || */
/* 	      (proximityMonitor.opCode == HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE)) */
/* 	  { */
/* 	    /\* This profile is in Advertisable_Idle state, it means that the profile is  */
/* 	     * waiting for set to discoverable mode request from application and hence  */
/* 	     * has not issued any command to BLE. Now a Command Complete Event for  */
/* 	     * Gap_Set_Discoverable HCI Command is received. It means that some other  */
/* 	     * profile is controlling the Advertising process. Hence there is nothing  */
/* 	     * much to do here, just change the profile's state and wait to BLE for  */
/* 	     * connection complete event  */
/* 	     *\/ */
/* 	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_BLE_ADVERTISING); */
/* 	  } */
/* 	} */
	break;
      case PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "STARTING Advertising for White List devices \n");
	  
	  hciCmdResult = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING);
	    
	    proximityMonitor.opCode = HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE;
                      
	    /* start a timer of 10 secconds to try to connnect to white list device */
	    Blue_NRG_Timer_Start (10, ProximityMonitor_Advertise_Period_Timeout_Handler,&proximityMonitor.timerID);
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityMonitor.PMtoBLEInf, 
						    BLE_PROFILE_STATE_ADVERTISING);
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST:
	break;
      case PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "STOPPING Advertising to White List devices \n");
          
	  hciCmdResult = aci_gap_set_non_discoverable();
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_STOPPING);
	    proximityMonitor.opCode = HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE;
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_STOPPING:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_DO_START);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityMonitor.PMtoBLEInf, 
						    BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	  }
	  else
	  {
	    break;
	  }
	}
      case PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_DO_START:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "STARTING FAST Limited Discoverable Mode \n");
	  
	  localName[0] = 0x08;
	  BLUENRG_memcpy ((void *)(localName+1), (void *)"PM", 0x02 );
	  
	  hciCmdResult = aci_gap_set_limited_discoverable(ADV_IND,
							  (uint16_t)PXP_FAST_CONN_ADV_INTERVAL_MIN,
							  (uint16_t)PXP_FAST_CONN_ADV_INTERVAL_MAX,
							  PUBLIC_ADDR, 
							  NO_WHITE_LIST_USE,
							  3,
							  (const char*)localName,
							  gBLEProfileContext.advtServUUIDlen,
							  gBLEProfileContext.advtServUUID,
							  0x0,    
							  0x0     
							  );
	  if ( hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE);

	    /* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
	    Blue_NRG_Timer_Start (30, ProximityMonitor_Advertise_Period_Timeout_Handler,&proximityMonitor.timerID);

	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityMonitor.PMtoBLEInf, 
						    BLE_PROFILE_STATE_ADVERTISING);
	  } 
	  else 
	  {
	    /* Request to put the device in limited discoverable mode is 
	     * unsuccess, put the device back in advrtisable idle mode */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityMonitor.PMtoBLEInf, 
						    BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	    
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_STARTING:
	break;
      case PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE:
	break;
      case PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "STOPPING FAST Limited Discoverable Mode \n");
          
	  hciCmdResult = aci_gap_set_non_discoverable();
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_STOPPING);
	    proximityMonitor.opCode = HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE;
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_STOPPING:
	{
	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_DO_START);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityMonitor.PMtoBLEInf, 
						    BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	  }
	  else
	  {
	    break;
	  }
	}
      case PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_DO_START:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "STARTING Low Power General Discoverable Mode \n");
	  
	  localName[0] = 0x08;
	  BLUENRG_memcpy ((void *)(localName+1), (void *)"PM", 0x02);

	  hciCmdResult = aci_gap_set_limited_discoverable(ADV_IND,
							  (uint16_t)PXP_LOW_PWR_ADV_INTERVAL_MIN,
							  (uint16_t)PXP_LOW_PWR_ADV_INTERVAL_MAX,
							  PUBLIC_ADDR, 
							  NO_WHITE_LIST_USE,
							  3,
							  (const char*)localName,
							  gBLEProfileContext.advtServUUIDlen,
							  gBLEProfileContext.advtServUUID,
							  0x0,    
							  0x0      
							  );
	  if ( hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityMonitor.PMtoBLEInf, 
						    BLE_PROFILE_STATE_ADVERTISING);
	  }
	  else
	  {
	    /* Request to put the device in limited discoverable mode is 
	     * unsuccess, put the device back in advrtisable idle mode */
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityMonitor.PMtoBLEInf, 
						    BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_STARTING:
	break;
      case PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE:
	break;
      case PROXIMITY_MONITOR_STATE_BLE_ADVERTISING:
	// ??????????????????
/* 	{ */
/* 	  if (proximityMonitor.opCode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE) */
/* 	  { */
/* 	    /\* Advertising process controlled by some other profile has been stopped *\/ */
/* 	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_ADVERTISABLE_IDLE); */
/* 	  } */
/* 	} */
	break;
      case PROXIMITY_MONITOR_STATE_PROCESSING_PAIRING:
	{
	  if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING)
	  {
	    PMProfile_Write_MainStateMachine(PROXIMITY_MONITOR_STATE_CONNECTED);
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DO_DISCONNECT);
	  }
	}
	break;
      }
    }
    break; /* PROXIMITY_MONITOR_STATE_ADVERTISE */
  case PROXIMITY_MONITOR_STATE_CONNECTED:
    {
      doCheckForDisconnectProcess = 1;
      switch (PMProfile_Read_SubStateMachine())
      {
      case PROXIMITY_MONITOR_STATE_CONNECTED_IDLE:
      case PROXIMITY_MONITOR_STATE_EXCHANGING_CONFIG:
      case PROXIMITY_MONITOR_STATE_EXCHG_CONFIG_COMPLETE:
      case PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_PRIMARY_SERVICES:
      case PROXIMITY_MONITOR_STATE_DISCOVERED_ALL_PRIMARY_SERVICES:
      case PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_LINK_LOSS_SERV:
      case PROXIMITY_MONITOR_STATE_DISCOVERED_CHARACS_OF_LINK_LOSS_SERV:
      case PROXIMITY_MONITOR_STATE_WRITING_LL_ALERT_CHARACTERISTIC:
      case PROXIMITY_MONITOR_STATE_WRITTEN_LL_ALERT_CHARACTERISTIC:
      case PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_IMMED_ALERT_SERV:
      case PROXIMITY_MONITOR_STATE_DISCOVERED_CHARACS_OF_IMMED_ALERT_SERV:
      case PROXIMITY_MONITOR_STATE_WRITING_IMM_ALERT_CHARACTERISTIC:
      case PROXIMITY_MONITOR_STATE_WRITTEN_IMM_ALERT_CHARACTERISTIC:
      case PROXIMITY_MONITOR_STATE_DISCOVERING_CHARACS_OF_TX_PWR_SERV:
      case PROXIMITY_MONITOR_STATE_DISCOVERED_CHARACS_OF_TX_PWR_SERV:
      case PROXIMITY_MONITOR_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR:
      case PROXIMITY_MONITOR_STATE_DISCOVERED_ALL_DESCRIPTORS_OF_TX_PWR_LEVL_CHAR:
      case PROXIMITY_MONITOR_STATE_READING_TX_POWER_LEVEL:
      case PROXIMITY_MONITOR_STATE_READ_TX_POWER_LEVEL_DONE:
      case PROXIMITY_MONITOR_STATE_ENABLING_TX_POWER_LEVEL_NOTIFICATION:
      case PROXIMITY_MONITOR_STATE_ENABLED_TX_POWER_LEVEL_NOTIFICATION:
	
	break;
      case PROXIMITY_MONITOR_STATE_MONITOR:
	{
	  /* the proximity monitor needs to keep monitoring the RSSI  value
	   * and calculate the path loss with respect to the TX power
	   * level of the reporter.
	   * If the path loss is greater than a threshold, i.e, it is above an
	   * acceptable value, the alert needs to be configured on the reporter
	   * in the immediate alert service
	   * When the pathloss lies within the threshold, i.e, it becomes acceptable, 
	   * the alert needs to be stopped on the reporter
	   */ 
	  if (proximityMonitor.opCode == HCI_STATUS_PARAM_CMD_READ_RSSI)
	  {
	    int8_t rssi;

	    if (hci_read_rssi(&proximityMonitor.connHandle, &rssi) != BLE_STATUS_SUCCESS)
	      rssi = 0;
	    pathLoss = proximityMonitor.txPwrLvl - rssi;
	    /* calculate path loss */
	    if(pathLoss > PATH_LOSS_THRESHOLD)
	    {
	      pathLoss = PATH_LOSS_ALERT_LEVEL;
	      proximityMonitor.applicationNotifyFunc(EVT_PM_PATH_LOSS_ALERT,1,&pathLoss);
              
	      if (proximityMonitor.servFound & IMMEDIATE_ALERT_SERVICE_MASK)
	      {
		PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_PATH_LOSS_DETECTED);
		proximityMonitor.pathLossNotify = 0x01;
	      }
	      else
	      {
		if (hci_read_rssi(&proximityMonitor.connHandle, &rssi) != BLE_STATUS_SUCCESS)
		  rssi = 0;
	      }
	    }
	    else if(proximityMonitor.pathLossNotify && (pathLoss <= PATH_LOSS_THRESHOLD))
	    {
	      /* we had previously notified a path loss. So the reporter would have started
	       * alerting. Now write to the alert level charcteristic to stop the alerting
	       */ 
	      proximityMonitor.pathLossNotify = 0x00;
	    }
            /* added else */
            else
            {
              break;
            }
	  }
	  else
	  {
	    break;
	  }
	}
      case PROXIMITY_MONITOR_STATE_PATH_LOSS_DETECTED:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "*********************Sending Immediate alert on path loss %d\n",proximityMonitor.pathLossNotify);
          
	  hciCmdResult = aci_gatt_write_without_response(proximityMonitor.connHandle,
							 proximityMonitor.immAlertLevelCharHandle, 
							 (uint8_t)1, 
							 (uint8_t *)&proximityMonitor.pathLossNotify);
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_SENDING_IMMEDIATE_ALERT);
	    proximityMonitor.opCode = HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE;
	  }
	}
	break;
      case PROXIMITY_MONITOR_STATE_SENDING_IMMEDIATE_ALERT:
	break;
      case PROXIMITY_MONITOR_STATE_DO_DISCONNECT:
      case PROXIMITY_MONITOR_STATE_DISCONNECTING:
      case PROXIMITY_MONITOR_STATE_DISCONNECTED:
	doCheckForDisconnectProcess = 0;
	break;
      }
            
      if (doCheckForDisconnectProcess)
      {
	if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING)
	{
	  PMProfile_Write_SubStateMachine(PROXIMITY_MONITOR_STATE_DO_DISCONNECT);
	}
      }
    }
    break; /* PROXIMITY_MONITOR_STATE_CONNECTED */
  }
  return (hciCmdResult);
}

/**
 * ProximityMonitor_Advertise_Period_Timeout_Handler
 * 
 * this function is called when the timer started
 * by the heart rate profile for advertising times out
 */ 
void ProximityMonitor_Advertise_Period_Timeout_Handler (void)
{
  tProfileState nextState = PMProfile_Read_SubStateMachine();
  
  Blue_NRG_Timer_Stop(proximityMonitor.timerID);
  proximityMonitor.timerID = 0xFF;

  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Advertising Period Timedout \n");
  if (PMProfile_Read_MainStateMachine() == PROXIMITY_MONITOR_STATE_ADVERTIZE)
  {
    switch (PMProfile_Read_SubStateMachine())
    {
    case PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST:
      nextState = PROXIMITY_MONITOR_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP;
      PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "White list Advertising Timedout \n");
      break;
    case PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE:
      nextState = PROXIMITY_MONITOR_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP;
      PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Fast Advertising Timedout \n");
      break;
    case PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE:
      nextState = PROXIMITY_MONITOR_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP;
      PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "Low Power Advertising Timedout \n");
      break;
    }

    if (nextState != PMProfile_Read_SubStateMachine())
    {
      PMProfile_Write_SubStateMachine(nextState);
  
      /* Call profile's State Machine engine for state change be effective. */
      proximityMonitor.opCode = HCI_CMD_NO_OPERATION;
      ProximityMonitorProfile_StateMachine();
    }
  }
}
