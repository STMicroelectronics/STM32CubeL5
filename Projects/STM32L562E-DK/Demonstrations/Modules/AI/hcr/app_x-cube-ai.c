
#ifdef __cplusplus
 extern "C" {
#endif
/**
  ******************************************************************************
  * @file           : app_x-cube-ai.c
  * @brief          : AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "app_x-cube-ai.h"
#include "ai_datatypes_defines.h"

/* USER CODE BEGIN includes */
/* USER CODE END includes */

/*************************************************************************
  *
  */

/* Multiple network support --------------------------------------------------*/

#include <string.h>
#include "ai_datatypes_defines.h"

static const ai_network_entry_t networks[AI_MNETWORK_NUMBER] = {
    {
        .name = (const char *)AI_NETWORK_MODEL_NAME,
        .config = AI_NETWORK_DATA_CONFIG,
        .ai_get_info = ai_network_get_info,
        .ai_create = ai_network_create,
        .ai_destroy = ai_network_destroy,
        .ai_get_error = ai_network_get_error,
        .ai_init = ai_network_init,
        .ai_run = ai_network_run,
        .ai_forward = ai_network_forward,
        .ai_data_weights_get_default = ai_network_data_weights_get,
        .params = { AI_NETWORK_DATA_WEIGHTS(0),
                AI_NETWORK_DATA_ACTIVATIONS(0)},
        .extActBufferStartAddr = AI_NETWORK_DATA_ACTIVATIONS_START_ADDR,
        .actBufferSize = AI_NETWORK_DATA_ACTIVATIONS_SIZE
    },
};

struct network_instance {
     const ai_network_entry_t *entry;
     ai_handle handle;
     ai_network_params params;
};

/* Number of instance is aligned on the number of network */
AI_STATIC struct network_instance gnetworks[AI_MNETWORK_NUMBER] = {0};

AI_DECLARE_STATIC
ai_bool ai_mnetwork_is_valid(const char* name,
        const ai_network_entry_t *entry)
{
    if (name && (strlen(entry->name) == strlen(name)) &&
            (strncmp(entry->name, name, strlen(entry->name)) == 0))
        return true;
    return false;
}

AI_DECLARE_STATIC
struct network_instance *ai_mnetwork_handle(struct network_instance *inst)
{
    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        if ((inst) && (&gnetworks[i] == inst))
            return inst;
        else if ((!inst) && (gnetworks[i].entry == NULL))
            return &gnetworks[i];
    }
    return NULL;
}

AI_DECLARE_STATIC
void ai_mnetwork_release_handle(struct network_instance *inst)
{
    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        if ((inst) && (&gnetworks[i] == inst)) {
            gnetworks[i].entry = NULL;
            return;
        }
    }
}

AI_API_ENTRY
const char* ai_mnetwork_find(const char *name, ai_int idx)
{
    const ai_network_entry_t *entry;

    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        entry = &networks[i];
        if (ai_mnetwork_is_valid(name, entry))
            return entry->name;
        else {
            if (!idx--)
                return entry->name;
        }
    }
    return NULL;
}

AI_API_ENTRY
ai_error ai_mnetwork_create(const char *name, ai_handle* network,
        const ai_buffer* network_config)
{
    const ai_network_entry_t *entry;
    const ai_network_entry_t *found = NULL;
    ai_error err;
    struct network_instance *inst = ai_mnetwork_handle(NULL);

    if (!inst) {
        err.type = AI_ERROR_ALLOCATION_FAILED;
        err.code = AI_ERROR_CODE_NETWORK;
        return err;
    }

    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        entry = &networks[i];
        if (ai_mnetwork_is_valid(name, entry)) {
            found = entry;
            break;
        }
    }

    if (!found) {
        err.type = AI_ERROR_INVALID_PARAM;
        err.code = AI_ERROR_CODE_NETWORK;
        return err;
    }

    if (network_config == NULL)
        err = found->ai_create(network, found->config);
    else
        err = found->ai_create(network, network_config);
    if ((err.code == AI_ERROR_CODE_NONE) && (err.type == AI_ERROR_NONE)) {
        inst->entry = found;
        inst->handle = *network;
        *network = (ai_handle*)inst;
    }

    return err;
}

AI_API_ENTRY
ai_handle ai_mnetwork_destroy(ai_handle network)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn) {
        ai_handle hdl = inn->entry->ai_destroy(inn->handle);
        if (hdl != inn->handle) {
            ai_mnetwork_release_handle(inn);
            network = AI_HANDLE_NULL;
        }
    }
    return network;
}

AI_API_ENTRY
ai_bool ai_mnetwork_get_info(ai_handle network, ai_network_report* report)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_get_info(inn->handle, report);
    else
        return false;
}

AI_API_ENTRY
ai_error ai_mnetwork_get_error(ai_handle network)
{
    struct network_instance *inn;
    ai_error err;
    err.type = AI_ERROR_INVALID_PARAM;
    err.code = AI_ERROR_CODE_NETWORK;

    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_get_error(inn->handle);
    else
        return err;
}

AI_API_ENTRY
ai_bool ai_mnetwork_init(ai_handle network, const ai_network_params* params)
{
    struct network_instance *inn;
    ai_network_params par;

    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn) {
        par = inn->entry->params;
        if (params->activations.n_batches)
            par.activations = params->activations;
        else
            par.activations.data = params->activations.data;
        if (params->params.n_batches)
            par.params = params->params;
        else
            par.params.data = inn->entry->ai_data_weights_get_default();
        return inn->entry->ai_init(inn->handle, &par);
    }
    else
        return false;
}

AI_API_ENTRY
ai_i32 ai_mnetwork_run(ai_handle network, const ai_buffer* input,
        ai_buffer* output)
{
    struct network_instance* inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_run(inn->handle, input, output);
    else
        return 0;
}

AI_API_ENTRY
ai_i32 ai_mnetwork_forward(ai_handle network, const ai_buffer* input)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_forward(inn->handle, input);
    else
        return 0;
}

AI_API_ENTRY
 int ai_mnetwork_get_private_handle(ai_handle network,
         ai_handle *phandle,
         ai_network_params *pparams)
 {
     struct network_instance* inn;
     inn =  ai_mnetwork_handle((struct network_instance *)network);
     if (inn && phandle && pparams) {
         *phandle = inn->handle;
         *pparams = inn->params;
         return 0;
     }
     else
         return -1;
 }

 AI_API_ENTRY
int ai_mnetwork_get_ext_data_activations(ai_handle network,
         ai_u32 *add,
         ai_u32 *size)
 {
     struct network_instance* inn;
     inn =  ai_mnetwork_handle((struct network_instance *)network);
     if (inn && add && size) {
         *add = inn->entry->extActBufferStartAddr;
         *size = inn->entry->actBufferSize;
         return 0;
     }
     else
         return -1;
 }

#ifdef __cplusplus
}
#endif
