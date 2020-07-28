/**
  ******************************************************************************
  * @file    tfm_bl2_shared_data.c
  * @author  MCD Application Team
  * @brief   This files recopies data shared between BOOT and TFM Appli
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "boot_hal_cfg.h"
#include "region_defs.h"
#include "tfm_boot_status.h"
#include "tfm_bl2_shared_data.h"
#include "low_level_rng.h"
#ifdef TFM_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_ERROR
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* TFM_DEV_MODE */
#include "bootutil/bootutil_log.h"
#include "boot_record.h"
#include "mbedtls/entropy_poll.h"
#include "platform/include/tfm_plat_boot_seed.h"
#include "platform/include/tfm_attest_hal.h"
#include "platform/include/tfm_plat_crypto_keys.h"
#include "platform/include/tfm_plat_device_id.h"
#include "bootutil/sha256.h"
extern RTC_HandleTypeDef RTCHandle;


/* Macros to pick linker symbols */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

#if defined(__ICCARM__)
#define CODE_START (void *)(BL2_CODE_START)
#else
REGION_DECLARE(Image$$, ER_CODE, $$Base);
#define CODE_START &REGION_NAME(Image$$, ER_CODE, $$Base)
#endif /* __ICCARM__ */
#if defined (__ARMCC_VERSION)
#define LOAD_NAME(a,b,c,d) a##b##c##d
#define LOAD_DECLARE(a,b,c,d) extern uint32_t LOAD_NAME(a, b, c,d)
LOAD_DECLARE(Load$$, LR$$, LR_CODE, $$Limit);
#define CODE_LIMIT &LOAD_NAME(Load$$, LR$$, LR_CODE, $$Limit)
#else
REGION_DECLARE(Image$$, HDP_CODE, $$Limit);
#define CODE_LIMIT &REGION_NAME(Image$$, HDP_CODE, $$Limit)
#endif /*__ARMCC_VERSION */
/**
  * @}
  */
static void ComputeImplementationId(uint8_t *hash_result);


#ifndef TFM_HUK_RANDOM_GENERATION
#if defined(__ICCARM__)
#pragma location="provision"
#endif /* __ICCARM__ */
const uint8_t huk_value[] __attribute__((section(".provision"))) =
{
  0x5f, 0x53, 0x54, 0x4d, 0x33, 0x32, 0x4c, 0x36,
  0x35, 0x32, 0x58, 0x58, 0x5f, 0x48, 0x55, 0x4b,
  0x5f, 0x43, 0x55, 0x53, 0x54, 0x4f, 0x4d, 0x49,
  0x5a, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x0d,
  0
};
#endif /*TFM_HUK_RANDOM_GENERATION*/

#if defined(__ICCARM__)
#pragma location="provision"
#endif /* __ICCARM__ */
const enum ecc_curve_t initial_attestation_curve_type  __attribute__((section(".provision"))) = P_256;
/* Initial attestation private key in raw format, without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MUST present on the device-
 */
#if defined(__ICCARM__)
#pragma location="provision"
#endif /* __ICCARM__ */
const uint8_t initial_attestation_private_key[] __attribute__((section(".provision"))) =
{

  0xA9, 0xB4, 0x54, 0xB2, 0x6D, 0x6F, 0x90, 0xA4,
  0xEA, 0x31, 0x19, 0x35, 0x64, 0xCB, 0xA9, 0x1F,
  0xEC, 0x6F, 0x9A, 0x00, 0x2A, 0x7D, 0xC0, 0x50,
  0x4B, 0x92, 0xA1, 0x93, 0x71, 0x34, 0x58, 0x5F
};
#if defined(__ICCARM__)
#pragma location="provision"
#endif /* __ICCARM__ */
const uint32_t initial_attestation_private_key_size __attribute__((section(".provision"))) =
  sizeof(initial_attestation_private_key);

/* Initial attestation x-coordinate of the public key in raw format,
 * without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MIGHT be present on the device.
 */
#if defined(__ICCARM__)
#pragma location="provision"
#endif /* __ICCARM__ */
const uint8_t initial_attestation_public_x_key[] __attribute__((section(".provision"))) =
{
  0x79, 0xEB, 0xA9, 0x0E, 0x8B, 0xF4, 0x50, 0xA6,
  0x75, 0x15, 0x76, 0xAD, 0x45, 0x99, 0xB0, 0x7A,
  0xDF, 0x93, 0x8D, 0xA3, 0xBB, 0x0B, 0xD1, 0x7D,
  0x00, 0x36, 0xED, 0x49, 0xA2, 0xD0, 0xFC, 0x3F
};


/* Initial attestation y-coordinate of the public key in raw format,
 * without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MIGHT be present on the device.
 */
#if defined(__ICCARM__)
#pragma location="provision"
#endif /* __ICCARM__ */
const uint8_t initial_attestation_public_y_key[] __attribute__((section(".provision"))) =
{
  0xBF, 0xCD, 0xFA, 0x89, 0x56, 0xB5, 0x68, 0xBF,
  0xDB, 0x86, 0x73, 0xE6, 0x48, 0xD8, 0xB5, 0x8D,
  0x92, 0x99, 0x55, 0xB1, 0x4A, 0x26, 0xC3, 0x08,
  0x0F, 0x34, 0x11, 0x7D, 0x97, 0x1D, 0x68, 0x64
};


/* Hash (SHA256) of initial attestation public key.
 * Byte string representation of ECC public key according to
 * psa_export_public_key() in interface/include/psa_crypto.h:
 * 0x04 || X_coord || Y_coord
 */
#if defined(__ICCARM__)
#pragma location="provision"
#endif /* __ICCARM__ */
const uint8_t initial_attestation_raw_public_key_hash[] __attribute__((section(".provision"))) =
{
  0xfa, 0x58, 0x75, 0x5f, 0x65, 0x86, 0x27, 0xce,
  0x54, 0x60, 0xf2, 0x9b, 0x75, 0x29, 0x67, 0x13,
  0x24, 0x8c, 0xae, 0x7a, 0xd9, 0xe2, 0x98, 0x4b,
  0x90, 0x28, 0x0e, 0xfc, 0xbc, 0xb5, 0x02, 0x48
};



/** @defgroup TFM_BL2_Exported_Functions Exported Functions
  * @{
  */
/**
  * @brief  Recopy bl2 shared data L5 specific
  * @param  None
  * @note   The boot seed is generated and recopied in BL2 shared data.
  * @note   tamper backup register is set privileged and secured.
  * @note   Attestation private key is recopied in tamper back up register.
  * @note   Attestation public key and hash is recopied in BL2 shared data.
  * @retval None
  */

void TFM_BL2_CopySharedData(void)
{
  uint32_t i;
  /*  default value */
  enum tfm_security_lifecycle_t lifecycle = TFM_SLC_SECURED;
  /* if 1 for TFM regression test */
  uint8_t boot_seed[BOOT_SEED_SIZE];
  uint8_t implementation_id[IMPLEMENTATION_ID_MAX_SIZE];
  size_t len = sizeof(boot_seed);
  mbedtls_hardware_poll(NULL, boot_seed, sizeof(boot_seed), &len);
  if (len != sizeof(boot_seed))
  {
    BOOT_LOG_ERR("BL2 BOOT SEED generation Failed !!");
    Error_Handler();
  }
#ifndef TFM_HUK_RANDOM_GENERATION
  enum shared_memory_err_t res;
  res = boot_add_data_to_shared_area(TLV_MAJOR_SST,
                                     TLV_MINOR_CORE_HUK,
                                     sizeof(huk_value) - 1,
                                     (const uint8_t *)huk_value);
  if (res)
  {
    BOOT_LOG_ERR("BL2 HUK set BL2 SHARED DATA Failed !!");
    Error_Handler();;
  }
  BOOT_LOG_INF("BL2 HUK %s set to BL2 SHARED DATA", huk_value);

#endif /*TFM_HUK_RANDOM_GENERATION*/
  /* Add BOOT SEED to the shared data */
  boot_add_data_to_shared_area(TLV_MAJOR_IAS,
                               TLV_MINOR_IAS_BOOT_SEED,
                               sizeof(boot_seed),
                               (const uint8_t *)boot_seed);
  /* Add Hardocded Lifecycle value */
  boot_add_data_to_shared_area(TLV_MAJOR_IAS,
                               TLV_MINOR_IAS_SLC,
                               sizeof(lifecycle),
                               (const uint8_t *)&lifecycle);

  /* add Attest key material in BL2 shared data */
  boot_add_data_to_shared_area(TLV_MAJOR_CORE,
                               TLV_MINOR_CORE_IAS_PUB_KEY_X,
                               sizeof(initial_attestation_public_x_key),
                               (const uint8_t *)&initial_attestation_public_x_key);
  boot_add_data_to_shared_area(TLV_MAJOR_CORE,
                               TLV_MINOR_CORE_IAS_PUB_KEY_Y,
                               sizeof(initial_attestation_public_y_key),
                               (const uint8_t *)&initial_attestation_public_y_key);
  boot_add_data_to_shared_area(TLV_MAJOR_CORE,
                               TLV_MINOR_CORE_IAS_PUB_KEY_HASH,
                               sizeof(initial_attestation_raw_public_key_hash),
                               (const uint8_t *)&initial_attestation_raw_public_key_hash);
  boot_add_data_to_shared_area(TLV_MAJOR_CORE,
                               TLV_MINOR_CORE_IAS_KEY_TYPE,
                               sizeof(initial_attestation_curve_type),
                               (const uint8_t *)&initial_attestation_curve_type);

  ComputeImplementationId(implementation_id);
  boot_add_data_to_shared_area(TLV_MAJOR_CORE,
                               TLV_MINOR_CORE_IAS_IMPLEMENTATION_ID,
                               sizeof(implementation_id),
                               (const uint8_t *)&implementation_id);


  /* secure the read/write access to tamper backup register BL2_RTC_SHARED_DATA_SIZE */
  MODIFY_REG(TAMP->SMCR, TAMP_SMCR_BKPRWDPROT, BL2_RTC_SHARED_DATA_SIZE>>2);
  TAMP->PRIVCR |= TAMP_PRIVCR_BKPRWPRIV;
 
  if (sizeof(initial_attestation_private_key) > BL2_RTC_SHARED_DATA_SIZE)
  {
    BOOT_LOG_ERR("RTC BL2 SHARED DATA Failed too small!!");
    Error_Handler();;
  }
  for (i = 0; i < sizeof(initial_attestation_private_key); i += 4)
  {
    HAL_RTCEx_BKUPWrite(&RTCHandle, i / 4, *((uint32_t *)(&initial_attestation_private_key[i])));
  }
}

/**
  * @brief  Compute Implementation ID
  * @param  destination buffer
  * @param  len of destination buffer
  * @retval None
  */

static void ComputeImplementationId(uint8_t *hash_result)
{
  bootutil_sha256_context sha256_ctx;
  BOOT_LOG_INF("Code %x %x", (uint32_t)CODE_START
               , (uint32_t)CODE_LIMIT);
  bootutil_sha256_init(&sha256_ctx);
  /* compute write protect area set as excuteable */
  bootutil_sha256_update(&sha256_ctx,
                         CODE_START,
                         (uint32_t)CODE_LIMIT - (uint32_t)CODE_START);

  bootutil_sha256_finish(&sha256_ctx, hash_result);
  BOOT_LOG_INF("hash TFM_SBSFU_Boot  %x%x%x%x .. %x%x%x%x",
               hash_result[0], hash_result[1], hash_result[2], hash_result[3],
               hash_result[28], hash_result[29], hash_result[30], hash_result[31]);

}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
