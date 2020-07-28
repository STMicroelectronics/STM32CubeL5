#!/bin/bash - 
sec1_end=117
sec2_start=127
sec2_end=0
wrp_start=4
wrp_end=35
hdp_end=34
wrp_bank2_start=116
wrp_bank2_end=127
echo "hardening script started"
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
bank2_secure="-ob SECWM2_PSTRT="$sec2_start" SECWM2_PEND="$sec2_end
connect="-c port=SWD mode=UR --hardRst"
connect_no_reset="-c port=SWD mode=HotPlug"
wrp_loader="WRP2A_PSTRT="$wrp_bank2_start" WRP2A_PEND="$wrp_bank2_end
wrp_sbsfu="WRP1A_PSTRT="$wrp_start" WRP1A_PEND="$wrp_end
write_protect_secure="-ob "$wrp_sbsfu" "$wrp_loader" SECWM1_PSTRT=0 SECWM1_PEND="$sec1_end" HDP1_PEND="$hdp_end" HDP1EN=1"
$stm32programmercli $connect_no_reset $bank2_secure
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "hardening script failed, press a key" -n1 -s; fi
  exit 1
fi
$stm32programmercli $connect_no_reset $write_protect_secure
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "hardening script failed, press a key" -n1 -s; fi
  exit 1
fi
if [ "$1" != "AUTO" ]; then read -p "hardening script Done, press a key" -n1 -s; fi
exit 0