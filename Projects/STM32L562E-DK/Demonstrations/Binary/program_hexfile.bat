:: STM32CubeProgrammer Utility flash script

@ECHO OFF
@setlocal
::COLOR 0B

:: Current Directory
@SET CUR_DIR=%CD%

:: Chip Name
@SET CHIP_NAME=STM32L562E
:: Main Board
@SET MAIN_BOARD=-DK
:: Demo Name
@SET DEMO_NAME=STM32Cube_Demo
:: Demo Version
@SET DEMO_VER=
:: Hex filename
@SET HEX_FILE="%DEMO_NAME%-%CHIP_NAME%%MAIN_BOARD%%DEMO_VER%.hex"
@IF NOT EXIST "%HEX_FILE%" @SET HEX_FILE=%DEMO_NAME%-%CHIP_NAME%%MAIN_BOARD%-%DEMO_VER%_FULL.hex
@IF NOT EXIST "%HEX_FILE%" @ECHO %HEX_FILE% Does not exist !! && GOTO goError

:: Board ID
@SET BOARD_ID=0
:: External Loader Name
@SET LOADER_CHIP_NAME=STM32L552E
@SET LOADER_BOARD=-EVAL
@SET EXT_LOADER=MX25LM51245G_%LOADER_CHIP_NAME%%LOADER_BOARD%

@SET STM32_PROGRAMMER_PATH="%ProgramFiles(x86)%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @SET STM32_PROGRAMMER_PATH="%ProgramW6432%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @SET STM32_PROGRAMMER_PATH="%ProgramFiles%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @ECHO STM32CubeProgrammer is not installed !! && GOTO goError
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @ECHO %STM32_PROGRAMMER_PATH% Does not exist !! && GOTO goError
@SET STM32_EXT_FLASH_LOADER=%STM32_PROGRAMMER_PATH%\ExternalLoader\%EXT_LOADER%.stldr
@IF NOT EXIST %STM32_EXT_FLASH_LOADER% @ECHO %STM32_EXT_FLASH_LOADER% Does not exist !! && GOTO goError

@SET STM32_EXT_FLASH_LOADER=%STM32_PROGRAMMER_PATH%\ExternalLoader\%EXT_LOADER%.stldr

TITLE STM32CubeProgrammer Utility for %CHIP_NAME%%MAIN_BOARD%

:: Add STM32CubeProgrammer to the PATH
@SET PATH=%STM32_PROGRAMMER_PATH%;%PATH%

@ECHO.
@ECHO =================================================
@ECHO Erase and Flash all memories and reboot the board
@ECHO =================================================
@ECHO. 
STM32_Programmer_CLI.exe -c port=SWD index=%BOARD_ID% reset=HWrst -el %STM32_EXT_FLASH_LOADER% -e all -d %HEX_FILE% -HardRst
@IF NOT ERRORLEVEL 0 (
  @GOTO goError
)

@GOTO goOut

:goError
@SET RETERROR=%ERRORLEVEL%
@COLOR 0C
@ECHO.
@ECHO Failure Reason Given is %RETERROR%
@PAUSE
@COLOR 07
@EXIT /b %RETERROR%

:goOut
