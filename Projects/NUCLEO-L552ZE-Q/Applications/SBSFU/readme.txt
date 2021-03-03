/**
  @page SBSFU

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application shows SBSFU on Cortex-M 
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description 

The SBSFU provides a Root of Trust solution including Secure Boot and Secure
Firmware Update functionalities that is used before executing the application
and provides an example of secure service (GPIO toggle) that is isolated from the
non-secure application but can be used by the non-secure application at run-time.

The Secure Boot and Secure Firmware Update(SBSFU_Boot) is based on the Open Source
TF-M reference implementation that has been ported on STM32 microcontroller to take 
benefit of STM32 HW security features such as CM33 Trust Zone/MPU, Trust Zone
aware peripherals, Memory protections (HDP, WRP) and enhanced life cycle scheme.

It allows the update of the STM32 microcontroller built-in program with new firmware 
versions, adding new features and correcting potential issues. The update process is
performed in a secure way to prevent unauthorized updates and access to confidential
on-device data:

-	The Secure Boot (Root of Trust services) is an immutable code, always
    executed after a system reset, that checks STM32 static protections,
    activates STM32 runtime protections and then verifies the authenticity
    (RSA or ECDSA signature) and integrity (SHA256) of the application code
    before execution in order to ensure that invalid or malicious
    code cannot be run. The default asymmetric key (RSA or ECDSA) is taken from
    Middlewares/Third_Party/trustedfirmware/bl2/ext/mcuboot/keys.c
    and is embedded in the provisionned data area in the secure boot and secure
    firmware update binary.

-	The Secure Firmware Update application is an immutable code that detects
    that a new firmware image is available, and that checks its authenticity and
    integrity. It also decrypts it (AES-CTR) during installation, if image is
    installed in internal flash. In case of image installation in external
    flash, image remains AES-CTR encrypted, as it will be on-the-fly decrytped
    during its execution.
    The AES-CTR key is encrypted (RSA-OAEP or ECIES-P256) and provided in the
    firmware image itself. The default asymmetric key (RSA or ECDSA) used to
    encrypt the AES-CTR key is distinct from the signature key, but also taken
    from Middlewares/Third_Party/trustedfirmware/bl2/ext/mcuboot/keys.c.
    The firmware update can be done either on the secure part of firmware
    image and/or on the non-secure part of the firmware image.

The SBSFU secure application configures 2 secure IOs with HAL GPIO :
- The secure IO (PC.07 which corresponds to LED1) toggles every second on Secure 
SysTick timer interrupt basis.
- The secure IO (PB.07 which corresponds to LED2) toogles after a non secure call.

The SBSFU Non secure application uses the secure LED toggle service provided by 
the SBSFU secure application.

For more details, refer to AN5447 "Overview of Secure Boot and Secure Firmware
Update solution on Arm® TrustZone® STM32L5 Series microcontrollers"
available from the STMicroelectronics microcontroller website www.st.com.

@par Directory contents

   - Linker      Linker files definition shared between SBSFU_Boot,
                 SBSFU_Appli and SBSFU_Loader projects.
   - SBSFU_Appli Secure services application and example of non-secure user
                 application. Refer to SBSFU_Appli\readme.txt.
   - SBSFU_Loader Local loader application. Refer to SBSFU_Loader\readme.txt.
   - SBSFU_Boot  Secure boot and secure firmware update application
                 Refer to SBSFU_Boot\readme.txt.

@par Hardware and Software environment

  - This example runs on STM32L552xx devices.
  
  - This example has been tested with STMicroelectronics NUCLEO-L552ZE-Q (MB1361)
    board and can be easily tailored to any other supported device
    and development board.

  - This example needs a terminal emulator supporting YModem protocol.
    (Tera Term for example, open source free software terminal emulator
    that can be downloaded from https://osdn.net/projects/ttssh2/)

  - This example needs at least STM32CubeProgrammer v2.4.0 version,
    available on www.st.com.

@Known limitations

  - IAR build not working if cube package is installed on a path with a space.

@par How to use it ? 

Several steps to run SBSFU application :

0. Prerequisite

   Two versions of imgtool (used during build process) are available in mcuboot
   middleware: windows executable and python version. By default, the windows
   executable is selected. It is possible to switch to python version by:
   - installing python (Python 3.6 or newer) with the required modules from
     Middlewares\Third_Party\mcuboot\scripts\requirements.txt:
       pip install -r requirements.txt
   - having python in execution path variable
   - deleting imgtool.exe in Middlewares\Third_Party\mcuboot\scripts\dist\imgtool

1. Build

   Build the SBSFU projects in the following order. This is mandatory as each
   projects requests some objects generated by the compilation of the previous
   one:
   1- Build SBSFU_Boot project.
      This step creates the secure boot and secure firmware update binary
      including provisionned user data (keys, IDs...).
      Depending on toolchain, it is located here:
        STM32CubeIDE: SBSFU_Boot\STM32CubeIDE\Release\SBSFU_Boot.bin
        EWARM:        SBSFU_Boot\EWARM\NUCLEO-L552ZE-Q_SBSFU_Boot\Exe\Project.bin
        MDK-ARM:      SBSFU_Boot\MDK-ARM\NUCLEO-L552ZE-Q\Exe\Project.bin

   2- Build SBSFU_Appli Secure project.
      This step creates the SBSFU appli Secure binary:
        STM32CubeIDE: SBSFU_Appli\STM32CubeIDE\Secure\Release\SBSFU_Appli_Secure.bin
        EWARM:        SBSFU_Appli\EWARM\NUCLEO-L552ZE-Q_SBSFU_Secure\Exe\Project.bin
        MDK-ARM:      SBSFU_Appli\MDK-ARM\NUCLEO-L552ZE-Q_S\Exe\Project.bin

   3- Build SBSFU_Appli Non Secure project.
      This step creates the SBSFU appli Non Secure binary:
        STM32CubeIDE: SBSFU_Appli\STM32CubeIDE\NonSecure\Release\SBSFU_Appli_NonSecure.bin
        EWARM:        SBSFU_Appli\EWARM\NUCLEO-L552ZE-Q_SBSFU_NonSecure\Exe\Project.bin
        MDK-ARM:      SBSFU_Appli\MDK-ARM\NUCLEO-L552ZE-Q_SBSFU_NS\Exe\Project.bin
      It also produces the assembled (S and NS) SBSFU Application binary in
      SBSFU_Appli\Binary\sbsfu.bin.
      It also produces encrypted assembled signed image in
      SBSFU_Appli\Binary\sbsfu_enc_sign.bin, and clear assembled signed image in
      SBSFU_Appli\Binary\sbsfu_sign.bin.

   4. Build SBSFU_Loader Secure project.
      This step creates the SBSFU loader Secure binary:
        STM32CubeIDE: SBSFU_Loader\STM32CubeIDE\Secure\Release\SBSFU_Loader_Secure.bin
        EWARM:        SBSFU_Loader\EWARM\NUCLEO-L552ZE-Q_SBSFU_Secure_Loader\Exe\Project.bin
        MDK-ARM:      SBSFU_Loader\MDK-ARM\NUCLEO-L552ZE-Q_SBSFU_Secure_Loader\Exe\Project.bin

   5. Build SBSFU_Loader Non Secure project.
      This step creates the SBSFU loader Non Secure binary:
        STM32CubeIDE: SBSFU_Loader\STM32CubeIDE\NonSecure\Release\SBSFU_Loader_NonSecure.bin
        EWARM:        SBSFU_Loader\EWARM\NUCLEO-L552ZE-Q_SBSFU_NonSecure_Loader\Exe\Project.bin
        MDK-ARM:      SBSFU_Loader\MDK-ARM\NUCLEO-L552ZE-Q_SBSFU_NonSecure_Loader\Exe\Project.bin
      It also produced the assembled (S and NS) SBSFU loader image in SBSFU_Loader\Binary\loader.bin.

2. Initialize the device

   Depending on your toolchain, execute regression script (relying on
   STM32CubeProgrammer CLI tool) to easily perform device initialization in a
   shot:
     STM32CubeIDE: SBSFU_Boot\STM32CubeIDE\regression.sh
     EWARM:        SBSFU_Appli\EWARM\regression.bat
     MDK-ARM:      SBSFU_Appli\MDK-ARM\regression.bat

   As an alternative, it is also possible to initialize and verify manually the
   Option Bytes by means of STM32CubeProgrammer GUI tool:
   - Please ensure with STM32CubeProgammer the following Option bytes
     configuration for the device:
     - RDP         = 0xAA (RDP Level 0)
     - DBANK       = 1 (Dual bank mode enabled)
     - SWAP_BANK   = 0 (Bank1 and Bank2 are not swapped)
     - TZEN        = 1 (TrustZone security enabled)
     - SECBOOTADD0 = 0x180030 (boot entry point address 0x0C001800)
     - SRAM2-RST   = 0 (SRAM2 erased at each reset)
   - Ensure also that these protections are all disabled in the Option Bytes
     configuration: BootLock, HDP, WRP, SECWM.
   - Ensure also that device is mass erased.

3. Use a terminal emulator

   Serial port configuration should be :
   - Baud rate    = 115200
   - Data         = 8 bits
   - Parity       = none
   - Stop         = 1 bit
   - Flow control = none 
   Terminal emulator is used for UART connection with the board.
   Connect terminal emulator to COM port of the board.
   The terminal emulator is used to log SBSFU_Boot information, and enter commands
   for User application.

4. Program the SBSFU into flash

   Program the SBSFU_Boot binary, the clear signed SBSFU_Appli image, and SBSFU_Loader
   image into internal flash.

   Depending on your toolchain, execute script SBSFU_UPDATE (relying on
   STM32CubeProgrammer CLI tool) to easily program the SBSFU into device internal
   flash in one shot:
     STM32CubeIDE: SBSFU_Boot\STM32CubeIDE\SBSFU_UPDATE.sh
     EWARM:        SBSFU_Boot\EWARM\SBSFU_UPDATE.bat
     MDK-ARM:      SBSFU_Boot\MDK-ARM\SBSFU_UPDATE.bat

5. Reset the board

   Press the board reset button, and check the log information on the terminal
   emulator:
     - SBSFU_Boot application starts
     - Then it configures the static protections (because 'development mode'
       is used by default)
     - Then it get locked (because intrusion is detected)

6. Unlock the board

   To exit from locked state (intrusion), remove the jumper JP5 (IDD) on the board,
   then put it back in place. Check the log information:
     - SBSFU_Boot starts
     - Then it checks the static protections
     - Then it verifies the SBSFU application image signature.
     - Then it jumps to the SBSFU application, displaying the User Application
       Main menu.

7. Use User App

   This user application gives access to a Main menu which allows to:
   1 - Test protections
   2 - Test Secure LED

   1. Press 1 to enter test protections menu

      - NonSecure try to access to Secure (#1): test unauthorized access to
        Secure side from Non Secure side. A fault occurs, then reset is performed.
        Several succesives unauthorized accesses to different areas are performed.

      - RDP regression (#2): For 'production mode', put the device in a state
        where RDP regression can be performed. The RDP regression can be performed
        either by connecting STM32CubeProgrammer tool in HotPlug mode and
        performing RDP level regression to RDP level 0, or by executing the
        regression script (described in step 2).

      Return to previous menu by pressing x.

   3. Press 2 to toggle Secure LED (LED2)

8. Use Local loader App

   Local loader application is an immutable code, in non secure and secure areas.
   To enter local loader application, press user button (blue) during board reset.

   This application gives access to a Main menu which allows to download new firmware
   image in download area.

   It is possible to download a new SBSFU application image.
   - Download Image (#2): Download encrypted signed SBSFU_Appli image
     (ex: SBSFU_Appli\Binary\sbsfu_enc_sign.bin)

   Send the signed binary with Tera Term by using menu:
   "File > Transfer > YMODEM > Send..."

   After the download, press (#1) to reset the board, or press the board
   reset button (black).
   After reset, the downloaded firmware image is detected, verified,
   decrypted and installed, then executed by SBSFU_Boot.

9. Configuration

   It is possible to change SBSFU configuration.
   When changing configuration, all the projects must be cleaned then fully rebuild.

   1. Crypto-scheme

      By default, the crypto scheme is RSA-2048 signature, and AES-CTR-128 image
      encryption with key RSA-OAEP encrypted.
      It is possible to select another crypto-scheme, thanks to CRYPTO_SCHEME define in 
      SBSFU_Boot\Inc\mcuboot_config\mcuboot_config.h.

      #define CRYPTO_SCHEME_RSA2048    0x0 /* RSA-2048 signature,
                                              AES-CTR-128 encryption with key RSA-OAEP encrypted */
      #define CRYPTO_SCHEME_RSA3072    0x1 /* RSA-3072 signature,
                                              AES-CTR-128 encryption with key RSA-OAEP encrypted */
      #define CRYPTO_SCHEME_EC256      0x2 /* ECDSA-256 signature,
                                              AES-CTR-128 encryption with key ECIES-P256 encrypted */

      #define CRYPTO_SCHEME            CRYPTO_SCHEME_RSA2048  /* Select one of available crypto schemes */

   2. Local loader

      By default, local loader application is included. It is possible to remove
      it thanks to MCUBOOT_EXT_LOADER define in Linker\flash_layout.h.

      #define MCUBOOT_EXT_LOADER         /* Defined: Add external local loader application.
                                                     To enter it, press user button at reset.
                                            Undefined: No external local loader application. */

      In this case, the build of SBSFU_Loader Secure and SBSFU_Loader NonSecure
      must be skipped.

   3. Number of images

      By default, 1 firmware image is managed with 1 single signature:
      1 image for both NS and S binaries.
      It is possible to separate NS and S binaries into 2 images
      with 2 distinct signatures, thanks to MCUBOOT_IMAGE_NUMBER define in
      Linker\flash_layout.h.

      #define MCUBOOT_IMAGE_NUMBER 1     /* 1: S and NS application binaries are assembled in one single image.
                                            2: Two separated images for S and NS application binaries. */

      Note: When 2 images are selected, the local loader menu proposes to download
      separately a new SBSFU Secure image or SBSFU Non Secure image or both.
      - Download Secure Image (#2): Download Secure signed image
        (ex: TFM_Appli\Binary\tfm_s_enc_sign.bin)
      - Download NonSecure Image (#3): Download Non Secure signed image
        (ex: TFM_Appli\Binary\tfm_ns_enc_sign.bin)

   4. Primary slot only

      By default, primary slot only configuration is used (for each image). 
      In this mode, the local loader downloads encrypted image directly in
      primary slot, and the SBSFU_Boot decrypts in place the image during
      installation process.
      It is possible to configure the SBSFU to use a secondary slot for image
      downloading. The SBSFU_Boot decrypts the image when installing it from
      secondary slot to primary slot.
      This is configured thanks to MCUBOOT_PRIMARY_ONLY define in
      Linker\flash_layout.h.
      
      #define MCUBOOT_PRIMARY_ONLY       /* Defined: No secondary (download) slot(s),
                                                     only primary slot(s) for each image.
                                            Undefined: Primary and secondary slot(s) for each image. */

      Note: When primary and secondary slot configuration is selected, an
      additional menu item (#3) 'Download a new Fw Image' is present in user
      application. Its usage is exactly the same as the local loader application.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
