echo "SBSFU UPDATE started"
# Absolute path to this script
SCRIPT=$(readlink -f $0)
# Absolute path this script
SCRIPTPATH=`dirname $SCRIPT`
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
connect_no_reset="-c port=SWD mode=HotPlug"
connect="-c port=SWD mode=HotPlug --hardRst"
echo "Write SBSFU_Appli Secure"
# part ot be updated according to flash_layout.h
slot0=0x0c00f000
slot1=0x0c013000
$stm32programmercli $connect -d $SCRIPTPATH/../../SBSFU_Appli/Binary/sbsfu_s_sign.bin $slot0 -v
echo "SBSFU_Appli Secure Written"
echo "Write SBSFU_Appli NonSecure"
$stm32programmercli $connect -d $SCRIPTPATH/../../SBSFU_Appli/Binary/sbsfu_ns_sign.bin $slot1 -v 
echo "SBSFU_Appli NonSecure Written"
echo "Write SBSFU_Boot"
$stm32programmercli $connect -d $SCRIPTPATH/Release/SBSFU_Boot.bin 0x0c000000 -v
read -p "SBSFU_UPDATE Done, press a key" -n1 -s