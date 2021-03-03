/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SAI/SAI_AudioPlay/Src/stm32l5xx_main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use SAI HAL API (SAI instance)
  *          to play an audio file using the DMA circular mode and a
  *          continuous buffer update.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l552e_eval.h"
#include "stm32l552e_eval_bus.h"
#include "audio.h"
#include "mfxstm32l152.h"
#include "../Components/cs42l51/cs42l51.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AUDIO_I2C_ADDRESS    0x94U
#define AUDIO_FREQUENCY_22K 22050U  /* Audio sample rate */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_b;

/* USER CODE BEGIN PV */
#define AUDIO_FILE_ADDRESS   0x08040000
#define AUDIO_FILE_SIZE      (180*1024)
#define PLAY_HEADER          0x2C
#define PLAY_BUFF_SIZE       4096
#define MFX_IO_EXPANDER_I2C_ADDRESS            0x84U


static void *comp_out_obj= {0};
AUDIO_Drv_t *audio_drv;
CS42L51_IO_t IOCtx;
uint32_t cs42l51_id;
static CS42L51_Object_t CS42L51Obj;
CS42L51_Init_t codec_init;
uint16_t PlayBuff[PLAY_BUFF_SIZE];
__IO int16_t UpdatePointer = -1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SAI1_Init(void);
/* USER CODE BEGIN PFP */
static void CS42L51_PowerUp(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t PlaybackPosition = PLAY_BUFF_SIZE + PLAY_HEADER;


  /* STM32L5xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* Configure LED4 */
  BSP_LED_Init(LED4);

  /* Configure LED5 */
  BSP_LED_Init(LED5);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SAI1_Init();
  /* USER CODE BEGIN 2 */
  CS42L51_PowerUp();
  /* Configure the audio driver */
  IOCtx.Address     = AUDIO_I2C_ADDRESS;
  IOCtx.Init        = BSP_I2C1_Init;
  IOCtx.DeInit      = BSP_I2C1_DeInit;
  IOCtx.ReadReg     = BSP_I2C1_ReadReg;
  IOCtx.WriteReg    = BSP_I2C1_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;
  if (CS42L51_RegisterBusIO(&CS42L51Obj, &IOCtx) != CS42L51_OK)
  {
    Error_Handler();
  }
  else
  {
    if(CS42L51_ReadID(&CS42L51Obj, &cs42l51_id) != CS42L51_OK)
    {
      Error_Handler();
    }
    else if ((cs42l51_id & CS42L51_ID_MASK) != CS42L51_ID)
    {
      Error_Handler();
    }
    /* Reset the Codec Registers */
    else if (CS42L51_Reset(&CS42L51Obj) != CS42L51_OK)
    {
      Error_Handler();
    }
  }

  audio_drv               = (AUDIO_Drv_t *)&CS42L51_Driver;
  comp_out_obj            = &CS42L51Obj;
  codec_init.Resolution   = CS42L51_RESOLUTION_16B;
  codec_init.Frequency    = AUDIO_FREQUENCY_22K;
  codec_init.InputDevice  = CS42L51_IN_NONE;
  codec_init.OutputDevice = CS42L51_OUT_HEADPHONE;
  codec_init.Volume       = 70;
  if(0 != audio_drv->Init(comp_out_obj, &codec_init))
  {
    Error_Handler();
  }

  /* Check if the buffer has been loaded in flash */
  if(*((uint64_t *)AUDIO_FILE_ADDRESS) != 0x017EFE2446464952 ) Error_Handler();

  /* Initialize the data buffer */
  for(int i=0; i < PLAY_BUFF_SIZE; i+=2)
  {
    PlayBuff[i]=*((__IO uint16_t *)(AUDIO_FILE_ADDRESS + PLAY_HEADER + i));
  }

  /* Start the playback */
  if(0 != audio_drv->Play(&CS42L51Obj))
  {
    Error_Handler();
  }

  if(HAL_OK != HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t *)PlayBuff, PLAY_BUFF_SIZE))
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    BSP_LED_Toggle(LED4);

    /* Wait a callback event */
    while(UpdatePointer==-1);

    int position = UpdatePointer;
    UpdatePointer = -1;

    /* Update the first or the second part of the buffer */
    for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
    {
      PlayBuff[i+position] = *(uint16_t *)(AUDIO_FILE_ADDRESS + PlaybackPosition);
      PlaybackPosition+=2;
    }

    /* check the end of the file */
    if((PlaybackPosition+PLAY_BUFF_SIZE/2) > AUDIO_FILE_SIZE)
    {
      PlaybackPosition = PLAY_HEADER;
    }

    if(UpdatePointer != -1)
    {
      /* Buffer update time is too long compare to the data transfer time */
      Error_Handler();
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */
  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockB1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockB1.Init.DataSize = SAI_DATASIZE_16;
  hsai_BlockB1.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockB1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockB1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_22K;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  hsai_BlockB1.Init.PdmInit.Activation = DISABLE;
  hsai_BlockB1.Init.PdmInit.MicPairsNbr = 1;
  hsai_BlockB1.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;
  hsai_BlockB1.FrameInit.FrameLength = 32;
  hsai_BlockB1.FrameInit.ActiveFrameLength = 16;
  hsai_BlockB1.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
  hsai_BlockB1.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockB1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockB1.SlotInit.FirstBitOffset = 0;
  hsai_BlockB1.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  hsai_BlockB1.SlotInit.SlotNumber = 2;
  hsai_BlockB1.SlotInit.SlotActive = 0x00000003;
  if (HAL_SAI_Init(&hsai_BlockB1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
/**
  * @brief Tx Transfer completed callbacks.
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_TxCpltCallback could be implemented in the user file
   */
  UpdatePointer = PLAY_BUFF_SIZE/2;
}

/**
  * @brief Tx Transfer Half completed callbacks
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_TxHalfCpltCallback could be implenetd in the user file
   */
  UpdatePointer = 0;
}


/**
  * @brief CODEC Activation Function
  * @param None
  * @retval None
  */
static void CS42L51_PowerUp(void)
{
  MFXSTM32L152_Object_t   MFXSTM32L152Obj = {0};
  MFXSTM32L152_IO_Init_t  MFX_IO_Init= {0};
  MFXSTM32L152_IO_t       MFX_IOCtx;
  uint32_t                mfxstm32l152_id;
  /* Configure the MFX_IO_EXPANDER driver */
  MFX_IOCtx.Init        = BSP_I2C1_Init;
  MFX_IOCtx.DeInit      = BSP_I2C1_DeInit;
  MFX_IOCtx.ReadReg     = BSP_I2C1_ReadReg;
  MFX_IOCtx.WriteReg    = BSP_I2C1_WriteReg;
  MFX_IOCtx.GetTick     = BSP_GetTick;
  MFX_IOCtx.Address     = 0x84U;

  if (MFXSTM32L152_RegisterBusIO(&MFXSTM32L152Obj, &MFX_IOCtx) != MFXSTM32L152_OK)
  {
    Error_Handler();
  }
  else if (MFXSTM32L152_ReadID(&MFXSTM32L152Obj, &mfxstm32l152_id) != MFXSTM32L152_OK)
  {
    Error_Handler();
  }
  else
  {
    if ((mfxstm32l152_id == MFXSTM32L152_ID) || (mfxstm32l152_id == MFXSTM32L152_ID_2))
    {
      if (MFXSTM32L152_Init(&MFXSTM32L152Obj) != MFXSTM32L152_OK)
      {
        Error_Handler();
      }
    }    

  }
  
  if (MFXSTM32L152_IO_Start(& MFXSTM32L152Obj,MFXSTM32L152_GPIO_PIN_15) != MFXSTM32L152_OK)
  {
    Error_Handler();
  }  
  
      
  MFX_IO_Init.Pin  = MFXSTM32L152_GPIO_PIN_15;
  MFX_IO_Init.Mode = MFXSTM32L152_GPIO_MODE_OUTPUT_PP;
  MFX_IO_Init.Pull = MFXSTM32L152_GPIO_PULLDOWN;  
  if (MFXSTM32L152_IO_Init(& MFXSTM32L152Obj, & MFX_IO_Init)!= MFXSTM32L152_OK)
  {
   Error_Handler();
  }
  else 
  {
    /* Un-reset the CS42L51 */
    MFXSTM32L152_IO_WritePin(&MFXSTM32L152Obj, MFXSTM32L152_GPIO_PIN_15, 1U);
  }
    
  
  /* Wait 1ms according CS42L51 datasheet */
  HAL_Delay(1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* LED5 On in error case */
  BSP_LED_On(LED5);
  while(1)
  {
  }

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
