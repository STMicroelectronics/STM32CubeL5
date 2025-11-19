:: arg1 is the build directory
:: arg2 is the version
:: arg3 is the binary type (1 nonsecure, 2 secure)
set "projectdir=%1"
call "%projectdir%\..\..\env.bat"
set "version=%2"
set "signing=%~3"
set "userAppBinary=%projectdir%\..\Binary"

set "nsclib_destination=%projectdir%\..\Secure_nsclib\secure_nsclib.o"
set "nsclib_source=%projectdir%\NUCLEO-L552ZE-Q_SBSFU_Secure\Exe\Project_s_import_lib.o"
set "sbsfu_ns_sign=%userAppBinary%\sbsfu_ns_sign.bin"
set "sbsfu_s_sign=%userAppBinary%\sbsfu_s_sign.bin"
set "sbsfu_s_enc_sign=%userAppBinary%\sbsfu_s_enc_sign.bin"
set "sbsfu_ns_enc_sign=%userAppBinary%\sbsfu_ns_enc_sign.bin"
:: variable for image_number = 1
set "sbsfu_enc_sign=%userAppBinary%\sbsfu_enc_sign.bin"
set "sbsfu_sign=%userAppBinary%\sbsfu_sign.bin"
set "sbsfu=%userAppBinary%\sbsfu.bin"

::field updated with sbsfu_boot postbuild

:: image_ns_size is the slot image size if image_number is 1
set image_ns_size=0x6b000
set image_s_size=0x8000
set primary_only=1
set image_number=1
set crypto_scheme=0
:: end of updated field
pushd %projectdir%\..\..\SBSFU_Boot\Src
set sbsfu_key_dir=%cd%
popd
::Make sure we have a Binary sub-folder in UserApp folder
if not exist "%userAppBinary%" (
mkdir "%userAppBinary%"
)
:: recopy nsclib to Cube expected directory when secure postbuild
IF "%signing%" == "nonsecure" (
goto :start;
)
set "command=copy %nsclib_source% %nsclib_destination%"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

if  "%image_number%" == "1" (
:: when image number is 1, image is assemble and sign during non secure postbuild
exit 0
)
:start
if exist %imgtool% (
goto postbuild
) else (
  echo ""
  echo "!!! WARNING : imgtool has not been found on your installation."
  echo ""
  echo "  Install CubeProgrammer on your machine in default path : C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer"
  echo "  or "
  echo "  Update your %projectdir%\..\..\env.bat with the proper path."
  echo ""
  exit 0
)

:postbuild

::sign mode
if "%crypto_scheme%" == "2" (
set "mode=ec256"
)
::keys selection
if  "%mode%" == "ec256" (
set "key_s=%sbsfu_key_dir%\root-ec-p256.pem"
set "key_ns=%sbsfu_key_dir%\root-ec-p256_1.pem"
set "key_enc_pub=%sbsfu_key_dir%\enc-ec256-pub.pem"
)
set "sbsfu_s=%projectdir%\NUCLEO-L552ZE-Q_SBSFU_Secure\Exe\Project.bin"
set "sbsfu_ns=%projectdir%\NUCLEO-L552ZE-Q_SBSFU_NonSecure\Exe\Project.bin"



if "%primary_only%" == "1" (
set "option=--primary-only"
goto signing
)
:signing
echo %signing% %mode% %option% image_number=%image_number% > %projectdir%\output.txt 2>&1

if "%image_number%" == "2" (
goto continue
)
echo "assemble image" >> %projectdir%\output.txt 2>&1
set "command_ass=%imgtool% ass -f %sbsfu_s% -o %image_s_size%  -i 0x0 %sbsfu_ns% %sbsfu% >> %projectdir%\output.txt 2>&1"
%command_ass%
IF %ERRORLEVEL% NEQ 0 goto :error_ass
:: sign assemble binary as secure image, slot size is image_ns_size
set "image_s_size=%image_ns_size%"
set "signing="
set "sbsfu_s=%sbsfu%"
set "sbsfu_s_sign=%sbsfu_sign%"
set "sbsfu_s_enc_sign=%sbsfu_enc_sign%"
:continue


IF "%signing%" == "nonsecure" (
echo "%signing% signing" >> %projectdir%\output.txt 2>&1
set "command_sign=%imgtool% sign -k %key_ns% -e little -S %image_ns_size% -H 0x400 --pad-header %option% -v %version% -s auto --align 8 %sbsfu_ns% %sbsfu_ns_sign% >> %projectdir%\output.txt 2>&1"
goto :docommand_sign
)
echo "%signing% signing" >> %projectdir%\output.txt 2>&1
set "command_sign=%imgtool% sign -k %key_s% -e little -S %image_s_size% -H 0x400 --pad-header %option% -v %version% -s auto --align 8 %sbsfu_s% %sbsfu_s_sign% >> %projectdir%\output.txt 2>&1"
:docommand_sign
%command_sign%
IF %ERRORLEVEL% NEQ 0 goto :error
IF "%signing%" == "nonsecure" (
echo "non secure encrypting" >> %projectdir%\output.txt 2>&1
set "command_enc=%imgtool% sign -k %key_ns% -E %key_enc_pub% -e little -S %image_ns_size% -H 0x400 --pad-header %option% -v %version%  -s auto --align 8 %sbsfu_ns% %sbsfu_ns_enc_sign% >> %projectdir%\output.txt 2>&1"
goto :docommand_enc
)
echo "secure encrypting" >> %projectdir%\output.txt 2>&1
set "command_enc=%imgtool% sign -k %key_s% -E %key_enc_pub% -e little -S %image_s_size% -H 0x400 --pad-header %option% -v %version%  -s auto --align 8 %sbsfu_s% %sbsfu_s_enc_sign% >> %projectdir%\output.txt 2>&1"
:docommand_enc
%command_enc%
IF %ERRORLEVEL% NEQ 0 goto :error_enc
exit 0
:error
echo "%command_sign% : failed" >> %projectdir%\\output.txt
exit 1
:error_enc
echo "%command_enc% : failed" >> %projectdir%\\output.txt
exit 1
:error_ass
echo "%command_ass% : failed" >> %projectdir%\\output.txt
exit 1
