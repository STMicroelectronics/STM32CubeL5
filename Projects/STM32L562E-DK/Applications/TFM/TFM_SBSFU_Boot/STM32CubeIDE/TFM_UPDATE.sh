#!/bin/bash - 
echo "TFM_UPDATE started"
# Absolute path to this script
SCRIPT=$(readlink -f $0)
# Absolute path this script
SCRIPTPATH=`dirname $SCRIPT`
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
external_loader="C:\PROGRA~1\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\ExternalLoader\MX25LM51245G_STM32L562E-DK.stldr"
connect_no_reset="-c port=SWD mode=UR -el $external_loader"
connect="-c port=SWD mode=UR --hardRst -el $external_loader"
slot0=0xc017000
slot1=0x90000000
slot2=0x90100000
slot3=0x90124000
boot=0xc002000
loader=0xc07a000
cfg_loader=1
image_number=2
if [ $image_number == 2 ]; then
echo "Write TFM_Appli Secure"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_s_sign.bin $slot0 -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Appli Secure Written"
echo "Write TFM_Appli NonSecure"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_ns_sign.bin $slot1 -v 
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Appli NonSecure Written"
fi
if [ $image_number == 1 ]; then
echo "Write TFM_Appli"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_sign.bin $slot0 -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Appli Written"
fi
# write loader if config loader is active
if [ $cfg_loader == 1 ]; then
echo "Write TFM_Loader"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Loader/Binary/loader.bin $loader -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Loader Written"
fi
echo "Write TFM_SBSFU_Boot"
$stm32programmercli $connect -d $SCRIPTPATH/Release/TFM_SBSFU_Boot.bin $boot -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_SBSFU_Boot Written"
if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script Done, press a key" -n1 -s; fi
exit 0