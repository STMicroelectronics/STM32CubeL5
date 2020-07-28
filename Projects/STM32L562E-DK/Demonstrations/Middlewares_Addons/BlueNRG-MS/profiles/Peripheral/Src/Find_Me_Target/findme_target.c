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
*   FILENAME        -  findme_target.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*
*   $Date$:      23/07/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Find Me Profile according to unified BlueNRG 
*                DK framework
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  This file conatins the implementation of the Find Me profile for the target
* role
******************************************************************************/
/******************************************************************************
* Includes
*****************************************************************************/
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
#include <ble_events.h>
#include <ble_profile.h>
#include <uuid.h>
#include <findme_target.h>

/******************************************************************************
* Macros
*****************************************************************************/
/* states */
#define FMT_UNINITIALIZED		(0x00)
#define FMT_INIT			(0x01)
#define FMT_IDLE			(0x02)
#define FMT_ADV_IN_FAST_DISC_MODE	(0x03)
#define FMT_START_ADV_IN_LOW_PWR_MODE   (0x04)
#define FMT_ADV_IN_LOW_PWR_MODE		(0x05)
#define FMT_STATE_CONNECTED		(0x06)

/* advertising intervals in terms of 625 micro seconds */
#define FMT_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define FMT_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define FMT_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define FMT_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/* Alert levels */
#define NO_ALERT					  (0x00)
#define MILD_ALERT                    (0x01)
#define HIGH_ALERT                    (0x02)

/******************************************************************************
* Types
*****************************************************************************/
typedef struct _tFindMeContext
{
	/**
	 * the current state of the
	 * find me target profile
	 */ 
	uint8_t fmtstate;
    
	/**
	 * ID of the timer started
	 */
	tTimerID timerID;
	
	/**
	 * store the handle of the
	 * immediate alert service once
	 * it is added
	 */ 
	uint16_t immediateAlertServHandle;
	
	/**
	 * stores the handle of the
	 * alert characteristic once
	 * it is added
	 */ 
	uint16_t alertCharHandle;
	
	/**
	 * internal structure used for
	 * registering the find me target
	 * with the ble profile
	 */ 
	tProfileInterfaceContext FMTtoBLEInf;
	
	/**
	 * application callback
	 * to be supplied by the application
	 * at the time of init
	 */ 
	BLE_CALLBACK_FUNCTION_TYPE applcb;
}tFindMeContext;

tFindMeContext findMeTarget;
extern void *profiledbgfile;
extern uint8_t bnrg_expansion_board;

/******************************************************************************
* Function Prototypes
******************************************************************************/
tBleStatus FMT_Add_Services_Characteristics(void);
void FMT_Event_Handler(void *pckt);
void FMT_SetState(uint8_t state);

static void FMTProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState FMTProfile_Read_MainStateMachine(void);

/******************************************************************************
* Global Functions(SAP)
*****************************************************************************/
tBleStatus FindMeTarget_Init(BLE_CALLBACK_FUNCTION_TYPE FindMeTargetcb)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  FMT_DBG_MSG(profiledbgfile,"find me init");
	
  if (Is_Profile_Present(PID_FIND_ME_TARGET) == BLE_STATUS_SUCCESS)
  {
    return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }
	
  if (FindMeTargetcb == NULL)
  {
    return BLE_STATUS_NULL_PARAM;
  }
	
  BLUENRG_memset(&findMeTarget,0,sizeof(findMeTarget));
	 
  /* initialize the profile context */
  findMeTarget.applcb = FindMeTargetcb;
  findMeTarget.timerID = 0xFF;
	
  /* Initialize Time Server Profile to BLE main Profile Interface data structure */
  findMeTarget.FMTtoBLEInf.callback_func = FMT_Event_Handler;
  findMeTarget.FMTtoBLEInf.voteForAdvertisableState = 0;
  findMeTarget.FMTtoBLEInf.profileID = PID_FIND_ME_TARGET;
  retval = BLE_Profile_Register_Profile(&findMeTarget.FMTtoBLEInf);
  if (retval == BLE_STATUS_SUCCESS)
  {
    FMT_SetState(FMT_INIT);
  }
       
  return (retval);
}

tBleStatus FMT_Add_Device_To_WhiteList(uint8_t addrType,uint8_t* bdAddr)
{
  uint8_t status;

  /* add the device to whitelist */
  FMT_DBG_MSG(profiledbgfile,"FMT_Add_Device_To_WhiteList\n");
  status = hci_le_add_device_to_white_list(addrType, bdAddr);

  return status;
}

tBleStatus FMT_Advertize()
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'F','i','n','d','M','e','T','a','r','g','e','t'};
	
  FMT_DBG_MSG(profiledbgfile,"FMT_Advertize\n");
  /* NOTE : PTS does not expect whitelist. So the PTS tests can be done only
   * by using the NO_WHITE_LIST_USE option
   */
  if(FMTProfile_Read_MainStateMachine() == FMT_IDLE)
  {
    status = aci_gap_set_limited_discoverable(ADV_IND,
					      FMT_FAST_CONN_ADV_INTERVAL_MIN, 
					      FMT_FAST_CONN_ADV_INTERVAL_MAX,
					      PUBLIC_ADDR, 
					      NO_WHITE_LIST_USE, // WHITE_LIST_FOR_ALL. Set to NO_WHITE_LIST_USE x running PTS IAS tests
					      13, 
					      name, 
					      0,
					      NULL,
					      0, 
					      0);
    if(status == BLE_STATUS_SUCCESS)
    {
      FMT_SetState(FMT_ADV_IN_FAST_DISC_MODE);
      FMT_DBG_MSG(profiledbgfile,"enabled advertising fast connection in limited discoverable mode\n");
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&findMeTarget.FMTtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
      /* we are in the limited discoverable mode.
       * now start a timer for 30 seconds. The advertising
       * intervals have to be changed on expiry of this
       * timer
       */
      Blue_NRG_Timer_Start(BLE_FAST_CONN_ADV_TIMEOUT,
		  FMT_LimDisc_Timeout_Handler,
		  &findMeTarget.timerID);
      /* this timer can be stopped on connection complete
       * even if it is not stopped, the timeout handler
       * will handle it correctly depending on states
       */ 
    }
    else
    {
      findMeTarget.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
    }
  }
	
  return status;
}

tBleStatus FMT_Add_Services_Characteristics(void)
{
  tBleStatus status = BLE_STATUS_FAILED;
  uint16_t uuid;
	
  /* the main profile has completed its initializations
   * so start the profile specific initialization
   * add the immediate alert service
   */
  FMT_DBG_MSG(profiledbgfile,"Adding immediate alert service\n");
  uuid = IMMEDIATE_ALERT_SERVICE_UUID;
  status = aci_gatt_add_serv(UUID_TYPE_16, 
			     (const uint8_t *) &uuid, 
			     PRIMARY_SERVICE, 
			     3,
			     &(findMeTarget.immediateAlertServHandle));
												  
  if (status == BLE_STATUS_SUCCESS)
  {
    FMT_DBG_MSG(profiledbgfile,"immediate alert service successfully added %x\n",findMeTarget.immediateAlertServHandle);
  } else {
    FMT_DBG_MSG(profiledbgfile,"immediate alert service adding failed\n");
    return status;
  }

  /* add the alert characteristic */  
  FMT_DBG_MSG(profiledbgfile,"Adding immediate alert characteristic\n");
  uuid = ALERT_LEVEL_CHARACTERISTIC_UUID;
  status = aci_gatt_add_char(findMeTarget.immediateAlertServHandle, 
			     UUID_TYPE_16, 
			     (const uint8_t *) &uuid , 
			     2, /* value len */ 
			     CHAR_PROP_WRITE_WITHOUT_RESP,/* char properties */ 
			     ATTR_PERMISSION_NONE, /* security permission */
			     GATT_NOTIFY_ATTRIBUTE_WRITE, /* events to be notified */
			     10, /* encryKeySize */
			     CHAR_VALUE_LEN_VARIABLE, /* isVariable */
			     &(findMeTarget.alertCharHandle));

  if (status == BLE_STATUS_SUCCESS)
  {
    FMT_DBG_MSG(profiledbgfile,"alert level characteristic successfully added %x\n", findMeTarget.alertCharHandle);
  }
  else 
  {
    FMT_DBG_MSG(profiledbgfile,"alert level characteristic adding failed\n");
  }

  return status;

}

 /******************************************************************************
* Local Functions
*****************************************************************************/
static void FMTProfile_Write_MainStateMachine(tProfileState localmainState)
{
  findMeTarget.fmtstate = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState FMTProfile_Read_MainStateMachine(void)
{
  return(findMeTarget.fmtstate);
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void FMTProfile_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  uint8_t attValue;
  
  if ((FMTProfile_Read_MainStateMachine() == FMT_STATE_CONNECTED) && (handle == (findMeTarget.alertCharHandle + 1)))
  {
    attValue = att_data[0];
    /* notify the application of the alert level */
    if(attValue == NO_ALERT)
    {
      FMT_DBG_MSG(profiledbgfile,"Stop alerting\n");
      findMeTarget.applcb(EVT_FMT_ALERT,1,&attValue);
    }	
    else if(attValue == MILD_ALERT)
    {
      FMT_DBG_MSG(profiledbgfile,"mild alert\n");
      findMeTarget.applcb(EVT_FMT_ALERT,1,&attValue);
    }
    else if(attValue == HIGH_ALERT)
    {
      FMT_DBG_MSG(profiledbgfile,"high alert\n");
      findMeTarget.applcb(EVT_FMT_ALERT,1,&attValue);
    }
    else
    {
      FMT_DBG_MSG(profiledbgfile,"invalid alert level\n");
    }
  }
  
}

/**
 * FMT_SetState
 * 
 * @param[in] state : the new state of the FMT
 * 
 * Changes the state of the FMT state machine atomically
 * as the states can be changed in the interrupt context 
 * as well as from the application
 */ 
void FMT_SetState(uint8_t state)
{
  FMTProfile_Write_MainStateMachine(state);
}

/**
 * FMT_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 *
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB() 
 */ 
void FMT_Event_Handler(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  FMT_DBG_MSG(profiledbgfile,"recv evt %x\n", event_pckt->evt);

  if(hci_pckt->type != HCI_EVENT_PKT)
  {
    HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Rx_Event_Handler(),Recevied packed is not an HCI Event: %02X !!\n", hci_pckt->type);
    return;
  }
  
  switch(event_pckt->evt)
  {
  case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *dc = (void *)event_pckt->data;

      FMT_DBG_MSG(profiledbgfile,"EVT_DISCONN_COMPLETE\n");
      if (dc->status == BLE_STATUS_SUCCESS)
      {
	FMT_SetState(FMT_IDLE);
	FMT_DBG_MSG(profiledbgfile,"disconnected\n");
	/* we have to put the device into advertising mode 
	 * According to the spec, the profile has to try
	 * for reconnection once it is disconnected
	 */
	FMT_Advertize();
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
	  
	  if(cc->status == BLE_STATUS_SUCCESS)
	  {
	    /* we are connected */
	    FMT_SetState(FMT_STATE_CONNECTED);
	    FMT_DBG_MSG(profiledbgfile,"connected\n");
	  }
	  else
	  {
	    /* there was an error in connection
	     * so wait for the application to put
	     * into discoverable mode
	     */
	    FMT_SetState(FMT_IDLE);
	  }
	}
	break;
      }
    }
    break;
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;

      /* the profile has no security requirements */
      switch(blue_evt->ecode)
      {
      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
	{
	  if ((FMTProfile_Read_MainStateMachine() == FMT_ADV_IN_LOW_PWR_MODE) || (FMTProfile_Read_MainStateMachine() == FMT_ADV_IN_FAST_DISC_MODE))
	  {
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State(&findMeTarget.FMTtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
				
	    if(FMTProfile_Read_MainStateMachine() == FMT_ADV_IN_LOW_PWR_MODE)
	    {
	      FMT_SetState(FMT_IDLE);
	      /* notify the application */
	      findMeTarget.applcb(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL);
	    }
	  }
	}
	break;
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
	{          
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            FMTProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            FMTProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
	  break;
	}
      }
    }
    break;
  }
}

void FMT_LimDisc_Timeout_Handler()
{	
  FMT_DBG_MSG(profiledbgfile,"FMT_LimDisc_Timeout_Handler\n");
  
  Blue_NRG_Timer_Stop(findMeTarget.timerID);
  findMeTarget.timerID = 0xFF;
  
  if(FMTProfile_Read_MainStateMachine() == FMT_ADV_IN_FAST_DISC_MODE) 
  {
    /* stop advertizing */
    if (aci_gap_set_non_discoverable() == BLE_STATUS_SUCCESS)
    {
      FMT_SetState(FMT_START_ADV_IN_LOW_PWR_MODE);
    }
    else
    {
      FMT_DBG_MSG(profiledbgfile,"Set Non Discoverable failed\n");
      FMT_SetState(FMT_IDLE);
    }  
  }
}

/**
 * FMTProfile_StateMachine
 * 
 * @param[in] None
 * 
 * FMT profile's state machine: to be called on application main loop.
 */ 
tBleStatus FMTProfile_StateMachine(void)
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'F','i','n','d','M','e','T','a','r','g','e','t'};
  static uint8_t mainState = FMT_IDLE;
  
  if (mainState != FMTProfile_Read_MainStateMachine())
  {
    FMT_DBG_MSG(profiledbgfile,"[cmplt]%x\n",FMTProfile_Read_MainStateMachine());
    mainState = FMTProfile_Read_MainStateMachine();
  }
	
  switch(FMTProfile_Read_MainStateMachine())
  {
  case FMT_INIT:
    {
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
	status = FMT_Add_Services_Characteristics();
	if (status == BLE_STATUS_SUCCESS)
	{ 
	  /* vote to the main profile to go to advertise state */
	  (void) BLE_Profile_Vote_For_Advertisable_State (&findMeTarget.FMTtoBLEInf);
	  FMT_SetState(FMT_IDLE);
	}
	else
	{
	  /* Let the application know that the initialization was not successful */
	  findMeTarget.applcb(EVT_FMT_INITIALIZED,1,&status);
	  FMT_SetState(FMT_UNINITIALIZED);
	}
      }
    }
    break;
  case FMT_IDLE:
    {
      FMT_DBG_MSG(profiledbgfile,"FMT_IDLE\n");
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
	FMT_DBG_MSG(profiledbgfile,"notify the application that initialization is complete\n");
	status = BLE_STATUS_SUCCESS;
	findMeTarget.applcb(EVT_FMT_INITIALIZED,1,&status);
      }
    }
    break;
  case FMT_START_ADV_IN_LOW_PWR_MODE:
    {
      FMT_DBG_MSG(profiledbgfile,"lim disc mode reduced power\n");		
      /* put the device in low power discoverable mode */
      status = aci_gap_set_limited_discoverable(ADV_IND,
						FMT_LOW_PWR_ADV_INTERVAL_MIN, 
						FMT_LOW_PWR_ADV_INTERVAL_MAX,
						PUBLIC_ADDR, 
						NO_WHITE_LIST_USE, // WHITE_LIST_FOR_ALL. Set to NO_WHITE_LIST_USE x running PTS IAS tests
						13, 
						name, 
						0,
						NULL,
						0, 
						0);

      if(status != BLE_STATUS_SUCCESS)
      {
	findMeTarget.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
	FMT_SetState(FMT_IDLE);
      }
      else
      {
	FMT_SetState(FMT_ADV_IN_LOW_PWR_MODE);
	FMT_DBG_MSG(profiledbgfile,"In Low Power Limited discoverable mode\n");
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&findMeTarget.FMTtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
      }
    }
    break;
  }
  return status;
}
