/**
  ******************************************************************************
  * @file    MetaDataManager.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version V1.0.0
  * @date    10-Sept-2019
  * @brief   Meta Data Manager APIs implementation
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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "MetaDataManager.h"

/* Local variables --------------------------------------------------*/
/* We move to the first MetaData position after the Meta Data Manager header */
/* Vector of Meta Data Header + Meta Data.
 * Static allocation for avoiding runtime problems */
static uint64_t uint64_MetaDataVector[MDM_MAX_DATASIZE_64];

static uint8_t *puint8_RW_MetaData = ((uint8_t *) uint64_MetaDataVector)+8;

static MDM_MetaDataManagerHeader_t *pMetaDataManagerHeader = (MDM_MetaDataManagerHeader_t *) uint64_MetaDataVector;

static uint32_t NumberOfKnownLic=0;
static uint32_t NumberOfKnownGMD=0;

MDM_knownOsxLicense_t known_OsxLic[]={
  {OSX_END,"LAST",""}/* THIS MUST BE THE LAST ONE */
};

/* Exported Variables ------------------------------------------ */

uint32_t NecessityToSaveMetaDataManager=0;

/* Table used for osxLicense Mapping */
MDM_TableLicElement_t MDM_LicTable[OSX_LICENSE_NUM] = {
  {OSX_END        ,"00","NULL"    ,0},
  {OSX_MOTION_FX  ,"FX","Motion"  ,0},
  {OSX_MOTION_AR  ,"AR","Motion"  ,0},
  {OSX_MOTION_CP  ,"CP","Motion"  ,0},
  {OSX_MOTION_GR  ,"GR","Motion"  ,0},
  {OSX_MOTION_PM  ,"PM","Motion"  ,0},
  {OSX_ACOUSTIC_SL,"SL","Acoustic",0},
  {OSX_ACOUSTIC_BF,"BF","Acoustic",0},
  {OSX_AUDIO_BV   ,"BV","Audio"   ,0},
  {OSX_MOTION_ID  ,"ID","Motion"  ,0}
};

/* Table used for Generic Meta Data Mapping */
MDM_TableGMDElement_t MDM_GMDTable[GMD_NUM] = {
  {GMD_END               ,"NULL"            ,0,0},
  {GMD_WIFI              ,"WIFI"            ,0,0},
  {GMD_BLUEMIX           ,"BLUEMIX"         ,0,0},
  {GMD_AZURE             ,"AZURE"           ,0,0},
  {GMD_AMAZON            ,"AMAZON"          ,0,0},
  {GMD_XIVELY            ,"XIVELY"          ,0,0},
  {GMD_ACC_CALIBRATION   ,"ACC_CALIBRATION" ,0,0},
  {GMD_MAG_CALIBRATION   ,"MAG_CALIBRATION" ,0,0},
  {GMD_NODE_NAME         ,"NODE_NAME"       ,0,0},
  {GMD_DATA_LOG_STATUS   ,"DATA_LOG_STATUS" ,0,0},
  {GMD_MEMS_DATA_FILENAME,"MEMS_DATA_FILE"  ,0,0},
  {GMD_GAS_SENSITIVITY   ,"GAS SENSITIVITY" ,0,0},
  {GMD_VIBRATION_PARAM   ,"VIBRATION_PARAM" ,0,0},
  {GMD_POSITION          ,"POSITION"        ,0,0}
};



/* Imported Functions -----------------------------------------------*/

/* Local prototypes -------------------------------------------------*/
static uint32_t ReCallMetaDataManager(void);
#ifdef MDM_ENABLE_PRINTF
static void PrintOut_MDM_LicenseStatus(void);
static void PrintOut_MDM_GMDStatus(void);
#endif /* MDM_ENABLE_PRINTF */

/**
  * @brief  Initialize the MetaData manager
  * @param  void *Ptr list to the couple of MetaData&MetaDataType
  * @retval None
  */
void InitMetaDataManager(void *Ptr,...) {
  MDM_MetaDataHeader_t *pMetaDataHeader;
  uint32_t NumberofFoundedLic=0;
  uint32_t NumberofFoundedGMD=0;
  void *KnownMetaDataTable;
  va_list ap;

  /* At the Init we start always from the beginning */
  puint8_RW_MetaData = ((uint8_t *) uint64_MetaDataVector)+8;

#ifdef MDM_DEBUG_PARSING
  MDM_PRINTF("Initial Value puint8_RW_MetaData=%x\r\n",puint8_RW_MetaData);
#endif /* MDM_DEBUG_PARSING */
  
  /* Read the Meta Data Manager from FLASH */
  if(ReCallMetaDataManager()==0) {
    /* Meta Data Manager not present or not Compatible */
    ReseMetaDataManager();
    NumberofFoundedLic=0;
    NumberofFoundedGMD=0;
  }

  /* Initialize the argument list. */
  va_start (ap, Ptr);

  KnownMetaDataTable= Ptr;
  while(KnownMetaDataTable!=NULL) {
    uint32_t Index;
    MDM_knownOsxLicense_t *known_OsxLic=NULL;
    MDM_knownGMD_t *known_GMD=NULL;

    MDM_MetaDataType_t  MetaDataType = (MDM_MetaDataType_t) va_arg(ap,int);

    switch(MetaDataType) {
      case MDM_DATA_TYPE_LIC:
        known_OsxLic = (MDM_knownOsxLicense_t *) KnownMetaDataTable;
        /* Compute the Number of known Licenses */
        for(Index =0; known_OsxLic[Index].LicEnum!=OSX_END ;Index++) {
          NumberOfKnownLic++;
        }
#ifdef MDM_DEBUG_PARSING
        MDM_PRINTF("Init MDM with %d osxLicenses\r\n",NumberOfKnownLic);
#endif /* MDM_DEBUG_PARSING */
      break;
     case MDM_DATA_TYPE_GMD:
        known_GMD = (MDM_knownGMD_t *) KnownMetaDataTable;
        /* Compute the Number of known Generic Meta Data Types */
        for(Index =0; known_GMD[Index].GMDType!=GMD_END ;Index++) {
          NumberOfKnownGMD++;
        }
#ifdef MDM_DEBUG_PARSING
        MDM_PRINTF("Init MDM with %d Generic Meta Data\r\n",NumberOfKnownGMD);
#endif /* MDM_DEBUG_PARSING */
      break;
      default:
        MDM_PRINTF("Error Unknow Meta Data Type\r\n");
        return;
    }

    /* Parse the Actual Content of MetaData Vector starting from beginning */
    pMetaDataHeader = (MDM_MetaDataHeader_t *) pMetaDataManagerHeader->puint8_MetaData;

    while(pMetaDataHeader->Type!=MDM_DATA_TYPE_END) {
      /* Until we reach the end of the Meta Data Manager */

      if(pMetaDataHeader->Type == MDM_DATA_TYPE_LIC) {
        /* The Meta Data is one osx License...we need to understand if we know it */
        int32_t found=0;
        MDM_PayLoadLic_t *PayLoad = (MDM_PayLoadLic_t *) pMetaDataHeader->puint8_PayLoad;

        /* Loop over all the known osx Licenses */
        for(Index =0; ((Index<NumberOfKnownLic) & (found==0));Index++) {

          /* Check If we know this License */
          if(known_OsxLic[Index].LicEnum==PayLoad->LicEnum) {
            found =1;
            MDM_LicTable[PayLoad->LicEnum].Address = (uint32_t)PayLoad;
            NumberofFoundedLic++;
          }
        }
      } else if(pMetaDataHeader->Type == MDM_DATA_TYPE_GMD) {
        /* The Meta Data is one Generic Meta Data type ...we need to understand if we know it */
        int32_t found=0;
        MDM_PayLoadGMD_t *PayLoad = (MDM_PayLoadGMD_t *) pMetaDataHeader->puint8_PayLoad;

        /* Loop over all the known Generic Meta Data Types */
        for(Index =0; ((Index<NumberOfKnownGMD) & (found==0));Index++) {
          /* Check If we know this Generic Meta Data */
          if(known_GMD[Index].GMDType==PayLoad->GMDTypeEnum) {
            found =1;
            MDM_GMDTable[PayLoad->GMDTypeEnum].Address = (uint32_t)PayLoad;
            NumberofFoundedGMD++;
            MDM_GMDTable[PayLoad->GMDTypeEnum].GMDSize = PayLoad->GMDSize;
            if(known_GMD[Index].GMDSize!=PayLoad->GMDSize) {
              MDM_PRINTF("Warning: The Meta Data Manager Contains for %s Generic Meta Data a size=%ld different from what we need =%ld\r\n\tIt will be ERASED\r\n",
                         MDM_GMDTable[known_GMD[Index].GMDType].GMDName,
                         PayLoad->GMDSize,
                         known_GMD[Index].GMDSize);
              ReseMetaDataManager();
              NumberofFoundedLic=0;
              NumberofFoundedGMD=0;
              goto MDM_ADD_META_DATA;
#ifdef MDM_DEBUG_PARSING
            } else {
              MDM_PRINTF("The Meta Data Manager Contains already %s Generic Meta Data with size=%d\r\n\t",
                         MDM_GMDTable[known_GMD[Index].GMDType].GMDName,
                         PayLoad->GMDSize);
#endif /* MDM_DEBUG_PARSING */
            }
          }
        }
      }
      /* Move the next Meta Data Header */
      pMetaDataHeader = (MDM_MetaDataHeader_t *) (((uint32_t) pMetaDataHeader)+pMetaDataHeader->Length);
    }

MDM_ADD_META_DATA:
     /* Add all new Meta Data if it's necessary */
    if(MetaDataType==MDM_DATA_TYPE_LIC) {
      /* osx Licenses Section */
      if(NumberofFoundedLic<NumberOfKnownLic) {
        NecessityToSaveMetaDataManager = 1;
        /* Check if we have enough space... if not... reset everything */
        if(((((uint32_t) puint8_RW_MetaData)+8/* for the MDM_DATA_TYPE_END */) -
             ((uint32_t)uint64_MetaDataVector) +
             (NumberOfKnownLic-NumberofFoundedLic)*(sizeof(MDM_PayLoadLic_t) + 8 /* MetaDataHeader */))>(MDM_MAX_DATASIZE_64<<3)) {
          /* We don't have enough spaces.... Reset everything */
          MDM_PRINTF("Meta Data Manager has not enough free space.. It will be reseted\r\n");
          puint8_RW_MetaData = ((uint8_t *) uint64_MetaDataVector)+8;
          for(Index=1;Index<OSX_LICENSE_NUM;Index++) {
            MDM_LicTable[Index].Address =0;
          }
          NumberofFoundedLic = 0;
        }

        /* Add all the known Licenses */
        for(Index=0;Index<NumberOfKnownLic; Index++) {
#ifdef MDM_DEBUG_PARSING
          MDM_PRINTF("Testing License=%d ->%s ",Index,MDM_LicTable[known_OsxLic[Index].LicEnum].LicName);
#endif /* MDM_DEBUG_PARSING */
          if(MDM_LicTable[known_OsxLic[Index].LicEnum].Address==0) {
            MDM_PayLoadLic_t *PayLoad;
#ifdef MDM_DEBUG_PARSING
            MDM_PRINTF("License Not yet allocated\r\n");
#endif /* MDM_DEBUG_PARSING */
            /* License Not Yet Allocated */
            pMetaDataHeader = (MDM_MetaDataHeader_t *) (puint8_RW_MetaData);
#ifdef MDM_DEBUG_PARSING
            MDM_PRINTF("pMetaDataHeader=%x ",puint8_RW_MetaData);
#endif /* MDM_DEBUG_PARSING */
            pMetaDataHeader->Type = MDM_DATA_TYPE_LIC;
            pMetaDataHeader->Length = sizeof(MDM_PayLoadLic_t) + 8 /* For Meta Data Header */;
            PayLoad = (MDM_PayLoadLic_t *) pMetaDataHeader->puint8_PayLoad;
#ifdef MDM_DEBUG_PARSING
            MDM_PRINTF("PayLoad=%x ",PayLoad);
#endif /* MDM_DEBUG_PARSING */
            PayLoad->LicEnum = known_OsxLic[Index].LicEnum;
            PayLoad->osxLicenseInitialized = 0;
            sprintf((char *)PayLoad->osxLibVersion,known_OsxLic[Index].osxLibVersion);
            MDM_PRINTF("Adding=%s%s Version=%s\r\n",MDM_LicTable[known_OsxLic[Index].LicEnum].LicType,MDM_LicTable[known_OsxLic[Index].LicEnum].LicName,PayLoad->osxLibVersion);
            MDM_LicTable[PayLoad->LicEnum].Address = (uint32_t)PayLoad;
            /* Move the R/W pointer */
            puint8_RW_MetaData +=pMetaDataHeader->Length;
          }
#ifdef MDM_DEBUG_PARSING
          else {
            MDM_PRINTF("License Already allocated\r\n");
          }
#endif /* MDM_DEBUG_PARSING */
        }
        /* Write the Termination Meta Data */
        *((uint32_t *) puint8_RW_MetaData)     = MDM_DATA_TYPE_END;
        *((uint32_t *) (puint8_RW_MetaData+4)) = 0; /* No Payload */
      }
    } else if(MetaDataType==MDM_DATA_TYPE_GMD){
      /* Generic Meta Data */
      if(NumberofFoundedGMD<NumberOfKnownGMD) {
        /* We need to Add some Generic Meta Data */
        uint32_t HowManyExtraSpaceINeed =0;
        int32_t Index;
        NecessityToSaveMetaDataManager = 1;
        /* Compute how many amount of Memory I need */
        for(Index =0; Index<NumberOfKnownGMD ;Index++) {
          if(MDM_GMDTable[known_GMD[Index].GMDType].Address==0) {
            HowManyExtraSpaceINeed += (((known_GMD[Index].GMDSize+7)>>3)<<3) /* Round to Multiple of 8 bytes */ + 
              8 + 8 /* For Meta Data Header */;
          }
        }
        /* Check if there is enough space */
        if(((((uint32_t) puint8_RW_MetaData)+8/* for the MDM_DATA_TYPE_END */) -
             ((uint32_t)uint64_MetaDataVector) +
             HowManyExtraSpaceINeed)>(MDM_MAX_DATASIZE_64<<3)) {
          /* We don't have enough spaces.... Reset everything */
          MDM_PRINTF("Meta Data Manager has not enough free space.. It will be reseted\r\n");
          puint8_RW_MetaData = ((uint8_t *) uint64_MetaDataVector)+8;
          for(Index=1;Index<GMD_NUM;Index++) {
            MDM_GMDTable[Index].Address =0;
          }
          NumberofFoundedGMD = 0;
        }

        /* Add all the known Generic Meta Data Types */
        for(Index=0;Index<NumberOfKnownGMD; Index++) {
#ifdef MDM_DEBUG_PARSING
          MDM_PRINTF("Testing Generic Meta Data=%d ->%s ",Index,MDM_GMDTable[known_GMD[Index].GMDType].GMDName);
#endif /* MDM_DEBUG_PARSING */
          if(MDM_GMDTable[known_GMD[Index].GMDType].Address==0) {
            MDM_PayLoadGMD_t *PayLoad;
#ifdef MDM_DEBUG_PARSING
            MDM_PRINTF("Generic Meta Data Not yet allocated\r\n");
#endif /* MDM_DEBUG_PARSING */
            /* License Not Yet Allocated */
            pMetaDataHeader = (MDM_MetaDataHeader_t *) (puint8_RW_MetaData);
#ifdef MDM_DEBUG_PARSING
            MDM_PRINTF("pMetaDataHeader=%x ",puint8_RW_MetaData);
#endif /* MDM_DEBUG_PARSING */
            pMetaDataHeader->Type = MDM_DATA_TYPE_GMD;
            pMetaDataHeader->Length = (((known_GMD[Index].GMDSize+7)>>3)<<3) /* Round to Multiple of 8 bytes */ + 
              8 + 8 /* For Meta Data Header */;
            PayLoad = (MDM_PayLoadGMD_t *) pMetaDataHeader->puint8_PayLoad;
#ifdef MDM_DEBUG_PARSING
            MDM_PRINTF("PayLoad=%x ",PayLoad);
#endif /* MDM_DEBUG_PARSING */
            PayLoad->GMDTypeEnum = known_GMD[Index].GMDType;
            PayLoad->GMDSize     = known_GMD[Index].GMDSize;
            MDM_GMDTable[known_GMD[Index].GMDType].Address = (uint32_t)PayLoad;
            MDM_GMDTable[known_GMD[Index].GMDType].GMDSize = PayLoad->GMDSize;
            MDM_PRINTF("Adding=%s (Pos=%d) Size=%ld\r\n",MDM_GMDTable[known_GMD[Index].GMDType].GMDName,
                       known_GMD[Index].GMDType,
                       MDM_GMDTable[known_GMD[Index].GMDType].GMDSize);
            /* Move the R/W pointer */
            puint8_RW_MetaData +=pMetaDataHeader->Length;
          }
#ifdef MDM_DEBUG_PARSING
          else {
            MDM_PRINTF("Generic Meta Data already allocated\r\n");
          }
#endif /* MDM_DEBUG_PARSING */
        }
        /* Write the Termination Meta Data */
        *((uint32_t *) puint8_RW_MetaData)     = MDM_DATA_TYPE_END;
        *((uint32_t *) (puint8_RW_MetaData+4)) = 0; /* No Payload */
      }
    }
    KnownMetaDataTable= va_arg(ap,void *);
  }

  /* Clean up. */
  va_end (ap);

  if(NecessityToSaveMetaDataManager) {
    uint32_t Success = EraseMetaDataManager();
    if(Success) {
      SaveMetaDataManager();
    }
  }

  /* Print Out the MDM status */
#ifdef MDM_ENABLE_PRINTF
  MDM_PRINTF("Meta Data Manager version=%ld.%ld.%ld\r\n",
               ((pMetaDataManagerHeader->Version)>>16),
               ((pMetaDataManagerHeader->Version)>> 8)&0xFF,
               ((pMetaDataManagerHeader->Version)    )&0xFF);
  if((NumberOfKnownLic!=0) | (NumberofFoundedLic!=0)) {
    PrintOut_MDM_LicenseStatus();
  }
  if((NumberOfKnownGMD!=0) | (NumberofFoundedGMD!=0)) {
    PrintOut_MDM_GMDStatus();
  }
#endif /* MDM_ENABLE_PRINTF */
}

/**
 * @brief  Check if there is a valid Meta Data Manager in Flash and read it
 * @param None
 * @retval uint32_t Success/Not Success
 */
static uint32_t ReCallMetaDataManager(void)
{
  uint32_t Address = MDM_FLASH_ADD;
  __IO uint32_t data32 = *(__IO uint32_t*) Address;
  uint32_t RetValue=0;

  if(data32== MDM_VALID_META_DATA_MANAGER){
    /* Read the Meta Data Manager Header */
    pMetaDataManagerHeader->IsIntialized = data32;
    Address+=4;

    data32 = *(__IO uint32_t*) Address;
    pMetaDataManagerHeader->Version = data32;
    Address+=4;
    
    if(pMetaDataManagerHeader->Version !=  MDM_VERSION) {
      MDM_PRINTF("Warning: The Meta Data Manager in FLASH is not Compatible\r\n\tIt will be ERASED\r\n");
      return RetValue;
    }

    /* Read the all Meta Datas */
    data32 = *(__IO uint32_t*) Address;

    while(data32!=MDM_DATA_TYPE_END) {
      /* Next Meta Data is Valid */
      MDM_MetaDataHeader_t *pMetaDataHeader;
      MDM_MetaDataType_t Type;
      uint32_t Length;
      uint32_t Index;
      uint32_t *puint32_MetaData;
      RetValue =1;

      /* Meta Data Type */
      Type = (MDM_MetaDataType_t) data32;
      Address+=4;
      /* Read the Meta Data Length */
      data32 = *(__IO uint32_t*) Address;
      Length = data32;
      Address+=4;

      pMetaDataHeader = (MDM_MetaDataHeader_t *) puint8_RW_MetaData;
      pMetaDataHeader->Type = Type;
      pMetaDataHeader->Length =  Length;      
      puint32_MetaData = (uint32_t *) pMetaDataHeader->puint8_PayLoad;
      puint8_RW_MetaData +=8;

#ifdef MDM_DEBUG_PARSING
      MDM_PRINTF("Found one MetaData puint8_RW_MetaData=%x Type=%d Length=%d\r\n",puint8_RW_MetaData,Type,Length);
#endif /* MDM_DEBUG_PARSING */

      /* Fill the Meta Data Payload (Word read) */
      for(Index=0;Index<(Length-8);Index+=4) {
        data32 = *(__IO uint32_t*) (Address+Index);
        puint32_MetaData[Index>>2] = data32;
      }
      /* Move to next Meta Data Type */
      Address +=Length-8;
      puint8_RW_MetaData +=Length-8;
      data32 = *(__IO uint32_t*) Address;
    }

    /* We are reached the Meta Data that Close the Values */
    *((uint32_t *) puint8_RW_MetaData)     = MDM_DATA_TYPE_END;
    *(((uint32_t *) puint8_RW_MetaData)+4) = 0; /* No Payload */

    MDM_PRINTF("Meta Data Manager read from Flash\r\n");
  } else {
    MDM_PRINTF("Meta Data Manager not present in FLASH\r\n");
  }
  return RetValue;
}

/**
 * @brief Reset the Meta Data Manager in RAM
 * @param None
 * @retval None
 */
void ReseMetaDataManager(void) {
  MDM_OsxLicenseType_t osxLic;
  MDM_GenericMetaDataType_t GMD;

  NecessityToSaveMetaDataManager=1;
  puint8_RW_MetaData = ((uint8_t *) uint64_MetaDataVector)+8;
  pMetaDataManagerHeader->IsIntialized = MDM_VALID_META_DATA_MANAGER;
  pMetaDataManagerHeader->Version = MDM_VERSION;
  /* add the MetaData Teminitation */
  *((uint32_t *) puint8_RW_MetaData)     = MDM_DATA_TYPE_END;
  *((uint32_t *) (puint8_RW_MetaData+4)) = 0; /* No Payload */

  /* Reset all the Payload pointers */
  for(osxLic=OSX_MOTION_FX;osxLic<OSX_LICENSE_NUM;osxLic++) {
    MDM_LicTable[osxLic].Address =0;
  }
  for(GMD=GMD_WIFI;GMD<GMD_NUM;GMD++){
    MDM_GMDTable[GMD].Address =0;
  }
}

/**
 * @brief Erase the Meta Data Manager in FLASH
 * @param uint32_t Success/NotSuccess [1/0]
 * @retval None
 */
uint32_t EraseMetaDataManager(void) {
  uint32_t Success = UserFunctionForErasingFlash();
  if(Success) {
    MDM_PRINTF("Meta Data Manager erased in FLASH\r\n");
  }
  return Success;
}

/**
 * @brief Save the Meta Data Manager in Flash
 * @param None
 * @retval None
 */
void SaveMetaDataManager(void)
{
  uint32_t Success = UserFunctionForSavingFlash((void *)uint64_MetaDataVector,(void *)(puint8_RW_MetaData +8));

  if(Success) {
    MDM_PRINTF("Meta Data Manager Saved in FLASH\r\n");
  }
  NecessityToSaveMetaDataManager=0;
}

#ifdef MDM_ENABLE_PRINTF
/**
 * @brief Prints out the Actual licenses content of Meta Data Manager
 * @param None
 * @retval None
 */
static void PrintOut_MDM_LicenseStatus(void)
{
  MDM_OsxLicenseType_t osxLic;
//  MDM_PRINTF("\tLicenses found:\r\n");
  for(osxLic=OSX_MOTION_FX;osxLic<OSX_LICENSE_NUM;osxLic++) {
    MDM_PayLoadLic_t *PayLoad = (MDM_PayLoadLic_t *) MDM_LicTable[osxLic].Address;
    if(PayLoad) {
//      MDM_PRINTF("\t\t %s%s %s (%s)\r\n",MDM_LicTable[PayLoad->LicEnum].LicType,MDM_LicTable[PayLoad->LicEnum].LicName,
//                 PayLoad->osxLicenseInitialized ? "Init  " : "NoInit",
//                 PayLoad->osxLibVersion);
    }
  }
}

/**
 * @brief Prints out the Actual Generic Meta Data types Included on Meta Data Manager
 * @param None
 * @retval None
 */
static void PrintOut_MDM_GMDStatus(void)
{
  MDM_GenericMetaDataType_t GMD;
  MDM_PRINTF("\tGeneric Meta Data found:\r\n");
  for(GMD=GMD_WIFI;GMD<GMD_NUM;GMD++) {
    MDM_PayLoadGMD_t *PayLoad = (MDM_PayLoadGMD_t *) MDM_GMDTable[GMD].Address;
    if(PayLoad) {
      MDM_PRINTF("\t\t %s Size=%ld [bytes]\r\n",MDM_GMDTable[PayLoad->GMDTypeEnum].GMDName,
                 MDM_GMDTable[PayLoad->GMDTypeEnum].GMDSize);
    }
  }
}
#endif /* MDM_ENABLE_PRINTF */
/**
 * @brief  This function makes the parsing of one string for osx License management
 * @param uint8_t *att_data attribute data
 * @param uint8_t data_length length of the data
 * @retval int32_t osxLicenseStart 1/0 if we are o not activating a licenses
 */
int32_t MDM_LicenseParsing(uint8_t * att_data, uint8_t data_length)
{
  static int32_t osxLicenseStart=0;
  static uint32_t osxLicenseCount=0;
  static MDM_OsxLicenseType_t OsxLicense=OSX_END;
  static uint8_t TmpLicense[4*3*4];
  
  if(osxLicenseStart==0){
    /* No osx License activation phase */
    int32_t found=0;
    int32_t Index;
    OsxLicense = OSX_END;
    /* Loop over all the known osx Licenses */
    for(Index =0; ((Index<NumberOfKnownLic) & (found==0));Index++) {
      /* Check If we know this License */
      if(!strncmp((char *)MDM_LicTable[known_OsxLic[Index].LicEnum].LicName,(char *)(att_data),2)) {
        found =1;
        OsxLicense = (MDM_OsxLicenseType_t) MDM_LicTable[known_OsxLic[Index].LicEnum].LicEnum;
      }
    }

    if(found) {
      int32_t counter;
      MDM_PayLoadLic_t *PayLoad = (MDM_PayLoadLic_t *) MDM_LicTable[OsxLicense].Address;
      
      if(PayLoad->osxLicenseInitialized==0) {      
        osxLicenseStart=1;
        osxLicenseCount=0;
        for(counter=2;counter<data_length;counter++) {
          TmpLicense[osxLicenseCount]=att_data[counter];
          osxLicenseCount++;
        }
      } else {
        OsxLicense = OSX_END;
        MDM_PRINTF("osx%s%s Already Initialized\r\n",known_OsxLic[OsxLicense].osxType,MDM_LicTable[OsxLicense].LicName);
      }
    }
  } else {
    /* osx License activation phase */
    int32_t counter;
    for(counter=0;counter<data_length;counter++) {
      TmpLicense[osxLicenseCount]=att_data[counter];
      osxLicenseCount++;
    }

    /* if we had received enough License's bytes ...*/
    if(osxLicenseCount == (3*4*4)) {
      MDM_PayLoadLic_t *PayLoad = (MDM_PayLoadLic_t *) MDM_LicTable[OsxLicense].Address;
      uint8_t * DestLicPointer = (uint8_t *) PayLoad->osxLicense;
      /* Swap endianess */
      for(counter=0;counter<3*4*4;counter+=4) {
        DestLicPointer[counter+3] = TmpLicense[counter+0];
        DestLicPointer[counter+0] = TmpLicense[counter+3];
        DestLicPointer[counter+2] = TmpLicense[counter+1];
        DestLicPointer[counter+1] = TmpLicense[counter+2];
      }      
      osxLicenseCount=0;
      osxLicenseStart=0;
      return -OsxLicense;
    }
  }
  return osxLicenseStart;
}

/**
 * @brief Check the activation status of one osxLicense inside the Media Data Manager
 * @param uint32_t Activation status [1/0]
 * @retval None
 */
uint32_t MetaDataLicenseStatus(MDM_OsxLicenseType_t osxLic) {  
  MDM_PayLoadLic_t *PayLoad = (MDM_PayLoadLic_t *) MDM_LicTable[osxLic].Address;
  return PayLoad->osxLicenseInitialized;
}

/**
 * @brief Save one Generic Meta Data to Meta Data Manager
 * @param MDM_GenericMetaDataType_t GMDType Generic Meta Data type that we want to save
 * @param void *GMD Pointer where find the Generic Meta Data
 * @retval uin32_t Success/NotSuccess (1/0)
 */
uint32_t MDM_SaveGMD(MDM_GenericMetaDataType_t GMDType,void *GMD)
{
  uint32_t RetValue=1;
  if(MDM_GMDTable[GMDType].Address!=0) {
    MDM_PayLoadGMD_t *MDMPayLoad = (MDM_PayLoadGMD_t *)MDM_GMDTable[GMDType].Address;
    memcpy((void *)MDMPayLoad->puint8_GMD,GMD,MDMPayLoad->GMDSize);
    NecessityToSaveMetaDataManager=1;
    MDM_PRINTF("Updating the Generic Meta Data type=%s\r\n",MDM_GMDTable[GMDType].GMDName);
  } else {
    MDM_PRINTF("ERROR before saving one Generic Meta Datait's necessary Initializes the MDM with the same type=%d\r\n",GMDType);
    RetValue=0;
  }
  return RetValue;
}

/**
 * @brief Retrieve One Generic Meta Data saved on Meta Data Manager
 * @param MDM_GenericMetaDataType_t GMDType Generic Meta Data type that we want to retrieve
 * @param void *GMD Pointer where to save the Generic Meta Data
 * @retval uin32_t Success/NotSuccess (1/0)
 */
uint32_t MDM_ReCallGMD(MDM_GenericMetaDataType_t GMDType,void *GMD)
{
  uint32_t RetValue=1;
  if(MDM_GMDTable[GMDType].Address!=0) {
    MDM_PayLoadGMD_t *MDMPayLoad = (MDM_PayLoadGMD_t *)MDM_GMDTable[GMDType].Address;
    memcpy(GMD,(void *)MDMPayLoad->puint8_GMD,MDMPayLoad->GMDSize);
  } else {
    MDM_PRINTF("ERROR there is not a Generic Meta Data type=%d inside the MDM\r\n",GMDType);
    RetValue=0;
  }
  return RetValue;
}

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
