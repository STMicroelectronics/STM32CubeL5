echo "TFM UPDATE started"
# Absolute path to this script
SCRIPT=$(readlink -f $0)
# Absolute path this script
SCRIPTPATH=`dirname $SCRIPT`
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
connect_no_reset="-c port=SWD mode=HotPlug"
connect="-c port=SWD mode=HotPlug --hardRst"
echo "Write TFM_Appli Secure"
# part ot be updated according to flash_layout.h
slot0=0x0c014000
slot1=0x0c038000
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_s_sign.bin $slot0 -v
echo "TFM_Appli Secure Written"
echo "Write TFM_Appli NonSecure"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_ns_sign.bin $slot1 -v 
echo "TFM_Appli NonSecure Written"
echo "Write TFM_SBSFU_Boot"
$stm32programmercli $connect -d $SCRIPTPATH/Release/TFM_SBSFU_Boot.bin 0x0c000000 -v
echo "TFM_SBSFU_Boot Written"
read -p "TFM_UPDATE Done, press a key" -n1 -s