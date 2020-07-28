/**
  ******************************************************************************
  * @file    usbpd_pdo_defs.h
  * @author  MCD Application Team
  * @brief   Header file for definition of PDO/APDO values for 1 port(SNK) configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#ifndef __USBPD_PDO_DEF_H_
#define __USBPD_PDO_DEF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(_GUI_INTERFACE)
#include "usbpd_gui_memmap.h"
#endif /* _GUI_INTERFACE */

/* Define   ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/**
  * @brief  USBPD Port PDO Structure definition
  *
  */
typedef struct
{
#if defined(_GUI_INTERFACE)
  uint32_t ListOfPDO[USBPD_MAX_NB_PDO];         /*!< List of Power Data Objects list, defining
                                                     port capabilities */
#else
  uint32_t *ListOfPDO;                          /*!< Pointer on Power Data Objects list, defining
                                                     port capabilities */
#endif /* _GUI_INTERFACE */
  uint8_t  NumberOfPDO;                         /*!< Number of Power Data Objects defined in ListOfPDO
                                                     This parameter must be set at max to @ref USBPD_MAX_NB_PDO value */
} USBPD_PortPDO_TypeDef;

/**
  * @brief  USBPD Port PDO Storage Structure definition
  */
typedef struct
{
  USBPD_PortPDO_TypeDef    SourcePDO;        /*!< SRC Power Data Objects */
  USBPD_PortPDO_TypeDef    SinkPDO;          /*!< SNK Power Data Objects */
}USBPD_PWR_Port_PDO_Storage_TypeDef;

/* Exported define -----------------------------------------------------------*/
#define USBPD_PDP_SRC_IN_WATTS          15  /* SRC PD Power in Watts */
#define USBPD_PDP_SNK_IN_WATTS          7.5 /* SINK PD Power in Watts */

/* Define board operating power and max power */
/* Request 5V */
#define USBPD_BOARD_REQUESTED_VOLTAGE_MV       5000
#define USBPD_BOARD_MIN_VOLTAGE_MV             5000
#define USBPD_BOARD_MAX_VOLTAGE_MV             5000

/* Definitions of nb of PDO and APDO for each port */
#define USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT 3
#define USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT 1500

/* Definitions of nb of PDO and APDO for each port */
#define PORT0_NB_SOURCEPDO         1   /* Number of Source PDOs (applicable for port 0)   */
#define PORT0_NB_SINKPDO           1   /* Number of Sink PDOs (applicable for port 0)     */
#define PORT0_NB_SOURCEAPDO        0   /* Number of Source APDOs (applicable for port 0)  */
#define PORT0_NB_SINKAPDO          0   /* Number of Sink APDOs (applicable for port 0)    */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
#ifndef __USBPD_PWR_IF_C

extern uint8_t USBPD_NbPDO[4];

#if (PORT0_NB_SOURCEPDO > 0) || (PORT0_NB_SOURCEAPDO > 0)
extern uint32_t PORT0_PDO_ListSRC[USBPD_MAX_NB_PDO];
#endif
#if (PORT0_NB_SINKPDO > 0) || (PORT0_NB_SINKAPDO > 0)
extern uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO];
#endif

#else

uint8_t USBPD_NbPDO[4] = {(PORT0_NB_SINKPDO + PORT0_NB_SINKAPDO),
                          ((PORT0_NB_SOURCEPDO + PORT0_NB_SOURCEAPDO)),
                          (0),
                          (0)};

/* Definition of Source PDO for Port 0 */
#if (PORT0_NB_SOURCEPDO > 0) || (PORT0_NB_SOURCEAPDO > 0)
uint32_t PORT0_PDO_ListSRC[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */ (0x00000000U),
  /* PDO 2 */ (0x00000000U),
  /* PDO 3 */ (0x00000000U),
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};
#endif

/* Definition of Sink PDO for Port 0 */
#if (PORT0_NB_SINKPDO > 0) || (PORT0_NB_SINKAPDO > 0)
uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT / 1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
          ((PWR_V_50MV(USBPD_BOARD_REQUESTED_VOLTAGE_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
#if defined(USBPD_REV30_SUPPORT)
           USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED                                                     |
#endif /*USBPD_REV30_SUPPORT*/
           USBPD_PDO_SNK_FIXED_DRD_SUPPORTED                                                         |
#if defined(USBPD_REV30_SUPPORT) && defined(_VDM) && defined(_SNK)
           USBPD_PDO_SNK_FIXED_USBCOMM_SUPPORTED                                                     |
#else
           USBPD_PDO_SNK_FIXED_USBCOMM_NOT_SUPPORTED                                                 |
#endif
           USBPD_PDO_SNK_FIXED_EXT_POWER_NOT_AVAILABLE                                               |
           USBPD_PDO_SNK_FIXED_HIGHERCAPAB_NOT_SUPPORTED                                             |
           USBPD_PDO_SNK_FIXED_DRP_SUPPORTED                                                         |
           USBPD_PDO_TYPE_FIXED
        ),
  /* PDO 2 */ (0x00000000U),
  /* PDO 3 */ (0x00000000U),
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};
#endif

#endif /* __USBPD_PWR_IF_C */

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_PDO_DEF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
