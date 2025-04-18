/**
  ******************************************************************************
  * @file    storage.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel storage functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "audio_player_app.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_STORAGE
  * @brief Kernel storage routines
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define STORAGE_BSP_INIT

#define STORAGE_THREAD_STACK_SIZE       configMINIMAL_STACK_SIZE
#define STORAGE_THREAD_PRIORITY         osPriorityNormal

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ )
#pragma data_alignment=32
#pragma location="sdio_heap"
#else
__attribute__((section(".sdio_heap"))) __attribute__ ((aligned (32)))
#endif
static FATFS              StorageDISK_FatFs[NUM_DISK_UNITS];          /* File system object for MSD disk logical drive */
static char               StorageDISK_Drive[NUM_DISK_UNITS+1][4];       /* Storage Host logical drive number */
static osSemaphoreId_t    osStorageSemaphoreHandle[NUM_DISK_UNITS];
#if defined(USE_SDCARD)
static Diskio_drvTypeDef const * Storage_Driver[NUM_DISK_UNITS];
#endif /* SE_SDCARD */

static  uint8_t           StorageID[NUM_DISK_UNITS];
static STORAGE_Status_t   StorageStatus[NUM_DISK_UNITS];

/* Definitions for osThread */
osThreadId_t osStorageThreadHandle = 0;
const osThreadAttr_t osStorageThread_attributes = {
  .name = "STORAGE",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = STORAGE_THREAD_STACK_SIZE
};

/* Definitions for osMessageQueue */
osMessageQueueId_t osStorageEventQueueHandle = 0;
const osMessageQueueAttr_t osStorageEventQueue_attributes = {
  .name = "osStorageEventQueue"
};

uint32_t   osStorageEventQueueMsg;

FILELIST_FileTypeDef FileList;
static char full_path[_MAX_LFN];
static char parent_dir[_MAX_LFN];
static int level ;

/* Private function prototypes -----------------------------------------------*/
#if defined(USE_SDCARD)
static STORAGE_Status_t StorageTryMount( const uint8_t unit );
static STORAGE_Status_t StorageTryUnMount( const uint8_t unit );
#endif /* SE_SDCARD */
static void StorageThread(void *argument);
#if defined(USE_SDCARD)
static uint8_t StorageInitMSD(void);
#endif /* USE_SDCARD */

/* Private functions ---------------------------------------------------------*/
#if defined(USE_SDCARD)
static STORAGE_Status_t StorageTryMount( const uint8_t unit )
{
  osSemaphoreAcquire(osStorageSemaphoreHandle[unit], osWaitForever);

#if defined(USE_SDCARD)
  if(unit == MSD_DISK_UNIT)
  {
    /* We need to check for SD Card before mounting the volume */
    if(!BSP_SD_IsDetected(0))
    {
      StorageStatus[unit] = STORAGE_UNMOUNTED;
      goto unlock_exit;
    }
  }
#endif /* USE_SDCARD */

  if(StorageStatus[unit] != STORAGE_MOUNTED)
  {
    /* Link the disk I/O driver */
    if(FATFS_LinkDriver(Storage_Driver[unit], StorageDISK_Drive[unit]) != 0)
      goto unlock_exit;

    if(f_mount(&StorageDISK_FatFs[unit], StorageDISK_Drive[unit], 0))
      goto unlock_exit;

    /* Set SD storage status */
    // if(unit == MSD_DISK_UNIT)
      StorageStatus[unit] = STORAGE_MOUNTED;
  }

unlock_exit:
  osSemaphoreRelease(osStorageSemaphoreHandle[unit]);
  return StorageStatus[unit];
}

static STORAGE_Status_t StorageTryUnMount( const uint8_t unit )
{
  if(StorageID[unit] == 0)
    return StorageStatus[unit];

  osSemaphoreAcquire(osStorageSemaphoreHandle[unit], osWaitForever);

  if(StorageStatus[unit] != STORAGE_MOUNTED)
    goto unlock_exit;

  if(f_mount(0, StorageDISK_Drive[unit], 0))
    goto unlock_exit;

  if(FATFS_UnLinkDriver(StorageDISK_Drive[unit]))
    goto unlock_exit;

  memset(StorageDISK_Drive[unit], 0, sizeof(StorageDISK_Drive[unit]));
  /* Reset storage status */
  StorageStatus[unit] = STORAGE_UNMOUNTED;

unlock_exit:
  osSemaphoreRelease(osStorageSemaphoreHandle[unit]);
  return StorageStatus[unit];
}
#endif /* USE_SDCARD */

/**
  * @brief  Storage Thread
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
static void StorageThread(void *argument)
{
  osStatus_t status;

  for( ;; )
  {
    status = osMessageQueueGet(osStorageEventQueueHandle, &osStorageEventQueueMsg, NULL, osWaitForever);

    if( status == osOK )
    {
      switch(osStorageEventQueueMsg)
      {
#if defined(USE_SDCARD)
        case MSDDISK_CONNECTION_EVENT:
#ifdef STORAGE_BSP_INIT
          /* Enable SD Interrupt mode */
          if(BSP_SD_DetectITConfig(0) == BSP_ERROR_NONE)
          {
            if(BSP_SD_Init(0) == BSP_ERROR_NONE)
              StorageTryMount(MSD_DISK_UNIT);
          }
#else
          StorageTryMount(MSD_DISK_UNIT);
#endif /* STORAGE_BSP_INIT */
        break;

        case MSDDISK_DISCONNECTION_EVENT:
          StorageTryUnMount(MSD_DISK_UNIT);
#ifdef STORAGE_BSP_INIT
          BSP_SD_DeInit(0);
#endif /* STORAGE_BSP_INIT */
        break;
#endif /* USE_SDCARD */
        default:
        break;
      }
    }
  }
}

#if defined(USE_SDCARD)
/**
  * @brief  MSD Storage initialization
  * @param  None
  * @retval uint8_t Storage status
  */
static uint8_t StorageInitMSD(void)
{
  uint8_t sd_status = BSP_ERROR_NONE;

  if(StorageID[MSD_DISK_UNIT] != 0)
    return StorageID[MSD_DISK_UNIT];

#ifdef STORAGE_BSP_INIT
  sd_status = BSP_SD_DetectITConfig(0);
  if( sd_status == BSP_ERROR_NONE )
  {
    /* Configure SD Interrupt mode */
    sd_status = BSP_SD_Init(0);
  }
#endif /* STORAGE_BSP_INIT */

  if(sd_status == BSP_ERROR_NONE)
  {
    /* Create Storage Semaphore */
    osStorageSemaphoreHandle[MSD_DISK_UNIT] = osSemaphoreNew(1, 1, NULL);
    /* Mark the storage as initialized */
    StorageID[MSD_DISK_UNIT] = 1;
    Storage_Driver[MSD_DISK_UNIT] = &SD_Driver;
    /* Try mount the storage */
    StorageTryMount(MSD_DISK_UNIT);
  }
  /* parse audio files: wav and mp3 */
  Storage_ParseAudioFiles(StorageDISK_Drive[0]);

  return StorageID[MSD_DISK_UNIT];
}

/**
  * @brief  MSD Storage destruction
  * @param  None
  * @retval uint8_t Storage status
  */
static void StorageDeInitMSD(void)
{
#ifdef STORAGE_BSP_INIT
  BSP_SD_DeInit(0);
#endif /* STORAGE_BSP_INIT */

  if(osStorageSemaphoreHandle[MSD_DISK_UNIT])
  {
    osSemaphoreDelete(osStorageSemaphoreHandle[MSD_DISK_UNIT]);
    osStorageSemaphoreHandle[MSD_DISK_UNIT] = 0;
  }
}
#endif /* USE_SDCARD */

/**
  * @brief  Storage drives initialization
  * @param  None
  * @retval None
  */
void Storage_Init(void)
{
  uint8_t storage_id = 0;

  /* Reset All storage status */
  for(storage_id = 0; storage_id < sizeof(StorageStatus); storage_id++)
  {
    StorageStatus[storage_id] = STORAGE_NOINIT;
  }

  memset(StorageDISK_Drive[NUM_DISK_UNITS], 0, sizeof(StorageDISK_Drive[NUM_DISK_UNITS]));

  /* Create Storage Message Queue */
  osStorageEventQueueHandle = osMessageQueueNew (30, sizeof(uint16_t), &osStorageEventQueue_attributes);

#if defined(USE_SDCARD)
  /* Initialize the MSD Storage */
  StorageInitMSD();
#endif /* USE_SDCARD */

  /* Check for configured Storages then create Storage thread */
  for(storage_id = 0; storage_id < sizeof(StorageStatus); storage_id++)
  {
    if( StorageID[storage_id] )
    {
      /* Create Storage task */
      osStorageThreadHandle = osThreadNew(StorageThread, NULL, &osStorageThread_attributes);

      return;
    }
  }

  /* Something went wrong */
  //Error_Handler();
}

/**
  * @brief  Storage drives de-initialization
  * @param  None
  * @retval None
  */
void Storage_DeInit(void)
{
#if defined(USE_SDCARD) || defined(USE_USB_FS) || defined(USE_USB_HS)
  uint8_t storage_id = 0;

  /* Try Unmount All storage */
  for(storage_id = 0; storage_id < sizeof(StorageStatus); storage_id++)
  {
    StorageTryUnMount(storage_id);
  }
#endif /* SE_SDCARD || USE_USB_FS || USE_USB_HS */

  /* Terminate Storage background task */
  if(osStorageThreadHandle != 0)
  {
    osThreadTerminate(osStorageThreadHandle);
    osStorageThreadHandle = 0;
  }

  /* Delete Storage Message Queue */
  if(osStorageEventQueueHandle != 0)
  {
    osMessageQueueDelete(osStorageEventQueueHandle);
    osStorageEventQueueHandle = 0;
  }

#if defined(USE_SDCARD)
  /* DeInit MSD Storage */
  StorageDeInitMSD();
#endif /* USE_SDCARD */
}

/**
  * @brief  Storage get status
  * @param  unit: logical storage unit index.
  * @retval int
  */
uint8_t Storage_GetStatus (uint8_t unit)
{
  uint8_t timeout = 10;
  uint8_t status = STORAGE_NOINIT;

  /* If it is just initialized then please wait for a while till init is done */
  while(timeout && (StorageStatus[unit] == STORAGE_NOINIT))
  {
    timeout--;
    osDelay(100);
  }

  if(StorageID[unit])
  {
    osSemaphoreAcquire(osStorageSemaphoreHandle[unit], osWaitForever);
    status = (StorageStatus[unit] == STORAGE_MOUNTED);
    osSemaphoreRelease(osStorageSemaphoreHandle[unit]);
  }

  return status;
}


/**
  * @brief  Storage get drive
  * @param  unit: logical storage unit index.
  * @retval char[4]
  */
const char *Storage_GetDrive (uint8_t unit)
{
  uint8_t timeout = 10;

  /* If it is just initialized then please wait for a while till init is done */
  while(timeout && (StorageStatus[unit] == STORAGE_NOINIT))
  {
    timeout--;
    osDelay(100);
  }

  if(StorageStatus[unit] == STORAGE_MOUNTED)
    return StorageDISK_Drive[unit];
  else
    return StorageDISK_Drive[NUM_DISK_UNITS];
}

#if defined(USE_SDCARD)
/**
  * @brief  BSP SD Callback.
  * @param  Instance SD Instance
  * @param  Status   Pin status
  * @retval None.
  */
void BSP_SD_DetectCallback( uint32_t Instance, uint32_t Status )
{
  if(Status == SD_NOT_PRESENT)
  {
    HAL_NVIC_SystemReset();
  }
}
#endif /* USE_SDCARD */

/**
  * @brief  Parse/find files recursively in given directory
  * @param  dirname: Pointer to the directory to open
  * @retval File function return code.
  */
FRESULT Storage_ParseAudioFiles(char *dirname)
{
  FRESULT res = FR_OK;  /* Return value */
  DIR dir;              /* Directory search object */
  FILINFO fno;          /* File information */

  if (level < 0)
  {
    FileList.ptr = 0;
  }
  Audio_InfoTypeDef AudioInfo;

  /* Start to search for wave files */
  res = f_findfirst(&dir, &fno, dirname, "*");

  /* Repeat while an item is found */
  while (fno.fname[0])
  {
    if(res == FR_OK)
    {
      if(FileList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if((fno.fattrib & AM_DIR) == 0)
        {
          if((strstr(fno.fname, "wav")) || (strstr(fno.fname, "WAV"))  || (strstr(fno.fname, "mp3")) || (strstr(fno.fname, "MP3")))
          {
            strncpy((char *)FileList.file[FileList.ptr].name, (char *)fno.fname, FILEMGR_FILE_NAME_SIZE);
            strncat((char *)FileList.file[FileList.ptr].full_name, dirname, strlen(dirname));
            strncat((char *)FileList.file[FileList.ptr].full_name, "/", 1);
            strncat((char *)FileList.file[FileList.ptr].full_name, (char *)fno.fname,FILEMGR_FILE_NAME_SIZE - strlen(dirname) - 1);
            FileList.file[FileList.ptr].type = FILETYPE_FILE;
            memset(&AudioInfo,0x00,sizeof(Audio_InfoTypeDef));
            AUDIOPLAYER_GetFileInfo(FileList.file[FileList.ptr].full_name,&AudioInfo);
            FileList.file[FileList.ptr].length = AudioInfo.Duration;
            FileList.ptr++;
          }
        }
        else
        {
          memset(full_path, '\0', _MAX_LFN);

          level++;
          if (level <= 0)
          {
            memset(parent_dir, '\0', _MAX_LFN);
            strcat(parent_dir, dirname);
          }

          int i = strlen(parent_dir);

          sprintf(full_path, "%s/%s", parent_dir, fno.fname);

          sprintf(parent_dir, full_path);

          /*
          * search media files in the subdirectory
          */
          Storage_ParseAudioFiles(full_path);
          level--;
          parent_dir[i] = 0;

        }
        /* Search for next item */
        res = f_findnext(&dir, &fno);
      }
    }
    else
    {
      break;
    }
  }

  f_closedir(&dir);
  return res;
}

/**
  * @}
  */

/**
  * @}
  */
