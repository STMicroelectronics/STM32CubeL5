/**
  ******************************************************************************
  * @file    hci_tl_template.c 
  * @author  AMG RF Application Team
  * @brief   Function for managing framework required for handling HCI interface.
  ******************************************************************************
  *
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  */ 
#include "hci_const.h"
#include "hci.h"
#include "hci_tl.h"

static tHciContext    hciContext;

void hci_init(void(* UserEvtRx)(void* pData), void* pConf)
{
  /* USER CODE BEGIN hci_init 1 */
  
  /* USER CODE END hci_init 1 */
  
  if(UserEvtRx != NULL)
  {
    hciContext.UserEvtRx = UserEvtRx;
  }
  
  /* Initialize TL BLE layer */
  hci_tl_lowlevel_init();
  
  /* USER CODE BEGIN hci_init 2 */
  
  /* USER CODE END hci_init 2 */
  
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
  /* USER CODE BEGIN hci_send_req */
  
  /* USER CODE END hci_send_req */
  
  return 0;
}
 
void hci_user_evt_proc(void)
{
  /* USER CODE BEGIN hci_user_evt_proc */
  
  /* USER CODE END hci_user_evt_proc */
}

int32_t hci_notify_asynch_evt(void* pdata)
{
  /* USER CODE BEGIN hci_notify_asynch_evt */
  
  /* USER CODE END hci_notify_asynch_evt */
  
  return 0;
}

void hci_resume_flow(void)
{
  /* USER CODE BEGIN hci_resume_flow */
  
  /* USER CODE END hci_resume_flow */
}

void hci_cmd_resp_wait(uint32_t timeout)
{
  /* USER CODE BEGIN hci_cmd_resp_wait */
  
  /* USER CODE END hci_cmd_resp_wait */
}

void hci_cmd_resp_release(uint32_t flag)
{
  /* USER CODE BEGIN hci_cmd_resp_release */
  
  /* USER CODE END hci_cmd_resp_release */
}
