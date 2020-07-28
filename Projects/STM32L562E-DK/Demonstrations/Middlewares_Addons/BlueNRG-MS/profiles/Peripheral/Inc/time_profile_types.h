/*******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  time_profile_types.h
*
*   COMPILER        -  IAR
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      31/07/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Redesigned the Time Profile's Data types according to 
*                unified BlueNRG DK framework     
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  This file defines the typedefs used for time profile's data types. 
*
*******************************************************************************/
#ifndef _TIME_PROFILE_TYPES_H_
#define _TIME_PROFILE_TYPES_H_


/*******************************************************************************
 * Macros
*******************************************************************************/

/* Macro definitions for year value */
#define YEAR_IS_NOT_KNOWN                                   (0x0000)
#define YEAR_MIN                                            (0x062E)     /* 1582 */
#define YEAR_MAX                                            (0x270F)     /* 9999 */

/* Macro definitions for month value */
#define MONTH_IS_NOT_KNOWN                                  (0x00)
#define JANUARY                                             (0x01)
#define FEBRUARY                                            (0x02)
#define MARCH                                               (0x03)
#define APRIL                                               (0x04)
#define MAY                                                 (0x05)
#define JUNE                                                (0x06)
#define JULY                                                (0x07)
#define AUGUST                                              (0x08)
#define SEPTEMBER                                           (0x09)
#define OCTOBER                                             (0x0A)
#define NOVEMBER                                            (0x0B)
#define DECEMBER                                            (0x0C)

/* Macro definitions for date value */
#define DATE_IS_NOT_KNOWN                                   (0x00)
#define DATE_MIN                                            (0x01)
#define DATE_MAX                                            (0x1F)

/* Macro definitions for hour value */
#define HOURS_MIN                                           (0x00)
#define HOURS_MAX                                           (0x17)

/* Macro definitions for minutes value */
#define MINUTES_MIN                                         (0x00)
#define MINUTES_MAX                                         (0x3B)

/* Macro definitions for second's value */
#define SECONDS_MIN                                         (0x00)
#define SECONDS_MAX                                         (0x3B)

/* Macro definitions for day of week value */
#define DAY_IS_NOT_KNOWN                                    (0x00)
#define MONDAY                                              (0x01)
#define TUESDAY                                             (0x02)
#define WEDNESDAY                                           (0x03)
#define THURSDAY                                            (0x04)
#define FRIDAY                                              (0x05)
#define SATURDAY                                            (0x06)
#define SUNDAY                                              (0x07)

/* Macro definitions for fraction 256 value */
#define FRACTION256_MIN                                     (0x00)
#define FRACTION256_MAX                                     (0xFF)

/* Macro definitions for adjust reason value */
#define ADJUST_REASON_NO_REASON                             (0x00)
#define ADJUST_REASON_MANUAL_TIME_UPDATE                    (0x01)
#define ADJUST_REASON_EXTERNAL_REFERENCE_TIME_UPDATE        (0x02)
#define ADJUST_REASON_CHANGE_OF_TIME_ZONE                   (0x04)
#define ADJUST_REASON_CHANGE_OF_DST                         (0x08)

/* Macro definitions for time zone value */
#define TIME_ZONE_UTC_M_1200                                (0xD0)      /* -48 := UTC-12:00 */
#define TIME_ZONE_UTC_M_1100                                (0xD4)      /* -44 := UTC-11:00 */
#define TIME_ZONE_UTC_M_1000                                (0xD8)      /* -40 := UTC-10:00 */
#define TIME_ZONE_UTC_M_0930                                (0xDA)      /* -38 := UTC-9:30 */
#define TIME_ZONE_UTC_M_0900                                (0xDC)      /* -36 := UTC-9:00 */
#define TIME_ZONE_UTC_M_0800                                (0xE0)      /* -32 := UTC-8:00 */
#define TIME_ZONE_UTC_M_0700                                (0xE4)      /* -28 := UTC-7:00 */
#define TIME_ZONE_UTC_M_0600                                (0xE8)      /* -24 := UTC-6:00 */
#define TIME_ZONE_UTC_M_0500                                (0xEC)      /* -20 := UTC-5:00 */
#define TIME_ZONE_UTC_M_0430                                (0xEE)      /* -18 := UTC-4:30 */
#define TIME_ZONE_UTC_M_0400                                (0xF0)      /* -16 := UTC-4:00 */
#define TIME_ZONE_UTC_M_0330                                (0xF2)      /* -14 := UTC-3:30 */
#define TIME_ZONE_UTC_M_0300                                (0xF4)      /* -12 := UTC-3:00 */
#define TIME_ZONE_UTC_M_0200                                (0xF8)      /* -8  := UTC-2:00 */
#define TIME_ZONE_UTC_M_0100                                (0xFC)      /* -4  := UTC-1:00 */
#define TIME_ZONE_UTC_0_0000                                (0x00)      /* 0   := UTC+0:00 */
#define TIME_ZONE_UTC_P_0100                                (0x04)      /* 4   := UTC+1:00 */
#define TIME_ZONE_UTC_P_0200                                (0x08)      /* 8   := UTC+2:00 */
#define TIME_ZONE_UTC_P_0300                                (0x0C)      /* 12  := UTC+3:00 */
#define TIME_ZONE_UTC_P_0330                                (0x0E)      /* 14  := UTC+3:30 */
#define TIME_ZONE_UTC_P_0400                                (0x10)      /* 16  := UTC+4:00 */
#define TIME_ZONE_UTC_P_0430                                (0x12)      /* 18  := UTC+4:30 */
#define TIME_ZONE_UTC_P_0500                                (0x14)      /* 20  := UTC+5:00 */
#define TIME_ZONE_UTC_P_0530                                (0x16)      /* 22  := UTC+5:30 */
#define TIME_ZONE_UTC_P_0545                                (0x17)      /* 23  := UTC+5:45 */
#define TIME_ZONE_UTC_P_0600                                (0x18)      /* 24  := UTC+6:00 */
#define TIME_ZONE_UTC_P_0630                                (0x1A)      /* 26  := UTC+6:30 */
#define TIME_ZONE_UTC_P_0700                                (0x1C)      /* 28  := UTC+7:00 */
#define TIME_ZONE_UTC_P_0800                                (0x20)      /* 32  := UTC+8:00 */
#define TIME_ZONE_UTC_P_0845                                (0x23)      /* 35  := UTC+8:45 */
#define TIME_ZONE_UTC_P_0900                                (0x24)      /* 36  := UTC+9:00 */
#define TIME_ZONE_UTC_P_0930                                (0x26)      /* 38  := UTC+9:30 */
#define TIME_ZONE_UTC_P_1000                                (0x28)      /* 40  := UTC+10:00 */
#define TIME_ZONE_UTC_P_1030                                (0x2A)      /* 42  := UTC+10:30 */
#define TIME_ZONE_UTC_P_1100                                (0x2C)      /* 44  := UTC+11:00 */
#define TIME_ZONE_UTC_P_1130                                (0x2E)      /* 46  := UTC+11:30 */
#define TIME_ZONE_UTC_P_1200                                (0x30)      /* 48  := UTC+12:00 */
#define TIME_ZONE_UTC_P_1245                                (0x33)      /* 51  := UTC+12:45 */
#define TIME_ZONE_UTC_P_1300                                (0x34)      /* 52  := UTC+13:00 */
#define TIME_ZONE_UTC_P_1400                                (0x38)      /* 56  := UTC+14:00 */
#define TIME_ZONE_OFFSET_NOT_KNOWN                          (0x80)      /**/

/* Macro definitions for DST offset value */
#define DST_OFFSET_STANDARD_TIME                            (0x00)     /* standard time */
#define DST_OFFSET_HALF_HOUR_DLT                            (0x02)     /* half an hour daylight time (+0.5h) */
#define DST_OFFSET_HOUR_DLT                                 (0x04)     /* daylight time (+1h) */
#define DST_OFFSET_DOUBLE_HOUR_DLT                          (0x08)     /* double daylight time (+2h) */
#define DST_OFFSET_NOT_KNOWN                                (0xFF)     /* DST is not known */

/* Macro definitions for time ref source value */
#define TIME_REFERENCE_UNKNOWN                              (0x00)
#define TIME_REFERENCE_NETWORK_TIME_PROTOCOL                (0x01)
#define TIME_REFERENCE_GPS                                  (0x02)
#define TIME_REFERENCE_RADIO_TIME_SIGNAL                    (0x03)
#define TIME_REFERENCE_MANUAL                               (0x04)
#define TIME_REFERENCE_ATOMIC_CLOCK                         (0x05)
#define TIME_REFERENCE_CELLULAR_NETWORK                     (0x06)

/* Macro definitions for time accuracy value */
#define TIME_ACCURACY_MIN                                   (0x00)
#define TIME_ACCURACY_MAX                                   (0XFD)
#define TIME_ACCURACY_OUT_OF_RANCE                          (0xFE)
#define TIME_ACCURACY_UNKNOWN                               (0xFF)

/* Macro definitions for days since update value */
#define DAYS_SINCE_UPDATE_MIN                               (0x00)
#define DAYS_SINCE_UPDATE_MAX                               (0xFE)
#define DAYS_SINCE_UPDATE_IS_255_OR_MORE                    (0xFF)

/* Macro definitions for hours since update value */
#define HOURS_SINCE_UPDATE_MIN                              (0x00)
#define HOURS_SINCE_UPDATE_MAX                              (0x17)
#define HOURS_SINCE_UPDATE_IS_255_OR_MORE                   (0xFF)

/* Macro definitions for time update control point value */
#define GET_REFERENCE_UPDATE                                (0x01)
#define CANCEL_REFERENCE_UPDATE                             (0x02)

/* Macro definitions for time update status value */
#define TIME_UPDATE_STATUS_IDLE                             (0x00)
#define TIME_UPDATE_STATUS_UPDATE_PENDING                   (0x01)

/* Macro definitions for time update result value */
#define TIME_UPDATE_RESULT_SUCCESSFUL                       (0x00)
#define TIME_UPDATE_RESULT_CANCELED                         (0x01)
#define TIME_UPDATE_RESULT_NO_CONNECTION_TO_REFERENCE       (0x02)
#define TIME_UPDATE_RESULT_REFERENCE_RESPONDED_WITH_ERROR   (0x03)
#define TIME_UPDATE_RESULT_TIMEOUT                          (0x04)
#define TIME_UPDATE_RESULT_NOT_ATTEMPTED_AFTER_RESET        (0x05)



/*******************************************************************************
 * type definitions
*******************************************************************************/

/* typedef definition for structure of current time value */
typedef struct _tCurrentTime 
{
  uint16_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t day_of_week;
  uint8_t fractions256;
  uint8_t adjustReason;	

}tCurrentTime;


/* typedef definition for structure of local time information value */
typedef struct _tLocalTimeInformation
{
  int8_t timeZone;
  uint8_t dstOffset;

}tLocalTimeInformation;


/* typedef definition for structure of reference time information value */
typedef struct _tReferenceTimeInformation 
{
  uint8_t source;
  uint8_t accuracy;
  uint8_t daysSinceUpdate;
  uint8_t hoursSinceUpdate;

}tReferenceTimeInformation;


/* typedef definition for structure of time with DST value */
typedef struct _tTimeWithDST
{
  uint16_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t dstOffset;

}tTimeWithDST;


/* typedef definition for time update control point value */
typedef uint8_t tTimeUpdateControlPoint;


/* typedef definition for structure of time update status value */
typedef struct _tTimeUpdateStatus 
{
  uint8_t currentStatus;
  uint8_t result;

}tTimeUpdateStatus;



#endif  /* _TIME_PROFILE_TYPES_H_ */
