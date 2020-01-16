/*
 * Copyright (c) 2018-2019 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform/include/tfm_plat_device_id.h"
#include "tfm_secure_api.h"
#include "bl2/include/tfm_boot_status.h"
#include <stddef.h>

/**
  * \brief Copy the device specific ID to the destination buffer
  *
  * \param[out]  p_dst  Pointer to buffer where to store ID
  * \param[in]   p_src  Pointer to the ID
  * \param[in]   size   Length of the ID
  */
static inline void copy_id(uint8_t *p_dst, const uint8_t *p_src, size_t size)
{
  uint32_t i;

  for (i = size; i > 0; i--)
  {
    *p_dst = *p_src;
    p_src++;
    p_dst++;
  }
}
/**
  * Instance ID is mapped to EAT Universal Entity ID (UEID)
  * This implementation creates the instance ID as follows:
  *  - byte 0:    0x01 indicates the type of UEID to be GUID
  *  - byte 1-32: Hash of attestation public key. Public key is hashed in raw
  *               format without any encoding.
  */
enum tfm_plat_err_t tfm_plat_get_instance_id(uint32_t *size, uint8_t *buf)
{
  uint8_t *p_dst;
  int32_t tfm_res;

  if (*size < INSTANCE_ID_MAX_SIZE)
  {
    return TFM_PLAT_ERR_SYSTEM_ERR;
  }
  /*  platform support maximum size */
  *size = INSTANCE_ID_MAX_SIZE;

  buf[0] = 0x01; /* First byte is type byte:  0x01 indicates GUID */
  p_dst = &buf[1];
  tfm_res = tfm_core_get_boot_value(SET_TLV_TYPE(TLV_MAJOR_CORE, TLV_MINOR_CORE_IAS_PUB_KEY_HASH), p_dst, (*size) - 1);
  if (tfm_res != TFM_SUCCESS)
  {
    return  TFM_PLAT_ERR_SYSTEM_ERR;
  }
  return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_get_implementation_id(uint32_t *size,
                                                   uint8_t  *buf)
{
  uint8_t *p_dst;
  int32_t tfm_res;;
  p_dst = &buf[0];
  if (*size < IMPLEMENTATION_ID_MAX_SIZE)
  {
    return TFM_PLAT_ERR_SYSTEM_ERR;
  }
  /*  platform support maximum size */
  *size = IMPLEMENTATION_ID_MAX_SIZE;

  tfm_res = tfm_core_get_boot_value(SET_TLV_TYPE(TLV_MAJOR_CORE, TLV_MINOR_CORE_IAS_IMPLEMENTATION_ID), p_dst, (*size));
  if (tfm_res != (int32_t)TFM_SUCCESS)
  {
    return  TFM_PLAT_ERR_SYSTEM_ERR;
  }
  return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_get_hw_version(uint32_t *size, uint8_t *buf)
{
#if 0
  const uint8_t *p_hw_version = (uint8_t *)UID_BASE;
  uint32_t hw_version_size = 12;
#else
  const uint8_t *p_hw_version = (uint8_t *)&DBGMCU->IDCODE;
  uint32_t hw_version_size = 4;
#endif
  if (*size < hw_version_size)
  {
    return TFM_PLAT_ERR_SYSTEM_ERR;
  }
  /*  recopy  */
  copy_id(buf, p_hw_version, hw_version_size);
  *size = hw_version_size;

  return TFM_PLAT_ERR_SUCCESS;
}
