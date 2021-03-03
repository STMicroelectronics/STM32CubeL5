/**
  ******************************************************************************
  * @file    MetaDataManager.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version V1.0.0
  * @date    10-Sept-2019
  * @brief   Meta Data Manager APIs
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _META_DATA_MANAGER_H_
#define _META_DATA_MANAGER_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ---------------------------------------------------- */
#include <stdlib.h>

#include "MetaDataManager_Config.h"


/* Exported Defines -------------------------------------------- */

/***************************************************************************
 *           Each User could change this section                            *
 ***************************************************************************/

/* Each application could customize the Max Meta Data Size [Double Words] <=4K bytes */
#define MDM_MAX_DATASIZE_64 0x200

/* Uncomment the following define for enabling the PRINTF capability if It's supported */
#define MDM_ENABLE_PRINTF

#ifdef MDM_ENABLE_PRINTF

  /* Each application must declare it's printf implementation in MetaDataManager_Config.h if it wants to use it */
  /* #define MDM_PRINTF Something */

  /* Uncomment the following define for enabling the DEBUG of Meta Data Manager */
  //#define MDM_DEBUG_PARSING
#else /* MDM_ENABLE_PRINTF */
  #define MDM_PRINTF(...)
#endif /* MDM_ENABLE_PRINTF */

/* Imported functions for FLASH Management --------------------------*/
extern uint32_t UserFunctionForErasingFlash(void);
extern uint32_t UserFunctionForSavingFlash(void *InitMetaDataVector,void *EndMetaDataVector);

/***************************************************************************
 *           DON'T Change the following section                            *
 ***************************************************************************/

/* Magic number for checking if there is a valid Meta Data Manager in Flash */
#define MDM_VALID_META_DATA_MANAGER ((uint32_t)0x12345678)

/* Meta Data Manager Version */
#define MDM_VERSION_MAJOR 1
#define MDM_VERSION_MINOR 0
#define MDM_VERSION_PATCH 0
#define MDM_VERSION ((MDM_VERSION_MAJOR<<16) | (MDM_VERSION_MINOR<<8) | (MDM_VERSION_PATCH))

/* The Meta Data Manager is saved on the last 4Kbytes of FLASH */
#ifdef USE_STM32F4XX_NUCLEO
/* Inside of Sector 7 */
#define MDM_FLASH_ADD ((uint32_t)(0x0807F000))
#define MDM_FLASH_SECTOR FLASH_SECTOR_7
#endif /* USE_STM32F4XX_NUCLEO */

#if (defined(USE_STM32L4XX_NUCLEO) || defined (STM32L476xx) || (defined STM32L475xx))
/* Base @ of Page 510 & 511, 4 Kbytes */
#define MDM_FLASH_ADD ((uint32_t)0x080FF000)
#endif /* (defined(USE_STM32L4XX_NUCLEO) || defined(STM32L476xx) || (defined STM32L475xx)) */

#ifdef STM32L4R9xx
/* Base @ of Page 511, 4 Kbytes */
#define MDM_FLASH_ADD ((uint32_t)0x081FF000)
#endif /* STM32L4R9xx */

#ifdef STM32L562xx
/* Base @ of latest 2 Pages of BLE module Flash area, 4 Kbytes */
#define MDM_FLASH_ADD ((uint32_t)0x0804F000)
#endif /* STM32L562xx */

/* Exported macros --------------------------------------------- */
#define MCR_OSX_COPY_LICENSE_FROM_MDM(osx_lic,MetaDataManagerLic) memcpy((osx_lic),&(MetaDataManagerLic),3*4*sizeof(int32_t))
#define MCR_OSX_COPY_LICENSE_TO_MDM(osx_lic,MetaDataManagerLic) memcpy((MetaDataManagerLic),&(osx_lic),3*4*sizeof(int32_t))

/* Exported types ---------------------------------------------- */

/********************************************
 *                                          *
 *               IMPORTANT!                 *
 *               ----------                 *
 *                                          *
 *   MetaDataManager Header,                *
 *   all the MetaData Headers and each      *
 *   single payload MUST be a multiple      *
 *   of Double Words for L4 FLASH's API     *
 *                                          *
 *******************************************/

/**
* @brief  License Manager Header structure definition
*/
typedef struct {
  uint32_t IsIntialized;
  uint32_t Version;
  uint8_t *puint8_MetaData[]; // Flexible Array (C99)
} MDM_MetaDataManagerHeader_t;

/**
 * @brief Meta Data Types
 */
typedef enum
{
  MDM_DATA_TYPE_LIC =0,     // osx License Meta Data
  MDM_DATA_TYPE_GMD,        // Generic Meta Data
  MDM_DATA_TYPE_END = 0xFF, // Termination Type
} MDM_MetaDataType_t;

/**
 * @brief Generic Meta Data Header
 */
typedef struct {
  uint32_t Type;
  uint32_t Length;    // Meta Data Length In bytes
  uint8_t *puint8_PayLoad[]; // Flexible Array (C99)
} MDM_MetaDataHeader_t;

/******************************* osxLicense Meta Data PayLoad ************/

/**
 * @brief osx Licenses enumeration type
 */
typedef enum
{
  OSX_END      =0, /* Termination License */
  OSX_MOTION_FX,
  OSX_MOTION_AR,
  OSX_MOTION_CP,
  OSX_MOTION_GR,
  OSX_MOTION_PM,
  OSX_ACOUSTIC_SL,
  OSX_ACOUSTIC_BF,
  OSX_AUDIO_BV,
  OSX_MOTION_ID,
  OSX_LICENSE_NUM /* Total Number of known Licenses */
} MDM_OsxLicenseType_t;

/**
 * @brief struct used for Describing the known osx Licenses
 */
typedef struct {
  MDM_OsxLicenseType_t LicEnum;
  char osxType[8];
  char osxLibVersion[36];
} MDM_knownOsxLicense_t;

/**
 * @brief Element for Table OSX License
 */
typedef struct {
  uint32_t LicEnum;      // example: OSX_MOTION_FX
  uint8_t LicName[12];   // It's the name used for License Activation. example: FX
  uint8_t LicType[12];   // Motion/Audio/Acoustic
  uint32_t Address;       // Address
} MDM_TableLicElement_t;

/**
 * @brief Payload for one osxLicense
 */
typedef struct {
  uint32_t LicEnum;               // example: OSX_MOTION_FX
  uint32_t osxLicenseInitialized; // Is it initialized? [1/0]
  char     osxLibVersion[36];     // Library Version
  uint32_t osxLicense[3][4];      // License
  uint32_t ExtraData[16];         // Extra Data
} MDM_PayLoadLic_t;

/****************************** Generic Meta Data PayLoad *********/
/**
 * @brief Generic enumeration Meta Data type
 */
typedef enum
{
  GMD_END   =0, /* Termination GMD type */
  GMD_WIFI,                 // WIFI Access Credential
  GMD_BLUEMIX,              // IBM BlueMix
  GMD_AZURE,                // Microsoft Azure
  GMD_AMAZON,               // Amazon Cloud
  GMD_XIVELY,               // Xively Cloud
  GMD_ACC_CALIBRATION,      // Accelerometer Calibration Board
  GMD_MAG_CALIBRATION,      // Magneto Calibration Board
  GMD_NODE_NAME,            // Name of the bluetooth node
  GMD_DATA_LOG_STATUS,      // Status of the environmental and/or mens data logging on SD Card
  GMD_MEMS_DATA_FILENAME,   // File name of the last environmental and/or mens data logging
  GMD_GAS_SENSITIVITY,      // Sensitivity value for gas sensor TGS5141
  GMD_VIBRATION_PARAM,      // Vibration parameters for predictive maintenance
  GMD_POSITION,             // Microsoft Azure
  GMD_NUM /* Total Number of known Generic Meta Data types */
} MDM_GenericMetaDataType_t;

/**
 * @brief Payload for Generic Meta Data Type
 */
typedef struct {
  uint32_t GMDTypeEnum;  // example: GMD_BLUEMIX
  uint32_t GMDSize;      // Size of the Meta Data To Save [bytes]
  uint8_t *puint8_GMD[]; // Flexible Array (C99)
} MDM_PayLoadGMD_t;

/**
 * @brief Element for Generic Meta Data Table
 */
typedef struct {
  uint32_t GMDTypeEnum;  // example: GMD_BLUEMIX
  uint8_t GMDName[16];   // example BLUEMIX
  uint32_t GMDSize;      // Size of the Generic Meta Data To Save [bytes]
  uint32_t Address;      // Address
} MDM_TableGMDElement_t;

/**
 * @brief struct used for Describing the known Generic Meta Data
 */
typedef struct {
  MDM_GenericMetaDataType_t GMDType;
  uint32_t GMDSize; // Size of the Generic Meta Data To Save [bytes]
} MDM_knownGMD_t;

/* Imported Variables ------------------------------------------ */
/* This Table MUST be defined by each application */
//extern MDM_knownOsxLicense_t known_OsxLic[];
/* Table with All the known osx License */


/* Exported Variables ------------------------------------------ */
extern MDM_TableLicElement_t MDM_LicTable[];
extern MDM_TableGMDElement_t MDM_GMDTable[];
extern uint32_t NecessityToSaveMetaDataManager;

/* Exported functions ------------------------------------------------------- */
extern void InitMetaDataManager(void *Ptr,...);
extern void SaveMetaDataManager(void);
extern void ReseMetaDataManager(void);
extern uint32_t EraseMetaDataManager(void);

extern uint32_t MetaDataLicenseStatus(MDM_OsxLicenseType_t osxLic);
extern int32_t MDM_LicenseParsing(uint8_t * att_data, uint8_t data_length);

extern uint32_t MDM_SaveGMD(MDM_GenericMetaDataType_t GMDType,void *GMD);
extern uint32_t MDM_ReCallGMD(MDM_GenericMetaDataType_t GMDType,void *GMD);

/* Control Section ----------------------------------------------------------- */
#if (MDM_MAX_DATASIZE_64>0x200)
  #error "MDM_MAX_DATASIZE_64 Could not be bigger than 4KBytes"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _META_DATA_MANAGER_H_ */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

