# Helper macros to convert spaces into question marks and back again
e :=
sp := $(e) $(e)
qs = $(subst ?,$(sp),$1)
sq = $(subst $(sp),?,$1)

# Get name of this Makefile (avoid getting word 0 and a starting space)
makefile_name := $(wordlist 1,1000,$(MAKEFILE_LIST))

# Get path of this Makefile
makefile_path := $(call qs,$(dir $(call sq,$(abspath $(call sq,$(makefile_name))))))

# Get path where the Application is
application_path := $(call qs,$(abspath $(call sq,$(makefile_path)..)))

# Change makefile_name to a relative path
makefile_name := $(subst $(call sq,$(application_path))/,,$(call sq,$(abspath $(call sq,$(makefile_name)))))

# Get relative path to makefile from application_path
makefile_path_relative := $(subst $(call sq,$(application_path))/,,$(call sq,$(abspath $(call sq,$(makefile_path)))))

# Get path to Middlewares
Middlewares_path := ../../Middlewares

# Get path to Drivers
Drivers_path := Drivers

# Get OS path
os_path := $(Middlewares_path)/Third_Party/FreeRTOS

# Get identification of this system
ifeq ($(OS),Windows_NT)
UNAME := MINGW32_NT-6.2
else
UNAME := $(shell uname -s)
endif

board_name := ST/STM32L562E_DISCO
platform := cortex_m33

.PHONY: all clean assets flash intflash

all: $(filter clean,$(MAKECMDGOALS))
all clean assets:
	@cd "$(application_path)" && $(MAKE) -r -f $(makefile_name) -s $(MFLAGS) _$@_

flash intflash: all
	@cd "$(application_path)" && $(MAKE) -r -f $(makefile_name) -s $(MFLAGS) _$@_

# Directories containing application-specific source and header files.
# Additional components can be added to this list. make will look for
# source files recursively in comp_name/src and setup an include directive
# for comp_name/include.
components := TouchGFX/gui target TouchGFX/generated/gui_generated

# Location of folder containing bmp/png files.
asset_images_input  := TouchGFX/assets/images

# Location of folder to search for ttf font files
asset_fonts_input  := TouchGFX/assets/fonts

# Location of folder where the texts.xlsx is placed
asset_texts_input  := TouchGFX/assets/texts

build_root_path := TouchGFX/build
object_output_path := $(build_root_path)/$(board_name)
binary_output_path := $(build_root_path)/bin

# Location of output folders where autogenerated code from assets is placed
asset_root_path := TouchGFX/generated
asset_images_output := $(asset_root_path)/images
asset_fonts_output := $(asset_root_path)/fonts
asset_texts_output := $(asset_root_path)/texts

#include application specific configuration
include $(application_path)/TouchGFX/config/gcc/app.mk

# corrects TouchGFX Path
touchgfx_path := ${subst ../,,$(touchgfx_path)}
#touchgfx_path := $(subst $(call sq,$(makefile_path))/,,$(call sq,$(abspath $(call sq,$(touchgfx_path)))))


os_source_files := $(os_path)/Source/croutine.c \
                   $(os_path)/Source/list.c \
                   $(os_path)/Source/queue.c \
                   $(os_path)/Source/tasks.c \
                   $(os_path)/Source/timers.c \
                   $(os_path)/Source/portable/MemMang/heap_4.c \
                   $(os_path)/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
                   $(os_path)/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c \
                   $(os_path)/Source/CMSIS_RTOS/cmsis_os.c

os_include_paths := $(os_path)/Source/include \
                    $(os_path)/Source/portable/GCC/ARM_CM33_NTZ/non_secure \
                    $(os_path)/Source/CMSIS_RTOS

os_wrapper := $(touchgfx_path)/os/OSWrappers.cpp

### END OF USER SECTION. THE FOLLOWING SHOULD NOT BE MODIFIED ###

ifeq ($(UNAME), Linux)
imageconvert_executable := $(touchgfx_path)/framework/tools/imageconvert/build/linux/imageconvert.out
fontconvert_executable := $(touchgfx_path)/framework/tools/fontconvert/build/linux/fontconvert.out
else
imageconvert_executable := $(touchgfx_path)/framework/tools/imageconvert/build/win/imageconvert.out
fontconvert_executable := $(touchgfx_path)/framework/tools/fontconvert/build/win/fontconvert.out
stm32cubeProgrammer := STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\bin\\STM32_Programmer_CLI.exe
stm32cubeLoader := STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\bin\\ExternalLoader\\MX25LM51245G_STM32L552E-EVAL.stldr

ifneq ("$(shell find "$(PROGRAMFILES)\\$(stm32cubeProgrammer)" 2> nul )","")
# STM32CubeProgrammer is placed in Program Files (x86)
st_stm32cube_programmer := "$(PROGRAMFILES)\\$(stm32cubeProgrammer)"
st_stm32cube_external_loader := "$(PROGRAMFILES)\\$(stm32cubeLoader)"
else
# STM32CubeProgrammer is placed in Program Files
st_stm32cube_programmer := "$(PROGRAMW6432)\\$(stm32cubeProgrammer)"
st_stm32cube_external_loader := "$(PROGRAMW6432)\\$(stm32cubeLoader)"
endif
endif

target_executable := target.elf
target_hex := target.hex

########### Compiler options #################
# Defines the assembler binary and options. These are optional and only
# of relevance if the component includes source files with an
# extension of .asm.
assembler         := arm-none-eabi-gcc
assembler_options += \
                    -nostartfiles -fno-exceptions\
                    $(no_libs) -mthumb -mno-thumb-interwork  \
                     -Wall
assembler_options += $(float_options)

c_compiler         := arm-none-eabi-gcc
c_compiler_options += \
                    -nostartfiles -mthumb -fno-exceptions \
                    -mno-thumb-interwork -std=c99 \
                    $(no_libs) \
                    -Os -fno-strict-aliasing -fdata-sections -ffunction-sections

c_compiler_options += $(float_options)

cpp_compiler         := arm-none-eabi-g++
cpp_compiler_options += -mthumb \
                    -nostartfiles $(no_libs) \
                    -mno-thumb-interwork -fno-rtti -fno-exceptions  \
                    -Os -fno-strict-aliasing -fdata-sections -ffunction-sections

cpp_compiler_options += $(float_options)

c_compiler_debug_options += -g \
                    -nostartfiles -mthumb -fno-exceptions \
                    -mno-thumb-interwork -std=c99 \
                    $(no_libs) \
                    -O0 -fno-strict-aliasing -fdata-sections -ffunction-sections

c_compiler_debug_options += $(float_options)

linker         := arm-none-eabi-g++

linker_options += -mthumb -specs=nosys.specs -specs=nano.specs -Wl,-Map=output.map -Wl,--gc-sections

objcopy := arm-none-eabi-objcopy

archiver := arm-none-eabi-ar

strip := arm-none-eabi-strip

####################### Additional toolchain configuration for Cortex-M4f targets.##########################
float_options := -mfpu=fpv5-sp-d16
ifneq ("$(float_abi)","hard")
float_options += -mfloat-abi=softfp
else
float_options += -mfloat-abi=hard
endif

#assembler_options += -mtune=cortex-m33 -march=armv8-m.main -Wno-psabi $(float_options) -DCORE_M33 -D__irq=""
assembler_options += -mcpu=cortex-m33 -Wno-psabi $(float_options) -DCORE_M33 -D__irq=""
c_compiler_options += -mcpu=cortex-m33 -Wno-psabi $(float_options) -DSTM32L562xx -DCORE_M33 -D__irq=""
cpp_compiler_options += -mcpu=cortex-m33 -Wno-psabi $(float_options) -DSTM32L562xx -DCORE_M33 -D__irq=""
c_compiler_debug_options += -mcpu=cortex-m33 -Wno-psabi $(float_options) -DSTM32L562xx -DCORE_M33 -D__irq=""
linker_options += -mcpu=cortex-m33 -Wno-psabi $(float_options)

############################################################################################################

#include everything + specific vendor folders
framework_includes := $(touchgfx_path)/framework/include

#this needs to change when assset include folder changes.
all_components := $(components) \
	$(asset_fonts_output) \
	$(asset_images_output) \
	$(asset_texts_output)

#keep framework include and source out of this mess! :)
include_paths := $(library_includes) $(foreach comp, $(all_components), $(comp)/include) $(framework_includes) $(source_Middlewares_paths)
source_paths = $(foreach comp, $(all_components), $(comp)/src)

# Finds files that matches the specified pattern. The directory list
# is searched recursively. It is safe to invoke this function with an
# empty list of directories.
#
# Param $(1): List of directories to search
# Param $(2): The file pattern to search for
define find
	$(foreach dir,$(1),$(foreach d,$(wildcard $(dir)/*),\
		$(call find,$(d),$(2))) $(wildcard $(dir)/$(strip $(2))))
endef
unexport find

fontconvert_ttf_lower_files := $(call find, $(asset_fonts_input), *.ttf)
fontconvert_ttf_upper_files := $(call find, $(asset_fonts_input), *.TTF)
fontconvert_otf_lower_files := $(call find, $(asset_fonts_input), *.otf)
fontconvert_otf_upper_files := $(call find, $(asset_fonts_input), *.OTF)
fontconvert_bdf_lower_files := $(call find, $(asset_fonts_input), *.bdf)
fontconvert_bdf_upper_files := $(call find, $(asset_fonts_input), *.BDF)
fontconvert_font_files := $(fontconvert_ttf_lower_files) \
			  $(fontconvert_ttf_upper_files) \
			  $(fontconvert_otf_lower_files) \
			  $(fontconvert_otf_upper_files) \
			  $(fontconvert_bdf_lower_files) \
			  $(fontconvert_bdf_upper_files)

source_files := $(call find, $(source_paths),*.cpp)

gcc_source_files := $(touchgfx_path)/framework/config/gcc/stdio.c \
                    $(touchgfx_path)/framework/config/gcc/stdlib.c \
                    $(touchgfx_path)/framework/config/gcc/string.c

board_c_files := \
	Core/Src/stm32l5xx_hal_timebase_tim.c \
	Core/Src/stm32l5xx_it.c \
	Core/Src/system_stm32l5xx.c \
	Core/Src/stm32l5xx_hal_msp.c \
	$(Middlewares_path)/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
	$(Middlewares_path)/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
	$(Middlewares_path)/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
	$(Middlewares_path)/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
	$(Middlewares_path)/Third_Party/FatFs/src/diskio.c \
	$(Middlewares_path)/Third_Party/FatFs/src/ff.c \
	$(Middlewares_path)/Third_Party/FatFs/src/ff_gen_drv.c \
	$(Middlewares_path)/Third_Party/FatFs/src/option/syscall.c \
	$(Drivers_path)/BSP/Components/ft6x06/ft6x06.c \
	$(Drivers_path)/BSP/Components/ft6x06/ft6x06_reg.c \
	$(Drivers_path)/BSP/Components/hx8347i/hx8347i.c \
	$(Drivers_path)/BSP/Components/hx8347i/hx8347i_reg.c \
	$(Drivers_path)/BSP/Components/cs42l51/cs42l51.c \
	$(Drivers_path)/BSP/Components/cs42l51/cs42l51_reg.c \
	$(Drivers_path)/BSP/Components/st7789h2/st7789h2.c \
  $(Drivers_path)/BSP/Components/st7789h2/st7789h2_reg.c \
  $(Drivers_path)/BSP/Components/mx25lm51245g/mx25lm51245g.c \
	$(Drivers_path)/BSP/Components/mfxstm32l152/mfxstm32l152.c \
  $(Drivers_path)/BSP/Components/mfxstm32l152/mfxstm32l152_reg.c \
	$(Drivers_path)/BSP/STM32L562E-Discovery/stm32l562e_discovery.c \
	$(Drivers_path)/BSP/STM32L562E-Discovery/stm32l562e_discovery_ospi.c \
	$(Drivers_path)/BSP/STM32L562E-Discovery/stm32l562e_discovery_lcd.c \
	$(Drivers_path)/BSP/STM32L562E-Discovery/stm32l562e_discovery_ts.c \
	$(Drivers_path)/BSP/STM32L562E-Discovery/stm32l562e_discovery_bus.c \
	$(Drivers_path)/BSP/STM32L562E-Discovery/stm32l562e_discovery_idd.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_adc.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_adc_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_cortex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_crc.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_crc_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_cryp.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_cryp_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dac.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dac_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dfsdm.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dma.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dma_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_exti.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_flash.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_flash_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_flash_ramfunc.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_gpio.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_hash.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_hash_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_i2c.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_i2c_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_irda.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_iwdg.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_lptim.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_nand.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_nor.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_opamp.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_opamp_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_pcd.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_pcd_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_pwr.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_pwr_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_rcc.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_rcc_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_rng.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_rtc.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_rtc_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_sai.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_sai_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_sd.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_smartcard.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_spi.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_sram.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_tim.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_tim_ex.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_uart.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_usart.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_wwdg.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_fmc.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_pwr.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_usb.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_sdmmc.c \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_lpuart.c

board_c_no_opt_files := \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_ospi.c

board_cpp_files := \
	Core/Src/main.cpp \
	Core/Src/fatfs.c \
	Core/Src/bsp_driver_sd.c \
	Core/Src/sd_diskio.c \
  Core/Src/usbd_conf.c \
  Core/Src/usbd_desc.c \
  Core/Src/usbd_cdc_if.c \
  Core/Src/usb_device.c \
	TouchGFX/target/BoardConfiguration.cpp \
	TouchGFX/target/STM32L5Instrumentation.cpp \
	TouchGFX/target/GPIO.cpp \
	TouchGFX/target/STM32L5TouchController.cpp \
	TouchGFX/target/STM32L5HAL_DPI.cpp

board_include_paths := \
	TouchGFX/gui/include \
	TouchGFX/target/include \
	TouchGFX/platform/os \
	TouchGFX/generated/fonts/include \
	TouchGFX/generated/images/include \
	TouchGFX/generated/texts/include \
	TouchGFX/generated/gui_generated/include \
	$(Middlewares_path)/ST/TouchGFX/touchgfx/framework/include \
	$(Middlewares_path)/Third_Party/FreeRTOS/Source/include \
	$(Middlewares_path)/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
	$(Middlewares_path)/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure \
	$(Middlewares_path)/Third_Party/FatFs/src \
	$(Middlewares_path)/ST/STM32_USB_Device_Library/Core/Inc \
	$(Middlewares_path)/ST/STM32_USB_Device_Library/Class/CDC/Inc \
	$(Drivers_path)/STM32L5xx_HAL_Driver/Inc \
	$(Drivers_path)/BSP/STM32L562E-Discovery \
	$(Drivers_path)/BSP/Components/hx8347i \
	$(Drivers_path)/BSP/Components/cs42l51 \
	$(Drivers_path)/BSP/Components/st7789h2 \
	$(Drivers_path)/BSP/Components/wm8994 \
	$(Drivers_path)/BSP/Components/mfxstm32l152 \
	$(Drivers_path)/CMSIS/Include \
	$(Drivers_path)/CMSIS/Device/ST/STM32l5xx/Include \
	TouchGFX/target \
	Core/Inc

asm_source_files := \
  $(Drivers_path)/CMSIS/Device/ST/STM32L5xx/Source/Templates/gcc/startup_stm32l562xx.s

# Compile time defines
assembler_options += -DUSE_OS_SYSTICK
c_compiler_options += -DST -DSTM32L562xx -DUSE_OS_SYSTICK -DUSE_STM32L562E_DISCO -DCORE_CM33
cpp_compiler_options +=  -DST -DSTM32L562xx -DUSE_OS_SYSTICK -DUSE_STM32L562E_DISCO -DCORE_CM33
c_compiler_debug_options +=  -DST -DSTM32L562xx -DUSE_OS_SYSTICK -DUSE_STM32L562E_DISCO -DCORE_CM33

include_paths += platform/os $(board_include_paths) $(os_include_paths)

c_source_files := $(call find, $(source_paths),*.c) $(os_source_files) $(board_c_files)
source_files += $(os_wrapper) \
                $(board_cpp_files)

object_files := $(source_files:$(touchgfx_path)/%.cpp=$(object_output_path)/touchgfx/%.o) $(c_source_files:$(touchgfx_path)/%.c=$(object_output_path)/touchgfx/%.o)
object_files := $(object_files:%.cpp=$(object_output_path)/%.o)
object_files := $(object_files:%.c=$(object_output_path)/%.o)
dependency_files := $(object_files:%.o=%.d)

object_asm_files := $(asm_source_files:%.s=$(object_output_path)/%.o)
object_asm_files := $(patsubst $(object_output_path)/%,$(object_output_path)/%,$(object_asm_files))

object_no_opt_files := $(board_c_no_opt_files:%.c=$(object_output_path)/no_opt_files/%.o)
object_no_opt_files := $(patsubst $(object_output_path)/%,$(object_output_path)/%,$(object_no_opt_files))

textconvert_script_path := $(touchgfx_path)/framework/tools/textconvert
textconvert_executable := $(call find, $(textconvert_script_path), *.rb)

text_database := $(asset_texts_input)/texts.xlsx

libraries := touchgfx
library_include_paths := $(touchgfx_path)/lib/core/$(platform)/gcc

.PHONY: _all_ _clean_ _assets_ _flash_ generate_assets build_executable

# Force linking each time
.PHONY: $(binary_output_path)/$(target_executable)

_all_: generate_assets

_flash_:
ifeq ($(shell find "$(application_path)" -wholename "$(application_path)/$(binary_output_path)/extflash.bin" -size +0c | wc -l | xargs echo),1)
	@$(st_stm32cube_programmer) -c port=SWD index=0 reset=HWrst -d $(binary_output_path)/target.hex -el $(st_stm32cube_external_loader) -hardRst
else
	@$(st_stm32cube_programmer) -c port=SWD index=0 reset=HWrst -d $(binary_output_path)/intflash.hex -el $(st_stm32cube_external_loader) -hardRst
endif

generate_assets: _assets_
	@$(MAKE) -f $(makefile_name) -r -s $(MFLAGS) build_executable
build_executable: $(binary_output_path)/$(target_executable)

$(binary_output_path)/$(target_executable): $(object_files) $(object_no_opt_files) $(object_asm_files)
	@echo Linking $(@)
	@mkdir -p $(@D)
	@mkdir -p $(object_output_path)
	@$(file >$(build_root_path)/objects.tmp) $(foreach F,$(object_files),$(file >>$(build_root_path)/objects.tmp,$F))
	@$(linker) \
		$(linker_options) -T $(makefile_path_relative)/STM32L562QEIx_FLASH.ld -Wl,-Map=$(@D)/application.map $(linker_options_local) \
		$(patsubst %,-L%,$(library_include_paths)) \
		@$(build_root_path)/objects.tmp $(object_no_opt_files) $(object_asm_files) -o $@ \
		-Wl,--start-group $(patsubst %,-l%,$(libraries)) -Wl,--end-group
	@rm -f $(build_root_path)/objects.tmp
	@echo "Producing additional output formats..."
	@echo "  target.hex   - Combined internal+external hex"
	@$(objcopy) -O ihex $@ $(@D)/target.hex
	@echo "  intflash.elf - Internal flash, elf debug"
	@$(objcopy) --remove-section=ExtFlashSection $@ $(@D)/intflash.elf 2>/dev/null
	@echo "  intflash.hex - Internal flash, hex"
	@$(objcopy) -O ihex --remove-section=ExtFlashSection $@ $(@D)/intflash.hex
	@echo "  extflash.bin - External flash, binary"
	@$(objcopy) -O binary --only-section=ExtFlashSection $@ $(@D)/extflash.bin

$(object_output_path)/no_opt_files/%.o : %.c TouchGFX/config/gcc/app.mk
	@echo [No OPT] Compiling $<
	@mkdir -p $(@D)
	@$(c_compiler) \
		-MMD -MP $(c_compiler_debug_options) $(c_compiler_options_local) $(user_cflags) \
		$(patsubst %,-I%,$(include_paths)) \
		-c $< -o $@

$(object_output_path)/touchgfx/%.o: $(touchgfx_path)/%.cpp TouchGFX/config/gcc/app.mk
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(cpp_compiler) \
		-MMD -MP $(cpp_compiler_options) $(cpp_compiler_options_local) $(user_cflags) \
		$(patsubst %,-I%,$(include_paths)) \
		-c $< -o $@

$(object_output_path)/%.o: %.cpp TouchGFX/config/gcc/app.mk
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(cpp_compiler) \
		-MMD -MP $(cpp_compiler_options) $(cpp_compiler_options_local) $(user_cflags) \
		$(patsubst %,-I%,$(include_paths)) \
		-c $< -o $@

$(object_output_path)/%.o: %.c TouchGFX/config/gcc/app.mk
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(c_compiler) \
		-MMD -MP $(c_compiler_options) $(c_compiler_options_local) $(user_cflags) \
		$(patsubst %,-I%,$(include_paths)) \
		-c $< -o $@

$(object_output_path)/%.o: %.s TouchGFX/config/gcc/app.mk
	@echo Compiling ASM $<
	@mkdir -p $(@D)
	@$(assembler) \
		$(assembler_options) \
		$(patsubst %,-I %,$(os_include_paths)) \
		-c $< -o $@

ifeq ($(MAKECMDGOALS),build_executable)
$(firstword $(dependency_files)): TouchGFX/config/gcc/app.mk
	@rm -rf $(object_output_path)
-include $(dependency_files)
endif

_assets_: BitmapDatabase TextKeysAndLanguages

.PHONY: BitmapDatabase TextKeysAndLanguages

BitmapDatabase:
	@$(imageconvert_executable) -r $(asset_images_input) -w $(asset_images_output)

TextKeysAndLanguages: $(text_database) TouchGFX/config/gcc/app.mk $(textconvert_executable) $(fontconvert_executable) $(fontconvert_font_files)
	@mkdir -p $(asset_texts_output)/include/texts
	@ruby $(textconvert_script_path)/main.rb $(text_database) $(fontconvert_executable) $(asset_fonts_output) $(asset_texts_output) $(asset_fonts_input) . $(text_converter_options)

_clean_:
	@echo Cleaning
	@rm -rf $(build_root_path)
	# Do not remove gui_generated
	@rm -rf $(asset_images_output)
	@rm -rf $(asset_fonts_output)
	@rm -rf $(asset_texts_output)
	# Create directory to avoid error if it does not exist
	@mkdir -p $(asset_root_path)
	# Remove assets folder if it is empty (i.e. no gui_generated folder)
	@rmdir --ignore-fail-on-non-empty $(asset_root_path)
