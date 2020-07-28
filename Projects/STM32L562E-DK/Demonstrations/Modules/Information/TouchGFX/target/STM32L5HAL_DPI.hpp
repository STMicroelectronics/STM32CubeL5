#ifndef STM32L5HAL_DPI_HPP
#define STM32L5HAL_DPI_HPP

#include <touchgfx/hal/HAL.hpp>
#include <platform/driver/touch/TouchController.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/hal/GPIO.hpp>
#include <touchgfx/hal/Types.hpp>

/**
 * @class STM32L5HAL_DPI STM32L5HAL_DPI.hpp platform/hal/ST/mcu/stm32f4cube/STM32L5HAL_DPI.hpp
 *
 * @brief HAL implementation for STM32L5.
 *
 *        HAL implementation for STM32L5. Based on the ST CubeF4 API.
 *
 * @sa HAL
 */

class STM32L5HAL_DPI : public touchgfx::HAL
{
public:
    /**
     * @fn STM32L5HAL_DPI::STM32L5HAL_DPI(touchgfx::DMA_Interface& dma, touchgfx::LCD& display, touchgfx::TouchController& tc, uint16_t width, uint16_t height) : touchgfx::HAL(dma, display, tc, width, height)
     *
     * @brief Constructor.
     *
     *        Constructor. Initializes members.
     *
     * @param [in,out] dma     Reference to DMA interface.
     * @param [in,out] display Reference to LCD interface.
     * @param [in,out] tc      Reference to Touch Controller driver.
     * @param width            Width of the display.
     * @param height           Height of the display.
     */
    STM32L5HAL_DPI(touchgfx::DMA_Interface& dma, touchgfx::LCD& display, touchgfx::TouchController& tc, uint16_t width, uint16_t height) : touchgfx::HAL(dma, display, tc, width, height)
    {
    }

    /**
     * @fn void STM32L5HAL_DPI::initialize();
     *
     * @brief This function is responsible for initializing stuff for the HAL driver.
     *
     *        This function is responsible for initializing stuff for the HAL driver.
     */
    void initialize();

    /**
     * @fn virtual void STM32L5HAL_DPI::disableInterrupts();
     *
     * @brief Disables the DMA and LCD interrupts.
     *
     *        Disables the DMA and LCD interrupts.
     */
    virtual void disableInterrupts();

    /**
     * @fn virtual void STM32L5HAL_DPI::enableInterrupts();
     *
     * @brief Enables the DMA and LCD interrupts.
     *
     *        Enables the DMA and LCD interrupts.
     */
    virtual void enableInterrupts();

    /**
     * @fn virtual void STM32L5HAL_DPI::configureInterrupts();
     *
     * @brief Sets the DMA and LCD interrupt priorities.
     *
     *        Sets the DMA and LCD interrupt priorities.
     */
    virtual void configureInterrupts();

    /**
     * @fn virtual void STM32L5HAL_DPI::enableLCDControllerInterrupt();
     *
     * @brief Configure the LCD controller to fire interrupts at VSYNC.
     *
     *        Configure the LCD controller to fire interrupts at VSYNC. Called automatically
     *        once TouchGFX initialization has completed.
     */
    virtual void enableLCDControllerInterrupt();

    using HAL::flushFrameBuffer;
    virtual void flushFrameBuffer(const touchgfx::Rect& rect);
    virtual void flushFrameBuffer();

    using HAL::blockCopy;
    virtual bool blockCopy(void *RESTRICT 	dest,
                           const void *RESTRICT src,
                           uint32_t 	        numBytes 
                           );
protected:
    /**
     * @fn virtual uint16_t* STM32L5HAL_DPI::getTFTFrameBuffer() const;
     *
     * @brief Gets the frame buffer address used by the TFT controller.
     *
     *        Gets the frame buffer address used by the TFT controller.
     *
     * @return The address of the frame buffer currently being displayed on the TFT.
     */
    virtual uint16_t* getTFTFrameBuffer() const;

    /**
     * @fn virtual void STM32L5HAL_DPI::setTFTFrameBuffer(uint16_t* adr);
     *
     * @brief Sets the frame buffer address used by the TFT controller.
     *
     *        Sets the frame buffer address used by the TFT controller.
     *
     * @param [in,out] adr New frame buffer address.
     */
    virtual void setTFTFrameBuffer(uint16_t* adr);


    /**
     * This function is overridden to detect whether there are any frame buffer changes in this frame.
     * @return The value of the base implementation.
     */
    virtual bool beginFrame();

    /**
     * This function is overridden to detect whether there are any frame buffer changes in this frame.
     * @note Will also call the base implementation.
     */
    virtual void endFrame();

private:
    void copyFrameBufferBlockToLCD(const touchgfx::Rect rect);
};

#endif // STM32L5HAL_DPI_HPP
