/**
  ******************************************************************************
  * @file    storage.h
  * @author  MCD Application Team
  * @brief   Header for storage.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STORAGE_H
#define __STORAGE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ff_gen_drv.h"

#if defined(USE_SDCARD)
#include "sd_diskio.h"
#endif /* USE_SDCARD */

/* Exported constants --------------------------------------------------------*/
#define FILEMGR_LIST_DEPDTH                    24
#define FILEMGR_FILE_NAME_SIZE                256
#define FILEMGR_MAX_LEVEL                       3
#define FILEMGR_MAX_EXT_SIZE                    3

#define FILETYPE_DIR                            0
#define FILETYPE_FILE                           1

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  MSD_DISK_UNIT ,
  NUM_DISK_UNITS
}
STORAGE_IdTypeDef;

typedef enum
{
  STORAGE_NOINIT
, STORAGE_UNMOUNTED /* same as STORAGE_CONNECTED */
, STORAGE_MOUNTED
} STORAGE_Status_t;

typedef enum
{
  USBDISK_DISCONNECTION_EVENT = 1,
  USBDISK_CONNECTION_EVENT,
  USBDISK_ACTIVE_CLASS_EVENT,
  MSDDISK_DISCONNECTION_EVENT,
  MSDDISK_CONNECTION_EVENT,
}
STORAGE_EventTypeDef;

typedef struct _FILELIST_LineTypeDef
{
  uint8_t               type;
  uint8_t               name[FILEMGR_FILE_NAME_SIZE];
  char                  full_name[FILEMGR_FILE_NAME_SIZE];

  uint32_t              length;

}
FILELIST_LineTypeDef;

typedef struct _FILELIST_FileTypeDef
{
  FILELIST_LineTypeDef  file[FILEMGR_LIST_DEPDTH] ;
  uint16_t              ptr;
  int16_t               FileIndex;
}
FILELIST_FileTypeDef;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern FILELIST_FileTypeDef FileList;
//extern osMessageQueueId_t osStorageEventQueueHandle;

/* Exported functions ------------------------------------------------------- */
void        Storage_Init(void);
void        Storage_DeInit(void);
uint8_t     Storage_GetStatus (uint8_t unit);
const char *Storage_GetDrive (uint8_t unit);
FRESULT     Storage_ParseAudioFiles(char *dirname);

#ifdef __cplusplus
}
#endif

#endif /*__STORAGE_H */
