echo "regression script started"
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
# remove write protection
connect="-c port=SWD mode=UR --hardRst"
connect_no_reset="-c port=SWD mode=HotPlug"
rdp_0="-ob RDP=0xAA"
remove_bank1_protect="-ob SECWM1_PSTRT=127 SECWM1_PEND=0 WRP1A_PSTRT=127  WRP1A_PEND=0   WRP1B_PSTRT=127  WRP1B_PEND=0"
remove_bank2_protect="-ob SECWM2_PSTRT=127 SECWM2_PEND=0 WRP2A_PSTRT=127  WRP2A_PEND=0   WRP2B_PSTRT=127  WRP2B_PEND=0"
erase_all="-e all"
remove_hdp_protection="-ob   HDP1_PEND=0 HDP1EN=0   HDP2_PEND=0 HDP2EN=0"
# SECBOOTADD0=0x180010 is shifted. The address of the BOOT0 is 0xC000800
default_ob="-ob  TZEN=1 SRAM2_RST=0 SECBOOTADD0=0x180010 DBANK=1 SECWM1_PSTRT=0 SECWM1_PEND=127  SECWM2_PSTRT=127 SECWM2_PEND=0"
echo "Regression to RDP 0"
# if we are in RDP lvl 1 we do not have to reset in order to perform a regression to rdp0. If we are in rdp0 it does nothing.
$stm32programmercli $connect_no_reset $rdp_0
echo "Remove bank1 protection and erase all"
$stm32programmercli $connect $remove_bank1_protect $erase_all
echo "Remove bank2 protection and erase all"
$stm32programmercli $connect $remove_bank2_protect $erase_all
echo "Remove hdp protection"
$stm32programmercli $connect_no_reset $remove_hdp_protection
echo "Set default OB (dual bank , tzen=1 , secure entry point=0xc0000000, bank 1 full secure"
$stm32programmercli $connect_no_reset $default_ob
read -p "regression script Done, press a key" -n1 -s