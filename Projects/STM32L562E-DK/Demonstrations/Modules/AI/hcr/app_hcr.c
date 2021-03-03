/**
******************************************************************************
* @file    app_hcr.c
* @author  AST Embedded Analytics Research Platform
* @date    7.9.2018
* @brief   Handwritten character recognition demonstrator
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

#define __APP_HCR_C

#include "main.h"
#include "app_hcr.h"
#include "app_x-cube-ai.h"

static int aiInit(void);
static int aiBootstrap(const char *nn_name, const int idx);

static logoDisplay logoTab[] = {
  { "STAI", (uint8_t*)stm32cubeai565_bmp, 30, 40},
  { "CALL", (uint8_t*)phone565_bmp, 72, 72 },
  { "PLAY", (uint8_t*)play565_bmp, 72, 72 },
  { "MAIL", (uint8_t*)mail565_bmp, 72, 72 },
  { "CHAT", (uint8_t*)chat565_bmp, 72, 72 },
  LOGO_END,
};

static uint16_t g_patch565[PATCH_SIZE*PATCH_SIZE];

AI_ALIGNED(4)
static ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

#if USE_UART_DEBUG
static UART_HandleTypeDef g_UartHandle;
static COM_InitTypeDef g_ComHandle;
#endif

/* -----------------------------------------------------------------------------
 * AI-related functions
 * -----------------------------------------------------------------------------
 */

struct network_exec_ctx {
    ai_handle handle;
    ai_network_report report;
} net_exec_ctx[AI_MNETWORK_NUMBER] = {0};

#define AI_BUFFER_NULL(ptr_)  \
  AI_BUFFER_OBJ_INIT( \
    AI_BUFFER_FORMAT_NONE|AI_BUFFER_FMT_FLAG_CONST, \
    0, 0, 0, 0, \
    AI_HANDLE_PTR(ptr_))


#if defined(AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE)
#if AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE != 0
AI_ALIGNED(4)
static ai_u8 activations[AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE];
#else
AI_ALIGNED(4)
static ai_u8 activations[1];
#endif
#else
AI_ALIGNED(4)
static ai_u8 activations[AI_MNETWORK_DATA_ACTIVATIONS_SIZE];
#endif

AI_ALIGNED(4)
static ai_i8 in_data[AI_MNETWORK_IN_1_SIZE_BYTES];

AI_ALIGNED(4)
static ai_i8 out_data[AI_MNETWORK_OUT_1_SIZE_BYTES];

static int aiBootstrap(const char *nn_name, const int idx)
{
    ai_error err;
    ai_u32 ext_addr,sz;

    /* Creating the network */
    err = ai_mnetwork_create(nn_name, &net_exec_ctx[idx].handle, NULL);
    if (err.type) {
        return -1;
    }

    /* Query the created network to get relevant info from it */
    if (!ai_mnetwork_get_info(net_exec_ctx[idx].handle, &net_exec_ctx[idx].report)) {
        err = ai_mnetwork_get_error(net_exec_ctx[idx].handle);
        ai_mnetwork_destroy(net_exec_ctx[idx].handle);
        net_exec_ctx[idx].handle = AI_HANDLE_NULL;
        return -2;
    }

    /* Initialize the instance */

    /* build params structure to provide the reference of the
     * activation and weight buffers */
    ai_network_params params = {
                AI_BUFFER_NULL(NULL),
                AI_BUFFER_NULL(NULL) };

    if (ai_mnetwork_get_ext_data_activations(net_exec_ctx[idx].handle, &ext_addr, &sz) == 0) {
    	if (ext_addr == 0xFFFFFFFF) {
    		params.activations.data = (ai_handle)activations;
    	}
    	else {
    		params.activations.data = (ai_handle)ext_addr;
    	}
    }

    if (!ai_mnetwork_init(net_exec_ctx[idx].handle, &params)) {
        err = ai_mnetwork_get_error(net_exec_ctx[idx].handle);
        ai_mnetwork_destroy(net_exec_ctx[idx].handle);
        net_exec_ctx[idx].handle = AI_HANDLE_NULL;
        return -4;
    }
    return 0;
}

static int aiInit(void)
{

  const char *nn_name;
    int idx;
    /* Discover and init the embedded network */
    idx = 0;
    do {
    	nn_name = ai_mnetwork_find(NULL, idx);
    	if (nn_name)
        {
    		if (aiBootstrap(nn_name, idx))
    		    return -1;
    	}
    	idx++;
    } while (nn_name);
    return 0;
}

int HCR_demo(void)
{
  TS_State_t           ts_state;
  uint32_t             Pixel;
  uint32_t             touch_time;
  char                 msg[30];
  uint8_t              txt[13];
  uint8_t              txt_i = 0;
  float                max;
  int8_t               imax;
  bool                 data_ready = false;
  char                 prediction;
  bool                 input_is_space = false;
  uint32_t             LcdXsize;
  uint32_t             LcdYsize;
  uint32_t             tstart, tend;
  uint32_t             iter;
  struct               dwtTime t;
  ai_buffer ai_input[1];
  ai_buffer ai_output[1];

  /* Clear screen */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);

  /*Get LCD X and Y size*/
  BSP_LCD_GetXSize(0, &LcdXsize);
  BSP_LCD_GetYSize(0, &LcdYsize);

  /* Init push button for space character */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

#if 0  // PLF
  /*Perform calibration if required*/
  if(TouchScreen_IsCalibrationDone() == 0)
  {
    Touchscreen_Calibration(LcdXsize, LcdYsize);
  }
#endif

#if USE_UART_DEBUG
  /* debug uart*/
  g_ComHandle.BaudRate     = 115200;
  g_ComHandle.WordLength   = UART_WORDLENGTH_8B;
  g_ComHandle.StopBits     = COM_STOPBITS_1;
  g_ComHandle.Parity       = COM_PARITY_NONE;
  g_ComHandle.HwFlowCtl    = COM_HWCONTROL_NONE;
  BSP_COM_Init(COM1,&g_ComHandle);
#endif

  /* gfx setup*/
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(0,215,(uint8_t*)"BACK",RIGHT_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(1,56,(uint8_t*)"DRAW",CENTER_MODE);
  UTIL_LCD_DisplayStringAt(1,96,(uint8_t*)"CHARACTERS",CENTER_MODE);

  /* ai setup: create & init the neural network*/
  aiInit();
  ai_input[0]  = net_exec_ctx[0].report.inputs[0];
  ai_output[0] = net_exec_ctx[0].report.outputs[0];

  ai_input[0].n_batches  = 1;
  ai_input[0].data       = AI_HANDLE_PTR(in_data);
  ai_output[0].n_batches = 1;
  ai_output[0].data      = AI_HANDLE_PTR(out_data);

  touch_time = HAL_GetTick();
  txt[0] = 0;

  /*DWT initialization*/
  dwtIpInit();

  while(1)
  {
    if( BSP_TS_GetState(0,&ts_state) == BSP_ERROR_NONE )
    {
      /* Touch event*/
      if( ts_state.TouchDetected > 0 && data_ready == false )
      {
        /* Manage back area to exit */
        if((ts_state.TouchX > 200) && (ts_state.TouchY > 220))
        {
          /* Exit */
          HAL_NVIC_SystemReset();
        }
        UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
        UTIL_LCD_DisplayStringAt(0,215,(uint8_t*)"BACK",RIGHT_MODE);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        data_ready = true;
      }
      else if( BSP_PB_GetState(BUTTON_USER) == 1 && data_ready==false)
      {
        input_is_space = true;
        while( BSP_PB_GetState(BUTTON_USER) == 1 );
      }

      /* Draw display*/
      for(int i=0;i<ts_state.TouchDetected;i++)
      {
        touch_time = HAL_GetTick();
        UTIL_LCD_FillCircle(ts_state.TouchX,ts_state.TouchY,PEN_POINT_SIZE, UTIL_LCD_COLOR_WHITE);
      }

      if( data_ready && (HAL_GetTick() - touch_time) > TOUCH_TIMEOUT)
      {
        /*retrieve (with downsampling) image from display memory*/
        /*remobe BACK */
        UTIL_LCD_DisplayStringAt(0,215,(uint8_t*)"    ",RIGHT_MODE);
        int ii = 0;
        for(uint16_t y=16;y<LcdYsize;y+=8)
        {
          for(uint16_t x=16;x<LcdXsize;x+=8)
          {
            UTIL_LCD_GetPixel(x,y, &Pixel);
            g_patch565[ii] = Pixel;
            *(ai_float *)(in_data + ii * 4)= (g_patch565[ii] > 0)?1.0F:0.0F;
            ii++;
          }
        }

#if USE_UART_DEBUG
        HAL_UART_Transmit(&g_UartHandle,(uint8_t*)in_data,AI_NETWORK_IN_SIZE*4,10000);
#endif
        UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
        UTIL_LCD_DisplayStringAt(0,215,(uint8_t*)"BACK",RIGHT_MODE);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

        /* Draw mini patch*/
        ii=0;
        for(uint16_t y=0;y<PATCH_SIZE;y+=1)
        {
          for(uint16_t x=0;x<PATCH_SIZE;x+=1)
          {
            UTIL_LCD_SetPixel(x,y,g_patch565[ii++]);
          }
        }

        /* DWT reset*/
        dwtReset();

        /* inference duration measurement*/
        tstart = dwtGetCycles();

        /* run NN*/
        ai_mnetwork_run(net_exec_ctx[0].handle, &ai_input[0], &ai_output[0]);

        /* inference duration measurement*/
        tend = dwtGetCycles() - tstart;

        dwtCyclesToTime(tend, &t);

        /* show results*/
        max = 0;
        imax = -1;
        for(ii=0;ii<AI_NETWORK_OUT_1_SIZE;ii++)
        {
          float tmp  = *(ai_float *) &out_data[4*ii];
          if( tmp > max ) { max = tmp; imax = ii; }
        }
        UTIL_LCD_SetFont(&Font24);
        if(imax >= 0 && max > 0.5F)
        {
          prediction = (imax<10)?imax+48:imax+55;

          sprintf(msg,"= %c",prediction);
          UTIL_LCD_DisplayStringAt(38,4,(uint8_t*)msg,LEFT_MODE);
          UTIL_LCD_SetFont(&Font16);
          // PLF  remove timing as impacted by code execution on external Flash
          // sprintf(msg,"(%d%% %dms)",(int)(max*100),t.ms);
          sprintf(msg,"(%d%%)",(int)(max*100));
          UTIL_LCD_DisplayStringAt(98,8,(uint8_t*)msg,LEFT_MODE);
          txt[txt_i++] = prediction;
          if(txt_i == 13) { txt_i = 1; txt[0] = prediction; }
          txt[txt_i] = 0;
        }
        else
        {
          UTIL_LCD_DisplayStringAt(38,4,(uint8_t*)"= ?",LEFT_MODE);
        }

        iter =0;
        while(logoTab[iter].logo_string)
        {
          if(strlen(logoTab[iter].logo_string)<= txt_i && strcmp((const char*)(txt+(txt_i - strlen(logoTab[iter].logo_string))),logoTab[iter].logo_string)==0)
          {
            txt[0] = 0; txt_i = 0;
            UTIL_LCD_DrawBitmap(logoTab[iter].x_pos,logoTab[iter].y_pos,logoTab[iter].logo_buffer);

            iter=0;

            break;
          }
          iter++;
        }

        if(iter)
        {
          UTIL_LCD_SetFont(&Font24);
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_YELLOW);
          UTIL_LCD_DisplayStringAt(1,120,txt,CENTER_MODE);
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        }

        data_ready = false;
      }
      else if( input_is_space )
      {
        if(!iter)
        {
          UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
        }
        UTIL_LCD_SetFont(&Font24);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
        UTIL_LCD_DisplayStringAt(0,215,(uint8_t*)"BACK",RIGHT_MODE);

        input_is_space = false;
        data_ready = false;
        txt[txt_i++] = 32;
        if(txt_i == 13) { txt_i = 1; txt[0] = 32; }
        txt[txt_i] = 0;
        UTIL_LCD_SetFont(&Font24);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_YELLOW);
        UTIL_LCD_DisplayStringAt(1,120,txt,CENTER_MODE);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
      }
    }
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
