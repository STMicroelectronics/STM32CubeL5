#section to be updated according to flash_layout.h changes
sec1_end=37
wrp_end=23
hdp_end=29
#section end
echo "hardening script started"
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
connect="-c port=SWD mode=UR --hardRst"
connect_no_reset="-c port=SWD mode=HotPlug"
write_protect_secure="-ob WRP1A_PSTRT=0  WRP1A_PEND="$wrp_end" SECWM1_PSTRT=0 SECWM1_PEND="$sec1_end" HDP1_PEND="$hdp_end" HDP1EN=0x1"
echo $write_protect_secure
$stm32programmercli $connect_no_reset $write_protect_secure
read -p "hardening script Done, press a key" -n1 -s