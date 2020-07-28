/**
  ******************************************************************************
  * @file    hci_tl.c 
  * @author  AMG RF Application Team
  * @version V1.0.0
  * @date    18-Jan-2018
  * @brief   Contains the basic functions for managing the framework required
  *          for handling the HCI interface
  ******************************************************************************
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics</center></h2>
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/ 
#include "hci_const.h"
#include "hci.h"
#include "hci_tl.h"

#define HCI_LOG_ON                      0
#define HCI_PCK_TYPE_OFFSET             0
#define EVENT_PARAMETER_TOT_LEN_OFFSET  2

/**
 * Increase this parameter to overcome possible issues due to BLE devices crowded environment 
 * or high number of incoming notifications from peripheral devices 
 */
#define HCI_READ_PACKET_NUM_MAX 	   (5)

#define MIN(a,b)      ((a) < (b))? (a) : (b)
#define MAX(a,b)      ((a) > (b))? (a) : (b)

tListNode             hciReadPktPool;
tListNode             hciReadPktRxQueue;
static tHciDataPacket hciReadPacketBuffer[HCI_READ_PACKET_NUM_MAX];
static tHciContext    hciContext;

/************************* Static internal functions **************************/

/**
  * @brief  Verify the packet type.
  *
  * @param  hciReadPacket The HCI data packet
  * @retval 0: valid packet, 1: incorrect packet, 2: wrong length (packet truncated or too long)
  */
static int verify_packet(const tHciDataPacket * hciReadPacket)
{
  const uint8_t *hci_pckt = hciReadPacket->dataBuff;
  
  if (hci_pckt[HCI_PCK_TYPE_OFFSET] != HCI_EVENT_PKT)
    return 1; /* Incorrect type */
  
  if (hci_pckt[EVENT_PARAMETER_TOT_LEN_OFFSET] != hciReadPacket->data_len - (1+HCI_EVENT_HDR_SIZE))
    return 2; /* Wrong length (packet truncated or too long) */
  
  return 0;      
}

/**
  * @brief  Send an HCI command.
  *
  * @param  ogf The Opcode Group Field
  * @param  ocf The Opcode Command Field
  * @param  plen The HCI command length
  * @param  param The HCI command parameters
  * @retval None
  */
static void send_cmd(uint16_t ogf, uint16_t ocf, uint8_t plen, void *param)
{
  uint8_t payload[HCI_MAX_PAYLOAD_SIZE];  
  hci_command_hdr hc;
  
  hc.opcode = htobs(cmd_opcode_pack(ogf, ocf));
  hc.plen = plen;

  payload[0] = HCI_COMMAND_PKT;
  BLUENRG_memcpy(payload + 1, &hc, sizeof(hc));
  BLUENRG_memcpy(payload + HCI_HDR_SIZE + HCI_COMMAND_HDR_SIZE, param, plen);
  
  if (hciContext.io.Send)
  {
    hciContext.io.Send (payload, HCI_HDR_SIZE + HCI_COMMAND_HDR_SIZE + plen);
  }
}

/**
  * @brief  Remove the tail from a source list and insert it to the head 
  *         of a destination list.
  *
  * @param  dest_list
  * @param  src_list
  * @retval None
  */
static void move_list(tListNode * dest_list, tListNode * src_list)
{
  pListNode tmp_node;
  
  while (!list_is_empty(src_list))
  {
    list_remove_tail(src_list, &tmp_node);
    list_insert_head(dest_list, tmp_node);
  }
}

/**
  * @brief  Free the HCI event list.
  *
  * @param  None
  * @retval None
  */
static void free_event_list(void)
{
  tHciDataPacket * pckt;
  
  while(list_get_size(&hciReadPktPool) < HCI_READ_PACKET_NUM_MAX/2){
    list_remove_head(&hciReadPktRxQueue, (tListNode **)&pckt);    
    list_insert_tail(&hciReadPktPool, (tListNode *)pckt);
  }
}

/********************** HCI Transport layer functions *****************************/

void hci_init(void(* UserEvtRx)(void* pData), void* pConf)
{
  uint8_t index;
  
  if(UserEvtRx != NULL)
  {
    hciContext.UserEvtRx = UserEvtRx;
  }
  
  /* Initialize TL BLE layer */
  hci_tl_lowlevel_init();
  
  /* Initialize list heads of ready and free hci data packet queues */
  list_init_head(&hciReadPktPool);
  list_init_head(&hciReadPktRxQueue);
  
  /* Initialize the queue of free hci data packets */
  for (index = 0; index < HCI_READ_PACKET_NUM_MAX; index++)
  {
    list_insert_tail(&hciReadPktPool, (tListNode *)&hciReadPacketBuffer[index]);
  } 
  
  /* Initialize low level driver */
  if (hciContext.io.Init)  hciContext.io.Init(NULL);
  if (hciContext.io.Reset) hciContext.io.Reset();
}

void hci_register_io_bus(tHciIO* fops)
{
  /* Register bus function */
  hciContext.io.Init    = fops->Init; 
  hciContext.io.Receive = fops->Receive;  
  hciContext.io.Send    = fops->Send;
  hciContext.io.GetTick = fops->GetTick;
  hciContext.io.Reset   = fops->Reset;    
}

int hci_send_req(struct hci_request* r, BOOL async)
{
  uint8_t *ptr;
  uint16_t opcode = htobs(cmd_opcode_pack(r->ogf, r->ocf));
  hci_event_pckt *event_pckt;
  hci_uart_pckt *hci_hdr;

  tHciDataPacket * hciReadPacket = NULL;
  tListNode hciTempQueue;
  
  list_init_head(&hciTempQueue);

  free_event_list();
  
  send_cmd(r->ogf, r->ocf, r->clen, r->cparam);
  
  if (async)
  {
    return 0;
  }
  
  while (1) 
  {
    evt_cmd_complete  *cc;
    evt_cmd_status    *cs;
    evt_le_meta_event *me;
    uint32_t len;
    
    uint32_t tickstart = HAL_GetTick();
      
    while (1)
    {
      if ((HAL_GetTick() - tickstart) > HCI_DEFAULT_TIMEOUT_MS)
      {
        goto failed;
      }
      
      if (!list_is_empty(&hciReadPktRxQueue)) 
      {
        break;
      }
    }
    
    /* Extract packet from HCI event queue. */
    list_remove_head(&hciReadPktRxQueue, (tListNode **)&hciReadPacket);    
    
    hci_hdr = (void *)hciReadPacket->dataBuff;

    if (hci_hdr->type == HCI_EVENT_PKT)
    {
      event_pckt = (void *)(hci_hdr->data);
    
      ptr = hciReadPacket->dataBuff + (1 + HCI_EVENT_HDR_SIZE);
      len = hciReadPacket->data_len - (1 + HCI_EVENT_HDR_SIZE);
    
      switch (event_pckt->evt) 
      {      
      case EVT_CMD_STATUS:
        cs = (void *) ptr;
        
        if (cs->opcode != opcode)
          goto failed;
        
        if (r->event != EVT_CMD_STATUS) {
          if (cs->status) {
            goto failed;
          }
          break;
        }

        r->rlen = MIN(len, r->rlen);
        BLUENRG_memcpy(r->rparam, ptr, r->rlen);
        goto done;
      
      case EVT_CMD_COMPLETE:
        cc = (void *) ptr;
      
        if (cc->opcode != opcode)
          goto failed;
      
        ptr += EVT_CMD_COMPLETE_SIZE;
        len -= EVT_CMD_COMPLETE_SIZE;
      
        r->rlen = MIN(len, r->rlen);
        BLUENRG_memcpy(r->rparam, ptr, r->rlen);
        goto done;
      
      case EVT_LE_META_EVENT:
        me = (void *) ptr;
      
        if (me->subevent != r->event)
          break;
      
        len -= 1;
        r->rlen = MIN(len, r->rlen);
        BLUENRG_memcpy(r->rparam, me->data, r->rlen);
        goto done;
      
      case EVT_HARDWARE_ERROR:            
        goto failed;
      
      default:      
        break;
      }
    }
    
    /* If there are no more packets to be processed, be sure there is at list one
       packet in the pool to process the expected event.
       If no free packets are available, discard the processed event and insert it
       into the pool. */
    if (list_is_empty(&hciReadPktPool) && list_is_empty(&hciReadPktRxQueue)) {
      list_insert_tail(&hciReadPktPool, (tListNode *)hciReadPacket);
      hciReadPacket=NULL;
    }
    else {
      /* Insert the packet in a different queue. These packets will be
      inserted back in the main queue just before exiting from send_req(), so that
      these events can be processed by the application.
    */
    list_insert_tail(&hciTempQueue, (tListNode *)hciReadPacket);
      hciReadPacket=NULL;
    }
  }
  
failed: 
  if (hciReadPacket!=NULL) {
    list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket);
  }
  move_list(&hciReadPktRxQueue, &hciTempQueue);  
  return -1;
  
done:
  /* Insert the packet back into the pool.*/
  list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket); 
  move_list(&hciReadPktRxQueue, &hciTempQueue);
  return 0;
}

void hci_user_evt_proc(void)
{
  tHciDataPacket * hciReadPacket = NULL;
     
  /* process any pending events read */
  while (list_is_empty(&hciReadPktRxQueue) == FALSE)
  {
    list_remove_head (&hciReadPktRxQueue, (tListNode **)&hciReadPacket);
    if (hciContext.UserEvtRx != NULL)
    {
      hciContext.UserEvtRx(hciReadPacket->dataBuff);
    }
    list_insert_tail(&hciReadPktPool, (tListNode *)hciReadPacket);
  }
}

int32_t hci_notify_asynch_evt(void* pdata)
{
  tHciDataPacket * hciReadPacket = NULL;
  uint8_t data_len;
  
  int32_t ret = 0;
  
  if (list_is_empty (&hciReadPktPool) == FALSE)
  {
    /* Queuing a packet to read */
    list_remove_head (&hciReadPktPool, (tListNode **)&hciReadPacket);
    
    if (hciContext.io.Receive)
    {
      data_len = hciContext.io.Receive(hciReadPacket->dataBuff, HCI_READ_PACKET_SIZE);
      if (data_len > 0)
      {                    
        hciReadPacket->data_len = data_len;
        if (verify_packet(hciReadPacket) == 0)
          list_insert_tail(&hciReadPktRxQueue, (tListNode *)hciReadPacket);
        else
          list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket);          
      }
      else 
      {
        /* Insert the packet back into the pool*/
        list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket);
      }
    }
  }
  else 
  {
    ret = 1;
  }
  return ret;

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
