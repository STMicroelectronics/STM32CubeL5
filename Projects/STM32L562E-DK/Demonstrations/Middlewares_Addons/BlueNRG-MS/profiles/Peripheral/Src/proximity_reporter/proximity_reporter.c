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
*   FILENAME        -  proximity_reporter.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      04/09/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Proximity Reporter Profile  according 
*                to unified BlueNRG DK framework.
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file will have implementation of Proximity Reporter Profile. 
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
#include <proximity_reporter.h>

/******************************************************************************
 * Macro Declarations
******************************************************************************/
#define READ_CURR_TRANSMIT_POWER_LEVEL   (0x00)

/* in terms of 625 micro seconds */
#define PXP_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define PXP_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define PXP_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define PXP_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */


/* Proximity Reporter Profile's main states */
#define PROXIMITY_REPORTER_STATE_UNINITIALIZED                 		 (0x00)
#define PROXIMITY_REPORTER_STATE_WAITING_BLE_INIT              		 (0x10)
#define PROXIMITY_REPORTER_STATE_INITIALIZING                 		 (0x20)
#define PROXIMITY_REPORTER_STATE_INITIALIZED                  		 (0x30)
#define PROXIMITY_REPORTER_STATE_ADVERTISE                    		 (0x40)
#define PROXIMITY_REPORTER_STATE_CONNECTED                    		 (0x50)
#define PROXIMITY_REPORTER_STATE_INVALID                       		 (0xFF)

/* sub states for PROXIMITY_REPORTER_STATE_INITIALIZING main state */
#define PROXIMITY_REPORTER_STATE_ADDING_LINK_LOSS_SERVICE                        (0x21)
#define PROXIMITY_REPORTER_STATE_ADDED_LINK_LOSS_SERVICE                         (0x22)
#define PROXIMITY_REPORTER_STATE_ADDING_LINK_LOSS_ALERT_LEVEL_CHAR               (0x23)
#define PROXIMITY_REPORTER_STATE_ADDED_LINK_LOSS_ALERT_LEVEL_CHAR                (0x24)
#define PROXIMITY_REPORTER_STATE_ADDING_IMMEDIATE_ALERT_SERVICE                  (0x25)
#define PROXIMITY_REPORTER_STATE_ADDED_IMMEDIATE_ALERT_SERVICE                   (0x26)
#define PROXIMITY_REPORTER_STATE_ADDING_IMMEDIATE_ALERT_LEVEL_CHAR               (0x27)
#define PROXIMITY_REPORTER_STATE_ADDED_IMMEDIATE_ALERT_LEVEL_CHAR                (0x28)
#define PROXIMITY_REPORTER_STATE_ADDING_TX_POWER_SERVICE                         (0x29)
#define PROXIMITY_REPORTER_STATE_ADDED_TX_POWER_SERVICE                          (0x2A)
#define PROXIMITY_REPORTER_STATE_ADDING_TX_POWER_LEVEL_CHAR                      (0x2B)
#define PROXIMITY_REPORTER_STATE_ADDED_TX_POWER_LEVEL_CHAR                       (0x2C)
#define PROXIMITY_REPORTER_STATE_ADDING_TX_PWR_LEVL_CHAR_PRESEN_FORMAT_DECS      (0x2D)
#define PROXIMITY_REPORTER_STATE_ADDED_TX_PWR_LEVL_CHAR_PRESEN_FORMAT_DECS       (0x2E)
#define PROXIMITY_REPORTER_STATE_READ_TX_PWR_LEVEL        			 (0x31)
#define PROXIMITY_REPORTER_STATE_UPDATE_TX_PWR_LEVEL_CHAR        		 (0x32)
#define PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR                            (0x2F)


/* Sub State for PROXIMITY_REPORTER_STATE_INITIALIZED main state */
#define PROXIMITY_REPORTER_STATE_WAITING_FOR_ADVERTISE_STATE           (0X31)

/* Sub State for PROXIMITY_REPORTER_STATE_ADVERTISE main state */
#define PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE                     		(0x41)
#define PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START   		(0x42)
#define PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING   		(0x43)
#define PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST            		(0x44)
#define PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP     (0x45)
#define PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_STOPPING            (0x46)
#define PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_DO_START                     (0x47)
#define PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_STARTING                     (0x48)
#define PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE                              (0x49)
#define PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP              (0x4A)
#define PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_STOPPING                     (0x4B)
#define PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE_DO_START                (0x4C)
#define PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE_STARTING                (0x4D)
#define PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE                         (0x4E)
#define PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP         (0x4F)
#define PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE_STOPPING                (0x50)
#define PROXIMITY_REPORTER_STATE_BLE_ADVERTISING                                (0x51)
#define PROXIMITY_REPORTER_STATE_PROCESSING_PAIRING                             (0x52)

/* Sub States for PROXIMITY_REPORTER_STATE_CONNECTED main state */
#define PROXIMITY_REPORTER_STATE_CONNECTED_IDLE                        (0x51)
#define PROXIMITY_REPORTER_STATE_DO_DISCONNECT                         (0x52)
#define PROXIMITY_REPORTER_STATE_DISCONNECTING                         (0x53)
#define PROXIMITY_REPORTER_STATE_DISCONNECTED                          (0x54)

/******************************************************************************
* type definitions
******************************************************************************/
typedef struct _tProximityReporterContext
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
   * this is a flag set during initialization
   * based on the application's value to tell the
   * profile whether the TX power level and
   * immediate alert services have to be exposed
   */ 
  uint8_t immAlertTxPowerServSupport;
    
  /**
   * handle of link loss service
   */ 
  uint16_t linkLossServiceHandle;
	
  /**
   * handle of the alert level charecteristic of
   * link loss service
   */ 
  uint16_t linkLossAlertLevelCharHandle;
	
  /**
   * handle of the immediate alert service
   */ 
  uint16_t immdiateAlertServiceHandle;
	
  /**
   * handle of the alert level characteristic
   * in the immediate alert service
   */ 
  uint16_t immediateAlertLevelCharHandle;
	
  /**
   * handle of the TX power level service
   */ 
  uint16_t txPowerServiceHandle;
	
  /**
   * handle of the TX power level characteristic 
   */ 
  uint16_t txPowerLevelCharHandle;

  /**
   * the alert level that has to be raised
   * on detecting a link loss
   */ 
  uint8_t linkLossAlertLevel;
	
  /**
   * ID of the timer started by the
   * proximity reporter
   */ 
  tTimerID timerID;
	
  /**
   * profile interface context to be registered
   * with the main profile
   */ 
  tProfileInterfaceContext PRtoBLEInf;
}tProximityReporterContext;

/******************************************************************************
* Variables
******************************************************************************/
tProximityReporterContext proximityReporter;

/******************************************************************************
 * Imported Variable
******************************************************************************/
extern void *profiledbgfile;
extern uint8_t bnrg_expansion_board;

/******************************************************************************
*  Function Prototypes
******************************************************************************/
static void ProximityReporter_Rx_Event_Handler(void *pckt);
static void ProximityReporter_Advertise_Period_Timeout_Handler(void);

static void PRProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState PRProfile_Read_MainStateMachine(void);
static void PRProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState PRProfile_Read_SubStateMachine(void);

/******************************************************************************
 * Global Functions (SAP)
******************************************************************************/
tBleStatus ProximityReporter_Init(uint8_t immAlertTxPowerSupport, 
				  BLE_CALLBACK_FUNCTION_TYPE profileCallbackFunc)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Proximity Reporter Profile Init \n" );
    
  if (Is_Profile_Present(PID_PROXIMITY_REPORTER) == BLE_STATUS_SUCCESS)
  {
    return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }

  if (profileCallbackFunc == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }

  /* Initialize Profile's Data Structures */
  BLUENRG_memset ( &proximityReporter, 0, sizeof(tProximityReporterContext) );
  /* Init proximity reporter context variables with their default values */
  proximityReporter.immAlertTxPowerServSupport = immAlertTxPowerSupport;
  proximityReporter.applicationNotifyFunc = profileCallbackFunc;

  /* Initialize Current Profile to BLE main Profile Interface data structure */
  BLUENRG_memset ( &proximityReporter.PRtoBLEInf, 0, sizeof(tProfileInterfaceContext) );
	
  proximityReporter.PRtoBLEInf.profileID = PID_PROXIMITY_REPORTER;
  proximityReporter.PRtoBLEInf.callback_func = ProximityReporter_Rx_Event_Handler;
  proximityReporter.timerID = 0xFF;
  proximityReporter.PRtoBLEInf.voteForAdvertisableState = 0x00;
	
  retval = BLE_Profile_Register_Profile ( &proximityReporter.PRtoBLEInf );
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    return (retval);
  }

  /* Initialize profile's state machine's state variables */
  PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_WAITING_BLE_INIT);
  PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_WAITING_BLE_INIT);

  PROXIMITY_REPORTER_MESG_DBG( profiledbgfile, "Proximity Reporter Profile is Initialized \n");
  return (BLE_STATUS_SUCCESS);
}

tBleStatus ProximityReporter_Make_Discoverable (uint8_t useBoundedDeviceList)
{
  tBleStatus retval = BLE_STATUS_FAILED;

  if ((PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_ADVERTISE)  &&  
      (PRProfile_Read_SubStateMachine() == PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE))
  {
    PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Proximity Reporter Set Discoverable mode: %d \n", useBoundedDeviceList );
    if ((useBoundedDeviceList) && (gBLEProfileContext.bleSecurityParam.bonding_mode))
    {
      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START);
    }
    else
    {
      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_DO_START);
    }
  }
  else
  {
    PROXIMITY_REPORTER_MESG_DBG(profiledbgfile, "PR Set Discoverable mode: Invalid Task State \n");
  }

  return (retval);
}

/***************************************************************************************
* Local functions
****************************************************************************************/

static void PRProfile_Write_MainStateMachine(tProfileState localmainState)
{
  proximityReporter.mainState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState PRProfile_Read_MainStateMachine(void)
{
  return(proximityReporter.mainState);
}

static void PRProfile_Write_SubStateMachine(tProfileState localsubState)
{
  proximityReporter.subState = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState PRProfile_Read_SubStateMachine(void)
{
  return(proximityReporter.subState);
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void PRProfile_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  uint8_t attValue;
  
  PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"EVT_BLUE_GATT_ATTRIBUTE_MODIFIED \n");
  
  if (handle == proximityReporter.linkLossAlertLevelCharHandle+1)
  {
    PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received Link Loss Alert Level Configuration \n");
    if ((PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_CONNECTED) && 
        (PRProfile_Read_SubStateMachine() != PROXIMITY_REPORTER_STATE_DO_DISCONNECT) &&
          (PRProfile_Read_SubStateMachine() != PROXIMITY_REPORTER_STATE_DISCONNECTING) )
    {
      /* when the alert level for the link loss is written by the client,
      * just store the value and it has to be used to start an alert when
      * a disconnection happens
      */ 
      attValue = *att_data;
      PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "alert val %d\n",attValue);
      if((attValue == NO_ALERT) ||
         (attValue == MILD_ALERT) ||
           (attValue == HIGH_ALERT))
      {
        proximityReporter.linkLossAlertLevel = attValue;
        PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"Alert Level set to %d\n", proximityReporter.linkLossAlertLevel);
      }
      else
      {
        /* invalid attribute value, neglect it 
        * TODO when an invalid value is received, it should be just
        * ignored or the alert level should be taken as NO_ALERT
        */
        PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"invalid alert level\n");
      }
    }
  }
  else if (handle == proximityReporter.immediateAlertLevelCharHandle+1)
  {
    /**
    * the alert level of the immediate alert service is written when
    * the proximity monitor detects a path loss. The application has to be 
    * notified so that it can start the alert procedure
    */ 
    PROXIMITY_REPORTER_MESG_DBG(profiledbgfile, "alert level for Immediate Alert service %x\n",PRProfile_Read_MainStateMachine());
    if ((PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_CONNECTED) && 
        (PRProfile_Read_SubStateMachine() != PROXIMITY_REPORTER_STATE_DO_DISCONNECT) &&
          (PRProfile_Read_SubStateMachine() != PROXIMITY_REPORTER_STATE_DISCONNECTING) )
    {
      attValue = *att_data;
      if (attValue == NO_ALERT)
      {
        PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received path Loss NO_ALERT \n");
        proximityReporter.applicationNotifyFunc(EVT_PR_PATH_LOSS_ALERT,1,&attValue);
      }
      else if (attValue == MILD_ALERT)
      {
        PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received path Loss MILD_ALERT \n");
        proximityReporter.applicationNotifyFunc(EVT_PR_PATH_LOSS_ALERT,1,&attValue);
      }
      else if (attValue == HIGH_ALERT)
      {
        PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received path Loss HIGH_ALERT \n");
        proximityReporter.applicationNotifyFunc(EVT_PR_PATH_LOSS_ALERT,1,&attValue);
      }
      else
      {
        /* invalid attribute value ignored*/
        PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"invalid alert level\n");
      }
    }
  }  
}

/**
 * ProximityReporter_Rx_Event_Handler
 * 
 * @param[in] pckt : pointer to the event data
 * 
 * parses the events received from HCI according
 * to the profile's state machine
 */ 
void ProximityReporter_Rx_Event_Handler(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Proximity Reporter: MainState %02X, SubState %02X \n", 
				PRProfile_Read_MainStateMachine(), PRProfile_Read_SubStateMachine() );

  switch(event_pckt->evt)
  {
  case EVT_DISCONN_COMPLETE:
    {
      uint8_t event;

      PROXIMITY_REPORTER_MESG_DBG( profiledbgfile, "Received event EVT_DISCONN_COMPLETE \n");
      if (((PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_ADVERTISE) && 
	   (PRProfile_Read_SubStateMachine() == PROXIMITY_REPORTER_STATE_PROCESSING_PAIRING) ) || 
	  (PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_CONNECTED))
      {
	PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISE);
	PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE);
	/* there is a link loss, so notify the application to stop alerting
	 * because of path loss and start alerting due to link loss
	 */
	event = NO_ALERT;
	proximityReporter.applicationNotifyFunc(EVT_PR_PATH_LOSS_ALERT,1,&event);
	/* though the application will know about link loss through
	 * the disconnection event to the main profile, this event is sent
	 * so that the application starts the alerting procedure only when
	 * the proximity reporter was running and the alert is of the level
	 * specified by the client
	 */ 
	proximityReporter.applicationNotifyFunc(EVT_PR_LINK_LOSS_ALERT,1,&proximityReporter.linkLossAlertLevel);
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

	  PROXIMITY_REPORTER_MESG_DBG( profiledbgfile, "Received event EVT_LE_CONN_COMPLETE \n");
                    
	  /* check event status */
	  if (cc->status == BLE_STATUS_SUCCESS)
	  {
	    if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTED)
	    {
	      PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_CONNECTED);
	      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_CONNECTED_IDLE);
	    }
	    else
	    {
	      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_PROCESSING_PAIRING);
	    }
	  }
	  else
	  {
	    /* connection was not successful, now we need to wait for
	     * the application to put the device in discoverable mode
	     */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE);
	  }  
	}
	break; /* EVT_LE_CONN_COMPLETE */
      } /* switch(evt->subevent) */
    }
    break; /* EVT_LE_META_EVENT */
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      PROXIMITY_REPORTER_MESG_DBG(profiledbgfile, "Received an VENDOR EVENT %04X \n", blue_evt->ecode);

      switch (blue_evt->ecode)
      {
      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
	{
	  PROXIMITY_REPORTER_MESG_DBG( profiledbgfile, "EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE \n");

	  if ((PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_ADVERTISE) && 
	      ((PRProfile_Read_SubStateMachine() == PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE) ||
	       (PRProfile_Read_SubStateMachine() == PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE)))
	  {
	    /* Put the main profile in connectable idle state */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityReporter.PRtoBLEInf, 
						     BLE_PROFILE_STATE_CONNECTABLE_IDLE);
																 
	    if(PRProfile_Read_SubStateMachine() == PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE)
	    {
	      /* Limited Discoverable mode has timed out */
	      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE);
	      
	      /* notify the application */
	      proximityReporter.applicationNotifyFunc(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL);
	    }
	  }
	}
	break; /* EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE */
      case EVT_BLUE_GAP_PAIRING_CMPLT:
	{
	  evt_gap_pairing_cmplt *pairing = (void*)blue_evt->data;
	  
	  PROXIMITY_REPORTER_MESG_DBG( profiledbgfile, "EVT_BLUE_GAP_PAIRING_CMPLT \n");

	  if ((PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_ADVERTISE) && 
	      (PRProfile_Read_SubStateMachine() == PROXIMITY_REPORTER_STATE_PROCESSING_PAIRING))
	  {
	    if (pairing->status == BLE_STATUS_SUCCESS)
	    {
	      PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_CONNECTED);
	      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_CONNECTED_IDLE);
	    }
	  }
	}
	break; /* EVT_BLUE_GAP_PAIRING_CMPLT */
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
	{
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            PRProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            PRProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }          
	}
	break;  /* EVT_BLUE_GATT_ATTRIBUTE_MODIFIED */
      }
    }
    break; /* EVT_VENDOR */
  } /* switch(event_pckt->evt) */
}

/**
 * ProximityReporterProfile_StateMachine
 * 
 * @param[in] None
 * 
 * Proximity Reporter profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ProximityReporterProfile_StateMachine(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint8_t localName[20] = {0};
  uint16_t UUID = 0;
  uint8_t serviceMaxAttributeRecord = 0;
  uint8_t txPwrLevlCharPresFormatDescValue[7] = {0};

  switch (PRProfile_Read_MainStateMachine())
  {
  case PROXIMITY_REPORTER_STATE_UNINITIALIZED:
    break;
  case PROXIMITY_REPORTER_STATE_WAITING_BLE_INIT:
    {
      /* chech whether the main profile has done with ble init or not */
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
	PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Adding Link Loss Service \n");
	
	/* Max_Attribute_Records = 2*no_of_char + 1 */
	/* serviceMaxAttributeRecord = 1 for link loss service itself +
	 *                             2 for Alert Level characteristic 
	 */
	serviceMaxAttributeRecord = 3;

	/* store the service UUID in LE format */ 
	UUID = LINK_LOSS_SERVICE_UUID;

	hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
					 (const uint8_t *)&UUID, 
					 PRIMARY_SERVICE, 
					 serviceMaxAttributeRecord,
					 &proximityReporter.linkLossServiceHandle);
	
	if (hciCmdResult == BLE_STATUS_SUCCESS)
	{
	  /* change profiles main and sub states */
	  PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZING);
	  PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_LINK_LOSS_SERVICE);

	  PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Link Loss Service Added Successfully \n");   
	  PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Link Loss Service Handle 0x%04x \n",
				       proximityReporter.linkLossServiceHandle);

	}
	else
	{
	  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "FAILED to add Link Loss Service: Error: 0x%02X !!\n", 
					hciCmdResult);
	  PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR);
	}
      }
    }
    break; /* PROXIMITY_REPORTER_STATE_WAITING_BLE_INIT */
  case PROXIMITY_REPORTER_STATE_INITIALIZING:
    {
      switch (PRProfile_Read_SubStateMachine())
      {
      case PROXIMITY_REPORTER_STATE_ADDED_LINK_LOSS_SERVICE:
	{
	  PROXIMITY_REPORTER_MESG_DBG (profiledbgfile,"Adding Link Loss Alert Level Characteristic. \n");

	  UUID = ALERT_LEVEL_CHARACTERISTIC_UUID;
	  hciCmdResult = aci_gatt_add_char(proximityReporter.linkLossServiceHandle, 
					   UUID_TYPE_16, 
					   (const uint8_t *)&UUID, 
					   1, 
					   CHAR_PROP_READ | CHAR_PROP_WRITE, 
					   ATTR_PERMISSION_NONE, 
					   GATT_NOTIFY_ATTRIBUTE_WRITE, 
					   10,
					   CHAR_VALUE_LEN_CONSTANT,
					   &proximityReporter.linkLossAlertLevelCharHandle);

	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_LINK_LOSS_ALERT_LEVEL_CHAR);
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Link Loss Alert Level Characteristic Added Successfully \n");
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Link Loss Alert Level Characteristic Handle 0x%04x \n",
					 proximityReporter.linkLossAlertLevelCharHandle);
	  }
	  else
	  {
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "FAILED to add Link Loss Alert Level Characteristic: Error: 0x%02X !!\n",
					 hciCmdResult);
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADDED_LINK_LOSS_ALERT_LEVEL_CHAR:
	{
	  if (proximityReporter.immAlertTxPowerServSupport)
	  {
	    PROXIMITY_REPORTER_MESG_DBG( profiledbgfile, "Adding Immediate Alert Service \n");
	    
	    /* Max_Attribute_Records = 2*no_of_char + 1 */
	    /* serviceMaxAttributeRecord = 1 for Immediate Alert service itself +
	     *                             2 for Alert Level characteristic
	     */
	    serviceMaxAttributeRecord = 3;

	    /* store the service UUID in LE format */ 
	    UUID = IMMEDIATE_ALERT_SERVICE_UUID;
            
	    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
					     (const uint8_t *)&UUID, 
					     PRIMARY_SERVICE, 
					     serviceMaxAttributeRecord,
					     &proximityReporter.immdiateAlertServiceHandle);
	    
	    if (hciCmdResult == BLE_STATUS_SUCCESS)
	    {
	      /* change profiles main and sub states */
	      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_IMMEDIATE_ALERT_SERVICE);
	      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Immediate Alert Service is added Successfully \n");
	      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Immediate Alert Service Handle 0x%04x \n",
					   proximityReporter.immdiateAlertServiceHandle);
	    }
	    else
	    {
	      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "FAILED to add Immediate Alert Service: Error: 0x%02X !!\n", 
					   hciCmdResult);
	      PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR);
	    }
	  }
	  else
	  {
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_UPDATE_TX_PWR_LEVEL_CHAR);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADDED_IMMEDIATE_ALERT_SERVICE:
	{
	  PROXIMITY_REPORTER_MESG_DBG (profiledbgfile,"Adding Immediate Alert Level Characteristic. \n");
	  
	  UUID = ALERT_LEVEL_CHARACTERISTIC_UUID;
	  hciCmdResult = aci_gatt_add_char(proximityReporter.immdiateAlertServiceHandle, 
					   UUID_TYPE_16, 
					   (const uint8_t *)&UUID, 
					   1, 
					   CHAR_PROP_WRITE_WITHOUT_RESP, 
					   ATTR_PERMISSION_NONE, 
					   GATT_NOTIFY_ATTRIBUTE_WRITE, 
					   10,
					   CHAR_VALUE_LEN_CONSTANT,
					   &proximityReporter.immediateAlertLevelCharHandle);

	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_IMMEDIATE_ALERT_LEVEL_CHAR);

	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Immediate Alert Level Characteristic Added Successfully \n");
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Immediate Alert Level Characteristic Handle 0x%04x \n",
					 proximityReporter.immediateAlertLevelCharHandle);
	  }
	  else
	  {
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "FAILED to add Immediate Alert Level Characteristic: Error: 0x%02X !!\n",
					 hciCmdResult);
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADDED_IMMEDIATE_ALERT_LEVEL_CHAR:
	{
	  PROXIMITY_REPORTER_MESG_DBG (profiledbgfile,"Adding Tx Power Service. \n");
	  
	  /* Max_Attribute_Records = 2*no_of_char + 1 */
	  /* serviceMaxAttributeRecord = 1 for Tx Power service itself +
	   *                             2 for Tx Power Level characteristic
	   *                             1 for client char config descriptor +
	   *                             1 for char presentation format descriptor 
	   */
	  serviceMaxAttributeRecord = 5;
	  
	  UUID = TX_POWER_SERVICE_UUID;
	  
	  hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16,
					   (const uint8_t *)&UUID,
					   PRIMARY_SERVICE, 
					   serviceMaxAttributeRecord,
					   &proximityReporter.txPowerServiceHandle);

	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_TX_POWER_SERVICE);

	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Tx Power Service Added Successfully \n");
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Tx Power Service Handle 0x%04x \n",
					 proximityReporter.txPowerServiceHandle);
	  }
	  else
	  {
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "FAILED to add Tx Power Service: Error: 0x%02X !!\n", 
					 hciCmdResult);
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADDED_TX_POWER_SERVICE:
	{
	  PROXIMITY_REPORTER_MESG_DBG (profiledbgfile,"Adding Tx Power Level Characteristic. \n");
	  
	  UUID = TX_POWER_LEVEL_CHARACTERISTIC_UUID;
	  hciCmdResult = aci_gatt_add_char(proximityReporter.txPowerServiceHandle, 
					   UUID_TYPE_16, 
					   (const uint8_t *)&UUID, 
					   1, 
					   CHAR_PROP_READ, 
					   ATTR_PERMISSION_NONE, 
					   GATT_DONT_NOTIFY_EVENTS, 
					   10,
					   CHAR_VALUE_LEN_CONSTANT,
					   &proximityReporter.txPowerLevelCharHandle);

	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_TX_POWER_LEVEL_CHAR);
	    
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Tx Power Level Characteristic Added Successfully \n");
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Tx Power Level Characteristic Handle 0x%04x \n", 
					 proximityReporter.txPowerLevelCharHandle);
	  }
	  else
	  {
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "FAILED to add Tx Power Level Characteristic: Error: %02X !!\n", 
					 hciCmdResult);
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADDED_TX_POWER_LEVEL_CHAR:
	{
	  uint16_t charDescHandle;

	  PROXIMITY_REPORTER_MESG_DBG (profiledbgfile,"Adding Tx Pwr Levl Char Presentation Format Descriptor. \n");
	  
	  txPwrLevlCharPresFormatDescValue[0] = 0x0C;
	  txPwrLevlCharPresFormatDescValue[1] = 0x00;
	  txPwrLevlCharPresFormatDescValue[2] = 0x00;
	  txPwrLevlCharPresFormatDescValue[3] = 0x27;
	  txPwrLevlCharPresFormatDescValue[4] = 0x00;
	  txPwrLevlCharPresFormatDescValue[5] = 0x00;
	  txPwrLevlCharPresFormatDescValue[6] = 0x00;

	  UUID = CHAR_PRESENTATION_FORMAT_DESCRIPTOR_UUID;
	  hciCmdResult = aci_gatt_add_char_desc(proximityReporter.txPowerServiceHandle, 
						proximityReporter.txPowerLevelCharHandle, 
						UUID_TYPE_16, 
						(const uint8_t *)&UUID, 
						CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH, 
						(uint8_t)CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH,
						(uint8_t *)&txPwrLevlCharPresFormatDescValue,
						ATTR_PERMISSION_NONE, 
						ATTR_ACCESS_READ_ONLY, 
						GATT_DONT_NOTIFY_EVENTS, 
						10, 
						CHAR_VALUE_LEN_CONSTANT,
						&charDescHandle);

	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_TX_PWR_LEVL_CHAR_PRESEN_FORMAT_DECS);

	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Char Presentation Format Descriptor Added Successfully \n");
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "Char Presentation Format Descriptor Handle 0x%04x \n", 
					 charDescHandle);
	  }
	  else
	  {
	    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "FAILED to add Tx Power Level Characteristic: Error: 0x%02X !!\n", 
					 hciCmdResult);
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADDED_TX_PWR_LEVL_CHAR_PRESEN_FORMAT_DECS:
	{
	  int8_t txPwrLevel=0;

	  /* set the value of the TX power level characteristic.
	   * For that we first need to read the current TX power level
	   */
	  PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"reading TX power level\n");
	  hci_read_transmit_power_level(&gBLEProfileContext.connectionHandle, 
					READ_CURR_TRANSMIT_POWER_LEVEL, 
					&txPwrLevel);
	  
	  PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"updating TX power level\n");
	  hciCmdResult = aci_gatt_update_char_value(proximityReporter.txPowerServiceHandle, 
						    proximityReporter.txPowerLevelCharHandle, 
						    0x00,
						    0x01, /* can it be 0 also ?*/
						    (uint8_t*)&txPwrLevel);
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_UPDATE_TX_PWR_LEVEL_CHAR);
	  else
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADDED_TX_PWR_LEVL_CHAR_PRESEN_FORMAT_DECS);
	}
	break;
      case PROXIMITY_REPORTER_STATE_UPDATE_TX_PWR_LEVEL_CHAR:
	{
	  /* change current profiles Main and Sub both states */
	  PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_INITIALIZED);
	  PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INVALID);
                    
	  /* vote to the main profile to go to advertise state */
	  (void) BLE_Profile_Vote_For_Advertisable_State (&proximityReporter.PRtoBLEInf);

	  BLE_Profile_Add_Advertisment_Service_UUID (LINK_LOSS_SERVICE_UUID);
	  if (proximityReporter.immAlertTxPowerServSupport)
	  {
	    BLE_Profile_Add_Advertisment_Service_UUID (IMMEDIATE_ALERT_SERVICE_UUID);
	    BLE_Profile_Add_Advertisment_Service_UUID (TX_POWER_SERVICE_UUID);
	  }         
	}
	break;
      }  /* switch (PRProfile_Read_SubStateMachine()) */

      if (PRProfile_Read_SubStateMachine() == PROXIMITY_REPORTER_STATE_INITIALIZATION_ERROR)
      {
	uint8_t cmdCmpltStatus;

	cmdCmpltStatus = BLE_STATUS_FAILED;
	/* Notify to application that Proximity Reporter profile initialization has failed */
	proximityReporter.applicationNotifyFunc(EVT_PR_INITIALIZED,1,&cmdCmpltStatus);
	/* Change profile state to represent uninitialized profile */
	PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_UNINITIALIZED);
	PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_INVALID);
	/* unregister the profile from BLE registered profile list */
	BLE_Profile_Unregister_Profile ( &proximityReporter.PRtoBLEInf );
      }
    }
    break; /* PROXIMITY_REPORTER_STATE_INITIALIZING */
  case PROXIMITY_REPORTER_STATE_INITIALIZED:
    {
      /* Proximity Reporter has completed its initialization process and voted the 
       * main profile for Advertisable state. Now its waiting for the main profile
       * to set to Advertisable state. */
      
      /* check main profile's state, if its in advertisable state, set the same for 
       * current profile as well */
      if ((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) && 
	  (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CONNECTABLE_IDLE))
      {
	uint8_t cmdCmpltStatus;

	PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISE);
	PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE);
	cmdCmpltStatus = BLE_STATUS_SUCCESS;
	proximityReporter.applicationNotifyFunc(EVT_PR_INITIALIZED,1,&cmdCmpltStatus);
      }
    }
    break; /* PROXIMITY_REPORTER_STATE_INITIALIZED */
  case PROXIMITY_REPORTER_STATE_ADVERTISE:
    {
      switch (PRProfile_Read_SubStateMachine())
      {
      case PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE:
	{
	  // ???? State To be verified
/* 	  if ((opcode == HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE) ||  */
/* 	      (opcode == HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE) || */
/* 	      (opcode == HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE)) */
/* 	  { */
/* 	    /\* This profile is in Advertisable_Idle state and cannot initiate advertising  */
/* 	     * process. Now a Command Complete Event for Gap_Set_Discoverable HCI Command  */
/* 	     * is received. It means that another profile has initiated the Advertising  */
/* 	     * process. So now just change the profile's state and wait to BLE for  */
/* 	     * connection complete event.  */
/* 	     *\/ */
/* 	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_BLE_ADVERTISING); */
/* 	  } */
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START:
	{
	  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "STARTING Advertising for White List devices \n");
          
	  hciCmdResult = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* start a timer of 10 secconds to try to connnect to white list device */
	    Blue_NRG_Timer_Start (10, ProximityReporter_Advertise_Period_Timeout_Handler,&proximityReporter.timerID);

	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityReporter.PRtoBLEInf, 
						    BLE_PROFILE_STATE_ADVERTISING);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST:
	break;
      case PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP:
	{
	  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "STOPPING Advertising to White List devices \n");
          
	  hciCmdResult = aci_gap_set_non_discoverable();
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_DO_START);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityReporter.PRtoBLEInf, 
						    BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_DO_START:
	{
	  PROXIMITY_MONITOR_MESG_DBG ( profiledbgfile, "STARTING FAST Limited Discoverable Mode \n");
	  
	  localName[0] = 0x08;
	  BLUENRG_memcpy ((void *)(localName+1), (void *)"PR", 0x02 );
	  
	  hciCmdResult = aci_gap_set_limited_discoverable(ADV_IND,
							  (uint16_t)PXP_FAST_CONN_ADV_INTERVAL_MIN,
							  (uint16_t)PXP_FAST_CONN_ADV_INTERVAL_MAX,
							  PUBLIC_ADDR, 
							  NO_WHITE_LIST_USE,
							  0x03,
							  (char *)localName,
							  gBLEProfileContext.advtServUUIDlen,
							  gBLEProfileContext.advtServUUID,
							  0x0,     
							  0x0      
							  );
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
	    Blue_NRG_Timer_Start (30, ProximityReporter_Advertise_Period_Timeout_Handler,&proximityReporter.timerID);

	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityReporter.PRtoBLEInf, 
						    BLE_PROFILE_STATE_ADVERTISING);
	  } 
	  else
	  {
	    /* Request to put the device in limited discoverable mode is 
	     * unsuccess, put the device back in advrtisable idle mode */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE:
	break;
      case PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP:
	{
	  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "STOPPING FAST General Discoverable Mode \n");
	  
	  hciCmdResult = aci_gap_set_non_discoverable();
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE_DO_START);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityReporter.PRtoBLEInf, 
						    BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE_DO_START:
	{
	  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "STARTING FAST Limited Discoverable Mode %d\n",gBLEProfileContext.advtServUUIDlen);
	  
	  localName[0] = 0x08;
	  BLUENRG_memcpy ((void *)(localName+1), (void *)"PR", 0x02 );
	  
	  hciCmdResult = aci_gap_set_limited_discoverable(ADV_IND, 
							  (uint16_t)PXP_LOW_PWR_ADV_INTERVAL_MIN, 
							  (uint16_t)PXP_LOW_PWR_ADV_INTERVAL_MAX, 
							  PUBLIC_ADDR, 
							  NO_WHITE_LIST_USE, 
							  0x03, 
							  (char *)localName, 
							  gBLEProfileContext.advtServUUIDlen,
							  gBLEProfileContext.advtServUUID,
							  0x0,     
							  0x0      
							  );
	  if (hciCmdResult == BLE_STATUS_SUCCESS)
	  {
	    /* change profile's sub states */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State (&proximityReporter.PRtoBLEInf, 
						    BLE_PROFILE_STATE_ADVERTISING);
	  }
	  else
	  {
	    /* Request to put the device in limited discoverable mode is 
	     * unsuccess, put the device back in advrtisable idle mode */
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_LOW_POWER_DISCOVERABLE:
	break;
      case PROXIMITY_REPORTER_STATE_BLE_ADVERTISING:
	{
	  // ??? State to be verified
/* 	  if (opcode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE) */
/* 	    { */
/* 		  /\* Advertising process controlled by some other profile has been timedout */
/* 		   * and hence stopped *\/ */
/* 		  /\* change profile's sub states *\/ */
/* 		  PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_ADVERTISABLE_IDLE); */
/* 		} */
	}
	break;
      case PROXIMITY_REPORTER_STATE_PROCESSING_PAIRING:
	{
	  /* check if any other profile has initiated the disconnect procedure 
	   * at BLE main profile. */
	  if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING)
	  {
	    PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_CONNECTED);
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_DO_DISCONNECT);
	  }
	}
	break;
      }  /* switch (PRProfile_Read_SubStateMachine()) */
    }
    break; /* PROXIMITY_REPORTER_STATE_ADVERTISE */
  case PROXIMITY_REPORTER_STATE_CONNECTED:
    {
      switch (PRProfile_Read_SubStateMachine())
      {
      case PROXIMITY_REPORTER_STATE_CONNECTED_IDLE:
	{
	  /* check if any other profile has initiated the disconnect procedure 
	   * at BLE main profile. */
	  if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING)
	  {
	    PRProfile_Write_MainStateMachine(PROXIMITY_REPORTER_STATE_CONNECTED);
	    PRProfile_Write_SubStateMachine(PROXIMITY_REPORTER_STATE_DO_DISCONNECT);
	  }
	}
	break;
      case PROXIMITY_REPORTER_STATE_DO_DISCONNECT:
	break;
      case PROXIMITY_REPORTER_STATE_DISCONNECTING:
	break;
      case PROXIMITY_REPORTER_STATE_DISCONNECTED:
	break;
      }
    }
    break; /* PROXIMITY_REPORTER_STATE_CONNECTED */
  }

  return (hciCmdResult);
}

/**
 * ProximityReporter_Advertise_Period_Timeout_Handler
 * 
 * this function is called when the timer started
 * by the heart rate profile for advertising times out
 */ 
void ProximityReporter_Advertise_Period_Timeout_Handler (void)
{
  tProfileState nextState = PRProfile_Read_SubStateMachine();
  
  Blue_NRG_Timer_Stop(proximityReporter.timerID);
  proximityReporter.timerID = 0xFF;

  PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Advertising Period Timedout \n");
  if (PRProfile_Read_MainStateMachine() == PROXIMITY_REPORTER_STATE_ADVERTISE)
  {
    switch (PRProfile_Read_SubStateMachine())
    {
    case PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST:
      nextState = PROXIMITY_REPORTER_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP;
      PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "White list Advertising Timedout \n");
      break;
    case PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE:
      nextState = PROXIMITY_REPORTER_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP;
      PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Fast Advertising Timedout \n");
      break;
    }
    
    if (nextState != PRProfile_Read_SubStateMachine())
    {
      PRProfile_Write_SubStateMachine(nextState);
    }
  }
}
