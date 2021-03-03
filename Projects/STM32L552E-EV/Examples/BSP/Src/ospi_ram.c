/**
******************************************************************************
* @file    ospi_ram.c
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
#define BUFFER_SIZE         ((uint32_t)0x0200)
#define WRITE_READ_ADDR     ((uint32_t)0x0050)
#define OSPI_RAM_BASE_ADDR  ((uint32_t)0x90000000)

/* Private macro -------------------------------------------------------------*/
#define LCD_TEXT_XPOS       20
#define LCD_TEXT_YPOS_START 80
#define LCD_TEXT_YPOS_INC   15

/* Private variables ---------------------------------------------------------*/
uint8_t ospi_ram_aTxBuffer[BUFFER_SIZE];
uint8_t ospi_ram_aRxBuffer[BUFFER_SIZE];
uint8_t ubOspiRamDmaCplt = 0;

/* Private function prototypes -----------------------------------------------*/
static void     OSPI_RAM_SetHint(void);
static void     Fill_Buffer (uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t  Buffercmp   (uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  OSPI RAM Demo
* @param  None
* @retval None
*/
void OSPI_RAM_demo(void)
{ 
  int32_t status;
  BSP_OSPI_RAM_Init_t sOSPI_RAM_Init;
  __IO uint8_t *data_ptr;
  uint32_t index;
  Led_TypeDef led = LED_ORANGE;
  uint16_t lcd_text_ypos = LCD_TEXT_YPOS_START;
  
  led = LED_GREEN;
  OSPI_RAM_SetHint();
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Put JP7 in position OSPI_SRAM_NCS", LEFT_MODE);
  while (1)
  {    
    if(WakeupButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      WakeupButtonPressed = RESET;

      break;
    }
    BSP_LED_Toggle(led);
    HAL_Delay(100);
  }
  BSP_LED_Off(led);
  
  /********************************** PART 1 **********************************/
  led = LED_ORANGE;
  lcd_text_ypos = LCD_TEXT_YPOS_START;
  OSPI_RAM_SetHint();
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Part I : Read/Write in polling/DMA modes", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;

  /*##-1- Configure the OSPI RAM device ######################################*/
  /* OSPI RAM device configuration */ 
  sOSPI_RAM_Init.LatencyType = BSP_OSPI_RAM_FIXED_LATENCY;
  sOSPI_RAM_Init.BurstType   = BSP_OSPI_RAM_LINEAR_BURST;
  sOSPI_RAM_Init.BurstLength = BSP_OSPI_RAM_BURST_32_BYTES;
  status = BSP_OSPI_RAM_Init(0, &sOSPI_RAM_Init);
  
  if (status != BSP_ERROR_NONE)
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Initialization : Failed", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
  }
  else
  {
//    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Initialization : OK", LEFT_MODE);
//    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    
    /*##-2- OSPI RAM memory read/write access  ###############################*/   
    /* Fill the buffer to write */
    Fill_Buffer(ospi_ram_aTxBuffer, BUFFER_SIZE, 0xD20F);   
    
    /* Write data to the OSPI RAM memory */
    if (BSP_OSPI_RAM_Write(0, ospi_ram_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Write : Failed", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Write : OK", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      
      /* Read back data from the OSPI RAM memory */
      if (BSP_OSPI_RAM_Read(0, ospi_ram_aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Read : Failed", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Read : OK", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        
        /*##-3- Checking data integrity ######################################*/  
        if(Buffercmp(ospi_ram_aRxBuffer, ospi_ram_aTxBuffer, BUFFER_SIZE) > 0)
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Compare : Failed", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
        }
        else
        {    
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Compare : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          
          /*##-4- OSPI RAM memory read/write access in DMA mode ##############*/  
          /* Fill the buffer to write */
          Fill_Buffer(ospi_ram_aTxBuffer, BUFFER_SIZE, 0xA1F0);   
          ubOspiRamDmaCplt =0;
          
          /* Write data to the OSPI RAM memory */
          if (BSP_OSPI_RAM_Write_DMA(0, ospi_ram_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM DMA Write : Failed", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
          }   
          else
          {
            /* Wait end of DMA transfer */
            while (ubOspiRamDmaCplt == 0)
            {
              if (WakeupButtonPressed == SET)
              {
                /* Add delay to avoid rebound and reset it status */
                HAL_Delay(500);
                WakeupButtonPressed = RESET;
                
                break;
              }
              BSP_LED_Toggle(LED_GREEN);
              HAL_Delay(100);
            }
            
            if (ubOspiRamDmaCplt != 0)
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM DMA Write : OK", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              ubOspiRamDmaCplt =0;
              
              /* Read back data from the OSPI RAM memory */
              if (BSP_OSPI_RAM_Read_DMA(0, ospi_ram_aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != BSP_ERROR_NONE)
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM DMA Read : Failed", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
              }
              else
              {
                /* Wait end of DMA transfer */
                while (ubOspiRamDmaCplt == 0)
                {
                  if (WakeupButtonPressed == SET)
                  {
                    /* Add delay to avoid rebound and reset it status */
                    HAL_Delay(500);
                    WakeupButtonPressed = RESET;
                    
                    break;
                  }
                  BSP_LED_Toggle(LED_GREEN);
                  HAL_Delay(100);
                }
                
                if (ubOspiRamDmaCplt != 0)
                {
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM DMA read : OK", LEFT_MODE);
                  lcd_text_ypos+=LCD_TEXT_YPOS_INC;

                  /*##-5- Checking data integrity ############################*/
                  if (Buffercmp(ospi_ram_aRxBuffer, ospi_ram_aTxBuffer, BUFFER_SIZE) > 0)
                  {
                    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Compare : Failed", LEFT_MODE);
                    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
                  }
                  else
                  {
                    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Compare : OK", LEFT_MODE);
                    lcd_text_ypos+=LCD_TEXT_YPOS_INC;

                    /*##-6- OSPI RAM memory in memory-mapped mode ############*/
                    /* Fill the buffer to write */
                    Fill_Buffer(ospi_ram_aTxBuffer, BUFFER_SIZE, 0xD20F);   

                    if (BSP_OSPI_RAM_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
                    {
                      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Cfg : Failed", LEFT_MODE);
                      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
                    }
                    else
                    {
                      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Cfg : OK", LEFT_MODE);
                      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                      
                      /* Memory-mapped mode write access */
                      for (index = 0, data_ptr = (__IO uint8_t *)(OSPI_RAM_BASE_ADDR + WRITE_READ_ADDR); 
                           index < BUFFER_SIZE; index++, data_ptr++)
                      {
                        *data_ptr = ospi_ram_aTxBuffer[index];
                      }

                      /* Memory-mapped mode read access */
                      for (index = 0, data_ptr = (__IO uint8_t *)(OSPI_RAM_BASE_ADDR + WRITE_READ_ADDR); 
                           index < BUFFER_SIZE; index++, data_ptr++)
                      {
                        if (*data_ptr != ospi_ram_aTxBuffer[index])
                        {
                          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Access : Failed", LEFT_MODE);
                          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
                          break;
                        }
                      }
                      
                      if (index == BUFFER_SIZE)
                      {
                        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Access : OK", LEFT_MODE);
                        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test : OK", LEFT_MODE);
                        led = LED_GREEN;
                      }
                    }
                  }
                }
                else
                {
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM DMA Read : Failed", LEFT_MODE);
                  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
                }
              }
            }
            else
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM DMA Write : Failed", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
            }
          }
        }  
      }
    }
  }
  
  while (1)
  {    
    if (WakeupButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      WakeupButtonPressed = RESET;

      break;
    }
    BSP_LED_Toggle(led);
    HAL_Delay(100);
  }
  BSP_LED_Off(led);
  
  /********************************** PART 2 **********************************/
  led = LED_ORANGE;
  lcd_text_ypos = LCD_TEXT_YPOS_START;
  OSPI_RAM_SetHint();
  
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"Part II : Enter & Leave HyperRAM Power", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"          Down mode", LEFT_MODE);
  lcd_text_ypos+=LCD_TEXT_YPOS_INC;

  /*##-1- Deconfigure the OSPI RAM device ####################################*/
  status = BSP_OSPI_RAM_DeInit(0);
  
  if (status != BSP_ERROR_NONE)
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM De-Initialization : Failed", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM De-Initialization : OK", LEFT_MODE);
    lcd_text_ypos+=LCD_TEXT_YPOS_INC;
    
    /*##-2- Reconfigure the OSPI RAM device ##################################*/
    /* OSPI RAM device configuration */ 
    status = BSP_OSPI_RAM_Init(0, &sOSPI_RAM_Init);
    
    if (status != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Initialization : Failed", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Initialization : OK", LEFT_MODE);
      lcd_text_ypos+=LCD_TEXT_YPOS_INC;
      
      /*##-4- OSPI RAM memory write access ###################################*/
      /* Fill the buffer to write */
      Fill_Buffer(ospi_ram_aTxBuffer, BUFFER_SIZE, 0xD20F);   
      
      /* Write data to the OSPI RAM memory */
      if (BSP_OSPI_RAM_Write(0, ospi_ram_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Write : Failed", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Write : OK", LEFT_MODE);
        lcd_text_ypos+=LCD_TEXT_YPOS_INC;
        
        /*##-5- Enter OSPI RAM memory in deep power down #####################*/ 
        if (BSP_OSPI_RAM_EnterDeepPowerDown(0) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Enter Deep PwrDn : Failed", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
        }
        else
        {
          /* !!! Warning : This is only a test of the API. To check if the memory is really in deep power down, 
          need to use Idd to check the consumption !!! */
          UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Enter Deep PwrDn : OK", LEFT_MODE);
          lcd_text_ypos+=LCD_TEXT_YPOS_INC;
          
          /*##-6- Leave OSPI RAM memory from deep power down #################*/ 
          HAL_Delay(100);
          if (BSP_OSPI_RAM_LeaveDeepPowerDown(0) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Leave Deep PwrDn : Failed", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
          }
          else
          {
            UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Leave Deep PwrDn : OK", LEFT_MODE);
            lcd_text_ypos+=LCD_TEXT_YPOS_INC;
            
            /*##-7- OSPI RAM memory in memory-mapped mode#####################*/
            if (BSP_OSPI_RAM_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Cfg : Failed", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
            }
            else
            {
              UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Cfg : OK", LEFT_MODE);
              lcd_text_ypos+=LCD_TEXT_YPOS_INC;
              
              for (index = 0, data_ptr = (__IO uint8_t *)(OSPI_RAM_BASE_ADDR + WRITE_READ_ADDR); 
                   index < BUFFER_SIZE; index++, data_ptr++)
              {
                if (*data_ptr != ospi_ram_aTxBuffer[index])
                {
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Access : Failed", LEFT_MODE);
                  lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                  UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test Aborted", LEFT_MODE);
                  break;
                }
              }
              
              if (index == BUFFER_SIZE)
              {
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Mem-Mapped Access : OK", LEFT_MODE);
                lcd_text_ypos+=LCD_TEXT_YPOS_INC;
                UTIL_LCD_DisplayStringAt(LCD_TEXT_XPOS, lcd_text_ypos, (uint8_t*)"OSPI HyperRAM Test : OK", LEFT_MODE);
                led = LED_GREEN;
              }
            }
          }  
        }
      }
    }
  }
  
  /* De-initialization in order to have correct configuration memory on next try */
  BSP_OSPI_RAM_DeInit(0);

  while (1)
  {    
    if(WakeupButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      BSP_LED_Off(led);
      HAL_Delay(500);
      WakeupButtonPressed = RESET;

      return;
    }
    BSP_LED_Toggle(led);
    HAL_Delay(100);
  }
}

/**
* @brief  Display OSPI HyperRAM Demo Hint
* @param  None
* @retval None
*/
static void OSPI_RAM_SetHint(void)
{
  /* Clear the LCD */ 
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  
  /* Set LCD Demo description */
  UTIL_LCD_FillRect(0, 0, 320, 60, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE); 
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t*)"OSPI HyperRAM", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t*)"Press Wakeup push-button", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t*)"to switch to next menu", CENTER_MODE); 

  UTIL_LCD_DrawRect(10, 70, 300, 160, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 71, 298, 158, UTIL_LCD_COLOR_BLUE);
  
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
  * @brief  Rx Transfer completed callback.
  * @param  hospi: OSPI handle
  * @retval None
  */
void HAL_OSPI_RxCpltCallback(OSPI_HandleTypeDef *hospi)
{
  ubOspiRamDmaCplt++;
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  hospi: OSPI handle
  * @retval None
  */
void HAL_OSPI_TxCpltCallback(OSPI_HandleTypeDef *hospi)
{
  ubOspiRamDmaCplt++; 
}
/**
* @}
*/ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
