/******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  findme_locator.c
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
*  This file contains the implementation of the Find Me profile for the locator
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
#include <findme_locator.h>

/******************************************************************************
* Macros
*****************************************************************************/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
                (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
                (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))

/* states */
#define FML_UNINITIALIZED		(0x00)
#define FML_INIT			(0x01)
#define FML_IDLE			(0x02)
#define FML_ADV_IN_FAST_DISC_MODE	(0x03)
#define FML_START_ADV_IN_LOW_PWR_MODE   (0x04)
#define FML_ADV_IN_LOW_PWR_MODE		(0x05)
#define FML_DISCOVER_SERVICE		(0x06)
#define FML_DISCOVER_CHARAC		(0x07)
#define FML_STATE_CONNECTED		(0x08)
#define FML_CONNECTED_IDLE		(0x09)

/* advertising intervals in terms of 625 micro seconds */
#define FML_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define FML_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define FML_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define FML_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/******************************************************************************
* Type definitions
*****************************************************************************/
typedef struct _tFMLContext
{
	/**
	 * the current state of the
	 * find me locator profile
	 */ 
	uint8_t fmlstate;
    
	/**
	 * connection handle 
	 */ 
	uint16_t connHandle;
	/**
	 * ID of the timer started
	 */
	tTimerID timerID;
	
	/**
	 * store the handle of the
	 * immediate alert service once
	 * it is added
	 */ 
	uint16_t immAlertServHandle;
	
	/**
	 * store the handle of the
	 * immediate alert service once
	 * it is added
	 */ 
	uint16_t immAlertServEndHandle;
	
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
	tProfileInterfaceContext FMLtoBLEInf;
	
	/**
	 * application callback
	 * to be supplied by the application
	 * at the time of init
	 */ 
	BLE_CALLBACK_FUNCTION_TYPE applcb;
}tFMLContext;

/******************************************************************************
* Variables
*****************************************************************************/
tFMLContext findMeLocator;
extern void *profiledbgfile;

/******************************************************************************
* Function Prototypes
*****************************************************************************/
void FML_Event_Handler(void *pckt);
void FML_SetState(uint8_t state);
void FML_Exit(void);

static void FMLProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState FMLProfile_Read_MainStateMachine(void);

/******************************************************************************
* SAP
*****************************************************************************/
tBleStatus FindMeLocator_Init(tSecurityParams* bleSecReq,
			      BLE_CALLBACK_FUNCTION_TYPE findMeLocatorcb)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  FMT_DBG_MSG(profiledbgfile,"find me init");
  
  if(Is_Profile_Present(PID_FIND_ME_LOCATOR) == BLE_STATUS_SUCCESS)
  {
    return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }
	
  if (findMeLocatorcb == NULL)
  {
    return BLE_STATUS_NULL_PARAM;
  }
    
  BLUENRG_memset(&findMeLocator,0,sizeof(findMeLocator));
	 
  /* initialize the profile context */
  findMeLocator.applcb = findMeLocatorcb;
  findMeLocator.timerID = 0xFF;
  findMeLocator.FMLtoBLEInf.profileID = PID_FIND_ME_LOCATOR;
  findMeLocator.FMLtoBLEInf.callback_func = FML_Event_Handler;
  findMeLocator.FMLtoBLEInf.voteForAdvertisableState = 0;

  retval = BLE_Profile_Register_Profile(&findMeLocator.FMLtoBLEInf);
  if (retval == BLE_STATUS_SUCCESS)
  {
    FMLProfile_Write_MainStateMachine(FML_UNINITIALIZED);
  }
  
  return (retval);
}

tBleStatus FML_Add_Device_To_WhiteList(uint8_t addrType,uint8_t* bdAddr)
{
  uint8_t status;

	/* add the device to whitelist */
  FMT_DBG_MSG(profiledbgfile,"FML_Add_Device_To_WhiteList\n");
  status = hci_le_add_device_to_white_list(addrType, bdAddr);
 
  return status;
}

tBleStatus FML_Advertize()
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'F','i','n','d','M','e','L','o','c','a','t','o','r'};
	
  FMT_DBG_MSG(profiledbgfile,"FML_Advertize\n");
  /* NOTE : PTS does not expect whitelist. So the PTS tests can be done only
   * by using the NO_WHITE_LIST_USE option
   */
  if(FMLProfile_Read_MainStateMachine() == FML_IDLE)
  {
    status = aci_gap_set_limited_discoverable(ADV_IND,
					      FML_FAST_CONN_ADV_INTERVAL_MIN, 
					      FML_FAST_CONN_ADV_INTERVAL_MAX,
					      PUBLIC_ADDR, 
					      NO_WHITE_LIST_USE, // WHITE_LIST_FOR_ALL. Set to NO_WHITE_LIST_USE x running PTS IAS tests
					      14, 
					      name, 
					      0,
					      NULL,
					      0, 
					      0);
    if(status == BLE_STATUS_SUCCESS)
    {
      FML_SetState(FML_ADV_IN_FAST_DISC_MODE);
      FMT_DBG_MSG(profiledbgfile,"enabled advertising fast connection in limited discoverable mode\n");
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&findMeLocator.FMLtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
      /* we are in the limited discoverable mode.
       * now start a timer for 30 seconds. The advertising
       * intervals have to be changed on expiry of this
       * timer
       */
      Blue_NRG_Timer_Start(BLE_FAST_CONN_ADV_TIMEOUT,
		  FML_LimDisc_Timeout_Handler,
		  &findMeLocator.timerID);
    }
    else
    {
      /* inform the application that the advertizing was not
       * enabled
       */
      FML_SetState(FML_IDLE);
      findMeLocator.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
    }	
  }
	
  return status;
}

tBleStatus FML_ALert_Target(uint8_t alertLevel)
{
  FMT_DBG_MSG(profiledbgfile,"FML_ALert_Target %x\n",FMLProfile_Read_MainStateMachine());
  if(FMLProfile_Read_MainStateMachine() == FML_STATE_CONNECTED)
  {
    if((alertLevel != NO_ALERT) &&
       (alertLevel != MILD_ALERT) &&
       (alertLevel != HIGH_ALERT))
    {
      return   BLE_STATUS_INVALID_PARAMS;
    }
		
    return aci_gatt_write_without_response(findMeLocator.connHandle, findMeLocator.alertCharHandle, 1, &alertLevel);
  }
  return ERR_COMMAND_DISALLOWED;
}

/******************************************************************************
* Local Functions
*****************************************************************************/
static void FMLProfile_Write_MainStateMachine(tProfileState localmainState)
{
  findMeLocator.fmlstate = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState FMLProfile_Read_MainStateMachine(void)
{
  return(findMeLocator.fmlstate);
}

/**
 * FML_SetState
 * 
 * @param[in] state : the new state of the FML
 * 
 * Changes the state of the FML state machine atomically
 * as the states can be changed in the interrupt context 
 * as well as from the application
 */ 
void FML_SetState(uint8_t state)
{
	FMLProfile_Write_MainStateMachine(state);
	FMT_DBG_MSG(profiledbgfile,"FML_SetState %x\n",FMLProfile_Read_MainStateMachine());
}

/**
 * FML_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 *
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB() 
 */ 
void FML_Event_Handler(void *pckt)
{
  uint8_t status, idx;
  	
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  FMT_DBG_MSG(profiledbgfile,"recv evt %x\n", event_pckt->evt);
	
  switch(event_pckt->evt)
  {
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent)
      {
      case EVT_LE_CONN_COMPLETE:
	{
	  evt_le_connection_complete *cc = (void *)evt->data;

	  /* In whichever state we are in we have to
	   * accept the event as advertising would have
	   * started only when all the profiles would have agreed
	   */ 
	  if(cc->status == BLE_STATUS_SUCCESS)
	  {
	    /* we are connected */
	    FML_SetState(FML_DISCOVER_SERVICE);
	    findMeLocator.connHandle = cc->handle;
	    FMT_DBG_MSG(profiledbgfile,"connected %d\n",findMeLocator.connHandle);
	    /* start the procedure to discover all the services on the peer */
	    if (aci_gatt_disc_all_prim_services(findMeLocator.connHandle) == BLE_STATUS_SUCCESS)
	    {
	      FMT_DBG_MSG(profiledbgfile,"Discovery all primary service\n");
	    } 
	    else 
	    {
	      FMT_DBG_MSG(profiledbgfile,"Discovery all primary service failed\n");
	    } 
	  }
	  else
	  {
	    /* there was an error in connection
	     * so wait for the application to put
	     * into discoverable mode
	     */
	    FML_SetState(FML_IDLE);
	  }
	}
	break;
      }
    }
    break;
  case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *dc = (void *)event_pckt->data;

      FMT_DBG_MSG(profiledbgfile,"EVT_DISCONN_COMPLETE\n");
      if (dc->status == BLE_STATUS_SUCCESS)
      {
	FML_SetState(FML_IDLE);
	FMT_DBG_MSG(profiledbgfile,"disconnected\n");
	/* we have to put the device into advertising mode 
	 * According to the spec, the profile has to try
	 * for reconnection once it is disconnected
	 */
	FML_Advertize();
      }
    }
    break;
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;

      switch(blue_evt->ecode)
      {
      case  EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
	{
	  if ((FMLProfile_Read_MainStateMachine() == FML_ADV_IN_LOW_PWR_MODE) || (FMLProfile_Read_MainStateMachine() == FML_ADV_IN_FAST_DISC_MODE))
	  {
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State(&findMeLocator.FMLtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);

	    if(FMLProfile_Read_MainStateMachine() == FML_ADV_IN_LOW_PWR_MODE)
	    {
	      FML_SetState(FML_IDLE);
	      /* notify the application */
	      findMeLocator.applcb(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL);
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
	{
	  evt_att_read_by_group_resp *pr = (void*)blue_evt->data;
	  uint8_t numServ, i;

	  if (FMLProfile_Read_MainStateMachine() == FML_DISCOVER_SERVICE)
	  {
	    numServ = (pr->event_data_length - 1) / pr->attribute_data_length;
	    /* the event data will be
	     * 2bytes start handle
	     * 2bytes end handle
	     * 2 or 16 bytes data
	     * we are intersted only if the UUID is 16 bit.
	     * So check if the data length is 6
	     */
	    if (pr->attribute_data_length == 6)
	    {
	      idx = 4;
	      for (i=0; i<numServ; i++)
	      {
		if((UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx])) == IMMEDIATE_ALERT_SERVICE_UUID)
		{
		  FMT_DBG_MSG(profiledbgfile,"found immediate alert service:\n");
		  findMeLocator.immAlertServHandle = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx-4]);
		  findMeLocator.immAlertServEndHandle = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx-2]);
		  FMT_DBG_MSG(profiledbgfile,"****Start Handle = 0x%02x\n", findMeLocator.immAlertServHandle);
		  FMT_DBG_MSG(profiledbgfile,"****End Handle = 0x%02x\n", findMeLocator.immAlertServEndHandle);
		}
		idx += 6;
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
	{
	  evt_att_read_by_type_resp *pr = (void*)blue_evt->data;

	  if (FMLProfile_Read_MainStateMachine() == FML_DISCOVER_CHARAC)
	  {
	    FMT_DBG_MSG(profiledbgfile, "EVT_BLUE_ATT_READ_BY_TYPE_RESP len %d\n", pr->event_data_length);
	    FMT_DBG_MSG(profiledbgfile, "Handle Value Pair len %d\n", pr->handle_value_pair_length);
	    for(idx=0; idx<(pr->event_data_length-1); idx++)
	    {
	      FMT_DBG_MSG(profiledbgfile, "%0x ", pr->handle_value_pair[idx]);
	    }
	    FMT_DBG_MSG(profiledbgfile,"\n");
				
	    /* the event data will be
	     * 2 bytes start handle
	     * 1 byte char properties
	     * 2 bytes value handle
	     * 2 or 16 bytes UUID data
	     */
	    idx = 5;
	    /* we are interested in only 16 bit UUIDs */
	    if (pr->handle_value_pair_length == 7)
	    {
	      pr->event_data_length -= 1;
	      while(pr->event_data_length > 0)
	      {
		if((UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx])) == ALERT_LEVEL_CHARACTERISTIC_UUID)
		{
		  FMT_DBG_MSG(profiledbgfile,"Discovered Alert Level Char\n");
		  findMeLocator.alertCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
		  break;
		}
		pr->event_data_length -= 7;
		idx += 7;
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
	{
	  if(FMLProfile_Read_MainStateMachine() == FML_DISCOVER_SERVICE)
	  {
	    if(findMeLocator.immAlertServHandle)
	    {
	      /* we have found the immediate alert service and now
	       * we need to find the alert characteristic
	       */
	      status = aci_gatt_disc_all_charac_of_serv(findMeLocator.connHandle, 
							findMeLocator.immAlertServHandle, 
							findMeLocator.immAlertServEndHandle); 
	      if (status != BLE_STATUS_SUCCESS)
	      {
		FMT_DBG_MSG(profiledbgfile,"Error in aci_gatt_disc_all_charac_of_serv 0x%02x\n", status);
		FML_Exit();
		break;
	      }
	      FML_SetState(FML_DISCOVER_CHARAC);
	    }
	    else
	    {
	      FMT_DBG_MSG(profiledbgfile,"Immediate alert service not found\n");
	      idx = IMM_ALERT_SERVICE_NOT_FOUND;
	      findMeLocator.applcb(EVT_FML_DISCOVERY_CMPLT,1,&idx);
	      /* the requirement for the find me locator is
	       * not met. So unregister the profile
	       */ 
	      FML_Exit();
	    }
	  }
	  else if(FMLProfile_Read_MainStateMachine() == FML_DISCOVER_CHARAC)
	  {
	    if(findMeLocator.alertCharHandle)
	    {
	      /* found the alert service */
	      FML_SetState(FML_STATE_CONNECTED);
	      idx = BLE_STATUS_SUCCESS;
	      findMeLocator.applcb(EVT_FML_DISCOVERY_CMPLT,1,&idx);
	    }
	    else
	    {
	      FMT_DBG_MSG(profiledbgfile,"alert charac not found\n");
	      /* we cannot write in this state */
	      FML_SetState(FML_CONNECTED_IDLE);
	      idx = ALERT_CHARACTERISTIC_NOT_FOUND;
	      findMeLocator.applcb(EVT_FML_DISCOVERY_CMPLT,1,&idx);
	      FML_Exit();
	    }
	  }
	}
	break;
      }
    }
    break;
  }
}

/**
 * FML_LimDisc_Timeout_Handler
 * 
 * This function handles the limited
 * discoverable mode timeout
 */ 
void FML_LimDisc_Timeout_Handler()
{	
  FMT_DBG_MSG(profiledbgfile,"FML_LimDisc_Timeout_Handler\n");
  
  Blue_NRG_Timer_Stop(findMeLocator.timerID);
  findMeLocator.timerID = 0xFF;

  if(FMLProfile_Read_MainStateMachine() == FML_ADV_IN_FAST_DISC_MODE)
  {
    /* stop advertizing */
    if (aci_gap_set_non_discoverable() == BLE_STATUS_SUCCESS)
    {
      FML_SetState(FML_START_ADV_IN_LOW_PWR_MODE);
    }
    else
    {
      FMT_DBG_MSG(profiledbgfile,"Set Non Discoverable failed\n");
      FML_SetState(FML_IDLE);
    }  
  }
}

/**
 * FMLProfile_StateMachine
 * 
 * @param[in] None
 * 
 * FML profile's state machine: to be called on application main loop.
 */ 
tBleStatus FMLProfile_StateMachine(void)
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'F','i','n','d','M','e','L','o','c','a','t','o','r'};
  static uint8_t mainState = FML_IDLE;
	
  if (mainState != FMLProfile_Read_MainStateMachine())
  {
    FMT_DBG_MSG(profiledbgfile,"[cmplt]%x\n",FMLProfile_Read_MainStateMachine());
    mainState = FMLProfile_Read_MainStateMachine();
  }
	
  switch(FMLProfile_Read_MainStateMachine())
  {
  case FML_UNINITIALIZED:
    {
      if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
	FMT_DBG_MSG(profiledbgfile,"vote for advertising\n");
	BLE_Profile_Vote_For_Advertisable_State(&findMeLocator.FMLtoBLEInf);
	FML_SetState(FML_INIT);
      }
    }
    break;
  case FML_INIT:
    {
      if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
	FMT_DBG_MSG(profiledbgfile,"initialized\n");
	status = BLE_STATUS_SUCCESS;
	findMeLocator.applcb(EVT_FML_INITIALIZED,1,&status);
	FML_SetState(FML_IDLE);
      }
    }
    break;
  case FML_START_ADV_IN_LOW_PWR_MODE:
    {
      FMT_DBG_MSG(profiledbgfile,"lim disc mode reduced power\n");		
      /* put the device in low power discoverable mode */
      status = aci_gap_set_limited_discoverable(ADV_IND,
						FML_LOW_PWR_ADV_INTERVAL_MIN, 
						FML_LOW_PWR_ADV_INTERVAL_MAX,
						PUBLIC_ADDR, 
						NO_WHITE_LIST_USE, // WHITE_LIST_FOR_ALL. Set to NO_WHITE_LIST_USE x running PTS IAS tests
						14, 
						name, 
						0,
						NULL,
						0, 
						0);

      if(status != BLE_STATUS_SUCCESS)
      {
	findMeLocator.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
	FML_SetState(FML_IDLE);
      }
      else
      {
	FML_SetState(FML_ADV_IN_LOW_PWR_MODE);
	FMT_DBG_MSG(profiledbgfile,"In Low Power Limited discoverable mode\n");
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&findMeLocator.FMLtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
      }
    }
    break;
  }
  return status;
}

/**
 * FML_Exit
 * 
 * This function unregisters the profile with
 * the main profile and resets the state machine
 */ 
void FML_Exit()
{
	FMT_DBG_MSG(profiledbgfile,"FML_Exit");
	/* the requirement for the find me locator is
	 * not met. So unregister the profile
	 */
	BLE_Profile_Unregister_Profile(&findMeLocator.FMLtoBLEInf);
	FMLProfile_Write_MainStateMachine(FML_UNINITIALIZED);
}
