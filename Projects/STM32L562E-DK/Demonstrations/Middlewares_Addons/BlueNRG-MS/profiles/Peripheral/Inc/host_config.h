/******************************************************************************
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
*   FILENAME        -  host_config.h
*
*   COMPILER        -  EWARM (gnu gcc v4.4.5)
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:       11/07/2011
*   $Revision$:   first version
*   $Author$:     
*   Comments:     Define BLE host stack configuration macros
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This header file defines the host stack configuration values. 
* 
* 
*******************************************************************************/
#ifndef _HOST_CONFIG_H_
#define _HOST_CONFIG_H_

#define PLATFORM_LINUX         (0x01)
#define PLATFORM_WINDOWS       (0x02)
#define PLATFORM_ARM           (0x03)

#define PLATFORM_TYPE          (PLATFORM_ARM)

/* profile codes */
#define HEART_RATE_SENSOR               (0x01)
#define PROXIMITY_REPORTER              (0x02)
#define PROXIMITY_MONITOR               (0x04)
#define TIME_SERVER                     (0x08)
#define TIME_CLIENT                     (0x10)
#define HID_DEVICE                      (0x20)
#define FIND_ME_TARGET                  (0x40)
#define FIND_ME_LOCATOR                 (0x80)
#define PHONE_ALERT_CLIENT              (0x100)
#define BLOOD_PRESSURE_SENSOR           (0x200)
#define HEALTH_THERMOMETER              (0x400)
#define ALERT_NOTIFICATION_CLIENT       (0x800)
#define ALERT_NOTIFICATION_SERVER       (0x1000)
#define GLUCOSE_SENSOR                  (0x2000)

/* It is only used for building the Profiles library */
#define ALL_PROFILES			(HEART_RATE_SENSOR|\
                                         PROXIMITY_REPORTER|\
                                         PROXIMITY_MONITOR|\
                                         TIME_SERVER|\
                                         TIME_CLIENT|\
                                         FIND_ME_TARGET|\
                                         FIND_ME_LOCATOR|\
                                         PHONE_ALERT_CLIENT|\
                                         BLOOD_PRESSURE_SENSOR|\
                                         HEALTH_THERMOMETER|\
                                         ALERT_NOTIFICATION_CLIENT|\
                                         ALERT_NOTIFICATION_SERVER|\
                                         GLUCOSE_SENSOR|\
                                         HID_DEVICE)

/* active profiles */
#ifndef BLE_CURRENT_PROFILE_ROLES
//#define BLE_CURRENT_PROFILE_ROLES  (ALERT_NOTIFICATION_CLIENT) /* not supported yet */
//#define BLE_CURRENT_PROFILE_ROLES  (ALERT_NOTIFICATION_SERVER)
//#define BLE_CURRENT_PROFILE_ROLES  (BLOOD_PRESSURE_SENSOR)
//#define BLE_CURRENT_PROFILE_ROLES  (FIND_ME_LOCATOR) /* not supported yet */
//#define BLE_CURRENT_PROFILE_ROLES  (FIND_ME_TARGET)
//#define BLE_CURRENT_PROFILE_ROLES  (GLUCOSE_SENSOR)
//#define BLE_CURRENT_PROFILE_ROLES  (HEALTH_THERMOMETER)
#define BLE_CURRENT_PROFILE_ROLES  (HEART_RATE_SENSOR)
//#define BLE_CURRENT_PROFILE_ROLES  (HID_DEVICE) /* not supported by L0 */
//#define BLE_CURRENT_PROFILE_ROLES  (PHONE_ALERT_CLIENT) /* not supported yet */
//#define BLE_CURRENT_PROFILE_ROLES  (PROXIMITY_MONITOR) /* not supported yet */
//#define BLE_CURRENT_PROFILE_ROLES  (PROXIMITY_REPORTER)
//#define BLE_CURRENT_PROFILE_ROLES  (TIME_CLIENT) /* not supported yet */
//#define BLE_CURRENT_PROFILE_ROLES  (TIME_SERVER)

#endif

#endif /* _HOST_CONFIG_H_ */ 
