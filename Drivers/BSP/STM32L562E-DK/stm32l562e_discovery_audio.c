/**
  ******************************************************************************
  * @file    stm32l562e_discovery_audio.c
  * @author  MCD Application Team
  * @brief   This file provides the Audio driver for the STM32L562E-DISCOVERY
  *          discovery board.
  @verbatim
  How To use this driver:
  -----------------------
   + This driver supports STM32L5xx devices on STM32L562E-DISCOVERY (MB1373) board.
   + Call the function BSP_AUDIO_OUT_Init() for AUDIO OUT initialization: 
        Instance : Select the output instance. Can only be 0 (SAI).
        AudioInit: Audio Out structure to select the following parameters.
                   - Device: Select the output device (headphone, speaker, ..).
                   - SampleRate: Select the output sample rate (8Khz .. 96Khz).
                   - BitsPerSample: Select the output resolution (16 or 32bits per sample).
                   - ChannelsNbr: Select the output channels number(1 for mono, 2 for stereo).
                   - Volume: Select the output volume(0% .. 100%).
                                     
      This function configures all the hardware required for the audio application (codec, I2C, SAI, 
      GPIOs, DMA and interrupt if needed). This function returns BSP_ERROR_NONE if configuration is OK.
      If the returned value is different from BSP_ERROR_NONE or the function is stuck then the communication with
      the codec has failed (try to un-plug the power or reset device in this case).

      User can update the SAI or the clock configurations by overriding the weak MX functions MX_SAI1_Init()
      and MX_SAI1_ClockConfig().
      User can override the default MSP configuration and register his own MSP callbacks (defined at application level)
      by calling BSP_AUDIO_OUT_RegisterMspCallbacks() function.
      User can restore the default MSP configuration by calling BSP_AUDIO_OUT_RegisterDefaultMspCallbacks().
      To use these two functions, user has to enable USE_HAL_SAI_REGISTER_CALLBACKS within stm32l5xx_hal_conf.h file.

   + Call the function BSP_AUDIO_OUT_Play() to play audio stream:
        Instance : Select the output instance. Can only be 0 (SAI).
        pBuf: pointer to the audio data file address.
        NbrOfBytes: Total size of the buffer to be sent in Bytes.

   + Call the function BSP_AUDIO_OUT_Pause() to pause playing.
   + Call the function BSP_AUDIO_OUT_Resume() to resume playing.
       Note. After calling BSP_AUDIO_OUT_Pause() function for pause, only BSP_AUDIO_OUT_Resume() should be called
          for resume (it is not allowed to call BSP_AUDIO_OUT_Play() in this case).
       Note. This function should be called only when the audio file is played or paused (not stopped).
   + Call the function BSP_AUDIO_OUT_Stop() to stop playing.
   + Call the function BSP_AUDIO_OUT_Mute() to mute the player.
   + Call the function BSP_AUDIO_OUT_UnMute() to unmute the player.
   + Call the function BSP_AUDIO_OUT_IsMute() to get the mute state(BSP_AUDIO_MUTE_ENABLED or BSP_AUDIO_MUTE_DISABLED).
   + Call the function BSP_AUDIO_OUT_SetDevice() to update the AUDIO OUT device.
   + Call the function BSP_AUDIO_OUT_GetDevice() to get the AUDIO OUT device.
   + Call the function BSP_AUDIO_OUT_SetSampleRate() to update the AUDIO OUT sample rate.
   + Call the function BSP_AUDIO_OUT_GetSampleRate() to get the AUDIO OUT sample rate.
   + Call the function BSP_AUDIO_OUT_SetBitsPerSample() to update the AUDIO OUT resolution.
   + Call the function BSP_AUDIO_OUT_GetBitPerSample() to get the AUDIO OUT resolution.
   + Call the function BSP_AUDIO_OUT_SetChannelsNbr() to update the AUDIO OUT number of channels.
   + Call the function BSP_AUDIO_OUT_GetChannelsNbr() to get the AUDIO OUT number of channels.
   + Call the function BSP_AUDIO_OUT_SetVolume() to update the AUDIO OUT volume.
   + Call the function BSP_AUDIO_OUT_GetVolume() to get the AUDIO OUT volume.
   + Call the function BSP_AUDIO_OUT_GetState() to get the AUDIO OUT state.

   + BSP_AUDIO_OUT_SetDevice(), BSP_AUDIO_OUT_SetSampleRate(), BSP_AUDIO_OUT_SetBitsPerSample() and
     BSP_AUDIO_OUT_SetChannelsNbr() cannot be called while the state is AUDIO_OUT_STATE_PLAYING.
   + For each mode, you may need to implement the relative callback functions into your code.
      The Callback functions are named AUDIO_OUT_XXX_CallBack() and only their prototypes are declared in 
      the stm32l562e_discovery_audio.h file. (refer to the example for more details on the callbacks implementations).


   + Call the function BSP_AUDIO_IN_Init() for AUDIO IN initialization: 
        Instance : Select the input instance. Can be 0 (SAI) or 1 (DFSDM).
        AudioInit: Audio In structure to select the following parameters.
                   - Device: Select the input device (analog, digital mic1, mic2, mic1 & mic2).
                   - SampleRate: Select the input sample rate (8Khz .. 96Khz).
                   - BitsPerSample: Select the input resolution (16 or 32bits per sample).
                   - ChannelsNbr: Select the input channels number(1 for mono, 2 for stereo).
                   - Volume: Select the input volume(0% .. 100%).
                                     
      This function configures all the hardware required for the audio application (codec, I2C, SAI, DFSDM 
      GPIOs, DMA and interrupt if needed). This function returns BSP_ERROR_NONE if configuration is OK.
      If the returned value is different from BSP_ERROR_NONE or the function is stuck then the communication with
      the codec has failed (try to un-plug the power or reset device in this case).

      User can update the DFSDM/SAI or the clock configurations by overriding the weak MX functions MX_SAI1_Init(),
      MX_SAI1_ClockConfig(), MX_DFSDM1_Init() and MX_DFDSM1_ClockConfig()
      User can override the default MSP configuration and register his own MSP callbacks (defined at application level)
      by calling BSP_AUDIO_IN_RegisterMspCallbacks() function.
      User can restore the default MSP configuration by calling BSP_AUDIO_IN_RegisterDefaultMspCallbacks().
      To use these two functions, user have to enable USE_HAL_SAI_REGISTER_CALLBACKS and/or USE_HAL_DFSDM_REGISTER_CALLBACKS 
      within stm32l5xx_hal_conf.h file.

   + Call the function BSP_AUDIO_IN_Record() to record audio stream. The recorded data are stored to user buffer in raw
        (L, R, L, R ...).
        Instance : Select the input instance. Can be 0 (SAI) or 1 (DFSDM).
        pBuf: pointer to user buffer.
        NbrOfBytes: Total size of the buffer to be sent in Bytes.

   + Call the function BSP_AUDIO_IN_Pause() to pause recording.
   + Call the function BSP_AUDIO_IN_Resume() to resume recording.
   + Call the function BSP_AUDIO_IN_Stop() to stop recording.
   + Call the function BSP_AUDIO_IN_SetDevice() to update the AUDIO IN device.
   + Call the function BSP_AUDIO_IN_GetDevice() to get the AUDIO IN device.
   + Call the function BSP_AUDIO_IN_SetSampleRate() to update the AUDIO IN sample rate.
   + Call the function BSP_AUDIO_IN_GetSampleRate() to get the AUDIO IN sample rate.
   + Call the function BSP_AUDIO_IN_SetBitPerSample() to update the AUDIO IN resolution.
   + Call the function BSP_AUDIO_IN_GetBitPerSample() to get the AUDIO IN resolution.
   + Call the function BSP_AUDIO_IN_SetChannelsNbr() to update the AUDIO IN number of channels.
   + Call the function BSP_AUDIO_IN_GetChannelsNbr() to get the AUDIO IN number of channels.
   + Call the function BSP_AUDIO_IN_SetVolume() to update the AUDIO IN volume.
   + Call the function BSP_AUDIO_IN_GetVolume() to get the AUDIO IN volume.
   + Call the function BSP_AUDIO_IN_GetState() to get the AUDIO IN state.

   + For each mode, you may need to implement the relative callback functions into your code.
      The Callback functions are named AUDIO_IN_XXX_CallBack() and only their prototypes are declared in
      the stm32l562e_discovery_audio.h file (refer to the example for more details on the callbacks implementations).

   + The driver API and the callback functions are at the end of the stm32l562e_discovery_audio.h file.

  Known Limitations:
  ------------------
   1- If the TDM Format used to play in parallel 2 audio Stream (the first Stream is configured in codec SLOT0 and second 
      Stream in SLOT1) the Pause/Resume, volume and mute feature will control the both streams.
   2- Parsing of audio file is not implemented (in order to determine audio file properties: Mono/Stereo, Data size, 
      File size, Audio Frequency, Audio Data header size ...). The configuration is fixed for the given audio file.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l562e_discovery_audio.h"
#include "stm32l562e_discovery_bus.h"
    
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L562E-DK
  * @{
  */ 
  
/** @defgroup STM32L562E-DK_AUDIO STM32L562E-DK AUDIO
  * @{
  */ 

/** @defgroup STM32L562E-DK_AUDIO_Private_Macros STM32L562E-DK AUDIO Private Macros
  * @{
  */
#define DFSDM_OVER_SAMPLING(__FREQUENCY__) \
        ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (256U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_11K)) ? (256U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (128U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_22K)) ? (128U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (64U)  \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_44K)) ? (64U)  \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (32U) : (16U)

#define DFSDM_CLOCK_DIVIDER(__FREQUENCY__) \
        ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (24U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_11K)) ? (4U)  \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (24U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_22K)) ? (4U)  \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (24U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_44K)) ? (4U)  \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (32U) : (32U)

#define DFSDM_FILTER_ORDER(__FREQUENCY__) \
        ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (DFSDM_FILTER_SINC3_ORDER) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_11K)) ? (DFSDM_FILTER_SINC3_ORDER) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (DFSDM_FILTER_SINC3_ORDER) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_22K)) ? (DFSDM_FILTER_SINC3_ORDER) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (DFSDM_FILTER_SINC4_ORDER) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_44K)) ? (DFSDM_FILTER_SINC3_ORDER) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (DFSDM_FILTER_SINC4_ORDER) : (DFSDM_FILTER_SINC5_ORDER)

#define DFSDM_MIC_BIT_SHIFT(__FREQUENCY__) \
        ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (5U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_11K)) ? (6U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (3U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_22K)) ? (3U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (6U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_44K)) ? (0U) \
      : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (2U) : (4U)

#define SaturaLH(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))
/**
  * @}
  */ 
  
/** @defgroup STM32L562E-DK_AUDIO_Exported_Variables STM32L562E-DK AUDIO Exported Variables
  * @{
  */
/* Audio in and out context */
AUDIO_OUT_Ctx_t Audio_Out_Ctx[AUDIO_OUT_INSTANCES_NBR] = {{AUDIO_OUT_HEADPHONE,
                                                           AUDIO_FREQUENCY_8K,
                                                           AUDIO_RESOLUTION_16b,
                                                           50U,
                                                           1U,
                                                           AUDIO_MUTE_DISABLED,
                                                           AUDIO_OUT_STATE_RESET}};

AUDIO_IN_Ctx_t  Audio_In_Ctx[AUDIO_IN_INSTANCES_NBR] = {{AUDIO_IN_ANALOG_MIC,
                                                         AUDIO_FREQUENCY_8K,
                                                         AUDIO_RESOLUTION_16b,
                                                         1U,
                                                         NULL,
                                                         0U,
                                                         50U,
                                                         AUDIO_IN_STATE_RESET},
                                                        {AUDIO_IN_DIGITAL_MIC,
                                                         AUDIO_FREQUENCY_8K,
                                                         AUDIO_RESOLUTION_16b,
                                                         2U,
                                                         NULL,
                                                         0U,
                                                         50U,
                                                         AUDIO_IN_STATE_RESET}};

/* Audio component object */
void *Audio_CompObj = NULL;

/* Audio driver */
AUDIO_Drv_t *Audio_Drv = NULL;

/* Audio in and out SAI handles */
SAI_HandleTypeDef haudio_out_sai = {0};
SAI_HandleTypeDef haudio_in_sai  = {0};

/* Audio in DFSDM handles */
DFSDM_Channel_HandleTypeDef haudio_in_dfsdm_channel = {0};
DFSDM_Filter_HandleTypeDef  haudio_in_dfsdm_filter  = {0};

/* Audio in DFSDM internal buffer */
int32_t Audio_DigMicRecBuff[BSP_AUDIO_IN_DEFAULT_BUFFER_SIZE];
/**
  * @}
  */

/** @defgroup STM32L562E-DK_AUDIO_Private_Variables STM32L562E-DK AUDIO Private Variables
  * @{
  */
/* Audio in and out DMA handles used by SAI */
static DMA_HandleTypeDef hDmaSaiTx, hDmaSaiRx;

/* Audio in DMA handle used by DFSDM */
static DMA_HandleTypeDef hDmaDfsdm;        

#if ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1))
static uint32_t AudioOut_IsMspCbValid[AUDIO_OUT_INSTANCES_NBR] = {0};
static uint32_t AudioIn_IsMspCbValid[AUDIO_IN_INSTANCES_NBR] = {0, 0};
#endif
/**
  * @}
  */ 

/** @defgroup STM32L562E-DK_AUDIO_Private_Function_Prototypes STM32L562E-DK AUDIO Private Function Prototypes
  * @{
  */
static int32_t CS42L51_Probe(void);
static void    CS42L51_PowerUp(void);
static void    CS42L51_PowerDown(void);
static void    SAI_MspInit(SAI_HandleTypeDef *hsai);
static void    SAI_MspDeInit(SAI_HandleTypeDef *hsai);
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
static void    SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void    SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void    SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
static void    SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void    SAI_ErrorCallback(SAI_HandleTypeDef *hsai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

static int32_t DFSDM_DeInit(DFSDM_Filter_HandleTypeDef *hDfsdmFilter, DFSDM_Channel_HandleTypeDef *hDfsdmChannel);
static void    DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
static void    DFSDM_ChannelMspDeInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
static void    DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
static void    DFSDM_FilterMspDeInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1)
static void    DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
static void    DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
static void    DFSDM_FilterErrorCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @addtogroup STM32L562E-DK_AUDIO_OUT_Exported_Functions
  * @{
  */
/**
  * @brief  Initialize the audio out peripherals.
  * @param  Instance Audio out instance.
  * @param  AudioInit Audio out init structure.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((AudioInit->BitsPerSample == AUDIO_RESOLUTION_32b) || (AudioInit->BitsPerSample == AUDIO_RESOLUTION_8b)))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 0U) && (Audio_In_Ctx[0].State != AUDIO_IN_STATE_RESET) && 
          ((Audio_In_Ctx[0].SampleRate != AudioInit->SampleRate) || (Audio_In_Ctx[0].BitsPerSample != AudioInit->BitsPerSample)))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 0U) && (Audio_In_Ctx[1].State != AUDIO_IN_STATE_RESET) && 
           (Audio_In_Ctx[1].SampleRate != AudioInit->SampleRate))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Un-reset audio codec if not currently used by audio in instance 0 */
    if (Audio_In_Ctx[0].State == AUDIO_IN_STATE_RESET)
    {
      CS42L51_PowerUp();
    }

    /* Fill audio out context structure */
    Audio_Out_Ctx[Instance].Device         = AudioInit->Device;
    Audio_Out_Ctx[Instance].SampleRate     = AudioInit->SampleRate;
    Audio_Out_Ctx[Instance].BitsPerSample  = AudioInit->BitsPerSample;
    Audio_Out_Ctx[Instance].ChannelsNbr    = AudioInit->ChannelsNbr;
    Audio_Out_Ctx[Instance].Volume         = AudioInit->Volume;

    /* Probe the audio codec */
    if (Audio_In_Ctx[0].State == AUDIO_IN_STATE_RESET)
    {
      if (CS42L51_Probe() != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }

    if (status == BSP_ERROR_NONE)
    {
      /* Set SAI instance */
      haudio_out_sai.Instance = SAI1_Block_A;

      /* Configure the SAI PLL according to the requested audio frequency if not already done by other instances */
      if ((Audio_In_Ctx[0].State == AUDIO_IN_STATE_RESET) && (Audio_In_Ctx[1].State == AUDIO_IN_STATE_RESET))
      {
        if (MX_SAI1_ClockConfig(&haudio_out_sai, AudioInit->SampleRate) != HAL_OK)
        {
          status = BSP_ERROR_CLOCK_FAILURE;
        }
      }

      if (status == BSP_ERROR_NONE)
      {
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 0)
        SAI_MspInit(&haudio_out_sai);
#else
        /* Register the SAI MSP Callbacks */
        if (AudioOut_IsMspCbValid[Instance] == 0U)
        {
          if (BSP_AUDIO_OUT_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }
        }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 0) */
      }

      if (status == BSP_ERROR_NONE)
      {
        /* Prepare SAI peripheral initialization */
        MX_SAI_InitTypeDef mxSaiInit;
        mxSaiInit.AudioFrequency    = AudioInit->SampleRate;
        mxSaiInit.AudioMode         = SAI_MODEMASTER_TX;
        mxSaiInit.ClockStrobing     = SAI_CLOCKSTROBING_FALLINGEDGE;
        mxSaiInit.MonoStereoMode    = (AudioInit->ChannelsNbr == 1U) ? SAI_MONOMODE : SAI_STEREOMODE;
        if (AudioInit->BitsPerSample == AUDIO_RESOLUTION_24b)
        {
          mxSaiInit.DataSize          = SAI_DATASIZE_24;
          mxSaiInit.FrameLength       = 64;
          mxSaiInit.ActiveFrameLength = 32;
        }
        else
        {
          mxSaiInit.DataSize          = SAI_DATASIZE_16;
          mxSaiInit.FrameLength       = 32;
          mxSaiInit.ActiveFrameLength = 16;
        }
        mxSaiInit.OutputDrive       = SAI_OUTPUTDRIVE_ENABLE;
        mxSaiInit.Synchro           = SAI_ASYNCHRONOUS;
        mxSaiInit.SynchroExt        = SAI_SYNCEXT_DISABLE;
        mxSaiInit.SlotActive        = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1;

        /* SAI peripheral initialization */
        if (MX_SAI1_Init(&haudio_out_sai, &mxSaiInit) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)    
        /* Register SAI TC, HT and Error callbacks */
        else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
        else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
        else
        {
          /* Initialize audio codec */
          CS42L51_Init_t codec_init;
          codec_init.InputDevice  = (Audio_In_Ctx[0].State == AUDIO_IN_STATE_RESET) ? CS42L51_IN_NONE : CS42L51_IN_MIC1;
          codec_init.OutputDevice = CS42L51_OUT_HEADPHONE;
          codec_init.Frequency    = AudioInit->SampleRate;
          codec_init.Resolution   = CS42L51_RESOLUTION_16B; /* Not used */
          codec_init.Volume       = AudioInit->Volume;
          if (Audio_Drv->Init(Audio_CompObj, &codec_init) < 0)
          {
            status = BSP_ERROR_COMPONENT_FAILURE;
          }
          else
          {
            /* Update audio out context state */
            Audio_Out_Ctx[Instance].State = AUDIO_OUT_STATE_STOP;
          }
        }
      }
    }
  }
  return status;
}

/**
  * @brief  De-initialize the audio out peripherals.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_DeInit(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_RESET)
  {
    /* Reset audio codec if not currently used by audio in instance 0 */
    if (Audio_In_Ctx[0].State == AUDIO_IN_STATE_RESET)
    {
      CS42L51_PowerDown();
    }

    /* SAI peripheral de-initialization */
    if (HAL_SAI_DeInit(&haudio_out_sai) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    /* De-initialize audio codec if not currently used by audio in instance 0 */
    else
    {
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 0)
      SAI_MspDeInit(&haudio_out_sai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 0) */
      if (Audio_In_Ctx[0].State == AUDIO_IN_STATE_RESET)
      {
        if (Audio_Drv->DeInit(Audio_CompObj) < 0)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
    }

    if (status == BSP_ERROR_NONE)
    {
      /* Update audio out context */
      Audio_Out_Ctx[Instance].State  = AUDIO_OUT_STATE_RESET;
      Audio_Out_Ctx[Instance].IsMute = 0U;
    }
  }
  else
  {
    /* Nothing to do */
  }
  return status;
}

/**
  * @brief  Start playing audio stream from a data buffer for a determined size.
  * @param  Instance Audio out instance.
  * @param  pData Pointer on data buffer.
  * @param  NbrOfBytes Size of buffer in bytes. Maximum size is 65535 bytes.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Play(uint32_t Instance, uint8_t* pData, uint32_t NbrOfBytes)
{
  int32_t  status = BSP_ERROR_NONE;
  uint16_t NbrOfDmaDatas;

  if ((Instance >= AUDIO_OUT_INSTANCES_NBR) || (pData == NULL) || (NbrOfBytes > 65535U))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Compute number of DMA data to transfer according resolution */
    if (Audio_Out_Ctx[Instance].BitsPerSample == AUDIO_RESOLUTION_16b)
    {
      NbrOfDmaDatas = (uint16_t) (NbrOfBytes / 2U);
    }
    else /* AUDIO_RESOLUTION_24b */
    {
      NbrOfDmaDatas = (uint16_t) (NbrOfBytes / 4U);
    }

    /* Initiate a DMA transfer of audio samples towards the serial audio interface */
    if (HAL_SAI_Transmit_DMA(&haudio_out_sai, pData, NbrOfDmaDatas) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    /* Call the audio codec play function */
    else if (Audio_Drv->Play(Audio_CompObj) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Update audio out state */
      Audio_Out_Ctx[Instance].State = AUDIO_OUT_STATE_PLAYING;
    }
  }
  return status;
}

/**
  * @brief  Pause playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Pause(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_PLAYING)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Call the audio codec pause function */
  else if (Audio_Drv->Pause(Audio_CompObj) < 0)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  /* Pause DMA transfer of audio samples towards the serial audio interface */
  else if (HAL_SAI_DMAPause(&haudio_out_sai) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Update audio out state */
    Audio_Out_Ctx[Instance].State = AUDIO_OUT_STATE_PAUSE;
  }
  return status;
}

/**
  * @brief  Resume playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Resume(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_PAUSE)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Call the audio codec resume function */
  else if (Audio_Drv->Resume(Audio_CompObj) < 0)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  /* Resume DMA transfer of audio samples towards the serial audio interface */
  else if (HAL_SAI_DMAResume(&haudio_out_sai) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Update audio out state */
    Audio_Out_Ctx[Instance].State = AUDIO_OUT_STATE_PLAYING;
  }
  return status;
}

/**
  * @brief  Stop playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Stop(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_STOP)
  {
    /* Nothing to do */
  }
  else if ((Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_PLAYING) &&
           (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_PAUSE))
  {
    status = BSP_ERROR_BUSY;
  }
  /* Call the audio codec stop function */
  else if (Audio_Drv->Stop(Audio_CompObj, CS42L51_PDWN_SW) < 0)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  /* Stop DMA transfer of audio samples towards the serial audio interface */
  else if (HAL_SAI_DMAStop(&haudio_out_sai) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Update audio out state */
    Audio_Out_Ctx[Instance].State = AUDIO_OUT_STATE_STOP;
  }
  return status;
}

/**
  * @brief  Mute playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Mute(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Check audio out mute status */
  else if (Audio_Out_Ctx[Instance].IsMute == 1U)
  {
    /* Nothing to do */
  }
  /* Call the audio codec mute function */
  else if (Audio_Drv->SetMute(Audio_CompObj, CS42L51_MUTE_ON) < 0)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Update audio out mute status */
    Audio_Out_Ctx[Instance].IsMute = 1U;
  }
  return status;
}

/**
  * @brief  Unmute playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_UnMute(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Check audio out mute status */
  else if (Audio_Out_Ctx[Instance].IsMute == 0U)
  {
    /* Nothing to do */
  }
  /* Call the audio codec mute function */
  else if (Audio_Drv->SetMute(Audio_CompObj, CS42L51_MUTE_OFF) < 0)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Update audio out mute status */
    Audio_Out_Ctx[Instance].IsMute = 0U;
  }
  return status;
}

/**
  * @brief  Check audio out mute status.
  * @param  Instance Audio out instance.
  * @param  IsMute Pointer to mute status. Value is 1 for mute, 0 for unmute status.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_IsMute(uint32_t Instance, uint32_t *IsMute)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current audio out mute status */
  else
  {
    *IsMute = Audio_Out_Ctx[Instance].IsMute;
  }
  return status;
}

/**
  * @brief  Set audio out volume.
  * @param  Instance Audio out instance.
  * @param  Volume Volume level in percentage from 0% to 100%.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_SetVolume(uint32_t Instance, uint32_t Volume)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= AUDIO_OUT_INSTANCES_NBR) || (Volume > 100U))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Call the audio codec volume control function */
    if (Audio_Drv->SetVolume(Audio_CompObj, VOLUME_OUTPUT, (uint8_t) Volume) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Store volume on audio out context */
      Audio_Out_Ctx[Instance].Volume = Volume;
    }
  }
  return status;
}

/**
  * @brief  Get audio out volume.
  * @param  Instance Audio out instance.
  * @param  Volume Pointer to volume level in percentage from 0% to 100%.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_GetVolume(uint32_t Instance, uint32_t *Volume)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current audio out volume */
  else
  {
    *Volume = Audio_Out_Ctx[Instance].Volume;
  }
  return status;
}

/**
  * @brief  Set audio out sample rate.
  * @param  Instance Audio out instance.
  * @param  SampleRate Sample rate of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_SetSampleRate(uint32_t Instance, uint32_t SampleRate)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Check if record on instance 0 is on going and corresponding sample rate */
  else if ((Audio_In_Ctx[0].State != AUDIO_IN_STATE_RESET) && 
           (Audio_In_Ctx[0].SampleRate != SampleRate))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Check if record on instance 1 is on going and corresponding sample rate */
  else if ((Audio_In_Ctx[1].State != AUDIO_IN_STATE_RESET) && 
           (Audio_In_Ctx[1].SampleRate != SampleRate))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Check if sample rate is modified */
  else if (Audio_Out_Ctx[Instance].SampleRate == SampleRate)
  {
    /* Nothing to do */
  }
  else
  {
    /* Update SAI1 clock config */
    haudio_out_sai.Init.AudioFrequency = SampleRate;
    if (MX_SAI1_ClockConfig(&haudio_out_sai, SampleRate) != HAL_OK)
    {
      status = BSP_ERROR_CLOCK_FAILURE;
    }
    /* Re-initialize SAI1 with new sample rate */
    else if (HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register SAI TC, HT and Error callbacks */
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    /* Store new sample rate on audio out context */
    else
    {
      Audio_Out_Ctx[Instance].SampleRate = SampleRate;
    }
  }
  return status;
}

/**
  * @brief  Get audio out sample rate.
  * @param  Instance Audio out instance.
  * @param  SampleRate Pointer to sample rate of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_GetSampleRate(uint32_t Instance, uint32_t *SampleRate)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current audio out sample rate */
  else
  {
    *SampleRate = Audio_Out_Ctx[Instance].SampleRate;
  }
  return status;
}

/**
  * @brief  Set audio out device.
  * @param  Instance Audio out instance.
  * @param  Device Device of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_SetDevice(uint32_t Instance, uint32_t Device)
{
  int32_t status = BSP_ERROR_NONE;

  UNUSED(Device);

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Nothing to do because there is only one device (AUDIO_OUT_HEADPHONE) */
  }
  return status;
}

/**
  * @brief  Get audio out device.
  * @param  Instance Audio out instance.
  * @param  Device Pointer to device of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_GetDevice(uint32_t Instance, uint32_t *Device)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current audio out device */
  else
  {
    *Device = Audio_Out_Ctx[Instance].Device;
  }
  return status;
}

/**
  * @brief  Set bits per sample for the audio out stream.
  * @param  Instance Audio out instance.
  * @param  BitsPerSample Bits per sample of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((BitsPerSample == AUDIO_RESOLUTION_32b) || (BitsPerSample == AUDIO_RESOLUTION_8b)))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 0U) && (Audio_In_Ctx[0].State != AUDIO_IN_STATE_RESET) && 
           (Audio_In_Ctx[0].BitsPerSample != BitsPerSample))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Store new bits per sample on audio out context */
    Audio_Out_Ctx[Instance].BitsPerSample = BitsPerSample;

    /* Update data size, frame length and active frame length parameters of SAI handle */ 
    if (BitsPerSample == AUDIO_RESOLUTION_24b)
    {
      haudio_out_sai.Init.DataSize               = SAI_DATASIZE_24;
      haudio_out_sai.FrameInit.FrameLength       = 64;
      haudio_out_sai.FrameInit.ActiveFrameLength = 32;
    }
    else
    {
      haudio_out_sai.Init.DataSize               = SAI_DATASIZE_16;
      haudio_out_sai.FrameInit.FrameLength       = 32;
      haudio_out_sai.FrameInit.ActiveFrameLength = 16;
    }

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 0)
    SAI_MspInit(&haudio_out_sai);
#else
    /* Update SAI state only to keep current MSP functions */
    haudio_out_sai.State = HAL_SAI_STATE_RESET;
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 0) */

    /* Re-initialize SAI1 with new parameters */
    if (HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register SAI TC, HT and Error callbacks */
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    else
    {
      /* Nothing to do */
    }
  }
  return status;
}

/**
  * @brief  Get bits per sample for the audio out stream.
  * @param  Instance Audio out instance.
  * @param  BitsPerSample Pointer to bits per sample of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current bits per sample of audio out stream */
  else
  {
    *BitsPerSample = Audio_Out_Ctx[Instance].BitsPerSample;
  }
  return status;
}

/**
  * @brief  Set channels number for the audio out stream.
  * @param  Instance Audio out instance.
  * @param  ChannelNbr Channels number of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Update mono or stereo mode of SAI handle */ 
    haudio_out_sai.Init.MonoStereoMode = (ChannelNbr == 1U) ? SAI_MONOMODE : SAI_STEREOMODE;

    /* Re-initialize SAI1 with new parameter */
    if (HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register SAI TC, HT and Error callbacks */
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    else
    {
      /* Store new channels number on audio out context */
      Audio_Out_Ctx[Instance].ChannelsNbr = ChannelNbr;
    }
  }
  return status;
}

/**
  * @brief  Get channels number for the audio out stream.
  * @param  Instance Audio out instance.
  * @param  ChannelNbr Pointer to channels number of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (Audio_Out_Ctx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current channels number of audio out stream */
  else
  {
    *ChannelNbr = Audio_Out_Ctx[Instance].ChannelsNbr;
  }
  return status;
}

/**
  * @brief  Get current state for the audio out stream.
  * @param  Instance Audio out instance.
  * @param  State Pointer to state of the audio out stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_GetState(uint32_t Instance, uint32_t *State)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Get the current state of audio out stream */
  else
  {
    *State = Audio_Out_Ctx[Instance].State;
  }
  return status;
}

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register default BSP AUDIO OUT msp callbacks.
  * @param  Instance AUDIO OUT Instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_RegisterDefaultMspCallbacks(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit callbacks */
    if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPINIT_CB_ID, SAI_MspInit) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    } 
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPDEINIT_CB_ID, SAI_MspDeInit) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      AudioOut_IsMspCbValid[Instance] = 1U;
    }
  }
  /* Return BSP status */
  return status;
}

/**
  * @brief  Register BSP AUDIO OUT msp callbacks.
  * @param  Instance AUDIO OUT Instance.
  * @param  CallBacks Pointer to MspInit/MspDeInit callback functions.
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_RegisterMspCallbacks(uint32_t Instance, BSP_AUDIO_OUT_Cb_t *CallBacks)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit callbacks */
    if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      AudioOut_IsMspCbValid[Instance] = 1U;
    }
  }
  /* Return BSP status */
  return status; 
}
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Manage the BSP audio out transfer complete event.
  * @param  Instance Audio out instance.
  * @retval None.
  */
__weak void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manage the BSP audio out half transfer complete event.
  * @param  Instance Audio out instance.
  * @retval None.
  */
__weak void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manages the BSP audio out error event.
  * @param  Instance Audio out instance.
  * @retval None.
  */
__weak void BSP_AUDIO_OUT_Error_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  BSP AUDIO OUT interrupt handler.
  * @param  Instance Audio out instance.
  * @param  Device Device of the audio out stream.
  * @retval None.
  */
void BSP_AUDIO_OUT_IRQHandler(uint32_t Instance, uint32_t Device)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
  UNUSED(Device);

  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/**
  * @brief  SAI1 clock Config.
  * @param  hsai SAI handle.
  * @param  SampleRate Audio sample rate used to play the audio stream.
  * @note   The SAI PLL configuration done within this function assumes that
  *         the SAI PLL input is MSI clock and that MSI is already enabled at 4 MHz.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_SAI1_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t SampleRate)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  HAL_StatusTypeDef        status; 
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

  /* Retrieve actual RCC configuration */
  HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

  /* Set the PLL configuration according to the audio frequency */
  if((SampleRate == AUDIO_FREQUENCY_11K) || (SampleRate == AUDIO_FREQUENCY_22K) || (SampleRate == AUDIO_FREQUENCY_44K) ||
     (SampleRate == AUDIO_FREQUENCY_88K) || (SampleRate == AUDIO_FREQUENCY_176K))
  {
    /* SAI1 clock config
    PLLSAI1_VCO = (4 Mhz / PLLSAI1M) * PLLSAI1N = 4 * 48 = VCO_192M
    SAI_CK_x = PLLSAI1_VCO/PLLSAI1P = 192/17 = 11.294 Mhz */
    RCC_ExCLKInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_SAI1;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1Source   = RCC_PLLSOURCE_MSI;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1M        = 1;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1N        = 48;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1P        = 17;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
    RCC_ExCLKInitStruct.Sai1ClockSelection      = RCC_SAI1CLKSOURCE_PLLSAI1;
  }
  else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_32K, AUDIO_FREQUENCY_48K, AUDIO_FREQUENCY_96K or AUDIO_FREQUENCY_192K */
  {
    /* SAI1 clock config
    PLLSAI1_VCO = (4 Mhz / PLLSAI1M) * PLLSAI1N = 4 * 86 = VCO_344M
    SAI_CK_x = PLLSAI1_VCO/PLLSAI1P = 344/7 = 49.142 Mhz */
    RCC_ExCLKInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_SAI1;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1Source   = RCC_PLLSOURCE_MSI;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1M        = 1;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1N        = 86;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1P        = 7;
    RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
    RCC_ExCLKInitStruct.Sai1ClockSelection      = RCC_SAI1CLKSOURCE_PLLSAI1;
  }
  status = HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);

  return status;
}

/**
  * @brief  Initialize SAI1.
  * @param  hsai SAI handle.
  * @param  MXInit SAI configuration structure.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_SAI1_Init(SAI_HandleTypeDef* hsai, MX_SAI_InitTypeDef *MXInit)
{ 
  HAL_StatusTypeDef status = HAL_OK;

  if ((hsai->Instance == SAI1_Block_B) || (hsai->Instance == SAI1_Block_A))
  {
    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(hsai);
  
    /* Configure SAI1_Block_B */
    hsai->Init.MonoStereoMode       = MXInit->MonoStereoMode;
    hsai->Init.AudioFrequency       = MXInit->AudioFrequency;
    hsai->Init.AudioMode            = MXInit->AudioMode;
    hsai->Init.NoDivider            = SAI_MASTERDIVIDER_ENABLE;
    hsai->Init.Protocol             = SAI_FREE_PROTOCOL;
    hsai->Init.DataSize             = MXInit->DataSize;
    hsai->Init.FirstBit             = SAI_FIRSTBIT_MSB;
    hsai->Init.ClockStrobing        = MXInit->ClockStrobing;
    hsai->Init.Synchro              = MXInit->Synchro;
    hsai->Init.OutputDrive          = MXInit->OutputDrive;
    hsai->Init.FIFOThreshold        = SAI_FIFOTHRESHOLD_1QF;
    hsai->Init.SynchroExt           = MXInit->SynchroExt;
    hsai->Init.CompandingMode       = SAI_NOCOMPANDING;
    hsai->Init.TriState             = SAI_OUTPUT_NOTRELEASED;
    hsai->Init.Mckdiv               = 0U;
    hsai->Init.MckOutput            = SAI_MCK_OUTPUT_ENABLE;
    hsai->Init.MckOverSampling      = SAI_MCK_OVERSAMPLING_DISABLE;
    hsai->Init.PdmInit.Activation   = DISABLE;

    /* Configure SAI1_Block_B Frame */
    hsai->FrameInit.FrameLength       = MXInit->FrameLength; 
    hsai->FrameInit.ActiveFrameLength = MXInit->ActiveFrameLength;  
    hsai->FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;
    hsai->FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;
    hsai->FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;

    /* Configure SAI1_Block_B Slot */
    hsai->SlotInit.FirstBitOffset     = 0;
    if ((MXInit->DataSize == AUDIO_RESOLUTION_24b) || (MXInit->DataSize == AUDIO_RESOLUTION_32b))
    {
      hsai->SlotInit.SlotSize         = SAI_SLOTSIZE_32B;
    }
    else
    {
      hsai->SlotInit.SlotSize         = SAI_SLOTSIZE_16B;
    }
    hsai->SlotInit.SlotNumber         = 2;
    hsai->SlotInit.SlotActive         = MXInit->SlotActive;

    if(HAL_SAI_Init(hsai) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }  
  return status;
}
/**
  * @}
  */ 

/** @addtogroup STM32L562E-DK_AUDIO_IN_Exported_Functions
  * @{
  */
/**
  * @brief  Initialize the audio in peripherals.
  * @param  Instance Audio in instance.
  * @param  AudioInit Audio in init structure.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  else if (AudioInit->BitsPerSample != AUDIO_RESOLUTION_16b)
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if (AudioInit->ChannelsNbr != 1U)
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Audio_Out_Ctx[0].State != AUDIO_OUT_STATE_RESET) && (Audio_Out_Ctx[0].SampleRate != AudioInit->SampleRate))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 0U) && (Audio_In_Ctx[1].State != AUDIO_IN_STATE_RESET) && (Audio_In_Ctx[1].SampleRate != AudioInit->SampleRate))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && (Audio_In_Ctx[0].State != AUDIO_IN_STATE_RESET) && (Audio_In_Ctx[0].SampleRate != AudioInit->SampleRate))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 0U) && (Audio_Out_Ctx[0].State != AUDIO_OUT_STATE_RESET) && 
           (Audio_Out_Ctx[0].BitsPerSample != AudioInit->BitsPerSample))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    if (Instance == 0U)
    {
      /* Un-reset audio codec if not currently used by audio out instance 0 */
      if (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET)
      {
        CS42L51_PowerUp();
      }
    
      /* Fill audio in context structure */
      Audio_In_Ctx[Instance].Device         = AudioInit->Device;
      Audio_In_Ctx[Instance].SampleRate     = AudioInit->SampleRate;
      Audio_In_Ctx[Instance].BitsPerSample  = AudioInit->BitsPerSample;
      Audio_In_Ctx[Instance].ChannelsNbr    = AudioInit->ChannelsNbr;
      Audio_In_Ctx[Instance].Volume         = AudioInit->Volume;

      /* Probe the audio codec */
      if (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET)
      {
        if (CS42L51_Probe() != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
      }

      if (status == BSP_ERROR_NONE)
      {
        /* Set SAI instances (SAI1_Block_A needed for MCLK, FS and CLK signals) */
        haudio_in_sai.Instance  = SAI1_Block_B;
        haudio_out_sai.Instance = SAI1_Block_A;

        /* Configure the SAI PLL according to the requested audio frequency if not already done by other instances */
        if ((Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET) && (Audio_In_Ctx[1].State == AUDIO_IN_STATE_RESET))
        {
          if (MX_SAI1_ClockConfig(&haudio_in_sai, AudioInit->SampleRate) != HAL_OK)
          {
            status = BSP_ERROR_CLOCK_FAILURE;
          }
        }

        if (status == BSP_ERROR_NONE)
        {
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 0)
          SAI_MspInit(&haudio_in_sai);
          if (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET)
          {
            SAI_MspInit(&haudio_out_sai);
          }
#else
          /* Register the SAI MSP Callbacks */
          if (AudioIn_IsMspCbValid[Instance] == 0U)
          {
            if (BSP_AUDIO_IN_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
          }
          if (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET)
          {
            if (AudioOut_IsMspCbValid[0] == 0U)
            {
              if (BSP_AUDIO_OUT_RegisterDefaultMspCallbacks(0) != BSP_ERROR_NONE)
              {
                status = BSP_ERROR_PERIPH_FAILURE;
              }
            }
          }
#endif /* #if (USE_HAL_SAI_REGISTER_CALLBACKS == 0) */
        }

        if (status == BSP_ERROR_NONE)
        {
          /* Prepare SAI peripheral initialization */
          MX_SAI_InitTypeDef mxSaiInit;
          mxSaiInit.AudioFrequency    = AudioInit->SampleRate;
          mxSaiInit.AudioMode         = SAI_MODESLAVE_RX;
          mxSaiInit.ClockStrobing     = SAI_CLOCKSTROBING_FALLINGEDGE;
          mxSaiInit.MonoStereoMode    = SAI_MONOMODE;
          mxSaiInit.DataSize          = SAI_DATASIZE_16;
          mxSaiInit.FrameLength       = 32;
          mxSaiInit.ActiveFrameLength = 16;
          mxSaiInit.OutputDrive       = SAI_OUTPUTDRIVE_ENABLE;
          mxSaiInit.Synchro           = SAI_SYNCHRONOUS;
          mxSaiInit.SynchroExt        = SAI_SYNCEXT_DISABLE;
          mxSaiInit.SlotActive        = SAI_SLOTACTIVE_0;

          /* SAI peripheral initialization */
          if (MX_SAI1_Init(&haudio_in_sai, &mxSaiInit) != HAL_OK)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }
          else
          {
            if (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET)
            {
              /* Initialize SAI peripheral used to generate clock and synchro */
              mxSaiInit.AudioMode  = SAI_MODEMASTER_TX;
              mxSaiInit.Synchro    = SAI_ASYNCHRONOUS;
              mxSaiInit.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1;
              if (MX_SAI1_Init(&haudio_out_sai, &mxSaiInit) != HAL_OK)
              {
                status = BSP_ERROR_PERIPH_FAILURE;
              }
            }
          }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)    
          if (status == BSP_ERROR_NONE)
          {
            /* Register SAI TC, HT and Error callbacks */
            if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_COMPLETE_CB_ID, SAI_RxCpltCallback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
            else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_HALFCOMPLETE_CB_ID, SAI_RxHalfCpltCallback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
            else
            {
              if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
              {
                status = BSP_ERROR_PERIPH_FAILURE;
              }
            }
          }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
          if (status == BSP_ERROR_NONE)
          {
            /* Initialize audio codec */
            CS42L51_Init_t codec_init;
            codec_init.InputDevice  = CS42L51_IN_MIC1;
            codec_init.OutputDevice = (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET) ? CS42L51_OUT_NONE : CS42L51_OUT_HEADPHONE;
            codec_init.Frequency    = AudioInit->SampleRate;
            codec_init.Resolution   = CS42L51_RESOLUTION_16B; /* Not used */
            codec_init.Volume       = Audio_Out_Ctx[0].Volume;
            if (Audio_Drv->Init(Audio_CompObj, &codec_init) < 0)
            {
              status = BSP_ERROR_COMPONENT_FAILURE;
            }
            else
            {
              /* Update audio in context state */
              Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_STOP;
            }
          }
        }
      }
    }
    else /* Instance = 1 */
    {
      /* Fill audio in context structure */
      Audio_In_Ctx[Instance].Device         = AudioInit->Device;
      Audio_In_Ctx[Instance].SampleRate     = AudioInit->SampleRate;
      Audio_In_Ctx[Instance].BitsPerSample  = AudioInit->BitsPerSample;
      Audio_In_Ctx[Instance].ChannelsNbr    = AudioInit->ChannelsNbr;
      Audio_In_Ctx[Instance].Volume         = AudioInit->Volume;

      /* Set DFSDM instances */
      haudio_in_dfsdm_channel.Instance = DFSDM1_Channel1;
      haudio_in_dfsdm_filter.Instance  = DFSDM1_Filter0;

      /* Configure DFSDM clock according to the requested audio frequency */
      if (MX_DFSDM1_ClockConfig(&haudio_in_dfsdm_channel, AudioInit->SampleRate) != HAL_OK)
      {
        status = BSP_ERROR_CLOCK_FAILURE;
      }
      else
      {
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0)      
        DFSDM_ChannelMspInit(&haudio_in_dfsdm_channel);
        DFSDM_FilterMspInit(&haudio_in_dfsdm_filter);
#else
        /* Register the DFSDM MSP Callbacks */
        if(AudioIn_IsMspCbValid[Instance] == 0U)
        {
          if(BSP_AUDIO_IN_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }
        }
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0) */
        if (status == BSP_ERROR_NONE)
        {
          /* Prepare DFSDM peripheral initialization */
          MX_DFSDM_InitTypeDef mxDfsdmInit;
          mxDfsdmInit.ChannelInstance = DFSDM1_Channel1;
          mxDfsdmInit.ClockDivider    = DFSDM_CLOCK_DIVIDER(Audio_In_Ctx[Instance].SampleRate);
          mxDfsdmInit.DigitalMicPins  = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
          mxDfsdmInit.DigitalMicType  = DFSDM_CHANNEL_SPI_RISING;
          mxDfsdmInit.RightBitShift   = DFSDM_MIC_BIT_SHIFT(Audio_In_Ctx[Instance].SampleRate);
          mxDfsdmInit.Channel4Filter  = DFSDM_CHANNEL_1;
          mxDfsdmInit.FilterInstance  = DFSDM1_Filter0;
          mxDfsdmInit.RegularTrigger  = DFSDM_FILTER_SW_TRIGGER;
          mxDfsdmInit.SincOrder       = DFSDM_FILTER_ORDER(Audio_In_Ctx[Instance].SampleRate);
          mxDfsdmInit.Oversampling    = DFSDM_OVER_SAMPLING(Audio_In_Ctx[Instance].SampleRate);
          if (MX_DFSDM1_Init(&haudio_in_dfsdm_filter, &haudio_in_dfsdm_channel, &mxDfsdmInit) != HAL_OK)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1)    
          if (status == BSP_ERROR_NONE)
          {
            /* Register DFSDM filter TC, HT and Error callbacks */
            if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_REGCONV_COMPLETE_CB_ID, DFSDM_FilterRegConvCpltCallback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
            else if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_REGCONV_HALFCOMPLETE_CB_ID, DFSDM_FilterRegConvHalfCpltCallback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
            else
            {
              if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_ERROR_CB_ID, DFSDM_FilterErrorCallback) != HAL_OK)
              {
                status = BSP_ERROR_PERIPH_FAILURE;
              }
            }
          }
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) */          
          if (status == BSP_ERROR_NONE)
          {
            /* Update audio in context state */
            Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_STOP;
          }
        }
      }
    }
  }
  return status;
}

/**
  * @brief  De-initialize the audio in peripherals.
  * @param  Instance Audio in instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_DeInit(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_RESET)
  {
    if (Instance == 0U)
    {
      /* Reset audio codec if not currently used by audio out instance 0 */
      if (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET)
      {    
        CS42L51_PowerDown();
      }

      /* SAI peripheral de-initialization */
      if (HAL_SAI_DeInit(&haudio_in_sai) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      /* De-initialize audio codec if not currently used by audio out instance 0 */
      else
      {
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 0)
        SAI_MspDeInit(&haudio_in_sai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 0) */
        if (Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET)
        {
          if (Audio_Drv->DeInit(Audio_CompObj) < 0)
          {
            status = BSP_ERROR_COMPONENT_FAILURE;
          }
        }
      }

      if (status == BSP_ERROR_NONE)
      {
        /* Update audio in context */
        Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_RESET;
      }
    }
    else /* Instance = 1 */
    {
      /* DFSDM peripheral de-initialization */
      if (DFSDM_DeInit(&haudio_in_dfsdm_filter, &haudio_in_dfsdm_channel) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0)
        DFSDM_FilterMspDeInit(&haudio_in_dfsdm_filter);
        DFSDM_ChannelMspDeInit(&haudio_in_dfsdm_channel);
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0) */
      }

      if (status == BSP_ERROR_NONE)
      {
        /* Update audio in context */
        Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_RESET;
      }
    }
  }
  else
  {
    /* Nothing to do */
  }
  return status;
}

/**
  * @brief  Start recording audio stream to a data buffer for a determined size.
  * @param  Instance Audio in instance.
  * @param  pData Pointer on data buffer.
  * @param  NbrOfBytes Size of buffer in bytes. Maximum size is 65535 bytes.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_Record(uint32_t Instance, uint8_t* pData, uint32_t NbrOfBytes)
{
  int32_t  status = BSP_ERROR_NONE;

  if ((Instance >= AUDIO_IN_INSTANCES_NBR) || (pData == NULL) || (NbrOfBytes > 65535U))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check the internal buffer size */
  else if ((Instance == 1U) && ((NbrOfBytes / 2U) > BSP_AUDIO_IN_DEFAULT_BUFFER_SIZE))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    if (Instance == 0U)
    {
      /* If no playback is on going, just transmit some bytes on audio out stream to generate SAI clock and synchro signals */
      if ((Audio_Out_Ctx[0].State != AUDIO_OUT_STATE_PLAYING) && (Audio_Out_Ctx[0].State != AUDIO_OUT_STATE_PAUSE))
      {
        uint8_t TxData[2] = {0x00U, 0x00U};
        if(HAL_SAI_Transmit(&haudio_out_sai, TxData, 2, 1000) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }

      if (status == BSP_ERROR_NONE)
      {
        /* Call the audio Codec Play function */
        if (Audio_Drv->Play(Audio_CompObj) < 0)
        {
          status = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          /* Initiate a DMA transfer of audio samples from the serial audio interface */
          /* Because only 16 bits per sample is supported, DMA transfer is in halfword size */
          if (HAL_SAI_Receive_DMA(&haudio_in_sai, pData, (uint16_t) NbrOfBytes/2U) != HAL_OK)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }
        }
      }
    }
    else /* Instance = 1 */
    {
      Audio_In_Ctx[Instance].pBuff = pData;
      Audio_In_Ctx[Instance].Size  = NbrOfBytes;

      /* Call the Media layer start function for MIC channel */
      if(HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_dfsdm_filter,
                                           Audio_DigMicRecBuff,
                                          (Audio_In_Ctx[Instance].Size / (2U * Audio_In_Ctx[Instance].ChannelsNbr))) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    if (status == BSP_ERROR_NONE)
    {
      /* Update audio in state */
      Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_RECORDING;
    }
  }
  return status;
}

/**
  * @brief  Pause record of audio stream.
  * @param  Instance Audio in instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_Pause(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_RECORDING)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    if (Instance == 0U)
    {
      /* Pause DMA transfer of audio samples from the serial audio interface */
      if (HAL_SAI_DMAPause(&haudio_in_sai) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    else /* Instance = 1 */
    {
      /* Call the Media layer stop function */
      if(HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_dfsdm_filter) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    if (status == BSP_ERROR_NONE)
    {
      /* Update audio in state */
      Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_PAUSE;
    }
  }
  return status;
}

/**
  * @brief  Resume record of audio stream.
  * @param  Instance Audio in instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_Resume(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_PAUSE)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    if (Instance == 0U)
    {
      /* Resume DMA transfer of audio samples from the serial audio interface */
      if (HAL_SAI_DMAResume(&haudio_in_sai) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    else /* Instance = 1 */
    {
      /* Call the Media layer start function for MIC2 channel */
      if(HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_dfsdm_filter,
                                           Audio_DigMicRecBuff,
                                          (Audio_In_Ctx[Instance].Size / (2U * Audio_In_Ctx[Instance].ChannelsNbr))) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    if (status == BSP_ERROR_NONE)
    {
      /* Update audio in state */
      Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_RECORDING;
    }
  }
  return status;
}

/**
  * @brief  Stop record of audio stream.
  * @param  Instance Audio in instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_Stop(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State == AUDIO_IN_STATE_STOP)
  {
    /* Nothing to do */
  }
  else if ((Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_RECORDING) &&
           (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_PAUSE))
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    if (Instance == 0U)
    {
      /* Call the audio codec stop function */
      if (Audio_Drv->Stop(Audio_CompObj, CS42L51_PDWN_SW) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
      /* Stop DMA transfer of audio samples from the serial audio interface */
      else
      {
        if (HAL_SAI_DMAStop(&haudio_in_sai) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }
    }
    else /* Instance = 1 */
    {
      /* Call the Media layer stop function */
      if(HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_dfsdm_filter) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    if (status == BSP_ERROR_NONE)
    {
      /* Update audio in state */
      Audio_In_Ctx[Instance].State = AUDIO_IN_STATE_STOP;
    }
  }
  return status;
}

/**
  * @brief  Set audio in volume.
  * @param  Instance Audio in instance.
  * @param  Volume Volume level in percentage from 0% to 100%.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_SetVolume(uint32_t Instance, uint32_t Volume)
{
  int32_t status;

  if ((Instance >= AUDIO_IN_INSTANCES_NBR) || (Volume > 100U))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Feature not supported */
  else
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  return status;
}

/**
  * @brief  Get audio in volume.
  * @param  Instance Audio in instance.
  * @param  Volume Pointer to volume level in percentage from 0% to 100%.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_GetVolume(uint32_t Instance, uint32_t *Volume)
{
  int32_t status;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Feature not supported */
  else
  {
    *Volume = 0U;
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  return status;
}

/**
  * @brief  Set audio in sample rate.
  * @param  Instance Audio in instance.
  * @param  SampleRate Sample rate of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_SetSampleRate(uint32_t Instance, uint32_t SampleRate)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Check if playback on instance 0 is on going and corresponding sample rate */
  else if ((Audio_Out_Ctx[0].State != AUDIO_OUT_STATE_RESET) && 
           (Audio_Out_Ctx[0].SampleRate != SampleRate))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Check if sample rate is modified */
  else if (Audio_In_Ctx[Instance].SampleRate == SampleRate)
  {
    /* Nothing to do */
  }
  else
  {
    if (Instance == 0U)
    {
      /* Update SAI1 clock config */
      haudio_in_sai.Init.AudioFrequency = SampleRate;
      haudio_out_sai.Init.AudioFrequency = SampleRate;
      if (MX_SAI1_ClockConfig(&haudio_in_sai, SampleRate) != HAL_OK)
      {
        status = BSP_ERROR_CLOCK_FAILURE;
      }
      /* Re-initialize SAI1 with new sample rate */
      else if (HAL_SAI_Init(&haudio_in_sai) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
      /* Register SAI TC, HT and Error callbacks */
      else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_COMPLETE_CB_ID, SAI_RxCpltCallback) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_HALFCOMPLETE_CB_ID, SAI_RxHalfCpltCallback) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
      /* Store new sample rate on audio in context */
      else
      {
        Audio_In_Ctx[Instance].SampleRate = SampleRate;
      }
    }
    else /* Instance = 1 */
    {
      /* Configure DFSDM clock according to the requested audio frequency */
      if (MX_DFSDM1_ClockConfig(&haudio_in_dfsdm_channel, SampleRate) != HAL_OK)
      {
        status = BSP_ERROR_CLOCK_FAILURE;
      }
      else
      {
        MX_DFSDM_InitTypeDef mxDfsdmInit;

        /* DeInitialize DFSDM */
        if (DFSDM_DeInit(&haudio_in_dfsdm_filter, &haudio_in_dfsdm_channel) != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0)
        else
        {
          DFSDM_FilterMspDeInit(&haudio_in_dfsdm_filter);
          DFSDM_ChannelMspDeInit(&haudio_in_dfsdm_channel);
        }
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0) */

        /* ReInitialize DFSDM with new sample rate */
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0)      
        if (status == BSP_ERROR_NONE)
        {
          DFSDM_ChannelMspInit(&haudio_in_dfsdm_channel);
          DFSDM_FilterMspInit(&haudio_in_dfsdm_filter);
        }
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 0) */
        if (status == BSP_ERROR_NONE)
        {
          mxDfsdmInit.ChannelInstance = DFSDM1_Channel1;
          mxDfsdmInit.ClockDivider    = DFSDM_CLOCK_DIVIDER(SampleRate);
          mxDfsdmInit.DigitalMicPins  = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
          mxDfsdmInit.DigitalMicType  = DFSDM_CHANNEL_SPI_RISING;
          mxDfsdmInit.RightBitShift   = DFSDM_MIC_BIT_SHIFT(SampleRate);
          mxDfsdmInit.Channel4Filter  = DFSDM_CHANNEL_1;
          mxDfsdmInit.FilterInstance  = DFSDM1_Filter0;
          mxDfsdmInit.RegularTrigger  = DFSDM_FILTER_SW_TRIGGER;
          mxDfsdmInit.SincOrder       = DFSDM_FILTER_ORDER(SampleRate);
          mxDfsdmInit.Oversampling    = DFSDM_OVER_SAMPLING(SampleRate);
          if (MX_DFSDM1_Init(&haudio_in_dfsdm_filter, &haudio_in_dfsdm_channel, &mxDfsdmInit) != HAL_OK)
          {
            status = BSP_ERROR_PERIPH_FAILURE;
          }
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1)
          if (status == BSP_ERROR_NONE)
          {
            /* Register DFSDM filter TC, HT and Error callbacks */
            if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_REGCONV_COMPLETE_CB_ID, DFSDM_FilterRegConvCpltCallback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
            else if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_REGCONV_HALFCOMPLETE_CB_ID, DFSDM_FilterRegConvHalfCpltCallback) != HAL_OK)
            {
              status = BSP_ERROR_PERIPH_FAILURE;
            }
            else
            {
              if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_ERROR_CB_ID, DFSDM_FilterErrorCallback) != HAL_OK)
              {
                status = BSP_ERROR_PERIPH_FAILURE;
              }
            }
          }
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) */
        }
      }
      /* Store new sample rate on audio in context */
      if (status == BSP_ERROR_NONE)
      {
        Audio_In_Ctx[Instance].SampleRate = SampleRate;
      }  
    }
  }
  return status;
}

/**
  * @brief  Get audio in sample rate.
  * @param  Instance Audio in instance.
  * @param  SampleRate Pointer to sample rate of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_GetSampleRate(uint32_t Instance, uint32_t *SampleRate)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State == AUDIO_IN_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current audio in sample rate */
  else
  {
    *SampleRate = Audio_In_Ctx[Instance].SampleRate;
  }
  return status;
}

/**
  * @brief  Set audio in device.
  * @param  Instance Audio in instance.
  * @param  Device Device of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_SetDevice(uint32_t Instance, uint32_t Device)
{
  int32_t status = BSP_ERROR_NONE;

  UNUSED(Device);

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Nothing to do because there is only one device for each instance */
  }
  return status;
}

/**
  * @brief  Get audio in device.
  * @param  Instance Audio in instance.
  * @param  Device Pointer to device of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_GetDevice(uint32_t Instance, uint32_t *Device)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State == AUDIO_IN_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current audio in device */
  else
  {
    *Device = Audio_In_Ctx[Instance].Device;
  }
  return status;
}

/**
  * @brief  Set bits per sample for the audio in stream.
  * @param  Instance Audio in instance.
  * @param  BitsPerSample Bits per sample of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (BitsPerSample != AUDIO_RESOLUTION_16b)
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Nothing to do because there is only one bits per sample supported (AUDIO_RESOLUTION_16b) */
  }
  return status;
}

/**
  * @brief  Get bits per sample for the audio in stream.
  * @param  Instance Audio in instance.
  * @param  BitsPerSample Pointer to bits per sample of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State == AUDIO_IN_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current bits per sample of audio in stream */
  else
  {
    *BitsPerSample = Audio_In_Ctx[Instance].BitsPerSample;
  }
  return status;
}

/**
  * @brief  Set channels number for the audio in stream.
  * @param  Instance Audio in instance.
  * @param  ChannelNbr Channels number of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (ChannelNbr != 1U)
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State != AUDIO_IN_STATE_STOP)
  {
    status = BSP_ERROR_BUSY;
  }
  else
  {
    /* Nothing to do because channels are already configurated and can't be modified */
  }
  return status;
}

/**
  * @brief  Get channels number for the audio in stream.
  * @param  Instance Audio in instance.
  * @param  ChannelNbr Pointer to channels number of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio in state */
  else if (Audio_In_Ctx[Instance].State == AUDIO_IN_STATE_RESET)
  {
    status = BSP_ERROR_BUSY;
  }
  /* Get the current channels number of audio in stream */
  else
  {
    *ChannelNbr = Audio_In_Ctx[Instance].ChannelsNbr;
  }
  return status;
}

/**
  * @brief  Get current state for the audio in stream.
  * @param  Instance Audio in instance.
  * @param  State Pointer to state of the audio in stream.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_GetState(uint32_t Instance, uint32_t *State)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  /* Get the current state of audio in stream */
  else
  {
    *State = Audio_In_Ctx[Instance].State;
  }
  return status;
}

#if ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1))
/**
  * @brief  Register default BSP AUDIO IN msp callbacks.
  * @param  Instance AUDIO IN Instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_RegisterDefaultMspCallbacks(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Instance == 0U)
    {
      /* Register MspInit/MspDeInit callbacks */
      if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPINIT_CB_ID, SAI_MspInit) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPDEINIT_CB_ID, SAI_MspDeInit) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        AudioIn_IsMspCbValid[Instance] = 1U;
      }
    }
    else /* Instance = 1 */
    {
      /* Register MspInit/MspDeInit callbacks */
      if (HAL_DFSDM_Channel_RegisterCallback(&haudio_in_dfsdm_channel, HAL_DFSDM_CHANNEL_MSPINIT_CB_ID, DFSDM_ChannelMspInit) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_MSPINIT_CB_ID, DFSDM_FilterMspInit) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_DFSDM_Channel_RegisterCallback(&haudio_in_dfsdm_channel, HAL_DFSDM_CHANNEL_MSPDEINIT_CB_ID, DFSDM_ChannelMspDeInit) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_MSPDEINIT_CB_ID, DFSDM_FilterMspDeInit) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }

      if (status == BSP_ERROR_NONE)
      {
        AudioIn_IsMspCbValid[Instance] = 1U;
      }        
    }
  }
  /* Return BSP status */
  return status;
}

/**
  * @brief  Register BSP AUDIO IN msp callbacks.
  * @param  Instance AUDIO IN Instance.
  * @param  CallBacks Pointer to MspInit/MspDeInit callback functions.
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_RegisterMspCallbacks(uint32_t Instance, BSP_AUDIO_IN_Cb_t *CallBacks)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Instance == 0U)
    {
      /* Register MspInit/MspDeInit callbacks */
      if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPINIT_CB_ID, CallBacks->pMspSaiInitCb) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPDEINIT_CB_ID, CallBacks->pMspSaiDeInitCb) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        AudioIn_IsMspCbValid[Instance] = 1U;
      }
    }
    else /* Instance = 1 */
    {
      /* Register MspInit/MspDeInit callbacks */
      if (HAL_DFSDM_Channel_RegisterCallback(&haudio_in_dfsdm_channel, HAL_DFSDM_CHANNEL_MSPINIT_CB_ID, CallBacks->pMspChannelInitCb) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_MSPINIT_CB_ID, CallBacks->pMspFilterInitCb) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else if (HAL_DFSDM_Channel_RegisterCallback(&haudio_in_dfsdm_channel, HAL_DFSDM_CHANNEL_MSPDEINIT_CB_ID, CallBacks->pMspChannelDeInitCb) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        if (HAL_DFSDM_Filter_RegisterCallback(&haudio_in_dfsdm_filter, HAL_DFSDM_FILTER_MSPDEINIT_CB_ID, CallBacks->pMspFilterDeInitCb) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }

      if (status == BSP_ERROR_NONE)
      {
        AudioIn_IsMspCbValid[Instance] = 1U;
      }
    }
  }
  /* Return BSP status */
  return status; 
}
#endif /* ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1)) */

/**
  * @brief  Manage the BSP audio in transfer complete event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
__weak void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manage the BSP audio in half transfer complete event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
__weak void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manages the BSP audio in error event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
__weak void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  BSP AUDIO IN interrupt handler.
  * @param  Instance Audio in instance.
  * @param  Device Device of the audio in stream.
  * @retval None.
  */
void BSP_AUDIO_IN_IRQHandler(uint32_t Instance, uint32_t Device)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Device);

  if (Instance == 0U)
  {
    HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
  }
  else
  {
    HAL_DMA_IRQHandler(haudio_in_dfsdm_filter.hdmaReg);
  }
}

/**
  * @brief  DFSDM1 clock Config.
  * @param  hDfsdmChannel DFSDM channel handle.
  * @param  SampleRate Audio sample rate used to record the audio stream.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_DFSDM1_ClockConfig(DFSDM_Channel_HandleTypeDef *hDfsdmChannel, uint32_t SampleRate)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hDfsdmChannel);

  HAL_StatusTypeDef        status = HAL_OK;
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

  /* Configure the SAI PLL according to the requested audio frequency if not already done by other instances */
  if ((Audio_Out_Ctx[0].State == AUDIO_OUT_STATE_RESET) && (Audio_In_Ctx[0].State == AUDIO_IN_STATE_RESET))
  {
    status = MX_SAI1_ClockConfig(&haudio_in_sai, SampleRate);
  }

  if (status == HAL_OK)
  {
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_DFSDM1AUDIO;
    RCC_ExCLKInitStruct.Dfsdm1AudioClockSelection = RCC_DFSDM1AUDIOCLKSOURCE_SAI1;
    status = HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
  }

  return status;
}

/**
  * @brief  Initialize DFSDM1.
  * @param  hDfsdmFilter  DFSDM filter handle.
  * @param  hDfsdmChannel DFSDM channel handle.
  * @param  MXInit DFSDM configuration structure.
  * @retval HAL_status.
  */
__weak HAL_StatusTypeDef MX_DFSDM1_Init(DFSDM_Filter_HandleTypeDef *hDfsdmFilter, DFSDM_Channel_HandleTypeDef *hDfsdmChannel, MX_DFSDM_InitTypeDef *MXInit)
{  
  HAL_StatusTypeDef status = HAL_OK;
  
  /* MIC channels initialization */
  hDfsdmChannel->Instance                      = MXInit->ChannelInstance;  
  hDfsdmChannel->Init.OutputClock.Activation   = ENABLE;
  hDfsdmChannel->Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  hDfsdmChannel->Init.OutputClock.Divider      = MXInit->ClockDivider;
  hDfsdmChannel->Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  hDfsdmChannel->Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  hDfsdmChannel->Init.Input.Pins               = MXInit->DigitalMicPins; 
  hDfsdmChannel->Init.SerialInterface.Type     = MXInit->DigitalMicType;
  hDfsdmChannel->Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  hDfsdmChannel->Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  hDfsdmChannel->Init.Awd.Oversampling         = 10;
  hDfsdmChannel->Init.Offset                   = 0;
  hDfsdmChannel->Init.RightBitShift            = MXInit->RightBitShift;
  
  if(HAL_OK != HAL_DFSDM_ChannelInit(hDfsdmChannel))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* MIC filters  initialization */
    hDfsdmFilter->Instance                          = MXInit->FilterInstance; 
    hDfsdmFilter->Init.RegularParam.Trigger         = MXInit->RegularTrigger;
    hDfsdmFilter->Init.RegularParam.FastMode        = ENABLE;
    hDfsdmFilter->Init.RegularParam.DmaMode         = ENABLE;
    hDfsdmFilter->Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
    hDfsdmFilter->Init.InjectedParam.ScanMode       = DISABLE;
    hDfsdmFilter->Init.InjectedParam.DmaMode        = DISABLE;
    hDfsdmFilter->Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
    hDfsdmFilter->Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
    hDfsdmFilter->Init.FilterParam.SincOrder        = MXInit->SincOrder;
    hDfsdmFilter->Init.FilterParam.Oversampling     = MXInit->Oversampling;   
    hDfsdmFilter->Init.FilterParam.IntOversampling  = 1;

    if(HAL_DFSDM_FilterInit(hDfsdmFilter) != HAL_OK)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Configure regular channel */
      if(HAL_DFSDM_FilterConfigRegChannel(hDfsdmFilter, MXInit->Channel4Filter, DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
      {
        status = HAL_ERROR;
      }
    }
  }

  return status;
}
/**
  * @}
  */ 

/** @defgroup STM32L562E-DK_AUDIO_Private_Functions STM32L562E-DK AUDIO Private Functions
  * @{
  */ 
/**
  * @brief  Probe the CS42L51 audio codec.
  * @retval BSP status.
  */
static int32_t CS42L51_Probe(void)
{
  int32_t                  status = BSP_ERROR_NONE;
  CS42L51_IO_t             IOCtx;
  uint32_t                 cs42l51_id;
  static CS42L51_Object_t  CS42L51Obj;

  /* Configure the audio driver */
  IOCtx.Address     = AUDIO_I2C_ADDRESS;
  IOCtx.Init        = BSP_I2C1_Init;
  IOCtx.DeInit      = BSP_I2C1_DeInit;
  IOCtx.ReadReg     = BSP_I2C1_ReadReg;
  IOCtx.WriteReg    = BSP_I2C1_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if (CS42L51_RegisterBusIO(&CS42L51Obj, &IOCtx) != CS42L51_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else if (CS42L51_ReadID(&CS42L51Obj, &cs42l51_id) != CS42L51_OK)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  } 
  else if ((cs42l51_id & CS42L51_ID_MASK) != CS42L51_ID)
  {
    status = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    Audio_Drv = (AUDIO_Drv_t *) &CS42L51_Driver;
    Audio_CompObj = &CS42L51Obj;
  }

  return status;
} 

/**
  * @brief  Initialize SAI MSP.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  GPIO_InitTypeDef         GPIO_InitStruct;

  /* Enable SAI clock */
  AUDIO_SAI1_CLK_ENABLE();

  if (hsai->Instance == SAI1_Block_A)
  {
    /* SAI pins configuration: FS, SCK, MCLK and SD pins */
    AUDIO_SAI1_MCLK_A_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = AUDIO_SAI1_MCLK_A_GPIO_AF;
    GPIO_InitStruct.Pin       = AUDIO_SAI1_MCLK_A_GPIO_PIN;
    HAL_GPIO_Init(AUDIO_SAI1_MCLK_A_GPIO_PORT, &GPIO_InitStruct);

    AUDIO_SAI1_FS_A_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Alternate = AUDIO_SAI1_FS_A_GPIO_AF;
    GPIO_InitStruct.Pin       = AUDIO_SAI1_FS_A_GPIO_PIN;
    HAL_GPIO_Init(AUDIO_SAI1_FS_A_GPIO_PORT, &GPIO_InitStruct);

    AUDIO_SAI1_SCK_A_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Alternate = AUDIO_SAI1_SCK_A_GPIO_AF;
    GPIO_InitStruct.Pin       = AUDIO_SAI1_SCK_A_GPIO_PIN;
    HAL_GPIO_Init(AUDIO_SAI1_SCK_A_GPIO_PORT, &GPIO_InitStruct);

    AUDIO_SAI1_SD_A_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Alternate = AUDIO_SAI1_SD_A_GPIO_AF;
    GPIO_InitStruct.Pin       = AUDIO_SAI1_SD_A_GPIO_PIN;
    HAL_GPIO_Init(AUDIO_SAI1_SD_A_GPIO_PORT, &GPIO_InitStruct);

    /* Configure the hDmaSaiTx handle parameters */
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    hDmaSaiTx.Init.Request             = DMA_REQUEST_SAI1_A;
    hDmaSaiTx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hDmaSaiTx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hDmaSaiTx.Init.MemInc              = DMA_MINC_ENABLE;
    if (Audio_Out_Ctx[0].BitsPerSample == AUDIO_RESOLUTION_16b)
    {
      hDmaSaiTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      hDmaSaiTx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    }
    else /* AUDIO_RESOLUTION_24b */
    {
      hDmaSaiTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
      hDmaSaiTx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    }
    hDmaSaiTx.Init.Mode                = DMA_CIRCULAR;
    hDmaSaiTx.Init.Priority            = DMA_PRIORITY_HIGH;
    hDmaSaiTx.Instance                 = DMA2_Channel1;
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmatx, hDmaSaiTx);
    /* Deinitialize the Stream for new transfer */
    if (HAL_DMA_DeInit(&hDmaSaiTx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* Configure the DMA Stream */
    if (HAL_DMA_Init(&hDmaSaiTx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, BSP_AUDIO_OUT_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
  }
  else /* SAI1_BlockB */
  {
    /* SAI pins configuration: SD pin */
    AUDIO_SAI1_SD_B_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = AUDIO_SAI1_SD_B_GPIO_AF;
    GPIO_InitStruct.Pin       = AUDIO_SAI1_SD_B_GPIO_PIN;
    HAL_GPIO_Init(AUDIO_SAI1_SD_B_GPIO_PORT, &GPIO_InitStruct);

    /* Configure the hDmaSaiRx handle parameters */
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    hDmaSaiRx.Init.Request             = DMA_REQUEST_SAI1_B;
    hDmaSaiRx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hDmaSaiRx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hDmaSaiRx.Init.MemInc              = DMA_MINC_ENABLE;
    if (Audio_In_Ctx[0].BitsPerSample == AUDIO_RESOLUTION_16b)
    {
      hDmaSaiRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      hDmaSaiRx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    }
    else /* AUDIO_RESOLUTION_24b */
    {
      hDmaSaiRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
      hDmaSaiRx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    }
    hDmaSaiRx.Init.Mode                = DMA_CIRCULAR;
    hDmaSaiRx.Init.Priority            = DMA_PRIORITY_HIGH;
    hDmaSaiRx.Instance                 = DMA2_Channel2;
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmarx, hDmaSaiRx);
    /* Deinitialize the Stream for new transfer */
    if (HAL_DMA_DeInit(&hDmaSaiRx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* Configure the DMA Stream */
    if (HAL_DMA_Init(&hDmaSaiRx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, BSP_AUDIO_IN_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
  }
}

/**
  * @brief  Deinitialize SAI MSP.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
  if(hsai->Instance == SAI1_Block_A)
  {
    /* Disable SAI DMA Channel IRQ */
    HAL_NVIC_DisableIRQ(DMA2_Channel1_IRQn);

    /* Reset the DMA Stream configuration*/
    if (HAL_DMA_DeInit(&hDmaSaiTx) != HAL_OK)
    {
      /* Nothing to do */
    }

    /* Don't disable the DMA clock potentially used for other SAI block */
    /* __HAL_RCC_DMA2_CLK_DISABLE(); */
    /* __HAL_RCC_DMAMUX1_CLK_DISABLE(); */

    /* De-initialize FS, SCK, MCK and SD pins*/
    HAL_GPIO_DeInit(AUDIO_SAI1_MCLK_A_GPIO_PORT, AUDIO_SAI1_MCLK_A_GPIO_PIN);
    HAL_GPIO_DeInit(AUDIO_SAI1_FS_A_GPIO_PORT, AUDIO_SAI1_FS_A_GPIO_PIN);
    HAL_GPIO_DeInit(AUDIO_SAI1_SCK_A_GPIO_PORT, AUDIO_SAI1_SCK_A_GPIO_PIN);
    HAL_GPIO_DeInit(AUDIO_SAI1_SD_A_GPIO_PORT, AUDIO_SAI1_SD_A_GPIO_PIN);

    /* Don't disable SAI clock potentially used for other SAI block */
    /*AUDIO_SAI1_CLK_DISABLE(); */
  }
  else /* SAI1_BlockB */
  {
    /* Disable SAI DMA Channel IRQ */
    HAL_NVIC_DisableIRQ(DMA2_Channel2_IRQn);

    /* Reset the DMA Stream configuration*/
    if (HAL_DMA_DeInit(&hDmaSaiRx) != HAL_OK)
    {
      /* Nothing to do */
    }

    /* Don't disable the DMA clock potentially used for other SAI block */
    /* __HAL_RCC_DMA2_CLK_DISABLE(); */
    /* __HAL_RCC_DMAMUX1_CLK_DISABLE(); */

    /* De-initialize SD pin */
    HAL_GPIO_DeInit(AUDIO_SAI1_SD_B_GPIO_PORT, AUDIO_SAI1_SD_B_GPIO_PIN);

    /* Don't disable SAI clock potentially used for other SAI block */
    /*AUDIO_SAI1_CLK_DISABLE(); */
  }
}

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
/**
  * @brief  SAI Tx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Tx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI Rx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Rx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI error callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_Error_CallBack(0);
  }
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_Error_CallBack(0);
  }
}
#else /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
/**
  * @brief  SAI Tx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Tx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI Rx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Rx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI error callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_Error_CallBack(0);
  }
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_Error_CallBack(0);
  }
}
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Un-reset CS42L51.
  * @retval None.
  */
static void CS42L51_PowerUp(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Initialize and configure the CS42L51 reset pin */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pin   = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* Un-reset the CS42L51 */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);
  /* Wait 1ms according CS42L51 datasheet */
  HAL_Delay(1);
}

/**
  * @brief  Reset CS42L51.
  * @retval None.
  */
static void CS42L51_PowerDown(void)
{
  /* Reset the CS42L51 */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
}

/**
  * @brief  DeInitialize DFSDM.
  * @param  hDfsdmFilter  DFSDM filter handle.
  * @param  hDfsdmChannel DFSDM channel handle.
  * @retval BSP status.
  */
static int32_t DFSDM_DeInit(DFSDM_Filter_HandleTypeDef *hDfsdmFilter, DFSDM_Channel_HandleTypeDef *hDfsdmChannel)
{  
  int32_t status = BSP_ERROR_NONE;
  
  /* MIC filters  initialization */
  if(HAL_DFSDM_FilterDeInit(hDfsdmFilter) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* MIC channels Deinitialization */
    if(HAL_OK != HAL_DFSDM_ChannelDeInit(hDfsdmChannel))
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Initialize DFSDM channel MSP.
  * @param  hdfsdm_channel DFSDM channel handle.
  * @retval None.
  */
static void DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel)
{
  if (hdfsdm_channel->Instance == DFSDM1_Channel1)
  {
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable DFSDM clock */
    AUDIO_DFSDM1_CLK_ENABLE();

    /* DFSDM pins configuration: DFSDM1_CKOUT, DFSDM1_DATIN1 pins */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();

    AUDIO_DFSDM1_CKOUT_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = AUDIO_DFSDM1_CKOUT_GPIO_AF;
    GPIO_InitStruct.Pin       = AUDIO_DFSDM1_CKOUT_GPIO_PIN;
    HAL_GPIO_Init(AUDIO_DFSDM1_CKOUT_GPIO_PORT, &GPIO_InitStruct);

    AUDIO_DFSDM1_DATIN1_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Alternate = AUDIO_DFSDM1_DATIN1_GPIO_AF;
    GPIO_InitStruct.Pin       = AUDIO_DFSDM1_DATIN1_GPIO_PIN;
    HAL_GPIO_Init(AUDIO_DFSDM1_DATIN1_GPIO_PORT, &GPIO_InitStruct);
  }
}

/**
  * @brief  DeInitialize DFSDM channel MSP.
  * @param  hdfsdm_channel DFSDM channel handle.
  * @retval None.
  */
static void DFSDM_ChannelMspDeInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel)
{
  if (hdfsdm_channel->Instance == DFSDM1_Channel1)
  {
    /* De-initialize DFSDM1_CKOUT, DFSDM1_DATIN1 pins */
    HAL_GPIO_DeInit(AUDIO_DFSDM1_CKOUT_GPIO_PORT, AUDIO_DFSDM1_CKOUT_GPIO_PIN);
    HAL_GPIO_DeInit(AUDIO_DFSDM1_DATIN1_GPIO_PORT, AUDIO_DFSDM1_DATIN1_GPIO_PIN);

    /* Disable DFSDM1 */
    AUDIO_DFSDM1_CLK_DISABLE();
  }
}

/**
  * @brief  Initialize DFSDM filter MSP.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
static void DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  if(hdfsdm_filter->Instance == DFSDM1_Filter0)
  {
    /* Enable the DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMAMUX1_CLK_ENABLE();

    /* Configure the hDmaDfsdm[0] handle parameters */
    hDmaDfsdm.Init.Request             = DMA_REQUEST_DFSDM1_FLT0;
    hDmaDfsdm.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hDmaDfsdm.Init.PeriphInc           = DMA_PINC_DISABLE;
    hDmaDfsdm.Init.MemInc              = DMA_MINC_ENABLE;
    hDmaDfsdm.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hDmaDfsdm.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hDmaDfsdm.Init.Mode                = DMA_CIRCULAR;
    hDmaDfsdm.Init.Priority            = DMA_PRIORITY_HIGH;
    hDmaDfsdm.Instance                 = DMA1_Channel4;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hdfsdm_filter, hdmaReg, hDmaDfsdm);

    /* Deinitialize the DMA channel for new transfer */
    if (HAL_DMA_DeInit(&hDmaDfsdm) != HAL_OK)
    {
      /* Nothing to do */
    }

    /* Configure the DMA Channel */
    if (HAL_DMA_Init(&hDmaDfsdm) != HAL_OK)
    {
      /* Nothing to do */
    }

    /* DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, BSP_AUDIO_IN_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  }
}

/**
  * @brief  DeInitialize DFSDM filter MSP.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
static void DFSDM_FilterMspDeInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  if(hdfsdm_filter->Instance == DFSDM1_Filter0)
  {
    /* Disable DMA  Channel IRQ */
    HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);

    /* De-initialize the DMA Channel */
    if (HAL_DMA_DeInit(&hDmaDfsdm) != HAL_OK)
    {
      /* Nothing to do */
    }
  }
}

#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1)
/**
  * @brief  DFSDM filter regular conversion complete callback.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
static void DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  uint32_t     index;
  uint32_t     recbufsize = (Audio_In_Ctx[1].Size / (2U * Audio_In_Ctx[1].ChannelsNbr));
  __IO int32_t tmp;

  UNUSED(hdfsdm_filter);

  for (index = (recbufsize / 2U); index < recbufsize; index++)
  {
    tmp = Audio_DigMicRecBuff[index] / 256;
    tmp = SaturaLH(tmp, -32768, 32767);
    Audio_In_Ctx[1].pBuff[2U * index]        = (uint8_t) tmp;
    Audio_In_Ctx[1].pBuff[(2U * index) + 1U] = (uint8_t) ((uint32_t) tmp >> 8);
  }

  /* Invoke 'TransferCompete' callback function */
  BSP_AUDIO_IN_TransferComplete_CallBack(1);
}

/**
  * @brief  DFSDM filter regular conversion half complete callback.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
static void DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  uint32_t     index;
  uint32_t     recbufsize = (Audio_In_Ctx[1].Size / (2U * Audio_In_Ctx[1].ChannelsNbr));
  __IO int32_t tmp;

  UNUSED(hdfsdm_filter);

  for (index = 0; index < (recbufsize / 2U); index++)
  {
    tmp = Audio_DigMicRecBuff[index] / 256;
    tmp = SaturaLH(tmp, -32768, 32767);
    Audio_In_Ctx[1].pBuff[2U * index]        = (uint8_t) tmp;
    Audio_In_Ctx[1].pBuff[(2U * index) + 1U] = (uint8_t) ((uint32_t) tmp >> 8);
  }

  /* Invoke the 'HalfTransfer' callback function */
  BSP_AUDIO_IN_HalfTransfer_CallBack(1);
}

/**
  * @brief  DFSDM filter error callback.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
static void DFSDM_FilterErrorCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  UNUSED(hdfsdm_filter);

  BSP_AUDIO_IN_Error_CallBack(1);
}
#else /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) */
/**
  * @brief  DFSDM filter regular conversion complete callback.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  uint32_t     index;
  uint32_t     recbufsize = (Audio_In_Ctx[1].Size / (2U * Audio_In_Ctx[1].ChannelsNbr));
  __IO int32_t tmp;

  UNUSED(hdfsdm_filter);

  for (index = (recbufsize / 2U); index < recbufsize; index++)
  {
    tmp = Audio_DigMicRecBuff[index] / 256;
    tmp = SaturaLH(tmp, -32768, 32767);
    Audio_In_Ctx[1].pBuff[2U * index]        = (uint8_t) tmp;
    Audio_In_Ctx[1].pBuff[(2U * index) + 1U] = (uint8_t) ((uint32_t) tmp >> 8);
  }

  /* Invoke 'TransferCompete' callback function */
  BSP_AUDIO_IN_TransferComplete_CallBack(1);
}

/**
  * @brief  DFSDM filter regular conversion half complete callback.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  uint32_t     index;
  uint32_t     recbufsize = (Audio_In_Ctx[1].Size / (2U * Audio_In_Ctx[1].ChannelsNbr));
  __IO int32_t tmp;

  UNUSED(hdfsdm_filter);

  for (index = 0; index < (recbufsize / 2U); index++)
  {
    tmp = Audio_DigMicRecBuff[index] / 256;
    tmp = SaturaLH(tmp, -32768, 32767);
    Audio_In_Ctx[1].pBuff[2U * index]        = (uint8_t) tmp;
    Audio_In_Ctx[1].pBuff[(2U * index) + 1U] = (uint8_t) ((uint32_t) tmp >> 8);
  }

  /* Invoke the 'HalfTransfer' callback function */
  BSP_AUDIO_IN_HalfTransfer_CallBack(1);
}

/**
  * @brief  DFSDM filter error callback.
  * @param  hdfsdm_filter DFSDM filter handle.
  * @retval None.
  */
void HAL_DFSDM_FilterErrorCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  UNUSED(hdfsdm_filter);

  BSP_AUDIO_IN_Error_CallBack(1);
}
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
