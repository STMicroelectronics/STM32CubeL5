@SET DEMO_SUFF=_
@IF "%~1"=="OoB" @SET DEMO_SUFF=_OoB

:: ====================
:: Flash Memory Mapping
:: ====================
@SET INT_FLASH_ADDR_START=0x08000000
@SET INT_FLASH_ADDR_STOP=0x0807FFFF
@SET EXT_FLASH_ADDR_START=0x90000000
@SET EXT_FLASH_ADDR_STOP=0x90FFFFFF

:: ======
:: OOB
:: ======

:: Chip Name
@SET CHIP_NAME=STM32L562E
:: Main Board
@SET MAIN_BOARD=-DK
:: LCD Board
@SET LCD_BOARD=
:: OOB Demo Name
@SET DEMO_NAME=STM32Cube_Demo
:: OOB Demo Filename
@SET DEMO_FILE_NAME=%DEMO_NAME%-%CHIP_NAME%%MAIN_BOARD%
:: BUILD Date
@SET BUILD_DATE=%date:~-4%%date:~3,2%%date:~0,2%
:: BUILD ID
@SET /a BUILD_ID=0

@ECHO.
@ECHO Loading Demonstration environment variables for %CHIP_NAME%%MAIN_BOARD% board

:: Menu-Launcher
@SET ML_NAME=MenuLauncher
@SET ML_PATH=%~dp0
@SET ML_SUFF=
@SET ML_FILE_NAME=%ML_NAME%-%CHIP_NAME%%MAIN_BOARD%%ML_SUFF%.hex
@IF NOT EXIST %ML_PATH%\%ML_FILE_NAME% @SET ML_PATH=%~dp0..\%ML_NAME%\EWARM\%CHIP_NAME%%MAIN_BOARD%\Exe
@SET ML_FILE=%ML_PATH%\%ML_FILE_NAME%

:: Measurements Module
@SET PWR_NAME=Measurements
@SET PWR_SUFF=%DEMO_SUFF%
@SET PWR_FILE_NAME=%PWR_NAME%-%CHIP_NAME%%MAIN_BOARD%%PWR_SUFF%.hex
@SET PWR_SIM=
@SET PWR_PATH=%~dp0
@IF NOT EXIST %PWR_PATH%\%PWR_FILE_NAME% @SET PWR_PATH=%~dp0..\Modules\%PWR_NAME%\EWARM\%CHIP_NAME%%MAIN_BOARD%\Exe
@SET PWR_FILE=%PWR_PATH%\%PWR_FILE_NAME%
@SET PWR_FILE_FW=PWR_firmware
@SET PWR_FILE_RES=PWR_resources
@SET PWR_FW_ADDR_START=0x08018000
@SET PWR_FW_ADDR_STOP=0x08027FFF
@SET PWR_RES_ADDR_START=0x90300000
@SET PWR_RES_ADDR_STOP=0x903FFFFF

:: AI Module
@SET AI_NAME=AI
@SET AI_SUFF=%DEMO_SUFF%
@SET AI_FILE_NAME=%AI_NAME%-%CHIP_NAME%%MAIN_BOARD%%AI_SUFF%.hex
@SET AI_SIM=
@SET AI_PATH=%~dp0
@IF NOT EXIST %AI_PATH%\%AI_FILE_NAME% @SET AI_PATH=%~dp0..\Modules\%AI_NAME%\EWARM\%CHIP_NAME%%MAIN_BOARD%\Exe
@SET AI_FILE=%AI_PATH%\%AI_FILE_NAME%
@SET AI_FILE_FW=AI_firmware
@SET AI_FILE_RES=AI_resources
@SET AI_FW_ADDR_START=0x08028000
@SET AI_FW_ADDR_STOP=0x08037FFF
@SET AI_RES_ADDR_START=0x90400000
@SET AI_RES_ADDR_STOP=0x904FFFFF

:: BLE Module
@SET BLE_NAME=BLE
@SET BLE_SUFF=%DEMO_SUFF%
@SET BLE_FILE_NAME=%BLE_NAME%-%CHIP_NAME%%MAIN_BOARD%%BLE_SUFF%.hex
@SET BLE_SIM=
@SET BLE_PATH=%~dp0
@IF NOT EXIST %BLE_PATH%\%BLE_FILE_NAME% @SET BLE_PATH=%~dp0..\Modules\%BLE_NAME%\EWARM\%CHIP_NAME%%MAIN_BOARD%\Exe
@SET BLE_FILE=%BLE_PATH%\%BLE_FILE_NAME%
@SET BLE_FILE_FW=BLE_firmware
@SET BLE_FILE_RES=BLE_resources
@SET BLE_FW_ADDR_START=0x08038000
@SET BLE_FW_ADDR_STOP=0x0804FFFF
@SET BLE_RES_ADDR_START=0x90500000
@SET BLE_RES_ADDR_STOP=0x905FFFFF


:: Audio Player Module
@SET AUD_NAME=AudioPlayer
@SET AUD_SUFF=%DEMO_SUFF%
@SET AUD_FILE_NAME=%AUD_NAME%-%CHIP_NAME%%MAIN_BOARD%%AUD_SUFF%.hex
@SET AUD_SIM=
@SET AUD_PATH=%~dp0
@IF NOT EXIST %AUD_PATH%\%AUD_FILE_NAME% @SET AUD_PATH=%~dp0..\Modules\%AUD_NAME%\EWARM\%CHIP_NAME%%MAIN_BOARD%\Exe
@SET AUD_FILE=%AUD_PATH%\%AUD_FILE_NAME%
@SET AUD_FILE_FW=AUD_firmware
@SET AUD_FILE_RES=
@SET AUD_FW_ADDR_START=0x08050000
@SET AUD_FW_ADDR_STOP=0x0806FFFF
@SET AUD_RES_ADDR_START=0x90600000
@SET AUD_RES_ADDR_STOP=0x90EFFFFF

:: Information Module
@SET INFO_NAME=Information
@SET INFO_SUFF=%DEMO_SUFF%
@SET INFO_FILE_NAME=%INFO_NAME%-%CHIP_NAME%%MAIN_BOARD%%INFO_SUFF%.hex
@SET INFO_SIM=
@SET INFO_PATH=%~dp0
@IF NOT EXIST %INFO_PATH%\%INFO_FILE_NAME% @SET INFO_PATH=%~dp0..\Modules\%INFO_NAME%\EWARM\%CHIP_NAME%%MAIN_BOARD%\Exe
@SET INFO_FILE=%INFO_PATH%\%INFO_FILE_NAME%
@SET INFO_FILE_FW=INFO_firmware
@SET INFO_FILE_RES=INFO_resources
@SET INFO_FW_ADDR_START=0x08070000
@SET INFO_FW_ADDR_STOP=0x0807FFFF
@SET INFO_RES_ADDR_START=0x90F00000
@SET INFO_RES_ADDR_STOP=0x90FFFFFF

@ECHO.
