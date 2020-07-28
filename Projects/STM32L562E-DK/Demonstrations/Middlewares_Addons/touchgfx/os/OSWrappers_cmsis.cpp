#include <touchgfx/hal/OSWrappers.hpp>
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include <touchgfx/hal/GPIO.hpp>
#include <touchgfx/hal/HAL.hpp>

using namespace touchgfx;

osSemaphoreId_t FrameBufferSem_Handle;
const osSemaphoreAttr_t FrameBufferSem_attributes = {
  .name = "FrameBufferSem"
};

osMessageQueueId_t VSyncQueue_Handle = 0;
const osMessageQueueAttr_t VSyncQueue_attributes = {
  .name = "VSyncQueue"
};

// Just a dummy value to insert in the VSYNC queue.
static uint8_t dummy = 0x5a;

void OSWrappers::initialize()
{
  FrameBufferSem_Handle = osSemaphoreNew(1, 1, &FrameBufferSem_attributes);
  configASSERT(FrameBufferSem_Handle);
  // Create a queue of length 1
  VSyncQueue_Handle = osMessageQueueNew(1, 1, &VSyncQueue_attributes);
  configASSERT(VSyncQueue_Handle);
}

void OSWrappers::takeFrameBufferSemaphore()
{
  osStatus_t  osStatus;

  osStatus = osSemaphoreAcquire(FrameBufferSem_Handle, osWaitForever);
  configASSERT(osStatus == osOK);
}

void OSWrappers::giveFrameBufferSemaphore()
{
  osStatus_t  osStatus;

  osStatus = osSemaphoreRelease(FrameBufferSem_Handle);
  configASSERT(osStatus == osOK);
}

void OSWrappers::tryTakeFrameBufferSemaphore()
{
  osStatus_t  osStatus;

  osStatus = osSemaphoreAcquire(FrameBufferSem_Handle, 0);
  configASSERT(osStatus == osOK);
}

void OSWrappers::giveFrameBufferSemaphoreFromISR()
{
  osStatus_t  osStatus;

    // Since this is called from an interrupt, FreeRTOS requires special handling to trigger a
    // re-scheduling. May be applicable for other OSes as well.
  osStatus = osSemaphoreRelease(FrameBufferSem_Handle);
  configASSERT(osStatus == osOK);
}

void OSWrappers::signalVSync()
{
  if (VSyncQueue_Handle)
  {
    osMessageQueuePut(VSyncQueue_Handle, &dummy, osPriorityNormal, 0);
  }
}

void OSWrappers::waitForVSync()
{
  osStatus_t  osStatus;

  // First make sure the queue is empty, by trying to remove an element with 0 timeout.
  osStatus = osMessageQueueGet(VSyncQueue_Handle, &dummy, NULL, 0);
  configASSERT((osStatus == osOK) || (osStatus == osErrorResource));

    // Then, wait for next VSYNC to occur.
  osStatus = osMessageQueueGet(VSyncQueue_Handle, &dummy, NULL, osWaitForever);
  configASSERT(osStatus == osOK);
}

void OSWrappers::taskDelay(uint16_t ms)
{
  osStatus_t  osStatus;
  osStatus = osDelay(ms);
  configASSERT(osStatus == osOK);
}

// FreeRTOS specific handlers
extern "C"
{
    void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                       signed portCHAR* pcTaskName)
    {
        while (1);
    }

    void vApplicationMallocFailedHook(TaskHandle_t xTask,
                                      signed portCHAR* pcTaskName)
    {
        while (1);
    }

    void vApplicationIdleHook(void)
    {
        // Set task tag in order to have the "IdleTaskHook" function called when the idle task is
        // switched in/out. Used solely for measuring MCU load, and can be removed if MCU load
        // readout is not needed.
        // vTaskSetApplicationTaskTag(NULL, IdleTaskHook);
    }

    void vApplicationTickHook( void )
    {
      extern void AppSysTick_Handler(void);

      AppSysTick_Handler();
    }
}
