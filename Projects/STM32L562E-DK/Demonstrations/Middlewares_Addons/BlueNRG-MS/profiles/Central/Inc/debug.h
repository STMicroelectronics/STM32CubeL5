/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2015 STMicroelectronics International NV
*
*   FILENAME        -  debug.h
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2015
*   $Revision$:  first version
*   $Author$:    
*   Comments:    debug macro Declarations x Profiles Central
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file implements the debug and utility functions used for debugging
*
*******************************************************************************/
#ifndef _DEBUG_H_
#define _DEBUG_H_
/******************************************************************************
 * Includes
 *****************************************************************************/
#include <host_config.h>
 
/******************************************************************************
 * Feature Flags
******************************************************************************/
//#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Print the data travelling over the SPI in the .csv format for the GUI*/
//#define PRINT_CSV_FORMAT 1
#if PRINT_CSV_FORMAT
#include <stdio.h>
#define PRINT_CSV(...) printf(__VA_ARGS__)
#else
#define PRINT_CSV(...)
#endif

#if DEBUG 
//#define DEBUG_TIMER 
#define DEBUG_PROFILE 
#define DEBUG_HRPROFILE

#define DEBUG_APPLICATION
#define DEBUG_CALLBACKS //Central general callbacks 

/* Central devices */
#define DEBUG_GLUCOSE_COLLECTOR
#define DEBUG_PHONE_ALERT_STATUS_SERVER
#define DEBUG_HEART_RATE_COLLECTOR
#define DEBUG_HEALTH_THERMOMETER_COLLECTOR
#define DEBUG_FIND_ME_LOCATOR
#define DEBUG_FIND_ME_TARGET
#define DEBUG_ALERT_NOTIFICATION_SERVER 
#define DEBUG_BLOOD_PRESSURE_COLLECTOR
#define DEBUG_TIME_SERVER
#define DEBUG_ALERT_NOTIFICATION_CLIENT
#define DEBUG_TIME_CLIENT
#define DEBUG_PROXIMITY_REPORTER
#endif
/******************************************************************************
 * Macros
 *****************************************************************************/
/**
 * Debug Macros
 */
#if (PLATFORM_TYPE == PLATFORM_ARM)
#include <stdio.h>

#define DBG_PRINT(format,...)           printf (format ,##__VA_ARGS__);
#define DBG_ASSERT(expression) 
#define PRINT_MESG_DBG(file,format,...)   printf (format ,##__VA_ARGS__);
#define PRINT_MESG_DATA(file,dptr,dlen)  \
    do {  \
        int i=0;  \
        for (i=0;i<dlen;i++)  \
        {  \
            if (i%16 == 0)  \
            {  \
                printf ("\n[%s]:[%d]:[--DATA--]: ", __FILE__, __LINE__);  \
            }  \
            printf ("%02X ", dptr[i]);  \
        }  \
        printf ("\n ");  \
    }while(0) 

#define PRINT_MESG_INFO(file,format,...)  printf (format ,##__VA_ARGS__);
#define PRINT_MESG_DBG(file,format,...)  printf (format ,##__VA_ARGS__);
#define PRINT_MESG_ERR(file,format,...)  printf (format ,##__VA_ARGS__);


#define PRINT_NO_MESG(file,format,...)
        
#else
#define DBG_PRINT(format,...) 
#define DBG_ASSERT(expression) 
#define PRINT_MESG_DBG(file,format,...)  
#define PRINT_MESG_DATA(file,dptr,dlen)  
#define PRINT_MESG_INFO(file,format,...)  
#define PRINT_MESG_ERR(file,format,...)  
#define PRINT_NO_MESG(file,format,...)
#endif

#define DEBUG_APPLICATION
//#define DEBUG_CALLBACKS      
void PRINT_MESG_UART(void *f, const char * format, ... );

#ifdef DEBUG_TIMER
#define TIMER_MESG_DBG  PRINT_MESG_DBG
#define TIMER_MESG_DATA  PRINT_MESG_DATA
#else
#define TIMER_MESG_DBG  PRINT_NO_MESG
#define TIMER_MESG_DATA  PRINT_NO_MESG
#endif

#ifdef DEBUG_PROFILE
#define PROFILE_MESG_DBG  PRINT_MESG_DBG
#else
#define PROFILE_MESG_DBG  PRINT_NO_MESG
#endif
#define PROFILE_MESG_ERR  PRINT_MESG_ERR
      
#ifdef DEBUG_CALLBACKS
#define CALLBACKS_MESG_DBG  PRINT_MESG_DBG
#else
#define CALLBACKS_MESG_DBG  PRINT_NO_MESG 
#endif

#ifdef DEBUG_APPLICATION
#define APPL_MESG_DBG   PRINT_MESG_UART
#define APPL_MESG_INFO  PRINT_MESG_UART
#define APPL_MESG_DATA  PRINT_MESG_DATA
#else
#define APPL_MESG_DBG  PRINT_NO_MESG
#define APPL_MESG_INFO PRINT_MESG_INFO //TBR
#define APPL_MESG_DATA PRINT_NO_MESG
#endif    
      
#ifdef DEBUG_GLUCOSE_COLLECTOR
#define GL_DBG_MSG  PRINT_MESG_DBG
#define GL_INFO_MSG PRINT_MESG_INFO
#define GL_ERR_MSG  PRINT_MESG_ERR
#else
#define GL_DBG_MSG PRINT_NO_MESG
#define GL_INFO_MSG PRINT_NO_MESG //PRINT_MESG_INFO
#define GL_ERR_MSG  PRINT_NO_MESG
#endif

#ifdef DEBUG_PHONE_ALERT_STATUS_SERVER
#define PAS_DBG_MSG  PRINT_MESG_DBG
#define PAS_INFO_MSG PRINT_MESG_INFO
#define PAS_ERR_MSG  PRINT_MESG_ERR
#else
#define PAS_DBG_MSG  PRINT_NO_MESG
#define PAS_INFO_MSG PRINT_NO_MESG
#define PAS_ERR_MSG  PRINT_NO_MESG
#endif

#ifdef DEBUG_HEART_RATE_COLLECTOR 
#define HR_DBG_MSG PRINT_MESG_DBG 
#define HR_INFO_MSG PRINT_MESG_INFO 
#define HR_ERR_MSG PRINT_MESG_ERR 
#else 
#define HR_DBG_MSG PRINT_NO_MESG 
#define HR_INFO_MSG PRINT_NO_MESG
#define HR_RESERVED_MSG PRINT_NO_MESG //PRINT_MESG_INFO
#define HR_ERR_MSG PRINT_NO_MESG 
#endif

#ifdef DEBUG_FIND_ME_LOCATOR
#define FML_DBG_MSG  PRINT_MESG_DBG
#define FML_INFO_MSG PRINT_MESG_INFO
#define FML_ERR_MSG  PRINT_MESG_ERR
#else
#define FML_DBG_MSG  PRINT_NO_MESG
#define FML_INFO_MSG PRINT_NO_MESG
#define FML_ERR_MSG  PRINT_NO_MESG
#endif

#ifdef DEBUG_FIND_ME_TARGET
#define FMT_DBG_MSG  PRINT_MESG_DBG
#define FMT_INFO_MSG PRINT_MESG_INFO
#define FMT_ERR_MSG  PRINT_MESG_ERR
#else
#define FMT_DBG_MSG  PRINT_NO_MESG
#define FMT_INFO_MSG PRINT_NO_MESG
#define FMT_ERR_MSG  PRINT_NO_MESG
#endif

#ifdef DEBUG_HEALTH_THERMOMETER_COLLECTOR 
#define HT_DBG_MSG PRINT_MESG_DBG 
#define HT_INFO_MSG PRINT_MESG_INFO 
#define HT_ERR_MSG PRINT_MESG_ERR 
#else 
#define HT_DBG_MSG PRINT_NO_MESG 
#define HT_INFO_MSG PRINT_NO_MESG
#define HT_RESERVED_MSG PRINT_NO_MESG //PRINT_MESG_INFO
#define HT_ERR_MSG PRINT_NO_MESG 
#endif

#ifdef DEBUG_ALERT_NOTIFICATION_SERVER 
#define ANS_DBG_MSG PRINT_MESG_DBG 
#define ANS_INFO_MSG PRINT_MESG_INFO 
#define ANS_ERR_MSG PRINT_MESG_ERR 
#define ANS_DBG_DATA PRINT_MESG_DATA
#else 
#define ANS_DBG_MSG PRINT_NO_MESG 
#define ANS_INFO_MSG PRINT_MESG_INFO //TBR
#define ANS_ERR_MSG PRINT_NO_MESG 
#define ANS_DBG_DATA PRINT_NO_MESG
#endif

#ifdef DEBUG_BLOOD_PRESSURE_COLLECTOR 
#define BP_DBG_MSG PRINT_MESG_DBG 
#define BP_INFO_MSG PRINT_MESG_INFO 
#define BP_ERR_MSG PRINT_MESG_ERR 
#else 
#define BP_DBG_MSG PRINT_NO_MESG 
#define BP_INFO_MSG PRINT_NO_MESG //PRINT_MESG_INFO
#define BP_ERR_MSG PRINT_NO_MESG 
#endif
      
#ifdef DEBUG_TIME_SERVER
#define TIME_SERVER_MESG_DBG  PRINT_MESG_DBG
#define TIME_SERVER_MESG_INFO PRINT_MESG_INFO
#define TIME_SERVER_MESG_DATA PRINT_MESG_DATA
#define TIME_SERVER_MESG_ERR  PRINT_MESG_ERR
#else
#define TIME_SERVER_MESG_DBG  PRINT_NO_MESG
#define TIME_SERVER_MESG_INFO PRINT_MESG_INFO //TBR
#define TIME_SERVER_MESG_DATA PRINT_NO_MESG
#define TIME_SERVER_MESG_ERR  PRINT_NO_MESG
#endif
      
#ifdef DEBUG_ALERT_NOTIFICATION_CLIENT 
#define ANC_DBG_MSG PRINT_MESG_DBG 
#define ANC_INFO_MSG PRINT_MESG_INFO 
#define ANC_ERR_MSG PRINT_MESG_ERR 
#define ANC_DBG_DATA PRINT_MESG_DATA
#else 
#define ANC_DBG_MSG PRINT_NO_MESG 
#define ANC_INFO_MSG PRINT_NO_MESG //PRINT_MESG_INFO
#define ANC_ERR_MSG PRINT_NO_MESG 
#define ANC_DBG_DATA PRINT_NO_MESG
#endif   
   
#ifdef DEBUG_TIME_CLIENT
#define TIME_CLIENT_MESG_DBG  PRINT_MESG_DBG
#define TIME_CLIENT_MESG_INFO PRINT_MESG_INFO
#define TIME_CLIENT_MESG_DATA PRINT_MESG_DATA
#define TIME_CLIENT_MESG_ERR  PRINT_MESG_ERR
#else
#define TIME_CLIENT_MESG_DBG  PRINT_NO_MESG
#define TIME_CLIENT_MESG_INFO PRINT_NO_MESG //PRINT_MESG_INFO
#define TIME_CLIENT_MESG_DATA PRINT_NO_MESG
#define TIME_CLIENT_MESG_ERR  PRINT_NO_MESG
#endif

#ifdef DEBUG_PROXIMITY_REPORTER
#define PROXIMITY_REPORTER_MESG_DBG  PRINT_MESG_DBG
#define PROXIMITY_REPORTER_MESG_INFO  PRINT_MESG_INFO
#define PROXIMITY_REPORTER_MESG_ERR  PRINT_MESG_ERR
#else
#define PROXIMITY_REPORTER_MESG_DBG  PRINT_NO_MESG
#define PROXIMITY_REPORTER_MESG_INFO  PRINT_MESG_INFO
#define PROXIMITY_REPORTER_MESG_ERR  PRINT_MESG_ERR
#endif



#endif /* _DEBUG_H_ */
