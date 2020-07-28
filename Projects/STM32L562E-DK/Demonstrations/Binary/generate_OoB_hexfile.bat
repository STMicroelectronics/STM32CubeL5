:: OoB Hexfile generation script

@ECHO OFF
@setlocal

@SET VAR_ENVS=demo_envs.bat
@IF NOT EXIST %VAR_ENVS% @ECHO %VAR_ENVS% Does not exist !! && GOTO goError

:: Current Directory
@SET CUR_DIR=%CD%
:: PATH
@SET PATH=%CUR_DIR%\..\..\..\..\Utilities\PC_Software\HexTools;%PATH%

:: Binary Temp folder
@SET TEMP_DIR=%CUR_DIR%\tmp
:: Binary Directory
@SET BINARY_DIR=%CUR_DIR%

:: Setup environment variables for OoB configuration
@CALL %VAR_ENVS% "OoB"

@SET DEMO_FILE=%BINARY_DIR%\%DEMO_FILE_NAME%.hex

:: Check for Binary files
@IF NOT EXIST %ML_FILE% @ECHO %ML_FILE% Does not exist !! && GOTO goError
@IF NOT EXIST %PWR_FILE% @ECHO %PWR_FILE% Does not exist !! && GOTO goError
@IF NOT EXIST %AI_FILE% @ECHO %AI_FILE% Does not exist !! && GOTO goError
@IF NOT EXIST %BLE_FILE% @ECHO %BLE_FILE% Does not exist !! && GOTO goError
@IF NOT EXIST %AUD_FILE% @ECHO %AUD_FILE% Does not exist !! && GOTO goError
@IF NOT EXIST %INFO_FILE% @ECHO %INFO_FILE% Does not exist !! && GOTO goError

:: Clean-Up
@IF EXIST %DEMO_FILE% @RD %DEMO_FILE% /S /Q
@IF EXIST %TEMP_DIR% @RD %TEMP_DIR% /S /Q

:: Create needed folders
@IF NOT EXIST %BINARY_DIR% @MKDIR %BINARY_DIR%
@MKDIR %TEMP_DIR%

@ECHO.
@ECHO Copying Binary files inside %TEMP_DIR%
@IF EXIST %ML_FILE%   @COPY /V %ML_FILE% %TEMP_DIR%\%ML_FILE_NAME%
@IF EXIST %PWR_FILE% @COPY /V %PWR_FILE% %TEMP_DIR%\%PWR_FILE_NAME%
@IF EXIST %AI_FILE% @COPY /V %AI_FILE% %TEMP_DIR%\%AI_FILE_NAME%
@IF EXIST %BLE_FILE% @COPY /V %BLE_FILE% %TEMP_DIR%\%BLE_FILE_NAME%
@IF EXIST %AUD_FILE% @COPY /V %AUD_FILE% %TEMP_DIR%\%AUD_FILE_NAME%
@IF EXIST %INFO_FILE% @COPY /V %INFO_FILE% %TEMP_DIR%\%INFO_FILE_NAME%

:: Move to Binary folder
@CD %TEMP_DIR%

@ECHO.
@ECHO Generating %DEMO_FILE% Using
@IF EXIST %ML_FILE_NAME%   @ECHO  + %ML_FILE_NAME%
@IF EXIST %PWR_FILE_NAME% @ECHO  + %PWR_FILE_NAME% %PWR_SIM%
@IF EXIST %AI_FILE_NAME% @ECHO  + %AI_FILE_NAME% %AI_SIM%
@IF EXIST %BLE_FILE_NAME% @ECHO  + %BLE_FILE_NAME% %BLE_SIM%
@IF EXIST %AUD_FILE_NAME% @ECHO  + %AUD_FILE_NAME% %AUD_SIM%
@IF EXIST %INFO_FILE_NAME% @ECHO  + %INFO_FILE_NAME% %INFO_SIM%

@IF EXIST %INFO_FILE_NAME% @srec_cat.exe -disable-sequence-warning %ML_FILE_NAME% -Intel %PWR_FILE_NAME% -Intel %AI_FILE_NAME% -Intel %BLE_FILE_NAME% -Intel %AUD_FILE_NAME% -Intel %INFO_FILE_NAME% -Intel -o %DEMO_FILE% -Intel
@IF NOT EXIST %DEMO_FILE% @GOTO goError

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
CD %CUR_DIR%
:: Clean-Up
@RD %TEMP_DIR% /S /Q
PAUSE
