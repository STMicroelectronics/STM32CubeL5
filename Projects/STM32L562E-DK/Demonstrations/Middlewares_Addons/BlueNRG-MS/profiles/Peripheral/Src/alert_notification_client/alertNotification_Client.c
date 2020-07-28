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
*   FILENAME        -  alertNotification_Client.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      30/07/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Alert Notification Client Profile  according 
*                to unified BlueNRG DK framework
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  source code for the client role of alert notification profile
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include "hci.h"
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
#include <alertNotification_Client.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
                (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
                (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))

/* states */
#define ANC_UNINITIALIZED		        (0x00)
#define ANC_INITIALIZED			        (0x01)
#define ANC_IDLE 			        (0x02)
#define ANC_UNDIRECTED_ADV_MODE 	        (0x03)
#define ANC_ADV_IN_FAST_DISC_MODE	        (0x04)
#define ANC_START_ADV_IN_LOW_PWR_MODE           (0x05)
#define ANC_ADV_IN_LOW_PWR_MODE		        (0x06)
#define ANC_DISCOVER_SERVICES		        (0x07)
#define ANC_DISCOVER_CHARACS		        (0x08)
#define ANC_DISCOVER_ALERT_CHAR_DESC		(0x09)
#define ANC_DISCOVER_UNREAD_ALERT_STATUS_DESC	(0x0A)
#define ANC_READ_NEW_ALERT_CATEGORY		(0x0B)
#define ANC_READ_UNREAD_ALERT_STATUS_CATEGORY	(0x0C)
#define ANC_CONNECTED				(0x0D)

/* advertising intervals in terms of 625 micro seconds */
#define ANC_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define ANC_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define ANC_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define ANC_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/******************************************************************************
* type definitions
******************************************************************************/
typedef struct _tANCContext
{
  /**
   * state of the blood pressure
   * sensor state machine
   */ 
  tProfileState state;
  
  /**
   * connection handle 
   */ 
  uint16_t connHandle;
  
  /**
   * bitmask of the attributes
   * discovered
   */ 
  uint8_t featuresFound;
  
  /**
   * flag which maintains
   * the connection status
   */
  uint8_t disconnected;
  
  /**
   * callback to notify the applciation with
   * the profile specific events
   */ 
  BLE_CALLBACK_FUNCTION_TYPE applcb;
  
  /**
   * handle of the alert notification service
   */ 
  uint16_t alertServiceHandle;
  
  /**
   * end handle of the alert notification service
   */ 
  uint16_t alertServiceEndHandle;
  
  /**
   * handle of the supported new alert category
   * characteristic
   */ 
  uint16_t newAlertCategoryHandle;
  
  /**
   * handle of the new alert characteristic
   */ 
  uint16_t newAlertHandle;
  
  /**
   * handle of the client configuration
   * descriptor of new alert characteristic
   */ 
  uint16_t newAlertDescHandle;
  
  /**
   * handle of the supported unread alert
   * category characteristic
   */ 
  uint16_t UnreadAlertCategoryHandle;
  
  /**
   * handle of the unread alert status
   * characteristic
   */ 
  uint16_t unreadAlertStatusHandle;
  
  /**
   * handle of the client configuration
   * descriptor of unread alert status characteristic
   */ 
  uint16_t unreadAlertStatusDescHandle;
  
  /**
   * handle of the alert notification
   * control point characteristic
   */ 
  uint16_t controlPointHandle;
  
  /**
   * alert category supported by the
   * peer server
   */ 
  uint8_t alertCategory[2];
  
  /**
   * unread alert status category
   * supported by the peer server
   */ 
  uint8_t unreadAlertCategory[2];
  
  /**
   * timer ID of the timer
   * started by the profile
   */ 
  tTimerID timerID;
  
  /**
   * profile interface context. This is
   * registered with the main profile
   */ 
  tProfileInterfaceContext ANCtoBLEInf;
}tANCContext;

uint8_t ANC_new_alert_status[10]=
{
	0x00, /*CATEGORY_ID_SIMPLE_ALERT*/
	0x00, /*CATEGORY_ID_EMAIL*/
	0x00, /*CATEGORY_ID_NEWS*/
	0x00, /*CATEGORY_ID_CALL*/
	0x00, /*CATEGORY_ID_MISSED_CALL*/
	0x00, /*CATEGORY_ID_SMS_MMS*/
	0x00, /*CATEGORY_ID_VOICE_MAIL*/
	0x00, /*CATEGORY_ID_SCHEDULE*/
	0x00, /*CATEGORY_ID_HIGH_PRIORITIZED_ALERT*/
	0x00  /*CATEGORY_ID_INSTANT_MESSAGE*/
};

uint8_t ANC_unread_alert_status[10]=
{
	0x00, /*CATEGORY_ID_SIMPLE_ALERT*/
	0x00, /*CATEGORY_ID_EMAIL*/
	0x00, /*CATEGORY_ID_NEWS*/
	0x00, /*CATEGORY_ID_CALL*/
	0x00, /*CATEGORY_ID_MISSED_CALL*/
	0x00, /*CATEGORY_ID_SMS_MMS*/
	0x00, /*CATEGORY_ID_VOICE_MAIL*/
	0x00, /*CATEGORY_ID_SCHEDULE*/
	0x00, /*CATEGORY_ID_HIGH_PRIORITIZED_ALERT*/
	0x00  /*CATEGORY_ID_INSTANT_MESSAGE*/
};
/*******************************************************************************
* Variables
*******************************************************************************/
tANCContext alertNotificationClient;
extern void *profiledbgfile;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void ANC_Event_Handler(void *pckt);
static void ANC_Exit(uint8_t errCode);
static void ANC_AdvTimeout_Handler(void);

static void ANCProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState ANCProfile_Read_MainStateMachine(void);

/*******************************************************************************
* SAP
*******************************************************************************/
tBleStatus ANC_Client_Init(BLE_CALLBACK_FUNCTION_TYPE applcb)
{
  tBleStatus retval = BLE_STATUS_FAILED;
	
  ANC_DBG_MSG(profiledbgfile, "ANP_Client_Init\n");

  if(Is_Profile_Present(PID_ALERT_NOTIFICATION_CLIENT) == BLE_STATUS_SUCCESS)
  {
    return BLE_STATUS_PROFILE_ALREADY_INITIALIZED;
  }

  if(applcb == NULL)
  {
    return BLE_STATUS_NULL_PARAM;
  }
	
  /* Initialize Data Structures */
  BLUENRG_memset(&alertNotificationClient,0,sizeof(alertNotificationClient));
  alertNotificationClient.applcb = applcb;
  alertNotificationClient.timerID = 0xFF;
	
  ANCProfile_Write_MainStateMachine(ANC_UNINITIALIZED);
	
  /* register the alert notification client with BLE main Profile */
  BLUENRG_memset(&alertNotificationClient.ANCtoBLEInf,0,sizeof(alertNotificationClient.ANCtoBLEInf));
  alertNotificationClient.ANCtoBLEInf.profileID = PID_ALERT_NOTIFICATION_CLIENT;
  alertNotificationClient.ANCtoBLEInf.callback_func = ANC_Event_Handler;
  alertNotificationClient.ANCtoBLEInf.voteForAdvertisableState = 0;

  retval = BLE_Profile_Register_Profile(&alertNotificationClient.ANCtoBLEInf);
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    return (retval);
  }
	
  ANC_DBG_MSG(profiledbgfile,"alert notification Profile is Initialized in Client role\n");
	
  return (BLE_STATUS_SUCCESS);
}

tBleStatus ANC_Advertize(uint8_t useWhitelist)
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'A','l','e','r','t','N','o','t','i','f','i','c','a','t','i','o','n'};
	
  ANC_DBG_MSG(profiledbgfile,"ANC_Advertize %x\n",ANCProfile_Read_MainStateMachine());
	
  if(ANCProfile_Read_MainStateMachine() == ANC_INITIALIZED)
  {
    if(useWhitelist)
    {
      /* start the undirected connectable mode */
      status = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);

      if(status == BLE_STATUS_SUCCESS)
      {
	ANCProfile_Write_MainStateMachine(ANC_UNDIRECTED_ADV_MODE);
	ANC_DBG_MSG(profiledbgfile,"Enabled undirect advertising\n");
      }
    }
    else
    {
      status = aci_gap_set_limited_discoverable(ADV_IND, 
						ANC_FAST_CONN_ADV_INTERVAL_MIN, 
						ANC_FAST_CONN_ADV_INTERVAL_MAX,
						PUBLIC_ADDR, 
						NO_WHITE_LIST_USE, 
						sizeof(name),
						name, 
						0, 
						NULL,
						0, 
						0);

      if(status == BLE_STATUS_SUCCESS)
      {
	ANCProfile_Write_MainStateMachine(ANC_ADV_IN_FAST_DISC_MODE);
	ANC_DBG_MSG(profiledbgfile,"Enabled advertising fast connection\n");
	/* start a timer for 30 seconds */
	Blue_NRG_Timer_Start(30,ANC_AdvTimeout_Handler,&alertNotificationClient.timerID);
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&alertNotificationClient.ANCtoBLEInf, 
					       BLE_PROFILE_STATE_ADVERTISING);
      } 
    }
    if (status != BLE_STATUS_SUCCESS)
    {
      /* inform the application that the advertizing was not
       * enabled
       */
      ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
      alertNotificationClient.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
    }

  }
	
  return status;
}

tBleStatus ANC_Write_Control_Point(tCommandID command,tCategoryID category)
{
  uint8_t idx = 0;
  uint8_t charVal[2];
  uint8_t Category_ID_To_Bitmask[10]=
    {
      CATEGORY_SIMPLE_ALERT,
      CATEGORY_EMAIL,
      CATEGORY_NEWS,
      CATEGORY_CALL,
      CATEGORY_MISSED_CALL,
      CATEGORY_SMS_MMS,
      CATEGORY_VOICE_MAIL,
      CATEGORY_SCHEDULE,
      CATEGORY_HIGH_PRIORITIZED_ALERT,
      CATEGORY_INSTANT_MESSAGE
    };
	
  if(ANCProfile_Read_MainStateMachine() == ANC_CONNECTED)
  {
    ANC_DBG_MSG(profiledbgfile,"ANC_Write_Control_Point\n");
    if((command > NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY) ||
       ((category != 0xFF) && (category > CATEGORY_ID_INSTANT_MESSAGE)))
    {
      return BLE_STATUS_INVALID_PARAMS;
    }
		
    /* if the category is 0xFF, then we
     * need not check for the categories supported
     * by the peer
     */ 
    if(category != 0xFF)
    {
      if(category > CATEGORY_ID_SCHEDULE)
      {
	idx = 1;
      }
			
      if((command == ENABLE_NEW_ALERT_NOTIFICATION) ||
	 (command == DISABLE_NEW_ALERT_NOTIFICATION) ||
	 (command == NOTIFY_NEW_ALERT_IMMEDIATELY))
      {
	if(!(alertNotificationClient.alertCategory[idx] & Category_ID_To_Bitmask[category]))
	{
	  return BLE_STATUS_INVALID_PARAMS;
	}
      }
      else
      {
	if(!(alertNotificationClient.unreadAlertCategory[idx] & Category_ID_To_Bitmask[category]))
	{
	  return BLE_STATUS_INVALID_PARAMS;
	}
      }
    }
    
    charVal[0] = command;
    charVal[1] = category;
		
    ANC_DBG_MSG(profiledbgfile,"writing\n");
    /* write to the control point */
    return aci_gatt_write_charac_value(alertNotificationClient.connHandle, 
				       alertNotificationClient.controlPointHandle, 
				       2, 
				       charVal);
  }
	
  return ERR_COMMAND_DISALLOWED;
}

tBleStatus ANC_Enable_Disable_New_Alert_Notification(uint16_t enable)
{
  tBleStatus status = ERR_COMMAND_DISALLOWED;
	
  if(ANCProfile_Read_MainStateMachine() == ANC_CONNECTED)
  {
    ANC_DBG_MSG(profiledbgfile,"ANC_Enable_Disable_New_Alert_Notification\n");
    if((enable != 0) && (enable != 1))
    {
      status = BLE_STATUS_INVALID_PARAMS;
    }
    else
    {
      status = aci_gatt_write_charac_descriptor(alertNotificationClient.connHandle,
						alertNotificationClient.newAlertDescHandle,
						2,
						(uint8_t *)&enable);
    }
  }
	
  return status;
}

tBleStatus ANC_Enable_Disable_Unread_Alert_Status_Notification(uint16_t enable)
{
  tBleStatus status = ERR_COMMAND_DISALLOWED;
	
  if(ANCProfile_Read_MainStateMachine() == ANC_CONNECTED)
  {
    ANC_DBG_MSG(profiledbgfile,"ANC_Enable_Disable_Unread_Alert_Status_Notification\n");
    if((enable != 0) && (enable != 1))
    {
      status = BLE_STATUS_INVALID_PARAMS;
    }
    else
    {
      status = aci_gatt_write_charac_descriptor(alertNotificationClient.connHandle,
						alertNotificationClient.unreadAlertStatusDescHandle, 
						2, 
						(uint8_t *)&enable);
    }
  }
	
  return status;
}

/*******************************************************************************
* Local Functions
*******************************************************************************/
static void ANCProfile_Write_MainStateMachine(tProfileState localmainState)
{
  alertNotificationClient.state = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState ANCProfile_Read_MainStateMachine(void)
{
  return(alertNotificationClient.state);
}

/**
 * HT_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 *
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB() 
 */ 
void ANC_Event_Handler(void *pckt)
{
  uint8_t status;

  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  switch(event_pckt->evt)
  {
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTED)
      {
	switch(evt->subevent)
	{
	case EVT_LE_CONN_COMPLETE:
	  {
	    evt_le_connection_complete *cc = (void *)evt->data;
	    
	    if (cc->status == BLE_STATUS_SUCCESS) 
	    {
	      alertNotificationClient.connHandle = cc->handle; 
	      if(alertNotificationClient.disconnected)
	      {
		/* we are recovering from a connection loss
		 * so write to the control point characteristic
		 * to receive alerts and unread alert status
		 */
		ANCProfile_Write_MainStateMachine(ANC_CONNECTED);
		ANC_Write_Control_Point(ENABLE_NEW_ALERT_NOTIFICATION,0xFF);
		ANC_Write_Control_Point(ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION,0xFF);
		ANC_Write_Control_Point(NOTIFY_NEW_ALERT_IMMEDIATELY,0xFF);
		ANC_Write_Control_Point(NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY,0xFF);
		ANCProfile_Write_MainStateMachine(ANC_DISCOVER_UNREAD_ALERT_STATUS_DESC);
	      }
	      else
	      {
		/* we are connected */
		ANCProfile_Write_MainStateMachine(ANC_DISCOVER_SERVICES);
		ANC_DBG_MSG(profiledbgfile,"connected\n");
		/* start the procedure to discover all the services on the peer */
		if (aci_gatt_disc_all_prim_services(alertNotificationClient.connHandle) != BLE_STATUS_SUCCESS)
		{
		  /* we are connected */
		  ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
		  ANC_DBG_MSG(profiledbgfile,"Discovery all primary service call failed\n");
		} 
	      }
	    }
	  }
	  break;
	}
      }
    }
    break;
  case EVT_DISCONN_COMPLETE:
    {
      ANC_DBG_MSG(profiledbgfile,"disconnected\n");
      ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
      alertNotificationClient.disconnected = 0x01;
      /* stop the running timer */
      Blue_NRG_Timer_Stop(alertNotificationClient.timerID);
      alertNotificationClient.timerID = 0xFF;
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
	  if ((ANCProfile_Read_MainStateMachine() == ANC_ADV_IN_LOW_PWR_MODE) || (ANCProfile_Read_MainStateMachine() == ANC_ADV_IN_FAST_DISC_MODE))
	  {
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State(&alertNotificationClient.ANCtoBLEInf, BLE_PROFILE_STATE_CONNECTABLE_IDLE);	    

	    if (ANCProfile_Read_MainStateMachine() == ANC_ADV_IN_LOW_PWR_MODE)
	    {
	      ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
	      alertNotificationClient.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
	    }
	  } 
	}
	break;
      case  EVT_BLUE_GAP_PAIRING_CMPLT:
	{
          ANC_DBG_MSG(profiledbgfile,"HCI_EVT_BLUE_GAP_PAIRING_CMPLT %x %x\n",ANCProfile_Read_MainStateMachine(),alertNotificationClient.disconnected);
          /* Do nothing when you receieve a pairing complete
            * because all actions required are taken on connection complete
            * itself. Pairing does not have any special meaning
            * in case of alert notification profile
            */
          
#if 0 /* Original code TBR */
	  evt_gap_pairing_cmplt *pairing = (void*)blue_evt->data;
	  
	  ANC_DBG_MSG(profiledbgfile,"EVT_BLUE_GAP_PAIRING_CMPLT %x %x\n",
		      ANCProfile_Read_MainStateMachine(),
		      alertNotificationClient.disconnected);

	  if (pairing->status == BLE_STATUS_SUCCESS) 
	  {
	    if(ANCProfile_Read_MainStateMachine() != ANC_CONNECTED)
	    {
	      if(alertNotificationClient.disconnected)
	      {
		/* we are recovering from a connection loss
		 * so write to the control point characteristic
		 * to receive alerts and unread alert status
		 */
		ANCProfile_Write_MainStateMachine(ANC_CONNECTED);
		ANC_Write_Control_Point(ENABLE_NEW_ALERT_NOTIFICATION,0xFF);
		//ANC_Write_Control_Point(ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION,0xFF);
		ANC_Write_Control_Point(NOTIFY_NEW_ALERT_IMMEDIATELY,0xFF);
		//ANC_Write_Control_Point(NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY,0xFF);
		ANCProfile_Write_MainStateMachine(ANC_DISCOVER_UNREAD_ALERT_STATUS_DESC);
	      }
	      else
	      {
		/* we are connected */
		ANCProfile_Write_MainStateMachine(ANC_DISCOVER_SERVICES);
		ANC_DBG_MSG(profiledbgfile,"Connected %d\n", alertNotificationClient.connHandle);
		/* start the procedure to discover all the services on the peer */
		status = aci_gatt_disc_all_prim_services(alertNotificationClient.connHandle);
		if (status != BLE_STATUS_SUCCESS)
		{
		  ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
		  ANC_DBG_MSG(profiledbgfile,"Discovery all primary service call failed\n");
		}
	      }
	    }
	  }
	  else
	  {
	    ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
	    alertNotificationClient.applcb(EVT_MP_PAIRING_COMPLETE,1,&pairing->status);
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State(&alertNotificationClient.ANCtoBLEInf, BLE_PROFILE_STATE_CONNECTABLE_IDLE);	    
	  }
#endif /* Original code */
	}
	break;
      case EVT_BLUE_GATT_NOTIFICATION:
	{
	  evt_gatt_attr_notification *pr = (void*)blue_evt->data;
	  uint8_t i;
	  
	  if(pr->attr_handle == alertNotificationClient.newAlertHandle)
	  {
	    /* the event buffer will have the data as follows:
	     * category ID - 1 byte
	     * number of alerts for that category - 1 byte
	     * text string information - optional(varying number of bytes : the rest
	     * of the available bytes as indicated by length has to be considered
	     * as the string
	     */
	    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
	    ANC_DBG_MSG(profiledbgfile,"New Alert Char CategoryId       0x%02x\n", pr->attr_value[0]);
	    ANC_DBG_MSG(profiledbgfile,"New Alert Char Number new Alert 0x%02x\n", pr->attr_value[1]);
	    ANC_DBG_MSG(profiledbgfile,"New Alert Char Text             ");
	    for (i=2; i<pr->event_data_length-2; i++)
	      ANC_DBG_MSG(profiledbgfile,"%c", pr->attr_value[i]);
	    ANC_DBG_MSG(profiledbgfile,"\n***************************************************************\n");
				
	    /* notify the application only if the alert
	     * for the category is different from the previous
	     * notification data
	     */ 
	    if((pr->attr_value[0] < CATEGORY_ID_INSTANT_MESSAGE) &&
	       (ANC_new_alert_status[pr->attr_value[0]] != pr->attr_value[1]))
	    {
	      ANC_new_alert_status[pr->attr_value[0]] = pr->attr_value[1];
	      alertNotificationClient.applcb(EVT_ANC_NEW_ALERT_RECEIVED,
					     pr->event_data_length-2,
					     pr->attr_value);   
	    }
	  }
	  else if(pr->attr_handle == alertNotificationClient.unreadAlertStatusHandle)
	  {
	    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
	    ANC_DBG_MSG(profiledbgfile,"Unread Alert Status Char CategoryId          0x%02x\n", pr->attr_value[0]);
	    ANC_DBG_MSG(profiledbgfile,"Unread Alert Status Char Number unread Alert 0x%02x\n", pr->attr_value[1]);
	    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");

	    /* the event buffer will have the data as follows:
	     * category ID - 1 byte
	     * number of unread alerts for that category - 1 byte
	     */ 
	    if((pr->attr_value[0] < CATEGORY_ID_INSTANT_MESSAGE) &&
	       (ANC_unread_alert_status[pr->attr_value[0]] != pr->attr_value[1]))
	    {
	      ANC_unread_alert_status[pr->attr_value[0]] = pr->attr_value[1];
	      alertNotificationClient.applcb(EVT_ANC_UNREAD_ALERT_STATUS_RECEIVED,
					     pr->event_data_length-2,
					     pr->attr_value);
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
	{
	  evt_att_read_by_group_resp *pr = (void*)blue_evt->data;
	  uint8_t numServ, i, idx;
	  uint16_t uuid;
			
	  ANC_DBG_MSG(profiledbgfile,"EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP\n");
			
	  if (ANCProfile_Read_MainStateMachine() == ANC_DISCOVER_SERVICES)
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
		uuid = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx]);
		if(uuid == ALERT_NOTIFICATION_SERVICE_UUID)
		{
		  alertNotificationClient.alertServiceHandle = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx-4]);
		  alertNotificationClient.alertServiceEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->attribute_data_list[idx-2]);
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  ANC_DBG_MSG(profiledbgfile,"*Found alert notification service start handle 0x%04x end handle 0x%04x\n",
			      alertNotificationClient.alertServiceHandle,
			      alertNotificationClient.alertServiceEndHandle);
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
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
	  uint8_t idx;
	  uint16_t uuid, handle;

	  ANC_DBG_MSG(profiledbgfile,"EVT_BLUE_ATT_READ_BY_TYPE_RESP\n");
	  
	  if (ANCProfile_Read_MainStateMachine() == ANC_DISCOVER_CHARACS)
	  {
	    ANC_DBG_MSG(profiledbgfile,"HCI_EVT_BLUE_ATT_READ_BY_TYPE_RESP len %d\n", pr->event_data_length);
	    
	    /* the event data will be
	     * 2 bytes start handle
	     * 1 byte char properties
	     * 2 bytes handle
	     * 2 or 16 bytes data
	     */
	    idx = 5;
	    /* we are interested in only 16 bit UUIDs */
	    if (pr->handle_value_pair_length == 7)
	    {
	      pr->event_data_length -= 1;
	      while(pr->event_data_length > 0)
	      {
		uuid = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx]);
		/* store the characteristic handle not the attribute handle */
		handle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
		if(uuid == SUPPORTED_NEW_ALERT_CATEGORY_CHAR_UUID)
		{
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category start handle 0x%04x\n", UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category properties   0x%02x\n", pr->handle_value_pair[idx-3]);
		  ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category handle       0x%04x\n", handle);
		  ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category uuid         0x%04x\n", uuid);
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  alertNotificationClient.newAlertCategoryHandle = handle;
		  alertNotificationClient.featuresFound |= SUPPORTED_NEW_ALERT_CATEGORY_CHAR_FOUND;
		}
		else if(uuid == NEW_ALERT_CHAR_UUID)
		{
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  ANC_DBG_MSG(profiledbgfile,"New Alert start handle 0x%04x\n", UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  ANC_DBG_MSG(profiledbgfile,"New Alert properties   0x%02x\n", pr->handle_value_pair[idx-3]);
		  ANC_DBG_MSG(profiledbgfile,"New Alert handle       0x%04x\n", handle);
		  ANC_DBG_MSG(profiledbgfile,"New Alert uuid         0x%04x\n", uuid);
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  alertNotificationClient.newAlertHandle = handle;
		  alertNotificationClient.featuresFound |= NEW_ALERT_CHAR_FOUND;
		}
		else if(uuid == SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_UUID)
		{
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Category start handle 0x%04x\n", UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Category properties   0x%02x\n", pr->handle_value_pair[idx-3]);
		  ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Category handle       0x%04x\n", handle);
		  ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Category uuid         0x%04x\n", uuid);
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  alertNotificationClient.UnreadAlertCategoryHandle = handle;
		  alertNotificationClient.featuresFound |= SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_FOUND;
		}
		else if(uuid == UNREAD_ALERT_STATUS_CHAR_UUID)
		{
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  ANC_DBG_MSG(profiledbgfile,"Unread Alert Status start handle 0x%04x\n", UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  ANC_DBG_MSG(profiledbgfile,"Unread Alert Status properties   0x%02x\n", pr->handle_value_pair[idx-3]);
		  ANC_DBG_MSG(profiledbgfile,"Unread Alert Status handle       0x%04x\n", handle);
		  ANC_DBG_MSG(profiledbgfile,"Unread Alert Status uuid         0x%04x\n", uuid);
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  alertNotificationClient.unreadAlertStatusHandle = handle;
		  alertNotificationClient.featuresFound |= UNREAD_ALERT_STATUS_CHAR_FOUND;
		}
		else if(uuid == ALERT_NOTIFICATION_CONTROL_POINT_CHAR_UUID)
		{
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  ANC_DBG_MSG(profiledbgfile,"Alert Notification Control Point start handle 0x%04x\n", UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  ANC_DBG_MSG(profiledbgfile,"Alert Notification Control Point properties   0x%02x\n", pr->handle_value_pair[idx-3]);
		  ANC_DBG_MSG(profiledbgfile,"Alert Notification Control Point handle       0x%04x\n", handle);
		  ANC_DBG_MSG(profiledbgfile,"Alert Notification Control Point uuid         0x%04x\n", uuid);
		  ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  alertNotificationClient.controlPointHandle = handle;
		  alertNotificationClient.featuresFound |= CONTROL_POINT_CHAR_FOUND;
		}
		pr->event_data_length -= 7;
		idx += 7;
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_FIND_INFORMATION_RESP:
	{
	  evt_att_find_information_resp *pr = (void*)blue_evt->data;
	  uint8_t numDesc, idx, i;
	  uint16_t uuid, handle;

	  /*
	   * event data will be of the format
	   * 2 bytes handle
	   * 2 bytes UUID
	   */ 
	  ANC_DBG_MSG(profiledbgfile,"EVT_BLUE_ATT_FIND_INFORMATION_RESP\n");

	  if((ANCProfile_Read_MainStateMachine() == ANC_DISCOVER_ALERT_CHAR_DESC) ||
	     (ANCProfile_Read_MainStateMachine() == ANC_DISCOVER_UNREAD_ALERT_STATUS_DESC))
	  {
	    numDesc = (pr->event_data_length - 1) / 4;
	    /* we are interested only in 16 bit UUIDs */
	    idx = 0;
	    if (pr->format == UUID_TYPE_16) 
	    {
	      for (i=0; i<numDesc; i++)
	      {
		handle = UNPACK_2_BYTE_PARAMETER(&pr->handle_uuid_pair[idx]);
		uuid = UNPACK_2_BYTE_PARAMETER(&pr->handle_uuid_pair[idx+2]);
						
		if(uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID)
		{
		  if(ANCProfile_Read_MainStateMachine() == ANC_DISCOVER_UNREAD_ALERT_STATUS_DESC)
		  {
		    alertNotificationClient.unreadAlertStatusDescHandle = handle;
		    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		    ANC_DBG_MSG(profiledbgfile,"Unread Alert Status Desc handle 0x%04x\n", handle);
		    ANC_DBG_MSG(profiledbgfile,"Unread Alert Status Desc uuid   0x%04x\n", uuid);
		    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  }
		  else
		  {
		    alertNotificationClient.newAlertDescHandle = handle;
		    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		    ANC_DBG_MSG(profiledbgfile,"New Alert Desc handle 0x%04x\n", handle);
		    ANC_DBG_MSG(profiledbgfile,"New Alert Desc uuid   0x%04x\n", uuid);
		    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
		  }
		}
		idx += 4;
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_RESP:
	{
	  evt_att_read_resp *pr = (void*)blue_evt->data;

	  ANC_DBG_MSG(profiledbgfile,"EVT_BLUE_ATT_READ_RESP\n");
	  if(ANCProfile_Read_MainStateMachine() == ANC_READ_NEW_ALERT_CATEGORY)
	  {	    
            alertNotificationClient.alertCategory[0] = pr->attribute_value[0];
	    if(pr->event_data_length == 2)
	    {
	      alertNotificationClient.alertCategory[1] = pr->attribute_value[1];
	    }
	    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
	    ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category BitMask[0] 0x%02x\n", alertNotificationClient.alertCategory[0]);
	    ANC_DBG_MSG(profiledbgfile,"Supported New Alert Category BitMask[1] 0x%02x\n", alertNotificationClient.alertCategory[1]);
	    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
	  }
	  else if(ANCProfile_Read_MainStateMachine() == ANC_READ_UNREAD_ALERT_STATUS_CATEGORY)
	  {
	    alertNotificationClient.unreadAlertCategory[0] = pr->attribute_value[0];
	    if(pr->event_data_length == 2)
	    {
	      alertNotificationClient.unreadAlertCategory[1] = pr->attribute_value[1];
	    }
	    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
	    ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Category BitMask[0] 0x%02x\n", alertNotificationClient.unreadAlertCategory[0]);
	    ANC_DBG_MSG(profiledbgfile,"Supported Unread Alert Category BitMask[1] 0x%02x\n", alertNotificationClient.unreadAlertCategory[1]);
	    ANC_DBG_MSG(profiledbgfile,"***************************************************************\n");
	  }
	}
	break;
      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
	{
	  uint8_t idx;

	  ANC_DBG_MSG(profiledbgfile,"EVT_BLUE_GATT_PROCEDURE_COMPLETE\n");
	  switch(ANCProfile_Read_MainStateMachine())
	  {
	  case ANC_DISCOVER_SERVICES:
	    {
	      if(alertNotificationClient.alertServiceHandle)
	      {
		/* we have found the alert notification service and now
		 * we need to find the characteristics of alert notification
		 * service
		 */
		status = aci_gatt_disc_all_charac_of_serv(alertNotificationClient.connHandle,
							  alertNotificationClient.alertServiceHandle,
							  alertNotificationClient.alertServiceEndHandle);
		if (status != BLE_STATUS_SUCCESS)
		{
		  ANC_DBG_MSG(profiledbgfile,"Discovery all charac of serv call failed\n");
		  ANC_Exit(ALERT_NOTIFICATION_SERVICE_NOT_FOUND);
		}
		else
		{
		  ANCProfile_Write_MainStateMachine(ANC_DISCOVER_CHARACS);
		}
	      }
	      else
	      {
		ANC_DBG_MSG(profiledbgfile,"Alert service not found\n");
		ANC_Exit(ALERT_NOTIFICATION_SERVICE_NOT_FOUND);
	      }
	    }
	    break;
	  case ANC_DISCOVER_CHARACS:
	    {
	      if((alertNotificationClient.featuresFound & ANC_FOUND_ALL_CHARACS) == ANC_FOUND_ALL_CHARACS)
	      {
		/* found all the mandatory characteristics,
		 * now discover descriptors
		 */
		status = aci_gatt_disc_all_charac_descriptors(alertNotificationClient.connHandle,
							      alertNotificationClient.newAlertHandle,
							      alertNotificationClient.newAlertHandle+2);
		if (status != BLE_STATUS_SUCCESS)
		{
		  ANC_DBG_MSG(profiledbgfile,"Discovery all charac descriptors call failed\n");
		  ANC_Exit(NEW_ALERT_CHAR_DESC_NOT_FOUND);
		}
		else
		{
		  ANCProfile_Write_MainStateMachine(ANC_DISCOVER_ALERT_CHAR_DESC);
		}
	      }
	      else
	      {
		ANC_DBG_MSG(profiledbgfile,"Alert charac not found\n");
		/* error code should correspond to the characteristics
		 * not found
		 */
		idx = ((~(alertNotificationClient.featuresFound & ANC_FOUND_ALL_CHARACS)) & 0x3E);
		ANC_Exit(idx);
	      }
	    }
	    break;
	  case ANC_DISCOVER_ALERT_CHAR_DESC:
	    {
	      if(alertNotificationClient.newAlertDescHandle)
	      {
		/* discover the client configuration descriptor of
		 * the unread alert status characteristic
		 */ 
		status = aci_gatt_disc_all_charac_descriptors(alertNotificationClient.connHandle,
							      alertNotificationClient.unreadAlertStatusHandle,
							      alertNotificationClient.unreadAlertStatusHandle+2);
		if (status != BLE_STATUS_SUCCESS)
		{
		  ANC_DBG_MSG(profiledbgfile,"Discovery all charac descriptors call failed\n");
		  ANC_Exit(UNREAD_ALERT_STATUS_DESC_NOT_FOUND);
		}
		else
		{
		  ANCProfile_Write_MainStateMachine(ANC_DISCOVER_UNREAD_ALERT_STATUS_DESC);
		}
	      }
	      else
	      {
		ANC_DBG_MSG(profiledbgfile,"new alert client configuration desc not found\n");
		ANC_Exit(NEW_ALERT_CHAR_DESC_NOT_FOUND);
	      }
	    }
	    break;
	  case ANC_DISCOVER_UNREAD_ALERT_STATUS_DESC:
	    {
	      if(alertNotificationClient.unreadAlertStatusDescHandle)
	      {
		ANC_DBG_MSG(profiledbgfile,"Read the Supported New Alert Category\n");
		/* read the Supported New Alert Category
		 */
		status = aci_gatt_read_charac_val(alertNotificationClient.connHandle, 
						  alertNotificationClient.newAlertCategoryHandle);
		if (status != BLE_STATUS_SUCCESS)
		{
		  ANC_DBG_MSG(profiledbgfile,"Read Charac Value call failed\n");
		  ANC_Exit(NO_CATEGORY_SUPPORTED_IN_ALERT_CHAR);
		  
		}
		else
		{
		  ANCProfile_Write_MainStateMachine(ANC_READ_NEW_ALERT_CATEGORY);
		}
	      }
	      else
	      {
		ANC_DBG_MSG(profiledbgfile,"Unread alert client configuration desc not found\n");
		ANC_Exit(UNREAD_ALERT_STATUS_DESC_NOT_FOUND);
	      }
	    }
	    break;
	  case ANC_READ_NEW_ALERT_CATEGORY:
	    {
	      if(alertNotificationClient.alertCategory[0])
	      {
		/* read the unread alert status category
		 */
		status = aci_gatt_read_charac_val(alertNotificationClient.connHandle,
						  alertNotificationClient.UnreadAlertCategoryHandle);
		if (status != BLE_STATUS_SUCCESS)
		{
		  ANC_DBG_MSG(profiledbgfile,"Read Charac Value call failed\n");
		  ANC_Exit(NO_CATEGORY_SUPPORTED_IN_UNREAD_ALERT_CHAR);
		}
		else
		{
		  ANCProfile_Write_MainStateMachine(ANC_READ_UNREAD_ALERT_STATUS_CATEGORY);
		}
	      }
	      else
	      {
		ANC_DBG_MSG(profiledbgfile,"no category supported in new alert\n");
		ANC_Exit(NO_CATEGORY_SUPPORTED_IN_ALERT_CHAR);
	      }
	    }
	    break;
	  case ANC_READ_UNREAD_ALERT_STATUS_CATEGORY:
	    {
	      if(alertNotificationClient.unreadAlertCategory[0])
	      {
		/* the initialization sequence is complete
		 * notify the application
		 */ 
		ANCProfile_Write_MainStateMachine(ANC_CONNECTED);
		idx = BLE_STATUS_SUCCESS;
		alertNotificationClient.applcb(EVT_ANC_DISCOVERY_CMPLT,1,&idx);
	      }
	      else
	      {
		ANC_DBG_MSG(profiledbgfile,"No category supported in unread alert status\n");
		ANC_Exit(NO_CATEGORY_SUPPORTED_IN_UNREAD_ALERT_CHAR);
	      }
	    }
	    break;
	  }
	}
	break;
      }
    }
    break;
  }
}

/**
 * ANCProfile_StateMachine
 * 
 * @param[in] None
 * 
 * ANC profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ANCProfile_StateMachine(void)
{
  uint8_t status;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'A','l','e','r','t','N','o','t','i','f','i','c','a','t','i','o','n'};
	
  switch(ANCProfile_Read_MainStateMachine())
  {
  case ANC_UNINITIALIZED:
    {
      if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
	ANC_DBG_MSG(profiledbgfile,"Vote for advertising\n");
	BLE_Profile_Vote_For_Advertisable_State(&alertNotificationClient.ANCtoBLEInf);
	ANCProfile_Write_MainStateMachine(ANC_IDLE);
      }
    }
    break;
  case ANC_IDLE:
    {
      if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
	ANC_DBG_MSG(profiledbgfile,"Initialized\n");
	status = BLE_STATUS_SUCCESS;
	alertNotificationClient.applcb(EVT_ANC_INITIALIZED,1,&status);
	ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
      }
    }
    break;
  case ANC_START_ADV_IN_LOW_PWR_MODE:
    {
      /* put the device in low power discoverable mode */
      status = aci_gap_set_limited_discoverable(ADV_IND, 
						ANC_LOW_PWR_ADV_INTERVAL_MIN,
						ANC_LOW_PWR_ADV_INTERVAL_MAX,
						PUBLIC_ADDR,
						NO_WHITE_LIST_USE,
						sizeof(name),
						name,
						0,
						NULL,
						0x0,
						0x0);
				
      if(status == BLE_STATUS_SUCCESS)
      {
	ANCProfile_Write_MainStateMachine(ANC_ADV_IN_LOW_PWR_MODE);
	ANC_DBG_MSG(profiledbgfile,"Enabled advertising in low power mode\n");
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&alertNotificationClient.ANCtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);

      }
      else
      {
	ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
	alertNotificationClient.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
    break;
  case ANC_UNDIRECTED_ADV_MODE:
    {
      /* start a timer for 120 seconds */
      Blue_NRG_Timer_Start(120,ANC_AdvTimeout_Handler,&alertNotificationClient.timerID);
      
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&alertNotificationClient.ANCtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);

    }
    break;
  }
  return status;
}

/**
 * HT_AdvTimeout_Handler
 * 
 * advertising timeout handler
 */ 
void ANC_AdvTimeout_Handler(void)
{
  uint8_t status;

  /* stop the running timer */
  Blue_NRG_Timer_Stop(alertNotificationClient.timerID);
  alertNotificationClient.timerID = 0xFF;
  if((ANCProfile_Read_MainStateMachine() == ANC_UNDIRECTED_ADV_MODE) ||
     (ANCProfile_Read_MainStateMachine() == ANC_ADV_IN_FAST_DISC_MODE))
  {
    /* stop advertizing */
    status = aci_gap_set_non_discoverable();
    if (status == BLE_STATUS_SUCCESS)
    {
      ANCProfile_Write_MainStateMachine(ANC_START_ADV_IN_LOW_PWR_MODE);
    }
    else
    {
      ANC_DBG_MSG(profiledbgfile,"Set Non Discoverable failed\n");
      ANCProfile_Write_MainStateMachine(ANC_INITIALIZED);
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&alertNotificationClient.ANCtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
    }
  }
}

/**
 * ANC_Exit
 * 
 * This function unregisters the profile with
 * the main profile and resets the state machine
 */ 
void ANC_Exit(uint8_t errCode)
{
  ANC_DBG_MSG(profiledbgfile,"ANC_Exit");
	
  /* the requirement for the alert notification client is
   * not met. Notify the application and unregister the profile
   */
  alertNotificationClient.applcb(EVT_ANC_DISCOVERY_CMPLT,1,&errCode);
  BLE_Profile_Unregister_Profile(&alertNotificationClient.ANCtoBLEInf);
  ANCProfile_Write_MainStateMachine(ANC_UNINITIALIZED);
}

