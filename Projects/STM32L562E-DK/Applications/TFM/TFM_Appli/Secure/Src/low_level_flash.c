/*
 * Copyright (c) 2013-2018 Arm Limited
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

#include "Driver_Flash.h"

#include <string.h>
#include "cmsis.h"
#include "flash_layout.h"
#include "stm32l5xx_hal.h"
#include <stdio.h>

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/*
#define DEBUG_FLASH_ACCESS
#define CHECK_ERASE
*/
#define CHECK_WRITE
/* Driver version */
#define ARM_FLASH_DRV_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,  /* Defined in the CMSIS Flash Driver header file */
    ARM_FLASH_DRV_VERSION
};

/**
 * \brief Flash driver capability macro definitions \ref ARM_FLASH_CAPABILITIES
 */
/* Flash Ready event generation capability values */
#define EVENT_READY_NOT_AVAILABLE   (0u)
#define EVENT_READY_AVAILABLE       (1u)
/* Data access size values */
#define DATA_WIDTH_8BIT             (0u)
#define DATA_WIDTH_16BIT            (1u)
#define DATA_WIDTH_32BIT            (2u)
/* Chip erase capability values */
#define CHIP_ERASE_NOT_SUPPORTED    (0u)
#define CHIP_ERASE_SUPPORTED        (1u)

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    EVENT_READY_NOT_AVAILABLE,
    DATA_WIDTH_32BIT,
    CHIP_ERASE_SUPPORTED
};

/**
 * \brief Flash status macro definitions \ref ARM_FLASH_STATUS
 */
/* Busy status values of the Flash driver  */
#define DRIVER_STATUS_IDLE      (0u)
#define DRIVER_STATUS_BUSY      (1u)
/* Error status values of the Flash driver */
#define DRIVER_STATUS_NO_ERROR  (0u)
#define DRIVER_STATUS_ERROR     (1u)

/**
 * \brief Arm Flash device structure.
 */
struct arm_flash_dev_t {
    ARM_FLASH_INFO *data;       /*!< FLASH memory device data */
};

/**
 * \brief      Check if the Flash memory boundaries are not violated.
 * \param[in]  flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]  offset     Highest Flash memory address which would be accessed.
 * \return     Returns true if Flash memory boundaries are not violated, false
 *             otherwise.
 */

static bool is_range_valid(struct arm_flash_dev_t *flash_dev,
                           uint32_t offset)
{
    uint32_t flash_limit = 0;

    /* Calculating the highest address of the Flash memory address range */
    flash_limit = FLASH_TOTAL_SIZE - 1;

    return (offset > flash_limit) ? (false) : (true) ;
}
/**
  * \brief        Check if the parameter is an erasebale page.
  * \param[in]    flash_dev  Flash device structure \ref arm_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is a sector eraseable, false
  *               otherwise.
  */
static bool is_erase_allow(struct arm_flash_dev_t *flash_dev,
                           uint32_t param)
{
  /*  allow erase inside  NV Counter,ITS, SST */
  return
    ((param >= FLASH_NV_COUNTERS_AREA_OFFSET) && 
       (param < (FLASH_ITS_AREA_OFFSET + FLASH_ITS_AREA_SIZE)))
    ? (true) : (false);
}
/**
  * \brief        Check if the parameter is writeable area.
  * \param[in]    flash_dev  Flash device structure \ref arm_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is aligned to program_unit, false
  *               otherwise.
  */

static bool is_write_allow(struct arm_flash_dev_t *flash_dev,
                           uint32_t start, uint32_t len)
{
  /*  allow write inside  NV Counter,ITS, SST */
  return ((start + len) < (FLASH_ITS_AREA_OFFSET + FLASH_ITS_AREA_SIZE)) &&
         (start >= (FLASH_NV_COUNTERS_AREA_OFFSET))
         ? true : false;
}
/**
 * \brief        Check if the parameter is aligned to program_unit.
 * \param[in]    flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]    param      Any number that can be checked against the
 *                          program_unit, e.g. Flash memory address or
 *                          data length in bytes.
 * \return       Returns true if param is aligned to program_unit, false
 *               otherwise.
 */

static bool is_write_aligned(struct arm_flash_dev_t *flash_dev,
                             uint32_t param)
{
    return ((param % flash_dev->data->program_unit) != 0) ? (false) : (true);
}
/**
 * \brief        Check if the parameter is aligned to page_unit.
 * \param[in]    flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]    param      Any number that can be checked against the
 *                          program_unit, e.g. Flash memory address or
 *                          data length in bytes.
 * \return       Returns true if param is aligned to sector_unit, false
 *               otherwise.
 */
static bool is_erase_aligned(struct arm_flash_dev_t *flash_dev,
                             uint32_t param)
{
/*  2 pages */
    return ((param % (flash_dev->data->sector_size)) != 0) ? (false) : (true);
}

/**
 * \brief        compute bank number.
 * \param[in]    flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]    param      &n address in flash
 * \return       Returns true if param is aligned to sector_unit, false
 *               otherwise.
 */
static uint32_t bank_number(struct arm_flash_dev_t *flash_dev,
                             uint32_t param)
{
    return ((param >= (FLASH_TOTAL_SIZE/2) ) ? 2 : 1);
}

/**
 * \brief        compute page number.
 * \param[in]    flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]    param      &n address in flash
 * \return       Returns true if param is aligned to sector_unit, false
 *               otherwise.
 */
static uint32_t page_number(struct arm_flash_dev_t *flash_dev,
                             uint32_t param)
{
    uint32_t page = param / flash_dev->data->page_size ;
    page = ((page > (flash_dev->data->sector_count))) ? page -((flash_dev->data->sector_count)) : page;
#ifdef DEBUG_FLASH_ACCESS
    printf("page = %x \r\n",page);
#endif
    return page;
}

static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = FLASH_TOTAL_SIZE / 0x1000,
    .sector_size    = 0x1000,
    .page_size      = 0x800,
    .program_unit   = 8u,       /* Minimum write size in bytes */
    .erased_value   = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
    .data   = &(ARM_FLASH0_DEV_DATA)
};

/* Flash Status */
static ARM_FLASH_STATUS ARM_FLASH0_STATUS = {0, 0, 0};

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
  ARG_UNUSED(cb_event);
  FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch(state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    default:
        return ARM_DRIVER_ERROR_PARAMETER;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    bool is_valid = true;

    ARM_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

    /* Check Flash memory boundaries */
    is_valid = is_range_valid(&ARM_FLASH0_DEV, addr + cnt -1);
    if(is_valid != true) {
        ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR_PARAMETER;
    }
    /*  ECC to implemeny with NMI */
    /*  do a memcpy */
#ifdef DEBUG_FLASH_ACCESS
    printf("read %x n=%x \r\n", (addr+FLASH_BASE), cnt);
#endif
    memcpy(data,(void*)((uint32_t)addr+FLASH_BASE), cnt);
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr,
                                     const void *data, uint32_t cnt)
{
    uint32_t loop = 0;
    HAL_StatusTypeDef err;
#ifdef CHECK_WRITE
    void *dest=(void *)(FLASH_BASE+addr);
#endif
    ARM_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;
#ifdef DEBUG_FLASH_ACCESS
    printf("write %x n=%lx \r\n", (addr+FLASH_BASE), cnt);
#endif
    /* Check Flash memory boundaries and alignment with minimum write size
     * (program_unit), data size also needs to be a multiple of program_unit.
     */
  if ((!is_range_valid(&ARM_FLASH0_DEV, addr + cnt - 1)) ||
      (!is_write_aligned(&ARM_FLASH0_DEV, addr))     ||
      (!is_write_aligned(&ARM_FLASH0_DEV, cnt))      ||
      (!is_write_allow(&ARM_FLASH0_DEV, addr, cnt)))
    {

        ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
#ifdef DEBUG_FLASH_ACCESS
        printf("failed write %lx n=%x \n", (addr+FLASH_BASE), cnt);
#endif
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    HAL_FLASH_Unlock();
    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;
    do {
       /* dword api*/
        uint64_t dword;
        memcpy(&dword,  (void *)((uint32_t)data+loop), sizeof(dword));
        err = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (FLASH_BASE+addr), dword);
        loop += sizeof(dword);
        addr += sizeof(dword);
    }while((loop!=cnt) && (err == HAL_OK));

    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;
    HAL_FLASH_Lock();
    /* compare data written */
#ifdef CHECK_WRITE
    if ((err == HAL_OK) && memcmp(dest,data,cnt)){
    err=HAL_ERROR;
#ifdef DEBUG_FLASH_ACCESS   
    printf("write %lx n=%x (cmp failed)\n", (addr+FLASH_BASE), cnt);
#endif
    }
#endif
#ifdef DEBUG_FLASH_ACCESS
    if (err != HAL_OK)
          printf("failed write %lx n=%x \r\n", (addr+FLASH_BASE), cnt);
#endif
    return (err == HAL_OK) ? ARM_DRIVER_OK :ARM_DRIVER_ERROR;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    FLASH_EraseInitTypeDef EraseInit;
    HAL_StatusTypeDef err;
    uint32_t pageError;
#ifdef CHECK_ERASE
    int i;
    uint32_t *pt;
#endif
#ifdef DEBUG_FLASH_ACCESS
    printf("erase %x\r\n", addr);
#endif
  if (!(is_range_valid(&ARM_FLASH0_DEV, addr)) ||
      !(is_erase_aligned(&ARM_FLASH0_DEV, addr)) ||
      !(is_erase_allow(&ARM_FLASH0_DEV, addr)))
    {
         ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
#ifdef DEBUG_FLASH_ACCESS
         printf("failed erase %x\r\n", addr);
#endif
         return ARM_DRIVER_ERROR_PARAMETER;
    }

    EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    /*  fix me assume dual bank, reading DBANK in OPTR in Flash init is better */
    /*  flash size in  DB256K in OPTR */
    EraseInit.Banks = bank_number(&ARM_FLASH0_DEV, addr);
    /*  erase 2 pages because we declare that we have 4096 pages */
    EraseInit.NbPages = 2;
    EraseInit.Page = page_number(&ARM_FLASH0_DEV, addr);

    ARM_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;
    HAL_FLASH_Unlock();
    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;
    err = HAL_FLASHEx_Erase(&EraseInit, &pageError);
    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;
    HAL_FLASH_Lock();
#ifdef DEBUG_FLASH_ACCESS
    if (err != HAL_OK)
      printf("erase failed \r\n");
#endif
#ifdef CHECK_ERASE
    pt=(uint32_t *)((uint32_t)FLASH_BASE+addr);
    for (i=0; i<0x400;i++)
      if (pt[i]!=0xffffffff)
      {
#ifdef DEBUG_FLASH_ACCESS
          printf("erase failed off %x %x %x\r\n",addr, &pt[i], pt[i]);
#endif
          return ARM_DRIVER_ERROR;
      }
#endif
  return (err == HAL_OK) ? ARM_DRIVER_OK :ARM_DRIVER_ERROR;
}

static int32_t ARM_Flash_EraseChip(void)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return ARM_FLASH0_STATUS;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
    return ARM_FLASH0_DEV.data;
}

ARM_DRIVER_FLASH TFM_Driver_FLASH0 = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo
};

