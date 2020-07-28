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
*   FILENAME        -  phoneAlert_client.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*
*   $Date$:      28/07/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Phone Alert Client Profile  according to unified
*                BlueNRG DK framework
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  code for the phone alert status client profile
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
#include <phone_alert_client.h>

/******************************************************************************
* Macros
*****************************************************************************/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
                (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
                (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))

/* PAC states */
#define PAC_UNINITIALIZED				(0x00)
#define PAC_INIT			         	(0x01)
#define PAC_IDLE			    		(0x02)
#define PAC_ADV_IN_FAST_DISC_MODE			(0x03)
#define PAC_START_ADV_IN_LOW_PWR_MODE                   (0x04)
#define PAC_ADV_IN_LOW_PWR_MODE				(0x05)
#define PAC_DISCOVER_SERVICE				(0x06)
#define PAC_DISC_ALL_CHARACS				(0x07)
#define PAC_DISC_DESC_OF_ALERT_STATUS			(0x08)
#define PAC_DISC_DESC_OF_RINGER_SETTING			(0x09)
#define PAC_ENABLE_NOTIFICATION_FOR_ALERT_STATUS	(0x0A)
#define PAC_ENABLE_NOTIFICATION_FOR_RINGER_SETTING	(0x0B)
#define PAC_CONNECTED					(0x0C)
#define PAC_CONNECTED_IDLE				(0x0D)
#define PAC_READ_ALERT_STATUS				(0x0E)
#define PAC_READ_RINGER_SETTING				(0x0F)

/* advertising intervals in terms of 625 micro seconds */
#define PAC_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define PAC_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define PAC_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define PAC_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/******************************************************************************
* types and globals
*****************************************************************************/
typedef struct _tPhoneClientContext
{
	/**
	 * the current state of the
	 * phone alert status client profile
	 */ 
	uint8_t pacState;
    
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
	 * phone alert status service once
	 * found
	 */ 
	uint16_t phoneAlertServHandle;
	
	/**
	 * store the end handle of the
	 * phone alert status service once
	 * found
	 */ 
	uint16_t phoneAlertServEndHandle;
	
	/**
	 * store the handle of the
	 * alert status characteristic once
	 * found
	 */ 
	uint16_t alertStatusCharHandle;
	
	/**
	 * store the handle of the
	 * ringer control point characteristic once
	 * found
	 */ 
	uint16_t ringerCntrlPntCharHandle;
	
	/**
	 * store the handle of the
	 * ringer setting characteristic once
	 * found
	 */ 
	uint16_t ringerSettingCharHandle;
	
	/**
	 * store the handle of the
	 * client configuration descriptor of 
	 * alert status characteristic once
	 * found
	 */ 
	uint16_t alertStatusCharDescHandle;
	
	/**
	 * store the handle of the
	 * client configuration descriptor of 
	 * ringer setting characteristic once
	 * found
	 */ 
	uint16_t ringerSettingCharDescHandle;
	
	/**
	 * internal structure used for
	 * registering the phone alert client
	 * with the ble profile
	 */ 
	tProfileInterfaceContext PACtoBLEInf;
	
	/**
	 * application callback
	 * to be supplied by the application
	 * at the time of init
	 */ 
	BLE_CALLBACK_FUNCTION_TYPE applcb;

}tPhoneClient;

tPhoneClient phoneAlertClient;

extern void *profiledbgfile;

/******************************************************************************
* Function Prototypes
*****************************************************************************/
void PAC_Event_Handler(void *pckt);
tBleStatus PACProfile_StateMachine(void);
tBleStatus PAC_Add_Device_To_WhiteList(uint8_t* bdAddr);
void Enable_Notification(uint16_t deschandle);
void PAC_LimDisc_Timeout_Handler(void);

static void PACProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState PACProfile_Read_MainStateMachine(void);

/******************************************************************************
* Functions
*****************************************************************************/
static void PACProfile_Write_MainStateMachine(tProfileState localmainState)
{
  phoneAlertClient.pacState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState PACProfile_Read_MainStateMachine(void)
{
  return(phoneAlertClient.pacState);
}

/**
 * PAC_SetState
 * 
 * @param[in] state : the new state of the PAC
 * 
 * Changes the state of the PAC state machine atomically
 * as the states can be changed in the interrupt context 
 * as well as from the application
 */ 
void PAC_SetState(uint8_t state)
{
  PACProfile_Write_MainStateMachine(state);
  
  PAC_DBG_MSG(profiledbgfile,"PAC_SetState %x\n",PACProfile_Read_MainStateMachine());
}

/**
 * PAC_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 *
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB() 
 */ 
void PAC_Event_Handler(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  PAC_DBG_MSG(profiledbgfile,"recv evt %x\n", event_pckt->evt);
	
  switch(event_pckt->evt)
  {
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      PAC_DBG_MSG(profiledbgfile,"EVT_LE_META_EVENT\n");
      
      switch(evt->subevent)
      {
      case EVT_LE_CONN_COMPLETE:
	{
	  evt_le_connection_complete *cc = (void *)evt->data;
	  
	  if (cc->status == BLE_STATUS_SUCCESS)
	    {
	    phoneAlertClient.connHandle = cc->handle;
	    /* we are connected */
	    PAC_SetState(PAC_DISCOVER_SERVICE);
	    PAC_DBG_MSG(profiledbgfile,"connected %d\n",phoneAlertClient.connHandle);
	    /* start the procedure to discover all the services on the peer */
	    if (aci_gatt_disc_all_prim_services(phoneAlertClient.connHandle) == BLE_STATUS_SUCCESS)
	    {
	      PAC_DBG_MSG(profiledbgfile,"Discovery all primary service\n");
	    } 
	    else 
	    {
	      PAC_DBG_MSG(profiledbgfile,"Discovery all primary service failed\n");
	    } 
	  }
	  else
	  {
	    /* there was an error in connection
	     * so wait for the application to put
	     * into discoverable mode
	     */
	    PAC_SetState(PAC_IDLE);
	  }
	}
	break;
      }  
    }
    break;
  case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *dc = (void *)event_pckt->data;
      
      PAC_DBG_MSG(profiledbgfile,"EVT_DISCONN_COMPLETE\n");
      if (dc->status == BLE_STATUS_SUCCESS)
      {
	PAC_SetState(PAC_IDLE);
	PAC_DBG_MSG(profiledbgfile,"disconnected\n");
	/* we have to put the device into advertising mode 
	 * According to the spec, the profile has to try
	 * for reconnection once it is disconnected
	 */
	PAC_Advertize();
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
	  if((PACProfile_Read_MainStateMachine() == PAC_ADV_IN_FAST_DISC_MODE) || (PACProfile_Read_MainStateMachine() == PAC_ADV_IN_LOW_PWR_MODE))
	  {
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State(&phoneAlertClient.PACtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	    
	    if(PACProfile_Read_MainStateMachine() == PAC_ADV_IN_LOW_PWR_MODE)
	    { 
	      PAC_SetState(PAC_IDLE);
	      /* notify the application */
	      phoneAlertClient.applcb(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL);
	    }
	  }
	}
	break;
      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
	{
	  evt_att_read_by_group_resp *pr = (void*)blue_evt->data;
	  uint8_t numServ, i, idx;
	  uint16_t uuid;
	  
	  if (PACProfile_Read_MainStateMachine() == PAC_DISCOVER_SERVICE)
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
		if(uuid == PHONE_ALERT_SERVICE_UUID)
		{
		  PAC_DBG_MSG(profiledbgfile,"********************Found phone alert service\n");
		  phoneAlertClient.phoneAlertServHandle = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx-4]);
		  phoneAlertClient.phoneAlertServEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->attribute_data_list[idx-2]);
		  PAC_DBG_MSG(profiledbgfile,"********************Phone alert service Start Handle 0x%02x\n", 
			      phoneAlertClient.phoneAlertServHandle);
		  PAC_DBG_MSG(profiledbgfile,"********************Phone alert service End Handle 0x%02x\n", 
			      phoneAlertClient.phoneAlertServEndHandle);
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
	  uint16_t uuid;

	  if (PACProfile_Read_MainStateMachine() == PAC_DISC_ALL_CHARACS)
	  {
	    PAC_DBG_MSG(profiledbgfile,"EVT_BLUE_ATT_READ_BY_TYPE_RESP len %d\n", pr->event_data_length);
				
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
		uuid = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx]);
		if(uuid == PHONE_ALERT_STATUS_CHARAC_UUID)
		{
		  PAC_DBG_MSG(profiledbgfile,"****************PHONE ALERT STATUS CHARAC ***********************\n");
		  PAC_DBG_MSG(profiledbgfile,"********************Phone Alert Status Charac Attr Handle 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  PAC_DBG_MSG(profiledbgfile,"********************Phone Alert Status Charac Properties 0x%02x\n", 
			      pr->handle_value_pair[idx-3]);
		  PAC_DBG_MSG(profiledbgfile,"********************Phone Alert Status Charac Value Handle 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]));
		  PAC_DBG_MSG(profiledbgfile,"********************Phone Alert Status Charac UUID 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx]));
		  phoneAlertClient.alertStatusCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
		}
		else if(uuid == RINGER_CNTRL_POINT_CHARAC_UUID)
		{
		  PAC_DBG_MSG(profiledbgfile,"****************RINGER CONTROL POINT CHARAC**********************\n");
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Control Point Charac Attr Handle 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Control Point Charac Properties 0x%02x\n", 
			      pr->handle_value_pair[idx-3]);
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Control Point Charac Value Handle 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]));
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Control Point Charac UUID 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx]));
		  phoneAlertClient.ringerCntrlPntCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
		}
		else if(uuid == RINGER_SETTING_CHARAC_UUID)
		{
		  PAC_DBG_MSG(profiledbgfile,"****************RINGER SETTING CHARAC**********************\n");
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Setting Charac Attr Handle 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-5]));
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Setting Charac Properties 0x%02x\n", 
			      pr->handle_value_pair[idx-3]);
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Setting Charac Value Handle 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]));
		  PAC_DBG_MSG(profiledbgfile,"********************Ringer Setting Charac UUID 0x%04x\n", 
			      UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx]));
		  phoneAlertClient.ringerSettingCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
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
	  uint16_t uuid;

	  /*
	   * event data will be of the format
	   * 2 bytes handle
	   * 2 bytes UUID
	   */ 
	  if((PACProfile_Read_MainStateMachine() == PAC_DISC_DESC_OF_ALERT_STATUS) ||
	     (PACProfile_Read_MainStateMachine() == PAC_DISC_DESC_OF_RINGER_SETTING))
	  {
	    numDesc = (pr->event_data_length - 1) / 4;
	    /* we are interested only in 16 bit UUIDs */
	    idx = 2;
	    if (pr->format == UUID_TYPE_16) 
	    {
	      for (i=0; i<numDesc; i++)
	      {
		uuid = UNPACK_2_BYTE_PARAMETER(&pr->handle_uuid_pair[idx]);
		if (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID)
		{
		  PAC_DBG_MSG(profiledbgfile, "******************Client configuration descriptor***************\n");
		  PAC_DBG_MSG(profiledbgfile, "******************Client configuration UUID 0x%04x\n", uuid);
		  if ((PACProfile_Read_MainStateMachine() == PAC_DISC_DESC_OF_ALERT_STATUS) && !phoneAlertClient.alertStatusCharDescHandle)
		  {
		    phoneAlertClient.alertStatusCharDescHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_uuid_pair[idx-2]);
		    PAC_DBG_MSG(profiledbgfile, "******************Client configuration Char Desc Handle 0x%04x\n", 
				phoneAlertClient.alertStatusCharDescHandle);
		  }
		  else if (PACProfile_Read_MainStateMachine() == PAC_DISC_DESC_OF_RINGER_SETTING)
		  {
		    phoneAlertClient.ringerSettingCharDescHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_uuid_pair[idx-2]);
		    PAC_DBG_MSG(profiledbgfile, "******************Client configuration Char Desc Handle 0x%04x\n", 
				phoneAlertClient.ringerSettingCharDescHandle);
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
	  
	  PAC_DBG_MSG(profiledbgfile,"read response\n");
	  if(PACProfile_Read_MainStateMachine() == PAC_READ_ALERT_STATUS)
	  {
	    PAC_DBG_MSG(profiledbgfile,"***********read response for alert status Value 0x%02x\n", pr->attribute_value[0]);
	    PAC_SetState(PAC_CONNECTED);
	    phoneAlertClient.applcb(EVT_PAC_ALERT_STATUS,1, &pr->attribute_value[0]);
	  }
	  else if(PACProfile_Read_MainStateMachine() == PAC_READ_RINGER_SETTING)
	  {
	    PAC_DBG_MSG(profiledbgfile,"*********read response for ringer setting Value 0x%02x\n", pr->attribute_value[0]);
	    PAC_SetState(PAC_CONNECTED);
	    phoneAlertClient.applcb(EVT_PAC_RINGER_SETTING,1,&pr->attribute_value[0]);
	  }
	}
	break;
      case EVT_BLUE_GATT_NOTIFICATION:
	{
	  evt_gatt_attr_notification *pr = (void*)blue_evt->data;

	  PAC_DBG_MSG(profiledbgfile,"notif handle %04x \n", pr->attr_handle);
			
	  if(PACProfile_Read_MainStateMachine() != PAC_INIT)
	  {
	    /* the first 2 bytes of the event data contain the handle and the
	     * next byte contains the new value 
	     */ 
	    if(pr->attr_handle == phoneAlertClient.alertStatusCharHandle)
	    {
	      PAC_DBG_MSG(profiledbgfile,"****************received notification handle 0x%04x Value 0x%02x\n", 
			  pr->attr_handle, pr->attr_value[0]);
	      phoneAlertClient.applcb(EVT_PAC_ALERT_STATUS,1,&pr->attr_value[0]);
	    }
	    else if(pr->attr_handle == phoneAlertClient.ringerSettingCharHandle)
	    {
	      PAC_DBG_MSG(profiledbgfile,"****received notification handle 0x%04x Value 0x%02x\n",
			  pr->attr_handle, pr->attr_value[0]);
	      phoneAlertClient.applcb(EVT_PAC_RINGER_SETTING,1, &pr->attr_value[0]);
	    }
	  }		
	}
	break;
      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
	{
	  uint8_t status;

	  if (PACProfile_Read_MainStateMachine() == PAC_DISCOVER_SERVICE)
	  {
	    if(phoneAlertClient.phoneAlertServHandle)
	    {
	      /* we have found the immediate alert service and now
	       * we need to find the alert characteristic
	       */
	      if (aci_gatt_disc_all_charac_of_serv(phoneAlertClient.connHandle,
					       phoneAlertClient.phoneAlertServHandle,
						   phoneAlertClient.phoneAlertServEndHandle) != BLE_STATUS_SUCCESS)
	      {
		/* ??? TODO : what should be done here? */
		PAC_DBG_MSG(profiledbgfile,"Discovery all Charac of Service call failed ???\n");
	      }
	      PAC_SetState(PAC_DISC_ALL_CHARACS);
	    }
	    else
	    {
	      PAC_DBG_MSG(profiledbgfile,"Immediate alert service not found\n");
	      status = PHONE_ALERT_SERVICE_NOT_FOUND;
	      phoneAlertClient.applcb(EVT_PAC_DISCOVERY_CMPLT,1,&status);
	    }
	  }
	  else if (PACProfile_Read_MainStateMachine() == PAC_DISC_ALL_CHARACS)
	  {
	    if(phoneAlertClient.alertStatusCharHandle &&
	       phoneAlertClient.ringerCntrlPntCharHandle &&
	       phoneAlertClient.ringerSettingCharHandle)
	    {
	      /* we have found all the characteritics. now find the descriptors */		
	      if (aci_gatt_disc_all_charac_descriptors(phoneAlertClient.connHandle, phoneAlertClient.alertStatusCharHandle, 
						       phoneAlertClient.phoneAlertServEndHandle) == BLE_STATUS_SUCCESS)
	      {
		PAC_SetState(PAC_DISC_DESC_OF_ALERT_STATUS);
	      }
	      else
	      {
		/* ??? TODO : what should be done here? */
		PAC_DBG_MSG(profiledbgfile,"Discovery all Charac Descriptors call failed ???\n");
	      }
	    }
	    else
	    {
	      PAC_DBG_MSG(profiledbgfile,"characs not found\n");
	   
	      /* we cannot write in this state */
	      PAC_SetState(PAC_CONNECTED_IDLE);
	      if(!phoneAlertClient.alertStatusCharHandle)
	      {
		status = PHONE_ALERT_STATUS_CHARAC_NOT_FOUND;
		phoneAlertClient.applcb(EVT_PAC_DISCOVERY_CMPLT,1,&status);
	      }
	      else if(!phoneAlertClient.ringerCntrlPntCharHandle)
	      {
		status = RINGER_CNTRL_POINT_CHARAC_NOT_FOUND;
		phoneAlertClient.applcb(EVT_PAC_DISCOVERY_CMPLT,1,&status);
	      }
	      else
	      {
		status = RINGER_SETTING_CHARAC_NOT_FOUND;
		phoneAlertClient.applcb(EVT_PAC_DISCOVERY_CMPLT,1,&status);
	      }
	    }
	  }
	  else if(PACProfile_Read_MainStateMachine() == PAC_DISC_DESC_OF_ALERT_STATUS)
	  {
	    if(phoneAlertClient.alertStatusCharDescHandle)
	    {
	      PAC_DBG_MSG(profiledbgfile,"found alert desc\n");		
	      status = aci_gatt_disc_all_charac_descriptors(phoneAlertClient.connHandle,
							    phoneAlertClient.ringerSettingCharHandle, 
							    phoneAlertClient.phoneAlertServEndHandle);
	      if (status != BLE_STATUS_SUCCESS)
	      {
		/* ??? TODO : what should be done here? */
		PAC_DBG_MSG(profiledbgfile,"Discovery all Charac Descriptors call failed ???\n");
	      }
	      PAC_SetState(PAC_DISC_DESC_OF_RINGER_SETTING);
	    }
	    else
	    {
	      /* we did not find the descriptor so inform the application */
	      PAC_DBG_MSG(profiledbgfile,"desc not found\n");
	      
	      /* we cannot write in this state */
	      PAC_SetState(PAC_CONNECTED_IDLE);
	      status = PHONE_ALERT_STATUS_DESC_NOT_FOUND;
	      phoneAlertClient.applcb(EVT_PAC_DISCOVERY_CMPLT,1,&status);
	    }
	  }
	  else if(PACProfile_Read_MainStateMachine() == PAC_DISC_DESC_OF_RINGER_SETTING)
	  {
	    if(phoneAlertClient.ringerSettingCharDescHandle)
	    {
	      PAC_DBG_MSG(profiledbgfile,"found ringer setting desc\n");		
	      Enable_Notification(phoneAlertClient.alertStatusCharDescHandle);
	      PAC_SetState(PAC_ENABLE_NOTIFICATION_FOR_ALERT_STATUS);
	    }
	    else
	    {
	      /* we did not find the descriptor so inform the application */
	      PAC_DBG_MSG(profiledbgfile,"desc not found\n");
	      
	      /* we cannot write in this state */
	      PAC_SetState(PAC_CONNECTED_IDLE);
	      status = RINGER_SETTING_DESC_NOT_FOUND;
	      phoneAlertClient.applcb(EVT_PAC_DISCOVERY_CMPLT,1,&status);
	    }
	  }
	  else if(PACProfile_Read_MainStateMachine() == PAC_ENABLE_NOTIFICATION_FOR_ALERT_STATUS)
	  {
	    Enable_Notification(phoneAlertClient.ringerSettingCharDescHandle);
	    PAC_SetState(PAC_ENABLE_NOTIFICATION_FOR_RINGER_SETTING);
	  }
	  else if(PACProfile_Read_MainStateMachine() == PAC_ENABLE_NOTIFICATION_FOR_RINGER_SETTING)
	  {
	    PAC_SetState(PAC_CONNECTED);
	    status = BLE_STATUS_SUCCESS;
	    phoneAlertClient.applcb(EVT_PAC_DISCOVERY_CMPLT,1,&status);
	  }
	}
	break;
      }
      break;
    }
  }
}

/**
 * Enable_Notification
 * 
 * @param[in] deschandle : handle of the descriptor for
 *            which the notification has to be enabled
 * 
 * starts a GATT procedure to enable notification 
 * on the peer device
 */ 
void Enable_Notification(uint16_t deschandle)
{
  uint8_t value[] = {0x01,0x00};
  uint8_t status;
  
  PAC_DBG_MSG(profiledbgfile,"Enable_Notification\n");
  status = aci_gatt_write_charac_descriptor(phoneAlertClient.connHandle, deschandle, 2, value);
  if (status != BLE_STATUS_SUCCESS)
    PAC_DBG_MSG(profiledbgfile,"Enable_Notification failed\n");
}

/**
 * Disable_Notification
 * 
 * @param[in] deschandle : handle of the descriptor for
 *            which the notification has to be disabled
 * 
 * starts a GATT procedure to disable notification 
 * on the peer device
 */ 
void Disable_Notification(uint16_t deschandle)
{
  uint8_t value[] = {0x00,0x00};
  uint8_t status;
  
  PAC_DBG_MSG(profiledbgfile,"Disable_Notification\n");
  status = aci_gatt_write_charac_descriptor(phoneAlertClient.connHandle, deschandle, 2, value);
  if (status != BLE_STATUS_SUCCESS)
    PAC_DBG_MSG(profiledbgfile,"Disable_Notification failed\n");
}

/**
 * PAC_Disable_ALert_Status_Notification
 * 
 * @param[in] None
 * 
 * Disable the Alert Status Notification
 */ 
void PAC_Disable_ALert_Status_Notification()
{
  PAC_DBG_MSG(profiledbgfile,"PAC_Disable_ALert_Status_Notification\n");
  Disable_Notification(phoneAlertClient.alertStatusCharDescHandle);
}

/**
 * PAC_Disable_Ringer_Status_Notification
 * 
 * @param[in] None
 * 
 * Disable the Ringer Status Notification
 */ 
void PAC_Disable_Ringer_Status_Notification()
{
  PAC_DBG_MSG(profiledbgfile,"PAC_Disable_Ringer_Status_Notification\n");
  Disable_Notification(phoneAlertClient.ringerSettingCharDescHandle);
}

/**
 * PACProfile_StateMachine
 * 
 * @param[in] None
 * 
 * PAC profile's state machine: to be called on application main loop.
 */ 
tBleStatus PACProfile_StateMachine(void)
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'P','h','o','n','e','A','l','e','r','t','C','l','i','e','n','t'};
  static uint8_t mainState = PAC_UNINITIALIZED;
	
  if (mainState != PACProfile_Read_MainStateMachine())
  {
    PAC_DBG_MSG(profiledbgfile,"[cmplt]%x",PACProfile_Read_MainStateMachine());
    mainState = PACProfile_Read_MainStateMachine();
  }
	
  switch(PACProfile_Read_MainStateMachine())
  {
  case PAC_UNINITIALIZED:
    {
      if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
	PAC_DBG_MSG(profiledbgfile,"vote for advertising\n");
	BLE_Profile_Vote_For_Advertisable_State (&phoneAlertClient.PACtoBLEInf);
	PAC_SetState(PAC_INIT);
      }
    }
    break;
  case PAC_INIT:
    {
      if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
	PAC_DBG_MSG(profiledbgfile,"initialized\n");
	status = BLE_STATUS_SUCCESS;
	phoneAlertClient.applcb(EVT_PAC_INITIALIZED,1,&status);
	PAC_SetState(PAC_IDLE);
      }
    }
    break;
  case PAC_START_ADV_IN_LOW_PWR_MODE:
    {
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&phoneAlertClient.PACtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
      
      /* put the device in low power discoverable mode */
      status = aci_gap_set_limited_discoverable(ADV_IND, 
						PAC_LOW_PWR_ADV_INTERVAL_MIN, 
						PAC_LOW_PWR_ADV_INTERVAL_MAX,
						PUBLIC_ADDR, 
						NO_WHITE_LIST_USE, // WHITE_LIST_FOR_ALL. Set to NO_WHITE_LIST_USE x running PTS PASP tests
						sizeof(name),
						name, 
						0,
						NULL,
						0, 
						0);
      if(status != BLE_STATUS_SUCCESS)
      {
	phoneAlertClient.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
	PAC_SetState(PAC_IDLE);
      }
      else
      {
	PAC_SetState(PAC_ADV_IN_LOW_PWR_MODE);
	PAC_DBG_MSG(profiledbgfile,"enabled advertising\n");
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&phoneAlertClient.PACtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
      }
    }
    break;
  }
  return status;
}

/**
 * PAC_LimDisc_Timeout_Handler
 * 
 * This function handles the timeout for
 * the limited discoverable mode
 */ 
void PAC_LimDisc_Timeout_Handler()
{	
  uint8_t status;

  Blue_NRG_Timer_Stop(phoneAlertClient.timerID);
  phoneAlertClient.timerID = 0xFF;
  if(PACProfile_Read_MainStateMachine() == PAC_ADV_IN_FAST_DISC_MODE)
  {
    /* stop advertizing */
    status = aci_gap_set_non_discoverable();
    if (status == BLE_STATUS_SUCCESS)
    {
      PAC_SetState(PAC_START_ADV_IN_LOW_PWR_MODE);
    }
    else
    {
      PAC_DBG_MSG(profiledbgfile,"Set Non Discoverable failed\n");
      PAC_SetState(PAC_IDLE);
    }
  }
}

/******************************************************************************
* SAP
*****************************************************************************/
tBleStatus PAC_Init(BLE_CALLBACK_FUNCTION_TYPE phoneAlertClientcb)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  PAC_DBG_MSG(profiledbgfile,"Phone Alert Client Init");
  if (phoneAlertClientcb == NULL)
  {
    return BLE_STATUS_NULL_PARAM;
  }
	
  BLUENRG_memset(&phoneAlertClient,0,sizeof(phoneAlertClient));
  
  /* initialize the profile context */
  phoneAlertClient.applcb = phoneAlertClientcb;
  phoneAlertClient.timerID = 0xFF;
	
  /* Initialize Phone Client Profile to BLE main Profile Interface data structure */
  phoneAlertClient.PACtoBLEInf.profileID = PID_PHONE_ALERT_CLIENT;
  phoneAlertClient.PACtoBLEInf.callback_func = PAC_Event_Handler;
  phoneAlertClient.PACtoBLEInf.voteForAdvertisableState = 0;
	
  retval = BLE_Profile_Register_Profile(&phoneAlertClient.PACtoBLEInf);
  if (retval == BLE_STATUS_SUCCESS)
  {
    PACProfile_Write_MainStateMachine(PAC_UNINITIALIZED);
  }
	
  return (retval);
}

tBleStatus PAC_Add_Device_To_WhiteList(uint8_t* bdAddr)
{
  /* add the device to whitelist */
  PAC_DBG_MSG(profiledbgfile,"PAC_Add_Device_To_WhiteList\n");
  hci_le_add_device_to_white_list(0x00, bdAddr);
	
  return BLE_STATUS_SUCCESS;
}

tBleStatus PAC_Advertize()
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'P','h','o','n','e','A','l','e','r','t','C','l','i','e','n','t'};
	
  PAC_DBG_MSG(profiledbgfile,"PAC_Advertize\n");
  
  if(PACProfile_Read_MainStateMachine() == PAC_IDLE)
  {
    status = aci_gap_set_limited_discoverable(ADV_IND, 
					      PAC_FAST_CONN_ADV_INTERVAL_MIN, 
					      PAC_FAST_CONN_ADV_INTERVAL_MAX,
					      PUBLIC_ADDR, 
					      NO_WHITE_LIST_USE, // WHITE_LIST_FOR_ALL. Set to NO_WHITE_LIST_USE x running PTS PASP tests
					      sizeof(name),
					      name, 
					      0,
					      NULL,
					      0, 
					      0);

    if(status == BLE_STATUS_SUCCESS)
    {
      PAC_SetState(PAC_ADV_IN_FAST_DISC_MODE);
      PAC_DBG_MSG(profiledbgfile,"enabled advertising\n");
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&phoneAlertClient.PACtoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
      /* we are in the limited discoverable mode.
       * now start a timer for 30 seconds. The advertising
       * intervals have to be changed on expiry of this
       * timer
       */
      Blue_NRG_Timer_Start(BLE_FAST_CONN_ADV_TIMEOUT,
		  PAC_LimDisc_Timeout_Handler,
		  &phoneAlertClient.timerID);
    } 
    else 
    {
      PAC_SetState(PAC_IDLE);
      phoneAlertClient.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);

    }
    
  }
  
  return status;
}

/**
 * PAC_Configure_Ringer
 * 
 * @param[in] ringerMode : the ringer mode to be set\n
 *            SILENT_MODE	    (0x01)\n
 *            MUTE_ONCE   	    (0x02)\n
 *            CANCEL_SILENT_MODE    (0x03)
 * 
 * Starts a write without response GATT procedure
 * to write the ringer mode command to the phone alert server
 * 
 * @return returns BLE_STATUS_SUCCESS if the parameters
 * are valid and the procedure has been started else
 * returns error codes
 */ 
tBleStatus PAC_Configure_Ringer(uint8_t ringerMode)
{
  if(PACProfile_Read_MainStateMachine() == PAC_CONNECTED)
  {
    if((ringerMode != SILENT_MODE) &&
       (ringerMode != CANCEL_SILENT_MODE) &&
       (ringerMode != MUTE_ONCE))
    {
      return BLE_STATUS_INVALID_PARAMS;
    }

    return aci_gatt_write_without_response(phoneAlertClient.connHandle, phoneAlertClient.ringerCntrlPntCharHandle, 
					       1, &ringerMode);
  }
  return ERR_COMMAND_DISALLOWED;
}

/**
 * PAC_Read_AlertStatus
 * 
 * When this function is called by the application,
 * the profile starts a gatt procedure to read the
 * characteristic value. The value read will be retuned
 * via the event to the application.
 * 
 * @return returns BLE_STATUS_SUCCESS if the procedure
 *         was started successfully
 */ 
tBleStatus PAC_Read_AlertStatus()
{
  PAC_DBG_MSG(profiledbgfile,"PAC_Read_AlertStatus\n");
  if(PACProfile_Read_MainStateMachine() == PAC_CONNECTED)
  {
    PAC_SetState(PAC_READ_ALERT_STATUS);

    return aci_gatt_read_charac_val(phoneAlertClient.connHandle, phoneAlertClient.alertStatusCharHandle);
  }
	
  return ERR_COMMAND_DISALLOWED;
}

/**
 * PAC_Read_RingerSetting
 * 
 * When this function is called by the application,
 * the profile starts a gatt procedure to read the
 * characteristic value. The value read will be returned
 * via the event to the application.
 * 
 * @return returns BLE_STATUS_SUCCESS if the procedure
 *         was started successfully
 */ 
tBleStatus PAC_Read_RingerSetting()
{
  if(PACProfile_Read_MainStateMachine() == PAC_CONNECTED)
  {
    PAC_SetState(PAC_READ_RINGER_SETTING);
    
    return aci_gatt_read_charac_val(phoneAlertClient.connHandle, phoneAlertClient.ringerSettingCharHandle);
  }
  
  return ERR_COMMAND_DISALLOWED;
}
