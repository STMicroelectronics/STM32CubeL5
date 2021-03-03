/**
******************************************************************************
* @file    ospi_nor.c
* @author  MCD Application Team
* @brief   This example code shows how to use the BSP OSPI Driver
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L5xx_HAL_Examples
* @{
*/

/** @addtogroup BSP
* @{
*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define OSPI_NOR_BUFFER_SIZE     ((uint32_t)0x0200)
#define OSPI_NOR_WRITE_READ_ADDR ((uint32_t)0x0050)
#define OSPI_NOR_BASE_ADDR  ((uint32_t)0x90000000)

/* Private macro -------------------------------------------------------------*/
#define LCD_TEXT_XPOS       20
#define LCD_TEXT_YPOS_START 80
#define LCD_TEXT_YPOS_INC   10

/* Private variables ---------------------------------------------------------*/
uint8_t ospi_nor_aTxBuffer[OSPI_NOR_BUFFER_SIZE];
uint8_t ospi_nor_aRxBuffer[MX25LM51245G_SUBSECTOR_4K];

/* Private function prototypes -----------------------------------------------*/
static void     OSPI_NOR_SetHint(void);
static void     Fill_Buffer (uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t  Buffercmp   (uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
static uint8_t  DataCmp     (uint8_t* pBuffer, uint8_t Pattern, uint32_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  OSPI NOR Demo
* @param  None
* @retval None
*/
void OSPI_NOR_demo(void)
{ 
  BSP_OSPI_NOR_Init_t sOSPI_NOR_Init;
  BSP_OSPI_NOR_Info_t sOSPI_NOR_Info;
  int32_t status;
  __IO uint8_t *data_ptr;
  uint32_t index;
  Led_TypeDef led = LED_RED;
  uint16_t lcd_text_ypos = LCD_TEXT_YPOS_START;
  
  /********************************** PART 1 **********************************/
  led = LED_RED;
  lcd_text_ypos = LCD_TEXT_YPOS_START;
  OSPI_NOR_SetHint();
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Part I : Write & Read,", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"         Data Integrity,", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"         Memory mapped", LEFT_MODE);
  lcd_text_ypos+=(LCD_TEXT_YPOS_INC * 2);
  
  /*##-1- Configure the OSPI NOR device ######################################*/
  /* OSPI NOR device configuration */ 
  sOSPI_NOR_Init.InterfaceMode = BSP_OSPI_NOR_SPI_MODE;
  sOSPI_NOR_Init.TransferRate  = BSP_OSPI_NOR_STR_TRANSFER;
  status = BSP_OSPI_NOR_Init(0, &sOSPI_NOR_Init);
  
  if (status != BSP_ERROR_NONE)
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : Failed", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : OK", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;

    /*##-2- Read & check the OSPI NOR info ###################################*/
    /* Initialize the structure */
    sOSPI_NOR_Info.FlashSize             = (uint32_t)0x00;
    sOSPI_NOR_Info.EraseSectorSize       = (uint32_t)0x00;
    sOSPI_NOR_Info.EraseSectorsNumber    = (uint32_t)0x00;
    sOSPI_NOR_Info.EraseSubSectorSize    = (uint32_t)0x00;
    sOSPI_NOR_Info.EraseSubSectorNumber  = (uint32_t)0x00;
    sOSPI_NOR_Info.EraseSubSector1Size   = (uint32_t)0x00;
    sOSPI_NOR_Info.EraseSubSector1Number = (uint32_t)0x00;
    sOSPI_NOR_Info.ProgPageSize          = (uint32_t)0x00;
    sOSPI_NOR_Info.ProgPagesNumber       = (uint32_t)0x00;
    
    /* Read the OSPI NOR memory info */
    if (BSP_OSPI_NOR_GetInfo(0, &sOSPI_NOR_Info) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    
    /* Test the correctness */
    if((sOSPI_NOR_Info.FlashSize             != MX25LM51245G_FLASH_SIZE)   ||
       (sOSPI_NOR_Info.EraseSectorSize       != MX25LM51245G_SECTOR_64K)   ||
       (sOSPI_NOR_Info.EraseSubSectorSize    != MX25LM51245G_SUBSECTOR_4K) ||
       (sOSPI_NOR_Info.EraseSubSector1Size   != MX25LM51245G_SUBSECTOR_4K) ||
       (sOSPI_NOR_Info.ProgPageSize          != MX25LM51245G_PAGE_SIZE)    ||
       (sOSPI_NOR_Info.EraseSectorsNumber    != (MX25LM51245G_FLASH_SIZE/MX25LM51245G_SECTOR_64K))   ||
       (sOSPI_NOR_Info.EraseSubSectorNumber  != (MX25LM51245G_FLASH_SIZE/MX25LM51245G_SUBSECTOR_4K)) ||
       (sOSPI_NOR_Info.EraseSubSector1Number != (MX25LM51245G_FLASH_SIZE/MX25LM51245G_SUBSECTOR_4K)) ||
       (sOSPI_NOR_Info.ProgPagesNumber       != (MX25LM51245G_FLASH_SIZE/MX25LM51245G_PAGE_SIZE)))
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Get Info : Failed", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Get Info : OK", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      
      /* Caution: Despite instruction cache is enabled, no need for cache invalidation here since no previous */
      /*          fetch (read/execute) instructions were executed from OSPI_NOR_WRITE_READ_ADDR               */

      /*##-3- Erase OSPI NOR memory ##########################################*/ 
      if (BSP_OSPI_NOR_Erase_Block(0, OSPI_NOR_WRITE_READ_ADDR, MX25LM51245G_ERASE_64K) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase : Failed", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase : OK", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;

        /*##-4- OSPI NOR memory read/write access  ###########################*/   
        /* Fill the buffer to write */
        Fill_Buffer(ospi_nor_aTxBuffer, OSPI_NOR_BUFFER_SIZE, 0xD20F);   
        
        /* Write data to the OSPI NOR memory */
        if (BSP_OSPI_NOR_Write(0, ospi_nor_aTxBuffer, OSPI_NOR_WRITE_READ_ADDR, OSPI_NOR_BUFFER_SIZE) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Write : Failed", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
        }
        else
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Write : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;

          /* Read back data from the OSPI NOR memory */
          if (BSP_OSPI_NOR_Read(0, ospi_nor_aRxBuffer, OSPI_NOR_WRITE_READ_ADDR, OSPI_NOR_BUFFER_SIZE) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Read : Failed", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
          }
          else
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Read : OK", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;

            /*##-5- Checking data integrity ##################################*/  
            if(Buffercmp(ospi_nor_aRxBuffer, ospi_nor_aTxBuffer, OSPI_NOR_BUFFER_SIZE) > 0)
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Compare : Failed", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
            }
            else
            {    
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Compare : OK", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;

              /*##-6- OSPI NOR memory in memory-mapped mode###################*/
              if (BSP_OSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Cfg : Failed", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
              }
              else
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Cfg : OK", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;

                for(index = 0, data_ptr = (__IO uint8_t *)(OSPI_NOR_BASE_ADDR + OSPI_NOR_WRITE_READ_ADDR); 
                    index < OSPI_NOR_BUFFER_SIZE; index++, data_ptr++)
                {
                  if(*data_ptr != ospi_nor_aTxBuffer[index])
                  {
                    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Access : Failed", LEFT_MODE);
                    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
                    break;
                  }
                }
                
                if(index == OSPI_NOR_BUFFER_SIZE)
                {
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Access : OK", LEFT_MODE);
                  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test : OK", LEFT_MODE);
                  led = LED_GREEN;
                }
              }
            }  
          }
        }
      }
    }
  }
  
  while (1)
  {    
    if(UserButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      UserButtonPressed = RESET;

      break;
    }
    BSP_LED_Toggle(led);
    HAL_Delay(100);
  }
  BSP_LED_Off(led);

  /********************************** PART 2 **********************************/
  /* Suspend/Resume during erase operation */
  led = LED_RED;
  lcd_text_ypos = LCD_TEXT_YPOS_START;
  OSPI_NOR_SetHint();
  
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Part II : Erase Suspend and", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"          Resume", LEFT_MODE);
  lcd_text_ypos+=(LCD_TEXT_YPOS_INC * 2);

  /*##-1- Deconfigure the OSPI NOR device ####################################*/
  status = BSP_OSPI_NOR_DeInit(0);
  
  if (status != BSP_ERROR_NONE)
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"De-Initialization : Failed", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"De-Initialization : OK", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;

    /*##-2- Reconfigure the OSPI NOR device ##################################*/
    /* QSPI device configuration */ 
    status = BSP_OSPI_NOR_Init(0, &sOSPI_NOR_Init);
    
    if (status != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : Failed", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : OK", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      
      /*##-3- Erase OSPI NOR memory ##########################################*/ 
      if (BSP_OSPI_NOR_Erase_Block(0, 0, MX25LM51245G_ERASE_4K) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase : Failed", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
      }
      else
      {

        /*##-4- Suspend erase OSPI NOR memory ################################*/
        if (BSP_OSPI_NOR_SuspendErase(0) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase Suspend : Failed", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
        }
        else
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase Suspend : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          
          /*##-6- Resume erase OSPI NOR memory ###############################*/ 
          if (BSP_OSPI_NOR_ResumeErase(0) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase Resume : Failed", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
          }
          else
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase Resume : OK", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            
            /*##-7- Check OSPI NOR memory status  ############################*/   
            /* Wait the end of the current operation on memory side */
            do
            {
              status = BSP_OSPI_NOR_GetStatus(0);
            } while((status != BSP_ERROR_NONE) && (status != BSP_ERROR_COMPONENT_FAILURE));
            
            if(status != BSP_ERROR_NONE)
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Memory Status : Failed", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
            }
            else
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Memory Status : OK", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              
              /*##-8- OSPI NOR memory read access  ###########################*/   
              /* Read back data from the OSPI NOR memory */
              if (BSP_OSPI_NOR_Read(0, ospi_nor_aRxBuffer, 0, MX25LM51245G_SUBSECTOR_4K) != BSP_ERROR_NONE)
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Read : Failed", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
              }
              else
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Read : OK", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                
                /*##-9- Checking data integrity ##############################*/  
                if (DataCmp(ospi_nor_aRxBuffer, 0xFF, MX25LM51245G_SUBSECTOR_4K) > 0)
                {
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Compare : Failed", LEFT_MODE);
                  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
                }
                else
                {    
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Compare : OK", LEFT_MODE);
                  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test : OK", LEFT_MODE);
                  led = LED_GREEN;
                }
              }
            }
          }  
        }
      }
    }
  }

  while (1)
  {    
    if(UserButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      UserButtonPressed = RESET;

      break;
    }
    BSP_LED_Toggle(led);
    HAL_Delay(100);
  }
  BSP_LED_Off(led);

  /********************************** PART 3 **********************************/
  led = LED_RED;
  lcd_text_ypos = LCD_TEXT_YPOS_START;
  OSPI_NOR_SetHint();
  
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Part III : Enter & Leave", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"           Flash Power Down", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"           mode", LEFT_MODE);
  lcd_text_ypos+=(LCD_TEXT_YPOS_INC * 2);

  /*##-1- Deconfigure the OSPI NOR device ####################################*/
  status = BSP_OSPI_NOR_DeInit(0);
  
  if (status != BSP_ERROR_NONE)
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"De-Initialization : Failed", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"De-Initialization : OK", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;

    /*##-2- Reconfigure the OSPI NOR device ##################################*/
    /* OSPI NOR device configuration */ 
    status = BSP_OSPI_NOR_Init(0, &sOSPI_NOR_Init);
    
    if (status != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : Failed", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : OK", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      
      /*##-3- Erase OSPI NOR memory ##########################################*/ 
      if (BSP_OSPI_NOR_Erase_Block(0, OSPI_NOR_WRITE_READ_ADDR, MX25LM51245G_ERASE_4K) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase : Failed", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Erase : OK", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;

        /*##-4- OSPI NOR memory write access #################################*/
        /* Fill the buffer to write */
        Fill_Buffer(ospi_nor_aTxBuffer, OSPI_NOR_BUFFER_SIZE, 0xD20F);   
        
        /* Write data to the OSPI NOR memory */
        if (BSP_OSPI_NOR_Write(0, ospi_nor_aTxBuffer, OSPI_NOR_WRITE_READ_ADDR, OSPI_NOR_BUFFER_SIZE) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Write : Failed", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
        }
        else
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Write : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;

          /*##-5- Enter OSPI NOR memory in deep power down ###################*/ 
          if (BSP_OSPI_NOR_EnterDeepPowerDown(0) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Enter Deep PwrDn : Failed", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
          }
          else
          {
            /* !!! Warning : This is only a test of the API. To check if the memory is really in deep power down, 
                   need to use Idd to check the consumption !!! */
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Enter Deep PwrDn : OK", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            
            /*##-6- Leave OSPI NOR memory from deep power down ###############*/ 
            HAL_Delay(100);
            if (BSP_OSPI_NOR_LeaveDeepPowerDown(0) != BSP_ERROR_NONE)
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Leave Deep PwrDn : Failed", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
            }
            else
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Leave Deep PwrDn : OK", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              
              /*##-7- OSPI NOR memory in memory-mapped mode#######################*/
              if (BSP_OSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Cfg : Failed", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
              }
              else
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Cfg : OK", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;

                for(index = 0, data_ptr = (__IO uint8_t *)(OSPI_NOR_BASE_ADDR + OSPI_NOR_WRITE_READ_ADDR); 
                    index < OSPI_NOR_BUFFER_SIZE; index++, data_ptr++)
                {
                  if(*data_ptr != ospi_nor_aTxBuffer[index])
                  {
                    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Access : Failed", LEFT_MODE);
                    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
                    break;
                  }
                }
                
                if(index == OSPI_NOR_BUFFER_SIZE)
                {
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Mem-Mapped Access : OK", LEFT_MODE);
                  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test : OK", LEFT_MODE);
                  led = LED_GREEN;
                }
              }
            }
          }  
        }
      }
    }
  }

  while (1)
  {    
    if(UserButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      UserButtonPressed = RESET;

      break;
    }
    BSP_LED_Toggle(led);
    HAL_Delay(100);
  }
  BSP_LED_Off(led);

  /********************************** PART 4 **********************************/
  /* Erase NOR  (up to 300s) */
  led = LED_RED;
  lcd_text_ypos = LCD_TEXT_YPOS_START;
  OSPI_NOR_SetHint();
  
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Part IV : Erase ...", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"          wait up to 300s", LEFT_MODE);
  lcd_text_ypos+=(LCD_TEXT_YPOS_INC * 2);

  /*##-1- Deconfigure the OSPI NOR device ####################################*/
  status = BSP_OSPI_NOR_DeInit(0);
  
  if (status != BSP_ERROR_NONE)
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"De-Initialization : Failed", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"De-Initialization : OK", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;

    /*##-2- Reconfigure the OSPI NOR device ##################################*/
    /* OSPI NOR device configuration */ 
    status = BSP_OSPI_NOR_Init(0, &sOSPI_NOR_Init);
    
    if (status != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : Failed", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Initialization : OK", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      
      /*##-3- Erase OSPI NOR memory ##########################################*/ 
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Chip Erasing...", LEFT_MODE);

      if (BSP_OSPI_NOR_Erase_Chip(0) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Chip Erase : Failed", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Chip Erase : OK", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;

        /* Wait the end of the current operation on memory side */
        do
        {
          status = BSP_OSPI_NOR_GetStatus(0);
        } while((status != BSP_ERROR_NONE) && (status != BSP_ERROR_COMPONENT_FAILURE));
            
        if(status != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Memory Status : Failed", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
        }
        else
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Memory Status : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;

          for (index = 0; index < (MX25LM51245G_FLASH_SIZE / MX25LM51245G_SUBSECTOR_4K); index++)
          {
            /*##-4- OSPI NOR memory read access  ###############################*/
            /* Read back data from the OSPI NOR memory */
            if (BSP_OSPI_NOR_Read(0, ospi_nor_aRxBuffer, (index * MX25LM51245G_SUBSECTOR_4K), MX25LM51245G_SUBSECTOR_4K) != BSP_ERROR_NONE)
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Read : Failed", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
              break;
            }
            else
            {
              /*##-5- Checking data integrity ################################*/  
              if(DataCmp(ospi_nor_aRxBuffer, 0xFF, MX25LM51245G_SUBSECTOR_4K) > 0)
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Compare : Failed", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test Aborted", LEFT_MODE);
                break;
              }
            }
          }

          if (index == (MX25LM51245G_FLASH_SIZE / MX25LM51245G_SUBSECTOR_4K))
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Test : OK", LEFT_MODE);
            led = LED_GREEN;
          }
        }
      }
    }
  }

  /* De-initialization in order to have correct configuration memory on next try */
  BSP_OSPI_NOR_DeInit(0);

  while (1)
  {    
    if(UserButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      BSP_LED_Off(led);
      HAL_Delay(500);
      UserButtonPressed = RESET;

      return;
    }
    BSP_LED_Toggle(led);
    HAL_Delay(100);
  }
}

/**
* @brief  Display OSPI NOR Demo Hint
* @param  None
* @retval None
*/
static void OSPI_NOR_SetHint(void)
{
  /* Clear the LCD */ 
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  
  /* Set LCD Demo description */
  UTIL_LCD_FillRect(0, 0, 240, 60, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE); 
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t*)"OSPI NOR", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t*)"Press User push-button", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t*)"to switch to next menu", CENTER_MODE); 
  
  UTIL_LCD_DrawRect(10, 70, 220, 160, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 71, 218, 158, UTIL_LCD_COLOR_BLUE);
  
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE); 
}

/**
* @brief  Fills buffer with user predefined data.
* @param  pBuffer: pointer on the buffer to fill
* @param  uwBufferLength: size of the buffer to fill
* @param  uwOffset: first value to fill on the buffer
* @retval None
*/
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;
  
  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLength; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
* @retval 1: pBuffer identical to pBuffer1
*         0: pBuffer differs from pBuffer1
*/
static uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }
    
    pBuffer1++;
    pBuffer2++;
  }
  
  return 0;
}

/**
* @brief  Compares a buffer with data.
* @param  pBuffer, pBuffer2: buffers to be compared.
* @param  Pattern: data reference pattern.
* @param  BufferLength: buffer's length
* @retval 1: pBuffer identical to pBuffer1
*         0: pBuffer differs from pBuffer1
*/
static uint8_t DataCmp(uint8_t* pBuffer, uint8_t Pattern, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer != Pattern)
    {
      return 1;
    }
    
    pBuffer++;
  }
  
  return 0;
}
/**
* @}
*/ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
