:: arg1 is the build directory
set "projectdir=%1"
set "operation=%~2"
set "userAppBinary=%projectdir%\..\Binary"
set "loader=%projectdir%\STM32L562E-DK_TFM_Loader\Exe\Project.bin"
set "binarydir=%projectdir%\..\Binary"
set "loader_bin=%binarydir%\loader.bin"
set loader_size=0x6000
pushd %projectdir%\..\..\..\..\..\..\Middlewares\Third_Party\mcuboot
set mcuboot_dir=%cd%
popd
::line for window executable
echo Postbuild with windows executable
set "imgtool=%mcuboot_dir%\scripts\dist\imgtool\imgtool.exe"
set "python="
if exist %imgtool% (
goto postbuild
)
:py
::line for python
echo Postbuild with python script
set "imgtool=%mcuboot_dir%\scripts\imgtool.py"
set "python=python "
:postbuild
::Make sure we have a Binary sub-folder in UserApp folder
if not exist "%binarydir%" (
mkdir "%binarydir%" 
)
:: padd loader binary 
set "command=%python%%imgtool% ass  -i %loader_size% %loader% %loader_bin% >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

exit 0
:error
echo "%command% : failed" >> %projectdir%\\output.txt
exit 1


