set sec1_end=117
set sec2_start=127
set sec2_end=0
set wrp_start=4
set wrp_end=35
set hdp_end=34
set wrp_bank2_start=116
set wrp_bank2_end=127
echo "hardening script started"
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set bank2_secure="-ob SECWM2_PSTRT=%sec2_start% SECWM2_PEND=%sec2_end%"
set connect="-c port=SWD mode=UR --hardRst"
set connect_no_reset="-c port=SWD mode=HotPlug"
set wrp_loader="WRP2A_PSTRT=%wrp_bank2_start% WRP2A_PEND=%wrp_bank2_end%"
set wrp_sbsfu="WRP1A_PSTRT=%wrp_start% WRP1A_PEND=%wrp_end%"
set write_protect_secure="-ob %wrp_sbsfu% %wrp_loader% SECWM1_PEND=%sec1_end% HDP1_PEND=%hdp_end% HDP1EN=1"
%stm32programmercli% %connect_no_reset% %bank2_secure%
IF %errorlevel% NEQ 0 goto :error
%stm32programmercli% %connect_no_reset% %write_protect_secure%
IF %errorlevel% NEQ 0 goto :error
echo "hardening script done, press key"
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo "hardening script failed, press key"
IF [%1] NEQ [AUTO] pause
exit 1
