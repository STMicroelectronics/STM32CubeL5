echo "TFM_UPDATE started"
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set external_loader="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\ExternalLoader\MX25LM51245G_STM32L562E-DK.stldr"
set connect_no_reset=-c port=SWD mode=UR -el %external_loader%
set connect=-c port=SWD mode=UR --hardRst -el %external_loader%
set slot0=0xc017000
set slot1=0x90000000
set slot2=0x90100000
set slot3=0x90124000
set boot=0xc002000
set loader=0xc07a000
set cfg_loader=1
set image_number=2
if  "%image_number%" == "2" (
echo "Write TFM_Appli Secure"
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_s_sign.bin %slot0% -v
IF %errorlevel% NEQ 0 goto :error
echo "TFM_Appli Secure Written"
echo "Write TFM_Appli NonSecure"
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_ns_sign.bin %slot1% -v
IF %errorlevel% NEQ 0 goto :error
echo "TFM_Appli NonSecure Written"
)
if  "%image_number%" == "1" (
echo "Write TFM_Appli"
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_sign.bin %slot0% -v
IF %errorlevel% NEQ 0 goto :error
echo "TFM_Appli Written"
)
:: write loader if config loader is active
if  "%cfg_loader%" == "1" (
echo "Write TFM_Loader"
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Loader\binary\loader.bin %loader% -v
IF %errorlevel% NEQ 0 goto :error
echo "TFM_Loader  Written"
)
echo "Write TFM_SBSFU_Boot"
%stm32programmercli% %connect% -d %~dp0.\STM32L562E-DK\Exe\project.bin %boot% -v
IF %errorlevel% NEQ 0 goto :error
echo "TFM_SBSFU_Boot Written"
echo "TFM_UPDATE script done, press a key"
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo "TFM_UPDATE script failed, press key"
IF [%1] NEQ [AUTO] pause
exit 1