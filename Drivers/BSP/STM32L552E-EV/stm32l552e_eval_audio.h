/**
  ******************************************************************************
  * @file    stm32l552e_eval_audio.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l552e_eval_audio.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L552E_EVAL_AUDIO_H
#define STM32L552E_EVAL_AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32l552e_eval_conf.h"
#include "stm32l552e_eval_errno.h"
#include "../Components/Common/audio.h"
#include "../Components/cs42l51/cs42l51.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32L552E-EV
  * @{
  */
    
/** @addtogroup STM32L552E-EV_AUDIO 
  * @{
  */

/** @defgroup STM32L552E-EV_AUDIO_Exported_Types STM32L552E-EV AUDIO Exported Types
  * @{
  */
typedef struct
{
  uint32_t Device;        /* Output or input device */
  uint32_t SampleRate;    /* From 8kHz to 192 kHz */
  uint32_t BitsPerSample; /* From 8 bits per sample to 32 bits per sample */
  uint32_t ChannelsNbr;   /* 1 for mono and 2 for stereo */
  uint32_t Volume;        /* In percentage from 0 to 100 */
} BSP_AUDIO_Init_t;

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)  
typedef struct
{
  pSAI_CallbackTypeDef  pMspInitCb;
  pSAI_CallbackTypeDef  pMspDeInitCb;
} BSP_AUDIO_OUT_Cb_t;
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

#if ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1))
typedef struct
{
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
  pSAI_CallbackTypeDef            pMspSaiInitCb;
  pSAI_CallbackTypeDef            pMspSaiDeInitCb;
#endif /* USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1)
  pDFSDM_Filter_CallbackTypeDef   pMspFilterInitCb;
  pDFSDM_Filter_CallbackTypeDef   pMspFilterDeInitCb;
  pDFSDM_Channel_CallbackTypeDef  pMspChannelInitCb;
  pDFSDM_Channel_CallbackTypeDef  pMspChannelDeInitCb;
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) */
} BSP_AUDIO_IN_Cb_t;
#endif /* ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1)) */

typedef struct
{
  uint32_t AudioFrequency;
  uint32_t MonoStereoMode;
  uint32_t AudioMode;
  uint32_t ClockStrobing;
  uint32_t Synchro;
  uint32_t OutputDrive;
  uint32_t SynchroExt;
  uint32_t DataSize;
  uint32_t FrameLength;
  uint32_t ActiveFrameLength;
  uint32_t SlotActive;
} MX_SAI_InitTypeDef;

typedef struct
{
  /* Filter parameters */
  DFSDM_Filter_TypeDef   *FilterInstance;
  uint32_t               RegularTrigger;
  uint32_t               SincOrder;
  uint32_t               Oversampling;
  /* Channel parameters */
  DFSDM_Channel_TypeDef *ChannelInstance;
  uint32_t              DigitalMicPins;
  uint32_t              DigitalMicType;
  uint32_t              Channel4Filter;
  uint32_t              ClockDivider;
  uint32_t              RightBitShift;
} MX_DFSDM_InitTypeDef;

/* Audio in and out context */
typedef struct
{
  uint32_t  Device;              /* Audio IN device to be used     */ 
  uint32_t  SampleRate;          /* Audio IN Sample rate           */
  uint32_t  BitsPerSample;       /* Audio IN Sample resolution     */
  uint32_t  ChannelsNbr;         /* Audio IN number of channel     */
  uint8_t   *pBuff;              /* Audio IN record buffer         */
  uint32_t  Size;                /* Audio IN record buffer size    */
  uint32_t  Volume;              /* Audio IN volume                */
  uint32_t  State;               /* Audio IN State                 */
} AUDIO_IN_Ctx_t;

typedef struct
{
  uint32_t  Device;              /* Audio out device                  */
  uint32_t  SampleRate;          /* Audio out sample rate             */
  uint32_t  BitsPerSample;       /* Audio out bits per sample         */
  uint32_t  Volume;              /* Audio out volume                  */
  uint32_t  ChannelsNbr;         /* Audio out number of channels      */
  uint32_t  IsMute;              /* Audio out mute state              */
  uint32_t  State;               /* Audio out state                   */
} AUDIO_OUT_Ctx_t;
/**
  * @}
  */ 

/** @defgroup STM32L552E-EV_AUDIO_Exported_Constants STM32L552E-EV AUDIO Exported Constants
  * @{
  */
/* Audio out instances */
#define AUDIO_OUT_INSTANCES_NBR 1U

/* Audio in instances */
#define AUDIO_IN_INSTANCES_NBR 2U

/* Audio out devices */
#define AUDIO_OUT_HEADPHONE 1U

/* Audio input devices */
#define AUDIO_IN_ANALOG_MIC        0x01U
#define AUDIO_IN_DIGITAL_MIC1      0x10U /* Left digital microphone  */
#define AUDIO_IN_DIGITAL_MIC2      0x20U /* Right digital microphone */
#define AUDIO_IN_DIGITAL_MIC       (AUDIO_IN_DIGITAL_MIC1 | AUDIO_IN_DIGITAL_MIC2)

/* Audio out states */
#define AUDIO_OUT_STATE_RESET   0U
#define AUDIO_OUT_STATE_PLAYING 1U
#define AUDIO_OUT_STATE_STOP    2U
#define AUDIO_OUT_STATE_PAUSE   3U

/* Audio in states */
#define AUDIO_IN_STATE_RESET     0U
#define AUDIO_IN_STATE_RECORDING 1U
#define AUDIO_IN_STATE_STOP      2U
#define AUDIO_IN_STATE_PAUSE     3U

/* Audio sample rate */
#define AUDIO_FREQUENCY_192K 192000U
#define AUDIO_FREQUENCY_176K 176400U
#define AUDIO_FREQUENCY_96K   96000U
#define AUDIO_FREQUENCY_88K   88200U
#define AUDIO_FREQUENCY_48K   48000U
#define AUDIO_FREQUENCY_44K   44100U
#define AUDIO_FREQUENCY_32K   32000U
#define AUDIO_FREQUENCY_22K   22050U
#define AUDIO_FREQUENCY_16K   16000U
#define AUDIO_FREQUENCY_11K   11025U
#define AUDIO_FREQUENCY_8K     8000U

/* Audio bits per sample */
#define AUDIO_RESOLUTION_8b   8U
#define AUDIO_RESOLUTION_16b 16U
#define AUDIO_RESOLUTION_24b 24U
#define AUDIO_RESOLUTION_32b 32U

/* Audio mute state */
#define AUDIO_MUTE_DISABLED 0U
#define AUDIO_MUTE_ENABLED  1U

/* Audio I2C address */
#define AUDIO_I2C_ADDRESS  0x94U

/* Audio in and out GPIOs */
#define AUDIO_SAI1_MCLK_B_GPIO_PORT           GPIOF
#define AUDIO_SAI1_MCLK_B_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()
#define AUDIO_SAI1_MCLK_B_GPIO_PIN            GPIO_PIN_7
#define AUDIO_SAI1_MCLK_B_GPIO_AF             GPIO_AF13_SAI1

#define AUDIO_SAI1_FS_B_GPIO_PORT             GPIOF
#define AUDIO_SAI1_FS_B_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define AUDIO_SAI1_FS_B_GPIO_PIN              GPIO_PIN_9
#define AUDIO_SAI1_FS_B_GPIO_AF               GPIO_AF13_SAI1

#define AUDIO_SAI1_SCK_B_GPIO_PORT            GPIOF
#define AUDIO_SAI1_SCK_B_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define AUDIO_SAI1_SCK_B_GPIO_PIN             GPIO_PIN_8
#define AUDIO_SAI1_SCK_B_GPIO_AF              GPIO_AF13_SAI1

#define AUDIO_SAI1_SD_B_GPIO_PORT             GPIOF
#define AUDIO_SAI1_SD_B_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define AUDIO_SAI1_SD_B_GPIO_PIN              GPIO_PIN_6
#define AUDIO_SAI1_SD_B_GPIO_AF               GPIO_AF13_SAI1

#define AUDIO_SAI1_SD_A_GPIO_PORT             GPIOA
#define AUDIO_SAI1_SD_A_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define AUDIO_SAI1_SD_A_GPIO_PIN              GPIO_PIN_10
#define AUDIO_SAI1_SD_A_GPIO_AF               GPIO_AF13_SAI1

#define AUDIO_SAI1_CLK_ENABLE()               __HAL_RCC_SAI1_CLK_ENABLE()
#define AUDIO_SAI1_CLK_DISABLE()              __HAL_RCC_SAI1_CLK_DISABLE()

#define AUDIO_DFSDM1_CKOUT_GPIO_PORT          GPIOF
#define AUDIO_DFSDM1_CKOUT_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
#define AUDIO_DFSDM1_CKOUT_GPIO_PIN           GPIO_PIN_10
#define AUDIO_DFSDM1_CKOUT_GPIO_AF            GPIO_AF6_DFSDM1

#define AUDIO_DFSDM1_DATIN1_GPIO_PORT         GPIOD
#define AUDIO_DFSDM1_DATIN1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define AUDIO_DFSDM1_DATIN1_GPIO_PIN          GPIO_PIN_6
#define AUDIO_DFSDM1_DATIN1_GPIO_AF           GPIO_AF6_DFSDM1

#define AUDIO_DFSDM1_CLK_ENABLE()             __HAL_RCC_DFSDM1_CLK_ENABLE()
#define AUDIO_DFSDM1_CLK_DISABLE()            __HAL_RCC_DFSDM1_CLK_DISABLE()
/**
  * @}
  */

/** @addtogroup STM32L552E-EV_AUDIO_Exported_Variables
  * @{
  */
/* Audio in and out context */
extern AUDIO_OUT_Ctx_t Audio_Out_Ctx[AUDIO_OUT_INSTANCES_NBR];
extern AUDIO_IN_Ctx_t  Audio_In_Ctx[AUDIO_IN_INSTANCES_NBR];

/* Audio component object */
extern void *Audio_CompObj;

/* Audio driver */
extern AUDIO_Drv_t *Audio_Drv;

/* Audio in and out SAI handles */
extern SAI_HandleTypeDef haudio_out_sai;
extern SAI_HandleTypeDef haudio_in_sai;

/* Audio in DFSDM handles */
extern DFSDM_Channel_HandleTypeDef haudio_in_dfsdm_channel[2];
extern DFSDM_Filter_HandleTypeDef  haudio_in_dfsdm_filter[2];

/* Audio in DFSDM internal buffers and global variables */
extern int32_t  Audio_DigMic1RecBuff[BSP_AUDIO_IN_DEFAULT_BUFFER_SIZE];
extern int32_t  Audio_DigMic2RecBuff[BSP_AUDIO_IN_DEFAULT_BUFFER_SIZE];
extern uint32_t Audio_DmaDigMic1RecHalfBuffCplt;
extern uint32_t Audio_DmaDigMic1RecBuffCplt;
extern uint32_t Audio_DmaDigMic2RecHalfBuffCplt;
extern uint32_t Audio_DmaDigMic2RecBuffCplt;
/**
  * @}
  */

/** @defgroup STM32L552E-EV_AUDIO_OUT_Exported_Functions STM32L552E-EV AUDIO_OUT Exported Functions
  * @{
  */
int32_t           BSP_AUDIO_OUT_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit);
int32_t           BSP_AUDIO_OUT_DeInit(uint32_t Instance);
int32_t           BSP_AUDIO_OUT_Play(uint32_t Instance, uint8_t *pData, uint32_t NbrOfBytes);
int32_t           BSP_AUDIO_OUT_Pause(uint32_t Instance);
int32_t           BSP_AUDIO_OUT_Resume(uint32_t Instance);
int32_t           BSP_AUDIO_OUT_Stop(uint32_t Instance);
int32_t           BSP_AUDIO_OUT_Mute(uint32_t Instance);
int32_t           BSP_AUDIO_OUT_UnMute(uint32_t Instance);
int32_t           BSP_AUDIO_OUT_IsMute(uint32_t Instance, uint32_t *IsMute);
int32_t           BSP_AUDIO_OUT_SetVolume(uint32_t Instance, uint32_t Volume);
int32_t           BSP_AUDIO_OUT_GetVolume(uint32_t Instance, uint32_t *Volume);
int32_t           BSP_AUDIO_OUT_SetSampleRate(uint32_t Instance, uint32_t SampleRate);
int32_t           BSP_AUDIO_OUT_GetSampleRate(uint32_t Instance, uint32_t *SampleRate);
int32_t           BSP_AUDIO_OUT_SetDevice(uint32_t Instance, uint32_t Device);
int32_t           BSP_AUDIO_OUT_GetDevice(uint32_t Instance, uint32_t *Device);
int32_t           BSP_AUDIO_OUT_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample);
int32_t           BSP_AUDIO_OUT_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample);
int32_t           BSP_AUDIO_OUT_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr);
int32_t           BSP_AUDIO_OUT_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr);
int32_t           BSP_AUDIO_OUT_GetState(uint32_t Instance, uint32_t *State);

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
int32_t           BSP_AUDIO_OUT_RegisterDefaultMspCallbacks(uint32_t Instance);
int32_t           BSP_AUDIO_OUT_RegisterMspCallbacks(uint32_t Instance, BSP_AUDIO_OUT_Cb_t *CallBacks);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

void              BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance);
void              BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance);
void              BSP_AUDIO_OUT_Error_CallBack(uint32_t Instance);

void              BSP_AUDIO_OUT_IRQHandler(uint32_t Instance, uint32_t Device);

HAL_StatusTypeDef MX_SAI1_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t SampleRate);
HAL_StatusTypeDef MX_SAI1_Init(SAI_HandleTypeDef* hsai, MX_SAI_InitTypeDef *MXInit);
/**
  * @}
  */ 

/** @defgroup STM32L552E-EV_AUDIO_IN_Exported_Functions STM32L552E-EV AUDIO_IN Exported Functions
  * @{
  */
int32_t           BSP_AUDIO_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit);
int32_t           BSP_AUDIO_IN_DeInit(uint32_t Instance);
int32_t           BSP_AUDIO_IN_Record(uint32_t Instance, uint8_t *pData, uint32_t NbrOfBytes);
int32_t           BSP_AUDIO_IN_Pause(uint32_t Instance);
int32_t           BSP_AUDIO_IN_Resume(uint32_t Instance);
int32_t           BSP_AUDIO_IN_Stop(uint32_t Instance);
int32_t           BSP_AUDIO_IN_SetVolume(uint32_t Instance, uint32_t Volume);
int32_t           BSP_AUDIO_IN_GetVolume(uint32_t Instance, uint32_t *Volume);
int32_t           BSP_AUDIO_IN_SetSampleRate(uint32_t Instance, uint32_t SampleRate);
int32_t           BSP_AUDIO_IN_GetSampleRate(uint32_t Instance, uint32_t *SampleRate);
int32_t           BSP_AUDIO_IN_SetDevice(uint32_t Instance, uint32_t Device);
int32_t           BSP_AUDIO_IN_GetDevice(uint32_t Instance, uint32_t *Device);
int32_t           BSP_AUDIO_IN_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample);
int32_t           BSP_AUDIO_IN_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample);
int32_t           BSP_AUDIO_IN_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr);
int32_t           BSP_AUDIO_IN_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr);
int32_t           BSP_AUDIO_IN_GetState(uint32_t Instance, uint32_t *State);

#if ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1))
int32_t           BSP_AUDIO_IN_RegisterDefaultMspCallbacks(uint32_t Instance);
int32_t           BSP_AUDIO_IN_RegisterMspCallbacks(uint32_t Instance, BSP_AUDIO_IN_Cb_t *CallBacks);
#endif /* ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1)) */

void              BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance);
void              BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance);
void              BSP_AUDIO_IN_Error_CallBack(uint32_t Instance);

void              BSP_AUDIO_IN_IRQHandler(uint32_t Instance, uint32_t Device);

HAL_StatusTypeDef MX_DFSDM1_ClockConfig(DFSDM_Channel_HandleTypeDef *hDfsdmChannel, uint32_t SampleRate);
HAL_StatusTypeDef MX_DFSDM1_Init(DFSDM_Filter_HandleTypeDef *hDfsdmFilter, DFSDM_Channel_HandleTypeDef *hDfsdmChannel, MX_DFSDM_InitTypeDef *MXInit);
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

#ifdef __cplusplus
}
#endif

#endif /* STM32L552E_EVAL_AUDIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
