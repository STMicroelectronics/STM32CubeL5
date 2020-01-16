/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_BlockBased_TrustZone/Secure/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for secure main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l552e_eval.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/
#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL (*funcptr)(void);
#endif

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* Base address of the Flash pages */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x0C000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x0C000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x0C001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x0C001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x0C002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x0C002800) /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x0C003000) /* Base @ of Page 6, 2 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x0C003800) /* Base @ of Page 7, 2 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x0C004000) /* Base @ of Page 8, 2 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x0C004800) /* Base @ of Page 9, 2 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x0C005000) /* Base @ of Page 10, 2 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x0C005800) /* Base @ of Page 11, 2 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x0C006000) /* Base @ of Page 12, 2 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x0C006800) /* Base @ of Page 13, 2 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x0C007000) /* Base @ of Page 14, 2 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x0C007800) /* Base @ of Page 15, 2 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x0C008000) /* Base @ of Page 16, 2 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x0C008800) /* Base @ of Page 17, 2 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x0C009000) /* Base @ of Page 18, 2 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x0C009800) /* Base @ of Page 19, 2 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0C00A000) /* Base @ of Page 20, 2 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0C00A800) /* Base @ of Page 21, 2 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0C00B000) /* Base @ of Page 22, 2 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0C00B800) /* Base @ of Page 23, 2 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0C00C000) /* Base @ of Page 24, 2 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0C00C800) /* Base @ of Page 25, 2 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0C00D000) /* Base @ of Page 26, 2 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0C00D800) /* Base @ of Page 27, 2 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0C00E000) /* Base @ of Page 28, 2 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0C00E800) /* Base @ of Page 29, 2 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0C00F000) /* Base @ of Page 30, 2 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0C00F800) /* Base @ of Page 31, 2 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x0C010000) /* Base @ of Page 32, 2 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x0C010800) /* Base @ of Page 33, 2 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x0C011000) /* Base @ of Page 34, 2 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x0C011800) /* Base @ of Page 35, 2 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x0C012000) /* Base @ of Page 36, 2 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x0C012800) /* Base @ of Page 37, 2 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x0C013000) /* Base @ of Page 38, 2 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x0C013800) /* Base @ of Page 39, 2 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x0C014000) /* Base @ of Page 40, 2 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x0C014800) /* Base @ of Page 41, 2 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x0C015000) /* Base @ of Page 42, 2 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x0C015800) /* Base @ of Page 43, 2 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x0C016000) /* Base @ of Page 44, 2 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x0C016800) /* Base @ of Page 45, 2 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x0C017000) /* Base @ of Page 46, 2 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x0C017800) /* Base @ of Page 47, 2 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x0C018000) /* Base @ of Page 48, 2 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x0C018800) /* Base @ of Page 49, 2 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x0C019000) /* Base @ of Page 50, 2 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x0C019800) /* Base @ of Page 51, 2 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0C01A000) /* Base @ of Page 52, 2 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0C01A800) /* Base @ of Page 53, 2 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0C01B000) /* Base @ of Page 54, 2 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0C01B800) /* Base @ of Page 55, 2 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0C01C000) /* Base @ of Page 56, 2 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0C01C800) /* Base @ of Page 57, 2 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0C01D000) /* Base @ of Page 58, 2 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0C01D800) /* Base @ of Page 59, 2 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0C01E000) /* Base @ of Page 60, 2 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0C01E800) /* Base @ of Page 61, 2 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0C01F000) /* Base @ of Page 62, 2 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0C01F800) /* Base @ of Page 63, 2 Kbytes */
#define ADDR_FLASH_PAGE_64    ((uint32_t)0x0C020000) /* Base @ of Page 64, 2 Kbytes */
#define ADDR_FLASH_PAGE_65    ((uint32_t)0x0C020800) /* Base @ of Page 65, 2 Kbytes */
#define ADDR_FLASH_PAGE_66    ((uint32_t)0x0C021000) /* Base @ of Page 66, 2 Kbytes */
#define ADDR_FLASH_PAGE_67    ((uint32_t)0x0C021800) /* Base @ of Page 67, 2 Kbytes */
#define ADDR_FLASH_PAGE_68    ((uint32_t)0x0C022000) /* Base @ of Page 68, 2 Kbytes */
#define ADDR_FLASH_PAGE_69    ((uint32_t)0x0C022800) /* Base @ of Page 69, 2 Kbytes */
#define ADDR_FLASH_PAGE_70    ((uint32_t)0x0C023000) /* Base @ of Page 70, 2 Kbytes */
#define ADDR_FLASH_PAGE_71    ((uint32_t)0x0C023800) /* Base @ of Page 71, 2 Kbytes */
#define ADDR_FLASH_PAGE_72    ((uint32_t)0x0C024000) /* Base @ of Page 72, 2 Kbytes */
#define ADDR_FLASH_PAGE_73    ((uint32_t)0x0C024800) /* Base @ of Page 73, 2 Kbytes */
#define ADDR_FLASH_PAGE_74    ((uint32_t)0x0C025000) /* Base @ of Page 74, 2 Kbytes */
#define ADDR_FLASH_PAGE_75    ((uint32_t)0x0C025800) /* Base @ of Page 75, 2 Kbytes */
#define ADDR_FLASH_PAGE_76    ((uint32_t)0x0C026000) /* Base @ of Page 76, 2 Kbytes */
#define ADDR_FLASH_PAGE_77    ((uint32_t)0x0C026800) /* Base @ of Page 77, 2 Kbytes */
#define ADDR_FLASH_PAGE_78    ((uint32_t)0x0C027000) /* Base @ of Page 78, 2 Kbytes */
#define ADDR_FLASH_PAGE_79    ((uint32_t)0x0C027800) /* Base @ of Page 79, 2 Kbytes */
#define ADDR_FLASH_PAGE_80    ((uint32_t)0x0C028000) /* Base @ of Page 80, 2 Kbytes */
#define ADDR_FLASH_PAGE_81    ((uint32_t)0x0C028800) /* Base @ of Page 81, 2 Kbytes */
#define ADDR_FLASH_PAGE_82    ((uint32_t)0x0C029000) /* Base @ of Page 82, 2 Kbytes */
#define ADDR_FLASH_PAGE_83    ((uint32_t)0x0C029800) /* Base @ of Page 83, 2 Kbytes */
#define ADDR_FLASH_PAGE_84    ((uint32_t)0x0C02A000) /* Base @ of Page 84, 2 Kbytes */
#define ADDR_FLASH_PAGE_85    ((uint32_t)0x0C02A800) /* Base @ of Page 85, 2 Kbytes */
#define ADDR_FLASH_PAGE_86    ((uint32_t)0x0C02B000) /* Base @ of Page 86, 2 Kbytes */
#define ADDR_FLASH_PAGE_87    ((uint32_t)0x0C02B800) /* Base @ of Page 87, 2 Kbytes */
#define ADDR_FLASH_PAGE_88    ((uint32_t)0x0C02C000) /* Base @ of Page 88, 2 Kbytes */
#define ADDR_FLASH_PAGE_89    ((uint32_t)0x0C02C800) /* Base @ of Page 89, 2 Kbytes */
#define ADDR_FLASH_PAGE_90    ((uint32_t)0x0C02D000) /* Base @ of Page 90, 2 Kbytes */
#define ADDR_FLASH_PAGE_91    ((uint32_t)0x0C02D800) /* Base @ of Page 91, 2 Kbytes */
#define ADDR_FLASH_PAGE_92    ((uint32_t)0x0C02E000) /* Base @ of Page 92, 2 Kbytes */
#define ADDR_FLASH_PAGE_93    ((uint32_t)0x0C02E800) /* Base @ of Page 93, 2 Kbytes */
#define ADDR_FLASH_PAGE_94    ((uint32_t)0x0C02F000) /* Base @ of Page 94, 2 Kbytes */
#define ADDR_FLASH_PAGE_95    ((uint32_t)0x0C02F800) /* Base @ of Page 95, 2 Kbytes */
#define ADDR_FLASH_PAGE_96    ((uint32_t)0x0C030000) /* Base @ of Page 96, 2 Kbytes */
#define ADDR_FLASH_PAGE_97    ((uint32_t)0x0C030800) /* Base @ of Page 97, 2 Kbytes */
#define ADDR_FLASH_PAGE_98    ((uint32_t)0x0C031000) /* Base @ of Page 98, 2 Kbytes */
#define ADDR_FLASH_PAGE_99    ((uint32_t)0x0C031800) /* Base @ of Page 99, 2 Kbytes */
#define ADDR_FLASH_PAGE_100   ((uint32_t)0x0C032000) /* Base @ of Page 100, 2 Kbytes */
#define ADDR_FLASH_PAGE_101   ((uint32_t)0x0C032800) /* Base @ of Page 101, 2 Kbytes */
#define ADDR_FLASH_PAGE_102   ((uint32_t)0x0C033000) /* Base @ of Page 102, 2 Kbytes */
#define ADDR_FLASH_PAGE_103   ((uint32_t)0x0C033800) /* Base @ of Page 103, 2 Kbytes */
#define ADDR_FLASH_PAGE_104   ((uint32_t)0x0C034000) /* Base @ of Page 104, 2 Kbytes */
#define ADDR_FLASH_PAGE_105   ((uint32_t)0x0C034800) /* Base @ of Page 105, 2 Kbytes */
#define ADDR_FLASH_PAGE_106   ((uint32_t)0x0C035000) /* Base @ of Page 106, 2 Kbytes */
#define ADDR_FLASH_PAGE_107   ((uint32_t)0x0C035800) /* Base @ of Page 107, 2 Kbytes */
#define ADDR_FLASH_PAGE_108   ((uint32_t)0x0C036000) /* Base @ of Page 108, 2 Kbytes */
#define ADDR_FLASH_PAGE_109   ((uint32_t)0x0C036800) /* Base @ of Page 109, 2 Kbytes */
#define ADDR_FLASH_PAGE_110   ((uint32_t)0x0C037000) /* Base @ of Page 110, 2 Kbytes */
#define ADDR_FLASH_PAGE_111   ((uint32_t)0x0C037800) /* Base @ of Page 111, 2 Kbytes */
#define ADDR_FLASH_PAGE_112   ((uint32_t)0x0C038000) /* Base @ of Page 112, 2 Kbytes */
#define ADDR_FLASH_PAGE_113   ((uint32_t)0x0C038800) /* Base @ of Page 113, 2 Kbytes */
#define ADDR_FLASH_PAGE_114   ((uint32_t)0x0C039000) /* Base @ of Page 114, 2 Kbytes */
#define ADDR_FLASH_PAGE_115   ((uint32_t)0x0C039800) /* Base @ of Page 115, 2 Kbytes */
#define ADDR_FLASH_PAGE_116   ((uint32_t)0x0C03A000) /* Base @ of Page 116, 2 Kbytes */
#define ADDR_FLASH_PAGE_117   ((uint32_t)0x0C03A800) /* Base @ of Page 117, 2 Kbytes */
#define ADDR_FLASH_PAGE_118   ((uint32_t)0x0C03B000) /* Base @ of Page 118, 2 Kbytes */
#define ADDR_FLASH_PAGE_119   ((uint32_t)0x0C03B800) /* Base @ of Page 119, 2 Kbytes */
#define ADDR_FLASH_PAGE_120   ((uint32_t)0x0C03C000) /* Base @ of Page 120, 2 Kbytes */
#define ADDR_FLASH_PAGE_121   ((uint32_t)0x0C03C800) /* Base @ of Page 121, 2 Kbytes */
#define ADDR_FLASH_PAGE_122   ((uint32_t)0x0C03D000) /* Base @ of Page 122, 2 Kbytes */
#define ADDR_FLASH_PAGE_123   ((uint32_t)0x0C03D800) /* Base @ of Page 123, 2 Kbytes */
#define ADDR_FLASH_PAGE_124   ((uint32_t)0x0C03E000) /* Base @ of Page 124, 2 Kbytes */
#define ADDR_FLASH_PAGE_125   ((uint32_t)0x0C03E800) /* Base @ of Page 125, 2 Kbytes */
#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0C03F000) /* Base @ of Page 126, 2 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0C03F800) /* Base @ of Page 127, 2 Kbytes */
#define ADDR_FLASH_PAGE_128   ((uint32_t)0x0C040000) /* Base @ of Page 128, 2 Kbytes */
#define ADDR_FLASH_PAGE_129   ((uint32_t)0x0C040800) /* Base @ of Page 129, 2 Kbytes */
#define ADDR_FLASH_PAGE_130   ((uint32_t)0x0C041000) /* Base @ of Page 130, 2 Kbytes */
#define ADDR_FLASH_PAGE_131   ((uint32_t)0x0C041800) /* Base @ of Page 131, 2 Kbytes */
#define ADDR_FLASH_PAGE_132   ((uint32_t)0x0C042000) /* Base @ of Page 132, 2 Kbytes */
#define ADDR_FLASH_PAGE_133   ((uint32_t)0x0C042800) /* Base @ of Page 133, 2 Kbytes */
#define ADDR_FLASH_PAGE_134   ((uint32_t)0x0C043000) /* Base @ of Page 134, 2 Kbytes */
#define ADDR_FLASH_PAGE_135   ((uint32_t)0x0C043800) /* Base @ of Page 135, 2 Kbytes */
#define ADDR_FLASH_PAGE_136   ((uint32_t)0x0C044000) /* Base @ of Page 136, 2 Kbytes */
#define ADDR_FLASH_PAGE_137   ((uint32_t)0x0C044800) /* Base @ of Page 137, 2 Kbytes */
#define ADDR_FLASH_PAGE_138   ((uint32_t)0x0C045000) /* Base @ of Page 138, 2 Kbytes */
#define ADDR_FLASH_PAGE_139   ((uint32_t)0x0C045800) /* Base @ of Page 139, 2 Kbytes */
#define ADDR_FLASH_PAGE_140   ((uint32_t)0x0C046000) /* Base @ of Page 140, 2 Kbytes */
#define ADDR_FLASH_PAGE_141   ((uint32_t)0x0C046800) /* Base @ of Page 141, 2 Kbytes */
#define ADDR_FLASH_PAGE_142   ((uint32_t)0x0C047000) /* Base @ of Page 142, 2 Kbytes */
#define ADDR_FLASH_PAGE_143   ((uint32_t)0x0C047800) /* Base @ of Page 143, 2 Kbytes */
#define ADDR_FLASH_PAGE_144   ((uint32_t)0x0C048000) /* Base @ of Page 144, 2 Kbytes */
#define ADDR_FLASH_PAGE_145   ((uint32_t)0x0C048800) /* Base @ of Page 145, 2 Kbytes */
#define ADDR_FLASH_PAGE_146   ((uint32_t)0x0C049000) /* Base @ of Page 146, 2 Kbytes */
#define ADDR_FLASH_PAGE_147   ((uint32_t)0x0C049800) /* Base @ of Page 147, 2 Kbytes */
#define ADDR_FLASH_PAGE_148   ((uint32_t)0x0C04a000) /* Base @ of Page 148, 2 Kbytes */
#define ADDR_FLASH_PAGE_149   ((uint32_t)0x0C04a800) /* Base @ of Page 149, 2 Kbytes */
#define ADDR_FLASH_PAGE_150   ((uint32_t)0x0C04b000) /* Base @ of Page 150, 2 Kbytes */
#define ADDR_FLASH_PAGE_151   ((uint32_t)0x0C04b800) /* Base @ of Page 151, 2 Kbytes */
#define ADDR_FLASH_PAGE_152   ((uint32_t)0x0C04c000) /* Base @ of Page 152, 2 Kbytes */
#define ADDR_FLASH_PAGE_153   ((uint32_t)0x0C04c800) /* Base @ of Page 153, 2 Kbytes */
#define ADDR_FLASH_PAGE_154   ((uint32_t)0x0C04d000) /* Base @ of Page 154, 2 Kbytes */
#define ADDR_FLASH_PAGE_155   ((uint32_t)0x0C04d800) /* Base @ of Page 155, 2 Kbytes */
#define ADDR_FLASH_PAGE_156   ((uint32_t)0x0C04e000) /* Base @ of Page 156, 2 Kbytes */
#define ADDR_FLASH_PAGE_157   ((uint32_t)0x0C04e800) /* Base @ of Page 157, 2 Kbytes */
#define ADDR_FLASH_PAGE_158   ((uint32_t)0x0C04f000) /* Base @ of Page 158, 2 Kbytes */
#define ADDR_FLASH_PAGE_159   ((uint32_t)0x0C04f800) /* Base @ of Page 159, 2 Kbytes */
#define ADDR_FLASH_PAGE_160   ((uint32_t)0x0C050000) /* Base @ of Page 160, 2 Kbytes */
#define ADDR_FLASH_PAGE_161   ((uint32_t)0x0C050800) /* Base @ of Page 161, 2 Kbytes */
#define ADDR_FLASH_PAGE_162   ((uint32_t)0x0C051000) /* Base @ of Page 162, 2 Kbytes */
#define ADDR_FLASH_PAGE_163   ((uint32_t)0x0C051800) /* Base @ of Page 163, 2 Kbytes */
#define ADDR_FLASH_PAGE_164   ((uint32_t)0x0C052000) /* Base @ of Page 164, 2 Kbytes */
#define ADDR_FLASH_PAGE_165   ((uint32_t)0x0C052800) /* Base @ of Page 165, 2 Kbytes */
#define ADDR_FLASH_PAGE_166   ((uint32_t)0x0C053000) /* Base @ of Page 166, 2 Kbytes */
#define ADDR_FLASH_PAGE_167   ((uint32_t)0x0C053800) /* Base @ of Page 167, 2 Kbytes */
#define ADDR_FLASH_PAGE_168   ((uint32_t)0x0C054000) /* Base @ of Page 168, 2 Kbytes */
#define ADDR_FLASH_PAGE_169   ((uint32_t)0x0C054800) /* Base @ of Page 169, 2 Kbytes */
#define ADDR_FLASH_PAGE_170   ((uint32_t)0x0C055000) /* Base @ of Page 170, 2 Kbytes */
#define ADDR_FLASH_PAGE_171   ((uint32_t)0x0C055800) /* Base @ of Page 171, 2 Kbytes */
#define ADDR_FLASH_PAGE_172   ((uint32_t)0x0C056000) /* Base @ of Page 172, 2 Kbytes */
#define ADDR_FLASH_PAGE_173   ((uint32_t)0x0C056800) /* Base @ of Page 173, 2 Kbytes */
#define ADDR_FLASH_PAGE_174   ((uint32_t)0x0C057000) /* Base @ of Page 174, 2 Kbytes */
#define ADDR_FLASH_PAGE_175   ((uint32_t)0x0C057800) /* Base @ of Page 175, 2 Kbytes */
#define ADDR_FLASH_PAGE_176   ((uint32_t)0x0C058000) /* Base @ of Page 176, 2 Kbytes */
#define ADDR_FLASH_PAGE_177   ((uint32_t)0x0C058800) /* Base @ of Page 177, 2 Kbytes */
#define ADDR_FLASH_PAGE_178   ((uint32_t)0x0C059000) /* Base @ of Page 178, 2 Kbytes */
#define ADDR_FLASH_PAGE_179   ((uint32_t)0x0C059800) /* Base @ of Page 179, 2 Kbytes */
#define ADDR_FLASH_PAGE_180   ((uint32_t)0x0C05a000) /* Base @ of Page 180, 2 Kbytes */
#define ADDR_FLASH_PAGE_181   ((uint32_t)0x0C05a800) /* Base @ of Page 181, 2 Kbytes */
#define ADDR_FLASH_PAGE_182   ((uint32_t)0x0C05b000) /* Base @ of Page 182, 2 Kbytes */
#define ADDR_FLASH_PAGE_183   ((uint32_t)0x0C05b800) /* Base @ of Page 183, 2 Kbytes */
#define ADDR_FLASH_PAGE_184   ((uint32_t)0x0C05c000) /* Base @ of Page 184, 2 Kbytes */
#define ADDR_FLASH_PAGE_185   ((uint32_t)0x0C05c800) /* Base @ of Page 185, 2 Kbytes */
#define ADDR_FLASH_PAGE_186   ((uint32_t)0x0C05d000) /* Base @ of Page 186, 2 Kbytes */
#define ADDR_FLASH_PAGE_187   ((uint32_t)0x0C05d800) /* Base @ of Page 187, 2 Kbytes */
#define ADDR_FLASH_PAGE_188   ((uint32_t)0x0C05e000) /* Base @ of Page 188, 2 Kbytes */
#define ADDR_FLASH_PAGE_189   ((uint32_t)0x0C05e800) /* Base @ of Page 189, 2 Kbytes */
#define ADDR_FLASH_PAGE_190   ((uint32_t)0x0C05f000) /* Base @ of Page 190, 2 Kbytes */
#define ADDR_FLASH_PAGE_191   ((uint32_t)0x0C05f800) /* Base @ of Page 191, 2 Kbytes */
#define ADDR_FLASH_PAGE_192   ((uint32_t)0x0C060000) /* Base @ of Page 192, 2 Kbytes */
#define ADDR_FLASH_PAGE_193   ((uint32_t)0x0C060800) /* Base @ of Page 193, 2 Kbytes */
#define ADDR_FLASH_PAGE_194   ((uint32_t)0x0C061000) /* Base @ of Page 194, 2 Kbytes */
#define ADDR_FLASH_PAGE_195   ((uint32_t)0x0C061800) /* Base @ of Page 195, 2 Kbytes */
#define ADDR_FLASH_PAGE_196   ((uint32_t)0x0C062000) /* Base @ of Page 196, 2 Kbytes */
#define ADDR_FLASH_PAGE_197   ((uint32_t)0x0C062800) /* Base @ of Page 197, 2 Kbytes */
#define ADDR_FLASH_PAGE_198   ((uint32_t)0x0C063000) /* Base @ of Page 198, 2 Kbytes */
#define ADDR_FLASH_PAGE_199   ((uint32_t)0x0C063800) /* Base @ of Page 199, 2 Kbytes */
#define ADDR_FLASH_PAGE_200   ((uint32_t)0x0C064000) /* Base @ of Page 200, 2 Kbytes */
#define ADDR_FLASH_PAGE_201   ((uint32_t)0x0C064800) /* Base @ of Page 201, 2 Kbytes */
#define ADDR_FLASH_PAGE_202   ((uint32_t)0x0C065000) /* Base @ of Page 202, 2 Kbytes */
#define ADDR_FLASH_PAGE_203   ((uint32_t)0x0C065800) /* Base @ of Page 203, 2 Kbytes */
#define ADDR_FLASH_PAGE_204   ((uint32_t)0x0C066000) /* Base @ of Page 204, 2 Kbytes */
#define ADDR_FLASH_PAGE_205   ((uint32_t)0x0C066800) /* Base @ of Page 205, 2 Kbytes */
#define ADDR_FLASH_PAGE_206   ((uint32_t)0x0C067000) /* Base @ of Page 206, 2 Kbytes */
#define ADDR_FLASH_PAGE_207   ((uint32_t)0x0C067800) /* Base @ of Page 207, 2 Kbytes */
#define ADDR_FLASH_PAGE_208   ((uint32_t)0x0C068000) /* Base @ of Page 208, 2 Kbytes */
#define ADDR_FLASH_PAGE_209   ((uint32_t)0x0C068800) /* Base @ of Page 209, 2 Kbytes */
#define ADDR_FLASH_PAGE_210   ((uint32_t)0x0C069000) /* Base @ of Page 210, 2 Kbytes */
#define ADDR_FLASH_PAGE_211   ((uint32_t)0x0C069800) /* Base @ of Page 211, 2 Kbytes */
#define ADDR_FLASH_PAGE_212   ((uint32_t)0x0C06a000) /* Base @ of Page 212, 2 Kbytes */
#define ADDR_FLASH_PAGE_213   ((uint32_t)0x0C06a800) /* Base @ of Page 213, 2 Kbytes */
#define ADDR_FLASH_PAGE_214   ((uint32_t)0x0C06b000) /* Base @ of Page 214, 2 Kbytes */
#define ADDR_FLASH_PAGE_215   ((uint32_t)0x0C06b800) /* Base @ of Page 215, 2 Kbytes */
#define ADDR_FLASH_PAGE_216   ((uint32_t)0x0C06c000) /* Base @ of Page 216, 2 Kbytes */
#define ADDR_FLASH_PAGE_217   ((uint32_t)0x0C06c800) /* Base @ of Page 217, 2 Kbytes */
#define ADDR_FLASH_PAGE_218   ((uint32_t)0x0C06d000) /* Base @ of Page 218, 2 Kbytes */
#define ADDR_FLASH_PAGE_219   ((uint32_t)0x0C06d800) /* Base @ of Page 219, 2 Kbytes */
#define ADDR_FLASH_PAGE_220   ((uint32_t)0x0C06e000) /* Base @ of Page 220, 2 Kbytes */
#define ADDR_FLASH_PAGE_221   ((uint32_t)0x0C06e800) /* Base @ of Page 221, 2 Kbytes */
#define ADDR_FLASH_PAGE_222   ((uint32_t)0x0C06f000) /* Base @ of Page 222, 2 Kbytes */
#define ADDR_FLASH_PAGE_223   ((uint32_t)0x0C06f800) /* Base @ of Page 223, 2 Kbytes */
#define ADDR_FLASH_PAGE_224   ((uint32_t)0x0C070000) /* Base @ of Page 224, 2 Kbytes */
#define ADDR_FLASH_PAGE_225   ((uint32_t)0x0C070800) /* Base @ of Page 225, 2 Kbytes */
#define ADDR_FLASH_PAGE_226   ((uint32_t)0x0C071000) /* Base @ of Page 226, 2 Kbytes */
#define ADDR_FLASH_PAGE_227   ((uint32_t)0x0C071800) /* Base @ of Page 227, 2 Kbytes */
#define ADDR_FLASH_PAGE_228   ((uint32_t)0x0C072000) /* Base @ of Page 228, 2 Kbytes */
#define ADDR_FLASH_PAGE_229   ((uint32_t)0x0C072800) /* Base @ of Page 229, 2 Kbytes */
#define ADDR_FLASH_PAGE_230   ((uint32_t)0x0C073000) /* Base @ of Page 230, 2 Kbytes */
#define ADDR_FLASH_PAGE_231   ((uint32_t)0x0C073800) /* Base @ of Page 231, 2 Kbytes */
#define ADDR_FLASH_PAGE_232   ((uint32_t)0x0C074000) /* Base @ of Page 232, 2 Kbytes */
#define ADDR_FLASH_PAGE_233   ((uint32_t)0x0C074800) /* Base @ of Page 233, 2 Kbytes */
#define ADDR_FLASH_PAGE_234   ((uint32_t)0x0C075000) /* Base @ of Page 234, 2 Kbytes */
#define ADDR_FLASH_PAGE_235   ((uint32_t)0x0C075800) /* Base @ of Page 235, 2 Kbytes */
#define ADDR_FLASH_PAGE_236   ((uint32_t)0x0C076000) /* Base @ of Page 236, 2 Kbytes */
#define ADDR_FLASH_PAGE_237   ((uint32_t)0x0C076800) /* Base @ of Page 237, 2 Kbytes */
#define ADDR_FLASH_PAGE_238   ((uint32_t)0x0C077000) /* Base @ of Page 238, 2 Kbytes */
#define ADDR_FLASH_PAGE_239   ((uint32_t)0x0C077800) /* Base @ of Page 239, 2 Kbytes */
#define ADDR_FLASH_PAGE_240   ((uint32_t)0x0C078000) /* Base @ of Page 240, 2 Kbytes */
#define ADDR_FLASH_PAGE_241   ((uint32_t)0x0C078800) /* Base @ of Page 241, 2 Kbytes */
#define ADDR_FLASH_PAGE_242   ((uint32_t)0x0C079000) /* Base @ of Page 242, 2 Kbytes */
#define ADDR_FLASH_PAGE_243   ((uint32_t)0x0C079800) /* Base @ of Page 243, 2 Kbytes */
#define ADDR_FLASH_PAGE_244   ((uint32_t)0x0C07a000) /* Base @ of Page 244, 2 Kbytes */
#define ADDR_FLASH_PAGE_245   ((uint32_t)0x0C07a800) /* Base @ of Page 245, 2 Kbytes */
#define ADDR_FLASH_PAGE_246   ((uint32_t)0x0C07b000) /* Base @ of Page 246, 2 Kbytes */
#define ADDR_FLASH_PAGE_247   ((uint32_t)0x0C07b800) /* Base @ of Page 247, 2 Kbytes */
#define ADDR_FLASH_PAGE_248   ((uint32_t)0x0C07c000) /* Base @ of Page 248, 2 Kbytes */
#define ADDR_FLASH_PAGE_249   ((uint32_t)0x0C07c800) /* Base @ of Page 249, 2 Kbytes */
#define ADDR_FLASH_PAGE_250   ((uint32_t)0x0C07d000) /* Base @ of Page 250, 2 Kbytes */
#define ADDR_FLASH_PAGE_251   ((uint32_t)0x0C07d800) /* Base @ of Page 251, 2 Kbytes */
#define ADDR_FLASH_PAGE_252   ((uint32_t)0x0C07e000) /* Base @ of Page 252, 2 Kbytes */
#define ADDR_FLASH_PAGE_253   ((uint32_t)0x0C07e800) /* Base @ of Page 253, 2 Kbytes */
#define ADDR_FLASH_PAGE_254   ((uint32_t)0x0C07f000) /* Base @ of Page 254, 2 Kbytes */
#define ADDR_FLASH_PAGE_255   ((uint32_t)0x0C07f800) /* Base @ of Page 255, 2 Kbytes */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
