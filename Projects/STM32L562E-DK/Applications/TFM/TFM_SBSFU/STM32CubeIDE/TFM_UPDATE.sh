echo "TFM UPDATE started"
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
connect_no_reset="-c port=SWD mode=HotPlug"
connect="-c port=SWD mode=HotPlug --hardRst"
echo "Write TFM_Appli Secure"
# part ot be updated according to flash_layout.h
slot0=0x0c014000
slot1=0x0c038000
$stm32programmercli $connect -d ../../TFM_Appli/Binary/tfm_s_sign.bin $slot0 -v
echo "TFM_Appli Secure Written"
echo "Write TFM_Appli NonSecure"
$stm32programmercli $connect -d ../../TFM_Appli/Binary/tfm_ns_sign.bin $slot1 -v 
echo "TFM_Appli NonSecure Written"
echo "Write TFM_SBSFU"
$stm32programmercli $connect -d ./Release/TFM_SBSFU.bin 0x0c000000 -v
echo "TFM_SBSFU Written"
