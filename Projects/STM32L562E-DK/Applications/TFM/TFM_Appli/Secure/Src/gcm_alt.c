/*
 *  NIST SP800-38D compliant GCM implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Copyright (C) 2019, STMicroelectronics, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file implements ST GCM HW services based on API from mbed TLS
 */

/* Includes ------------------------------------------------------------------*/
#include "mbedtls/gcm.h"

#if defined(MBEDTLS_GCM_C)
#if defined(MBEDTLS_GCM_ALT)
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"

/* Parameter validation macros */
#define GCM_VALIDATE_RET( cond ) \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_GCM_BAD_INPUT )
#define GCM_VALIDATE( cond ) \
    MBEDTLS_INTERNAL_VALIDATE( cond )

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/      
#define ST_GCM_TIMEOUT    0xFFU  /* 255 ms timeout for the crypto processor */
#define IV_LENGTH         12U    /* implementations restrict support to 96 bits */

/* Private macro -------------------------------------------------------------*/
#define SWAP_B8_TO_B32(b32,b8,i)                         \
{                                                        \
  (b32) = ( (uint32_t) (b8)[(i) + 3]       )             \
        | ( (uint32_t) (b8)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b8)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b8)[(i)    ] << 24 );            \
}

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
 * Initialize a context
 */
void mbedtls_gcm_init( mbedtls_gcm_context *ctx )
{
    GCM_VALIDATE( ctx != NULL );

    memset( ctx, 0, sizeof( mbedtls_gcm_context ) );
}

int mbedtls_gcm_setkey( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits )
{
    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( key != NULL );

    switch (keybits) {
        case 128:
            ctx->hcryp_gcm.Init.KeySize = CRYP_KEYSIZE_128B;;

            SWAP_B8_TO_B32(ctx->gcm_key[0],key,0);
            SWAP_B8_TO_B32(ctx->gcm_key[1],key,4);
            SWAP_B8_TO_B32(ctx->gcm_key[2],key,8);
            SWAP_B8_TO_B32(ctx->gcm_key[3],key,12);
            break;

        case 192:
            return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;

        case 256:
            ctx->hcryp_gcm.Init.KeySize = CRYP_KEYSIZE_256B;

            SWAP_B8_TO_B32(ctx->gcm_key[0],key,0);
            SWAP_B8_TO_B32(ctx->gcm_key[1],key,4);
            SWAP_B8_TO_B32(ctx->gcm_key[2],key,8);
            SWAP_B8_TO_B32(ctx->gcm_key[3],key,12);
            SWAP_B8_TO_B32(ctx->gcm_key[4],key,16);
            SWAP_B8_TO_B32(ctx->gcm_key[5],key,20);
            SWAP_B8_TO_B32(ctx->gcm_key[6],key,24);
            SWAP_B8_TO_B32(ctx->gcm_key[7],key,28);
            break;

        default :
            return (MBEDTLS_ERR_GCM_BAD_INPUT);
    }

    ctx->hcryp_gcm.Instance = AES;
    ctx->hcryp_gcm.Init.DataType = CRYP_DATATYPE_8B;
    ctx->hcryp_gcm.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;
    ctx->hcryp_gcm.Init.pKey = ctx->gcm_key;
    ctx->hcryp_gcm.Init.Algorithm  = CRYP_AES_GCM_GMAC;

    /* Enable AES clock */
    __HAL_RCC_AES_CLK_ENABLE();

    /* Deinitializes the CRYP peripheral */
    if (HAL_CRYP_DeInit(&ctx->hcryp_gcm) != HAL_OK) {
        return (MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
    }
    if (HAL_CRYP_Init(&ctx->hcryp_gcm) != HAL_OK) {
        return (MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
    }

    /* allow multi-instance of CRYP use: save context for CRYP HW module CR */
    ctx->ctx_save_cr = ctx->hcryp_gcm.Instance->CR;

    return( 0 );
}

int mbedtls_gcm_starts( mbedtls_gcm_context *ctx,
                int mode,
                const unsigned char *iv,
                size_t iv_len,
                const unsigned char *add,
                size_t add_len )
{
    static uint32_t iv_32B[4];

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( mode != MBEDTLS_GCM_ENCRYPT || mode != MBEDTLS_GCM_DECRYPT);
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );

    /* IV and AD are limited to 2^64 bits, so 2^61 bytes */
    /* IV is not allowed to be zero length */
    if( iv_len == 0 ||
      ( (uint64_t) iv_len  ) >> 61 != 0 ||
      ( (uint64_t) add_len ) >> 61 != 0 )
    {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    /* HW implementation restrict support to the length of 96 bits */
    if( IV_LENGTH != iv_len )
    {
        return( MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED );
    }

    /* allow multi-context of CRYP use: restore context */
    ctx->hcryp_gcm.Instance->CR = ctx->ctx_save_cr;

    if ( HAL_CRYP_Init( &ctx->hcryp_gcm ) != HAL_OK )
    {
        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    ctx->mode = mode;
    ctx->len = 0;

    /* Set IV with invert endianness */
    SWAP_B8_TO_B32(iv_32B[0],iv,0);
    SWAP_B8_TO_B32(iv_32B[1],iv,4);
    SWAP_B8_TO_B32(iv_32B[2],iv,8);
    SWAP_B8_TO_B32(iv_32B[3],iv,12);

    /* counter value must be set to 2 when processing the first block of payload */
    iv_32B[3] = 0x00000002;

    ctx->hcryp_gcm.Init.pInitVect = iv_32B;

    if (add_len != 0)
    {
      ctx->hcryp_gcm.Init.Header = (uint32_t *)add;
      /* header buffer in byte length */
      ctx->hcryp_gcm.Init.HeaderSize = (uint32_t)add_len;
    }
    else
    {
      ctx->hcryp_gcm.Init.Header = NULL;
      ctx->hcryp_gcm.Init.HeaderSize = 0;
    }

    /* Additional Authentication Data in bytes unit */
    ctx->hcryp_gcm.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;

    /* Do not Allow IV reconfiguration at every gcm update */
    ctx->hcryp_gcm.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ONCE;

    /* reconfigure the CRYP */
    if ( HAL_CRYP_SetConfig( &ctx->hcryp_gcm, &ctx->hcryp_gcm.Init ) != HAL_OK )
    {
        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    /* allow multi-context of CRYP : save context */
    ctx->ctx_save_cr = ctx->hcryp_gcm.Instance->CR;

    return( 0 );
}

int mbedtls_gcm_update( mbedtls_gcm_context *ctx,
                size_t length,
                const unsigned char *input,
                unsigned char *output )
{
    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );

    if( output > input && (size_t) ( output - input ) < length )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
     * Also check for possible overflow */
    if( ctx->len + length < ctx->len ||
        (uint64_t) ctx->len + length > 0xFFFFFFFE0ull )
    {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    /* allow multi-context of CRYP use: restore context */
    ctx->hcryp_gcm.Instance->CR = ctx->ctx_save_cr;

    ctx->len += length;

    if( ctx->mode == MBEDTLS_GCM_DECRYPT )
    {
         if (HAL_CRYP_Decrypt(&ctx->hcryp_gcm,
                              (uint32_t *)input,
                              length,
                              (uint32_t *)output,
                              ST_GCM_TIMEOUT) != HAL_OK)
         {
            return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
         }
    }
    else
    {
         if (HAL_CRYP_Encrypt(&ctx->hcryp_gcm,
                              (uint32_t *)input,
                              length,
                              (uint32_t *)output,
                              ST_GCM_TIMEOUT) != HAL_OK)
         {
            return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
         }
    }

    /* allow multi-context of CRYP : save context */
    ctx->ctx_save_cr = ctx->hcryp_gcm.Instance->CR;

    return( 0 );
}

int mbedtls_gcm_finish( mbedtls_gcm_context *ctx,
                unsigned char *tag,
                size_t tag_len )
{
    __ALIGN_BEGIN uint8_t mac[16]      __ALIGN_END; /* temporary mac         */

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( tag != NULL );

    if( tag_len > 16 || tag_len < 4 )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    /* implementation restrict support to a 16 bytes tag buffer */
    if( tag_len != 16 )
        return( MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED );

    /* allow multi-context of CRYP use: restore context */
    ctx->hcryp_gcm.Instance->CR = ctx->ctx_save_cr;

    /* Tag has a variable length */
    memset(mac, 0, sizeof(mac));

    /* Generate the authentication TAG */
    if (HAL_CRYPEx_AESGCM_GenerateAuthTAG(&ctx->hcryp_gcm,
                                          (uint32_t *)mac,
                                          ST_GCM_TIMEOUT)!= HAL_OK)
    {
        return (MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
    }

    memcpy( tag, mac, tag_len );

    /* allow multi-context of CRYP : save context */
    ctx->ctx_save_cr = ctx->hcryp_gcm.Instance->CR;

    return( 0 );
}

int mbedtls_gcm_crypt_and_tag( mbedtls_gcm_context *ctx,
                       int mode,
                       size_t length,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t tag_len,
                       unsigned char *tag )
{
    int ret;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );
    GCM_VALIDATE_RET( tag != NULL );

    if( ( ret = mbedtls_gcm_starts( ctx, mode, iv, iv_len, add, add_len ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_gcm_update( ctx, length, input, output ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_gcm_finish( ctx, tag, tag_len ) ) != 0 )
        return( ret );

    return( 0 );
}

int mbedtls_gcm_auth_decrypt( mbedtls_gcm_context *ctx,
                      size_t length,
                      const unsigned char *iv,
                      size_t iv_len,
                      const unsigned char *add,
                      size_t add_len,
                      const unsigned char *tag,
                      size_t tag_len,
                      const unsigned char *input,
                      unsigned char *output )
{
    int ret;
    unsigned char check_tag[16];
    size_t i;
    int diff;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );
    GCM_VALIDATE_RET( tag != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );

    if( ( ret = mbedtls_gcm_crypt_and_tag( ctx, MBEDTLS_GCM_DECRYPT, length,
                                   iv, iv_len, add, add_len,
                                   input, output, tag_len, check_tag ) ) != 0 )
    {
        return( ret );
    }

    /* Check tag in "constant-time" */
    for( diff = 0, i = 0; i < tag_len; i++ )
        diff |= tag[i] ^ check_tag[i];

    if( diff != 0 )
    {
        mbedtls_platform_zeroize( output, length );
        return( MBEDTLS_ERR_GCM_AUTH_FAILED );
    }

    return( 0 );
}

void mbedtls_gcm_free( mbedtls_gcm_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_gcm_context ) );
}

#endif /* MBEDTLS_GCM_ALT */
#endif /* MBEDTLS_GCM_C */