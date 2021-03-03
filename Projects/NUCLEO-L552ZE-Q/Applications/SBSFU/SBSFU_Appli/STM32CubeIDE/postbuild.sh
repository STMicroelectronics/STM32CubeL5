#!/bin/bash - 
# arg1 is the build directory
# arg2 is the version
# arg3 is the binary type (1 nonsecure, 2 secure)
projectdir=$1
version=$2
signing=$3
userAppBinary=$projectdir"/../../../Binary"

nsclib_destination=$projectdir"/../../../Secure_nsclib/secure_nsclib.o"
nsclib_source=$projectdir"/secure_nsclib.o"
tfm_ns_sign=$userAppBinary"/sbsfu_ns_sign.bin"
tfm_s_sign=$userAppBinary"/sbsfu_s_sign.bin"
tfm_s_enc_sign=$userAppBinary"/sbsfu_s_enc_sign.bin"
tfm_ns_enc_sign=$userAppBinary"/sbsfu_ns_enc_sign.bin"
# variable for image_number = 1
tfm_enc_sign=$userAppBinary"/sbsfu_enc_sign.bin"
tfm_sign=$userAppBinary"/sbsfu_sign.bin"
tfm=$userAppBinary"/sbsfu.bin"

# field updated with sbsfu_boot postbuild

# image_ns_size is the slot image size if image_number is 1
image_ns_size=0x6b000
image_s_size=0x8000
primary_only=1
image_number=1
crypto_scheme=0
# end of updated field

current_directory=`pwd`
echo $current_directory
cd $projectdir"/../../../../../../../../Middlewares/Third_Party/trustedfirmware/bl2/ext/mcuboot"
tfm_mcuboot_dir=`pwd`
cd $current_directory
cd $projectdir"/../../../../../../../../Middlewares/Third_Party/mcuboot"
mcuboot_dir=`pwd`
cd $current_directory
#Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $userAppBinary ]; then 
mkdir $userAppBinary
fi
if [ $signing == "secure" ]; then
command="cp "$nsclib_source" "$nsclib_destination
$command
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
if [ $image_number == 1 ]; then
# when image number is 1, image is assemble and sign during non secure postbuild
exit 0
fi
fi

imgtool=$mcuboot_dir"/scripts/dist/imgtool/imgtool.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$imgtool" ]; then
#line for window executable
echo Postbuild with windows executable
python=""
else
#line for python
echo Postbuild with python script
imgtool=$mcuboot_dir"/scripts/imgtool.py"
python="python "
fi

#sign mode
if [ $crypto_scheme == 0  ]; then
mode="rsa2048"
fi
if [ $crypto_scheme == 1  ]; then
mode="rsa3072"
fi
if [ $crypto_scheme == 2  ]; then
mode="ec256"
fi
#keys selection
if [ $mode == "rsa2048" ]; then
key_s=$tfm_mcuboot_dir"/root-rsa-2048.pem"
key_ns=$tfm_mcuboot_dir"/root-rsa-2048_1.pem"
key_enc_pub=$tfm_mcuboot_dir"/enc-rsa2048-pub.pem"
fi
if [ $mode == "rsa3072" ]; then
key_s=$tfm_mcuboot_dir"/root-rsa-3072.pem"
key_ns=$tfm_mcuboot_dir"/root-rsa-3072_1.pem"
key_enc_pub=$tfm_mcuboot_dir"/enc-rsa2048-pub.pem"
fi
if [ $mode == "ec256" ]; then
key_s=$tfm_mcuboot_dir"/root-ec-p256.pem"
key_ns=$tfm_mcuboot_dir"/root-ec-p256_1.pem"
key_enc_pub=$tfm_mcuboot_dir"/enc-ec256-pub.pem"
fi
tfm_s=$projectdir"/../../Secure/Release/SBSFU_Appli_Secure.bin"
tfm_ns=$projectdir"/../../NonSecure/Release/SBSFU_Appli_NonSecure.bin"

if [ $primary_only == 1 ]; then
option="--primary-only"
else
option=""
fi

#signing
echo $signing" "$mode" "$option" image_number="$image_number

if [ $image_number == 1 ]; then
echo assemble image
command_ass=$python$imgtool" ass -f "$tfm_s" -o "$image_s_size"  -i 0x0 "$tfm_ns" "$tfm
echo $command_ass
$command_ass >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
# sign assemble binary as secure image, slot size is image_ns_size
let "image_s_size=$image_ns_size"
signing=
tfm_s=$tfm
tfm_s_sign=$tfm_sign
tfm_s_enc_sign=$tfm_enc_sign
fi

echo $signing signing
if [ "$signing" == "nonsecure" ]; then
command_sign=$python$imgtool" sign -k "$key_ns" -e little -S "$image_ns_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$tfm_ns" "$tfm_ns_sign
else
command_sign=$python$imgtool" sign -k "$key_s" -e little -S "$image_s_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$tfm_s" "$tfm_s_sign
fi
$command_sign >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi

echo $signing encrypting
if [ "$signing" == "nonsecure" ]; then
command_enc=$python$imgtool" sign -k "$key_ns" -E "$key_enc_pub" -e little -S "$image_ns_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$tfm_ns" "$tfm_ns_enc_sign
else
command_enc=$python$imgtool" sign -k "$key_s" -E "$key_enc_pub" -e little -S "$image_s_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$tfm_s" "$tfm_s_enc_sign
fi
$command_enc >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
exit 0
