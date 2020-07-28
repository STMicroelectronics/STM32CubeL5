/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  timer.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      
*   $Revision$:  first version
*   $Author$:    
*   Comments: 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file have implementation of Timer task to be used for timeout 
* functionality. 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <ble_list.h>
#include <timer.h>
#include "stm32xx_timerserver.h"

/******************************************************************************
 * Function Definitions 
******************************************************************************/
tBleStatus Blue_NRG_Timer_Init(void)
{
  // FIXME: implement me
  return (BLE_STATUS_SUCCESS);
}

tBleStatus Blue_NRG_Timer_Start(uint32_t expiryTime,
                                TIMER_TIMEOUT_NOTIFY_CALLBACK_TYPE timercb,
                                tTimerID *timerID)
{
  TIMER_Create(eTimerModuleID_BlueNRG_Profile, timerID, eTimerMode_SingleShot, (pf_TIMER_TimerCallBack_t) timercb);
  TIMER_Start(*timerID, expiryTime*1000000/TIMERSERVER_TICK_VALUE);
  return (BLE_STATUS_SUCCESS);
}					   

tBleStatus Blue_NRG_Timer_Stop(tTimerID timerID)
{
  TIMER_Delete(timerID);
  return (BLE_STATUS_SUCCESS);
}

void Blue_NRG_Timer_Process_Q(void)
{
  // FIXME: implement me
}

void Blue_NRG_Timer_ExpiryCallback(void)
{
  // FIXME: implement me
}


#if 0
/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  timer.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      
*   $Revision$:  first version
*   $Author$:    
*   Comments: 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file have implementation of Timer task to be used for timeout 
* functionality. 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include <hal_types.h>
#include <debug.h>
#include <ble_status.h>
#include <ble_list.h>
#include <timer.h>
#include <hal.h>
#include <osal.h>
/******************************************************************************
 * Macro Declarations
******************************************************************************/
#define TIMER_MAX_COUNT			(0xFFFF)/* 16 bit counter */
#define TIMER_THRESH			(100) /* 100 milliseconds */
/* this macro should be set to the maximum numer of 
 * milliseconds for which the timer can be started
 */ 
#define MAX_MS_TIMEOUT          (30000)
/* Timer status codes */
#define TIMER_STOPPED			(0x00)
#define TIMER_RUNNING			(0x01)
#define TIMER_EXPIRED			(0x02)

#define TIME_DIFF(a, b) ((a-b)&TIMER_MAX_COUNT)
/******************************************************************************
 * Local Function Prototypes
******************************************************************************/
void Stop_Timer(void);
void Start_Timer(long int expTime);
void Blue_NRG_Timer_ExpiryCallback(void);
uint32_t Calculate_Timer_Expiry(uint32_t msTimeout);
/******************************************************************************
 * Local Variable Declarations
******************************************************************************/

/******************************************************************************
 * Global Variable Declarations
******************************************************************************/
uint8_t timerTaskInitFlag = 0;
tTimerContext timerContext;

/******************************************************************************
 * Imported Variable 
******************************************************************************/
extern void * profiledbgfile;

/******************************************************************************
 * Function Definitions 
******************************************************************************/
static void Start_Next_Timer()
{
  tListNode     *node;
  uint32_t	minMSTimeout = 0;
 
 TIMER_MESG_DBG(profiledbgfile,"Start_Next_Timer\n");
 
  if(!list_is_empty(&timerContext.timerQ))
  {
    node = (&timerContext.timerQ)->next;
    minMSTimeout = ((tTimerData*)node)->expiryTime;
    while(node != &timerContext.timerQ)
    {
      if(minMSTimeout > ((tTimerData *)(node))->expiryTime )
      {
        minMSTimeout = ((tTimerData *)(node))->expiryTime ;
      }
      node = node->next;
    }
   
   Start_Timer(Calculate_Timer_Expiry(minMSTimeout));
  }
  else
  {
    TIMER_MESG_DBG(profiledbgfile,"list empty\n");
    /* list is empty. so disable the
    * timer
    */ 
    Stop_Timer();
  }
}

void Blue_NRG_Timer_Process_Q()
{
	tListNode *node = NULL;
	tListNode *remNode[MAX_TIMER_ENTRIES];
	uint8_t idx = 0;
	uint8_t removeNode = 0;
	tTimerData* timerNode;
	
	if(timerContext.timerNeeded == TIMER_EXPIRED)
	{
		TIMER_MESG_DBG(profiledbgfile,"Blue_NRG_Timer_Process_Q");
		
		/* search if there are any other expired timer nodes */
		if(!list_is_empty(&timerContext.timerQ))
		{
			TIMER_MESG_DBG(profiledbgfile,"list not empty\n");
			
			node = timerContext.timerQ.next;
			
			while(node != &timerContext.timerQ)
			{
			  timerNode = (tTimerData *)(node);
			  
			  if(timerNode->expiryTime == 0)
			  {
				TIMER_MESG_DBG(profiledbgfile,"timer expired %d\n",timerNode->timerID);
				removeNode++;
				remNode[idx++] = node;
				/* call the callback */
				((tTimerData*)node)->timercb();
			  }
				
			  node = node->next;
			}
			/* if the timer for the node has expired, then remove it */
			idx = 0;
			while(removeNode--)
			{
			  TIMER_MESG_DBG(profiledbgfile,"removed timer node\n");
			  list_remove_node(remNode[idx]);
			   /* release the node back to the pool */
			  list_insert_tail(&timerContext.timerPool,remNode[idx]);
			  idx++;
			}
			Start_Next_Timer();
		}
	}
}

tBleStatus Blue_NRG_Timer_Init(void)
{
	uint8_t idx;
	
    TIMER_MESG_DBG(profiledbgfile,"Blue_NRG_Timer_Init\n");
    
    if (timerTaskInitFlag)
    {
        return (ERR_COMMAND_DISALLOWED);
    }
    
    timerTaskInitFlag = 0x01;

	/* initialize the timer context */
	list_init_head(&timerContext.timerQ);
	list_init_head(&timerContext.timerPool);

	for(idx=0; idx<MAX_TIMER_ENTRIES; idx++)
	{
		timerContext.timerData[idx].timerID = idx;
		list_insert_tail(&timerContext.timerPool,(tListNode*)&timerContext.timerData[idx]);
	}
	timerContext.timerNeeded = TIMER_STOPPED;

    return (BLE_STATUS_SUCCESS);
}

/**
 * Blue_NRG_Timer_Start
 * 
 * @param[in] expiryTime: duration of the timer in seconds
 * @param[in] timercb   : callback funtion to be called on expiry
 * 
 * When a timer needs to be started, the current expiry time
 * is compared with the new expiry time received. If the expiry time is
 * earlier than the current expiry time, then the current expiry
 * time is set to the new value received and the older expiry is
 * put back into the timerQ
 * 
 * @return returns success if the timer is successfully started or
 * enqueued or error codes are returned
 */ 
tBleStatus Blue_NRG_Timer_Start(uint32_t expiryTime,
					   TIMER_TIMEOUT_NOTIFY_CALLBACK_TYPE timercb,
					   tTimerID *timerID)
{
	tBleStatus retVal = BLE_STATUS_SUCCESS;
	tTimerData* timerNode;
	uint32_t curtime;
	uint32_t timeDiff;
	tListNode *node; 
	
	TIMER_MESG_DBG(profiledbgfile,"Blue_NRG_Timer_Start %d\n",expiryTime);
    if ((expiryTime == 0) || 
		(timercb == NULL))
    {
        return BLE_STATUS_INVALID_PARAMS;
    }
    
	node = (&timerContext.timerQ)->next;
	if(!list_is_empty(&timerContext.timerPool))
	{
		/* get a free node */
		list_remove_head(&timerContext.timerPool,(tListNode**)&timerNode);
		/* fill in the node */
		timerNode->expiryTime = (expiryTime*1000);
		curtime = Osal_Get_Cur_Time();
		 TIMER_MESG_DBG(profiledbgfile,"[start]%ld\n",timerNode->expiryTime);
		 TIMER_MESG_DBG(profiledbgfile,"[curtime]%ld\n",curtime);
		timerNode->timercb = timercb;
		*timerID = timerNode->timerID;
		TIMER_MESG_DBG(profiledbgfile,"[timerID]%x\n",*timerID);
		if(timerContext.timerNeeded == TIMER_RUNNING)
		{
			timeDiff = TIME_DIFF(timerContext.expiryTime,curtime);
			TIMER_MESG_DBG(profiledbgfile,"[timeDiff]%ld\n",timeDiff);
			if(timeDiff >= timerNode->expiryTime)
			{
			  Stop_Timer();

			  while(node != &timerContext.timerQ)
			  {
				((tTimerData *)(node))->expiryTime += timeDiff;
				 node = node->next;
			  }
			  
			  list_insert_tail(&timerContext.timerQ,(tListNode*)timerNode);
			  
			  Start_Timer(Calculate_Timer_Expiry(timerNode->expiryTime));
			}
			else
			{
			  list_insert_tail(&timerContext.timerQ,(tListNode*)timerNode);
			  timerNode->expiryTime -= timeDiff;

			}
		}
		else
		{
			TIMER_MESG_DBG(profiledbgfile,"starting timer\n");
			list_insert_tail(&timerContext.timerQ,(tListNode*)timerNode);//adding the new node 
            Start_Timer(Calculate_Timer_Expiry(timerNode->expiryTime));
			timerContext.timerNeeded = TIMER_RUNNING;
		}
	}
	else
	{
		TIMER_MESG_DBG ( profiledbgfile, "No free nodes in timer pool\n" );
		retVal = TIMER_INSUFFICIENT_RESOURCES;
	}

	return retVal;
}


tBleStatus Blue_NRG_Timer_Stop(tTimerID timerID)
{
	tListNode *pCurr = NULL;
	
	TIMER_MESG_DBG(profiledbgfile,"Blue_NRG_Timer_Stop\n");
	
	if(timerID > MAX_TIMER_ENTRIES)
	{
		/* this is not a valid index */
		return BLE_STATUS_FAILED;
	}
    
	if(timerContext.timerNeeded == TIMER_RUNNING)
	{
		/* if the Q is not empty, then find the timer specified */
		if(!list_is_empty(&timerContext.timerQ))
		{
			pCurr = timerContext.timerQ.next;
		
			while(pCurr != &timerContext.timerQ)
			{
				if(((tTimerData*)pCurr)->timerID == timerID)
				{
					TIMER_MESG_DBG(profiledbgfile,"removing node\n");
					/* remove the node from the timerQ */
					list_remove_node(pCurr);
					/* release the node to pool */
					list_insert_tail(&timerContext.timerPool,pCurr);
					/* there is already a timer that is currently running
					 * so no need to try to start another timer
					 */ 
					 break;
				}
				pCurr = pCurr->next;
			}
			if(list_is_empty(&timerContext.timerQ))
			{
				Stop_Timer();
			}
		}
	}

	return (BLE_STATUS_SUCCESS);
}

static uint32_t Calculate_Timer_Expiry(uint32_t msTimeout)
{
  uint32_t expTime = 0;
  uint32_t subTimeout = 0;
  tListNode *node; 
  node = (&timerContext.timerQ)->next;
  
  if(msTimeout < MAX_MS_TIMEOUT)
  {
    subTimeout = msTimeout;
  }
  else
  {
    subTimeout = MAX_MS_TIMEOUT;
  }
  //loop to traverse the timer Q
  while(node != &timerContext.timerQ)
  {
     ((tTimerData *)(node))->expiryTime -= subTimeout; 
      node = node->next;
  }
  
  expTime = ((Osal_Get_Cur_Time() + subTimeout) % TIMER_MAX_COUNT);
  TIMER_MESG_DBG(profiledbgfile,"subTimeout %d expTime %d\n",subTimeout,expTime);
  return expTime;
}

void Blue_NRG_Timer_ExpiryCallback()
{
	/* a timer has expired */
	timerContext.timerNeeded = TIMER_EXPIRED;
}

static void Start_Timer(long int expTime)
{
	/* load the register with the
	 * specified value expTime
	 */
    Hal_Start_Timer(expTime);
    timerContext.timerNeeded = TIMER_RUNNING;
    timerContext.expiryTime = expTime;
}

void Stop_Timer()
{
	/* write to the register to stop the running
	 * timer
	 */
    Hal_Stop_Timer();
    timerContext.timerNeeded = TIMER_STOPPED;
}
#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
