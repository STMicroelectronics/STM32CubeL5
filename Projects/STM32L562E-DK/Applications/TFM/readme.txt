/**
  @page TFM

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application shows Trusted Firmware on Cortex-M (TFM)
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

@par Application Licence Clarification

The application TFM components are licenced by ST under Ultimate Liberty 
license SLA0044. Read the file q_a.txt for Application Licence Clarification.

@par Application Description 

The TFM provides a Root of Trust solution including Secure Boot and Secure
Firmware Update functionalities that is used before executing the application
and provides TFM secure services that are isolated form the non-secure
application but can be used by the non-secure application at run-time.

TFM application is based on the Open Source TF-M reference implementation
that has been ported on STM32 microcontroller to take benefit of STM32 HW
security features such as CM33 Trust Zone/MPU, Trust Zone aware peripherals,
Memory protections (HDP, WRP) and enhanced life cycle scheme.

The Secure Boot and Secure Firmware Update solution allows the update of the
STM32 microcontroller built-in program with new firmware versions, adding new
features and correcting potential issues. The update process is performed in
a secure way to prevent unauthorized updates and access to confidential
on-device data:

-	The Secure Boot (Root of Trust services) is an immutable code, always
    executed after a system reset, that checks STM32 static protections,
    activates STM32 runtime protections and then verifies the authenticity
    (RSA or ECDSA signature) and integrity (SHA256) of the application code
    before execution in order to ensure that invalid or malicious
    code cannot be run. The default asymetric key (RSA or ECDSA) is taken from
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
    firmware image itself. The default asymetric key (RSA or ECDSA) used to
    encrypt the AES-CTR key is distinct from the signature key, but also taken
    from Middlewares/Third_Party/trustedfirmware/bl2/ext/mcuboot/keys.c.
    The firmware update can be done either on the secure part of firmware
    image and/or on the non-secure part of the firmware image.

The secure services are an updatable code implementing a set of services
managing critical assets that are isolated from the non-secure application
so that the non-secure application can’t access directly to any of the critical
assets but can only use secure services that use the critical assets:

-	Crypto: secure cryptographic services based on opaque key APIs

-	Secure Storage: protect data Confidentiality/authenticity/Integrity

-	Internal Trusted Storage

-	Attestation: prove product identity via Entity Attestation Token


For more details, refer to UM2671 "Getting Started with the TF-M applications"
available from the STMicroelectronics microcontroller website www.st.com.

@note The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
      from Flash memory and external memories, and reach the maximum performance.

@par Directory contents

   - Linker     Linker files definition shared between TFM_SBSFU_Boot,
                TFM_Appli and TFM_Loader projects.
   - TFM_Appli  Secure services application and example of non-secure user
                application. Refer to TFM_Appli\readme.txt.
   - TFM_Loader Local loader application. Refer to TFM_Loader\readme.txt.
   - TFM_SBSFU_Boot
				Secure boot and secure firmware update application.
                Refer to TFM_SBSFU_Boot\readme.txt.

@par Hardware and Software environment

  - This example runs on STM32L562xx devices.
  
  - This example has been tested with an STMicroelectronics STM32L562E-DK
    board and can be easily tailored to any other supported device 
    and development board.

  - This example needs a terminal emulator supporting YModem protocol.
    (Tera Term for example, open source free software terminal emulator
    that can be downloaded from https://osdn.net/projects/ttssh2/).

  - This example needs at least STM32CubeProgrammer v2.4.0 version,
    available on www.st.com.

@Known limitations

  - IAR build not working if cube package is installed on a path with a space.

@par How to use it ? 

Several steps to run TFM application :

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

   Build the TFM projects in the following order. This is mandatory as each
   projects requests some objects generated by the compilation of the previous
   one:
   1. Build TFM_SBSFU_Boot project.
      This step creates the secure boot and secure firmware update binary
      including provisionned user data (keys, IDs...).
      Depending on toolchain, it is located here:
        STM32CubeIDE: TFM_SBSFU_Boot\STM32CubeIDE\Release\TFM_SBSFU_Boot.bin
        EWARM:        TFM_SBSFU_Boot\EWARM\STM32L562E-DK_TFM_SBSFU_Boot\Exe\Project.bin
        MDK-ARM:      TFM_SBSFU_Boot\MDK-ARM\STM32L562E-DK\Exe\Project.bin

   2. Build TFM_Appli Secure project.
      This step creates the TFM Secure binary:
        STM32CubeIDE: TFM_Appli\STM32CubeIDE\Secure\Release\TFM_Appli_Secure.bin
        EWARM:        TFM_Appli\EWARM\STM32L562E-DK_S\Exe\Project.bin
        MDK-ARM:      TFM_Appli\MDK-ARM\STM32L562E-DK_S\Exe\Project.bin
      It also produces the encrypted TFM Secure signed image in TFM_Appli\Binary\tfm_s_enc_sign.bin
      and clear TFM Secure signed image in TFM_Appli\Binary\tfm_s_sign.bin.

   3. Build TFM_Appli Non Secure project.
      This step creates the TFM Non Secure binary:
        STM32CubeIDE: TFM_Appli\STM32CubeIDE\NonSecure\Release\TFM_Appli_NonSecure.bin
        EWARM:        TFM_Appli\EWARM\STM32L562E-DK_NS\Exe\Project.bin
        MDK-ARM:      TFM_Appli\MDK-ARM\STM32L562E-DK_NS\Exe\Project.bin
      It also produces the encrypted TFM Non Secure signed image in TFM_Appli\Binary\tfm_ns_enc_sign.bin
      and clear TFM Non Secure signed image in TFM_Appli\Binary\tfm_ns_sign.bin.

   4. Build TFM_Loader Non Secure project.
      This step creates the TFM loader binary:
        STM32CubeIDE: TFM_Loader\STM32CubeIDE\Release\TFM_Loader.bin
        EWARM:        TFM_Loader\EWARM\STM32L562E-DK_TFM_Loader\Exe\Project.bin
        MDK-ARM:      TFM_Loader\MDK-ARM\STM32L562E-DK_TFM_Loader\Exe\Project.bin
      It also produces the TFM loader image in TFM_Loader\Binary\loader.bin.

2. Initialize the device

   Depending on your toolchain, execute regression script (relying on
   STM32CubeProgrammer CLI tool) to easily perform device initialization in one
   shot:
     STM32CubeIDE: TFM_SBSFU_Boot\STM32CubeIDE\regression.sh
     EWARM:        TFM_SBSFU_Boot\EWARM\regression.bat
     MDK-ARM:      TFM_SBSFU_Boot\MDK-ARM\regression.bat

   As an alternative, it is also possible to initialize and verify manually the
   Option Bytes by means of STM32CubeProgrammer GUI tool:
   - Please ensure with STM32CubeProgammer the following Option bytes
     configuration for the device:
     - RDP         = 0xAA (RDP Level 0)
     - DBANK       = 1 (Dual bank mode enabled)
     - SWAP_BANK   = 0 (Bank1 and Bank2 are not swapped)
     - TZEN        = 1 (TrustZone security enabled)
     - SECBOOTADD0 = 0x180052 (boot entry point address 0x0C002900)
     - SRAM2_RST   = 0 (SRAM2 erased at each reset)
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
   The terminal emulator is used to log TFM informations, and enter commands
   for User application.

4. Program the TFM into flash

   Program the TFM_SBSFU_Boot binary, the clear signed TFM_Appli Secure image,
   and TFM_Loader image into internal flash. And program the encrypted signed
   TFM_Appli Non Secure image into OSPI external flash (encrypted image will
   be on-the-fly decrypted during its execution, with OTFDEC peripheral).
   
   Depending on your toolchain, execute script TFM_UPDATE (relying on
   STM32CubeProgrammer CLI tool) to easily program the TFM into device internal
   and external flash in one shot:
     STM32CubeIDE: TFM_SBSFU_Boot\STM32CubeIDE\TFM_UPDATE.sh
     EWARM:        TFM_SBSFU_Boot\EWARM\TFM_UPDATE.bat
     MDK-ARM:      TFM_SBSFU_Boot\MDK-ARM\TFM_UPDATE.bat

5. Reset the board

   Press the board reset button, and check the log information on the terminal
   emulator:
     - TFM_SBSFU_Boot application starts
     - Then it configures the static protections (because 'development mode'
       is used by default)
     - Then it get locked (because intrusion is detected)

6. Unlock the board

   To exit from locked state (intrusion), remove the jumper JP2 (IDD) on the board,
   then put it back in place. Check the log information:
     - TFM_SBSFU_Boot starts
     - Then it checks the static protections
     - Then it initializes the BL2 NV area (BootLoader2 Non Volatile area)
     - Then it verifies the TFM application image signatures.
     - Then it jumps to the TFM application, displaying the User Application
       Main menu.

7. Use User App

   This user application gives access to a Main menu which allows to:
   1 - Test protections
   2 - Test TFM Secure services

   1. Press 1 to enter test protections menu

      - NonSecure try to access to Secure (#1): test unauthorized access to
        Secure side from Non Secure side. A fault occurs, then reset is performed.
        Several successives unauthorized accesses to different areas are performed.

      - RDP regression (#2): For 'production mode', put the device in a state
        where RDP regresion can be performed. The RDP regression can be performed
        either by connecting STM32CubeProgrammer tool in HotPlug mode and
        performing RDP level regression to RDP level 0, or by executing the
        regression script (described in step 2).

      Return to previous menu by pressing x.

   2. Press 2 to enter test TFM menu

      This menu allows to test TFM Secure services from Non Secure side through
      PSA API usage.
      - All tests (#0):            Execute all below tests in a row
      - AES-GCM (#1):              Test AES-GCM crypto service
      - AES-CBC (#2):              Test AES-CBC crypto service
      - AES-CCM (#3):              Test AES-CCM crypto service
      - SST set UID (#4):          Test UID creation in Secure Storage area
      - SST read / check UID (#5): Test UID read / check in Secure Storage area
      - SST remove UID (#6):       Test UID removal in Secure Storage area (requires UID set before)
      - EAT (#7):                  Test Initial Attestation
      - ITS set UID (#8):          Test UID creation in Internal Trusted Storage area
      - ITS read / check UID (#9): Test UID read / check in Internal Trusted Storage area
      - ITS remove UID (#a):       Test UID removal in Internal Trusted Storage area (requires UID set before)
      - SHA224 (#b):               Test SHA224 crypto service
      - SHA256 (#c):               Test SHA256 crypto service

      Return to previous menu by pressing x.

8. Use Local loader App

   Local loader application is an immutable code, in non secure area.
   To enter local loader application, press user button (blue) during board reset.

   This application gives access to a Main menu which allows to download new firmware
   image in download area.

   It is possible to download a new TFM Secure image or TFM Non Secure image or both.
   - Download Secure Image (#2): Download encrypted signed TFM_Appli Secure image
     (ex: TFM_Appli\Binary\tfm_s_enc_sign.bin)
   - Download NonSecure Image (#3): Download encrypted signed TFM_Appli Non Secure image
     (ex: TFM_Appli\Binary\tfm_ns_enc_sign.bin)

   In both cases, send the signed binary with Tera Term by using menu:
   "File > Transfer > YMODEM > Send..."

   After the download(s), press (#1) to reset the board, or press the board
   reset button (black).
   After reset, the downloaded firmware image(s) is(are) detected, verified,
   decrypted and installed, then executed, by TFM_SBSFU_Boot.

9. Configuration

   It is possible to change TFM configuration.
   When changing configuration, all the projects must be cleaned then fully rebuild.

   1. Crypto-scheme

      By default, the crypto scheme is RSA-2048 signature, and AES-CTR-128 image
      encryption with key RSA-OAEP encrypted.
      It is possible to select another crypto-scheme, thanks to CRYPTO_SCHEME define in 
      TFM_SBSFU_Boot\Inc\mcuboot_config\mcuboot_config.h.

      #define CRYPTO_SCHEME_RSA2048    0x0 /* RSA-2048 signature,
                                              AES-CTR-128 encryption with key RSA-OAEP encrypted */
      #define CRYPTO_SCHEME_RSA3072    0x1 /* RSA-3072 signature,
                                              AES-CTR-128 encryption with key RSA-OAEP encrypted */
      #define CRYPTO_SCHEME_EC256      0x2 /* ECDSA-256 signature,
                                              AES-CTR-128 encryption with key ECIES-P256 encrypted */

      #define CRYPTO_SCHEME            CRYPTO_SCHEME_RSA2048  /* Select one of available crypto schemes */

      Note: The cryptography is accelerated thanks to the HW crypto peripherals
      of the device (PKA, AES, HASH).

   2. Local loader

      By default, local loader application is included. It is possible to remove
      it thanks to MCUBOOT_EXT_LOADER define in Linker\flash_layout.h.

      #define MCUBOOT_EXT_LOADER         /* Defined: Add external local loader application.
                                                     To enter it, press user button at reset.
                                            Undefined: No external local loader application. */

      In this case, the build of TFM_Loader must be skipped.

   3. External OSPI flash usage

      By default, the OSPI external flash is used for Secure image secondary slot
      (download), and Non Secure image primary slot (execution) and secondary slot
      (download), to maximize Non Secure image size.
      It is possible to avoid usage of external OSPI flash, and use only
      internal flash for all slots with TFM_EXTERNAL_FLASH_ENABLE define
      in Linker\flash_layout.h.

      #define TFM_EXTERNAL_FLASH_ENABLE  /* Defined: External OSPI flash used for S image secondary slot,
                                                     and NS image primary and secondary slots.
                                            Undefined: External OSPI flash not used. */

      Note: When external flash usage is disabled, an additional menu item (#3)
      'Download a new Fw Image' is present in user application. Its usage is exactly the
      same as the local loader application.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
