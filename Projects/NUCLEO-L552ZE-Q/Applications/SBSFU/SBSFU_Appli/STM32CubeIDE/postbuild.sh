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
sbsfu_ns_sign=$userAppBinary"/sbsfu_ns_sign.bin"
sbsfu_s_sign=$userAppBinary"/sbsfu_s_sign.bin"
sbsfu_s_enc_sign=$userAppBinary"/sbsfu_s_enc_sign.bin"
sbsfu_ns_enc_sign=$userAppBinary"/sbsfu_ns_enc_sign.bin"
# variable for image_number = 1
sbsfu_enc_sign=$userAppBinary"/sbsfu_enc_sign.bin"
sbsfu_sign=$userAppBinary"/sbsfu_sign.bin"
sbsfu=$userAppBinary"/sbsfu.bin"

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
cd $projectdir"/../../../../SBSFU_Boot/Src"
sbsfu_key_dir=`pwd`
cd $current_directory
cd $projectdir/../../../../
envdir=`pwd`
cd "$current_directory"
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

source $envdir/env.sh

if [ ! -e "$imgtool" ];then
  echo ""
  echo "!!! WARNING : imgtool has not been found on your installation."
  echo ""
  echo "  Install CubeProgrammer on your machine in default path : ~/STMicroelectronics/STM32Cube/STM32CubeProgrammer"
  echo "  or "
  echo "  Update your $envdir/env.sh with the proper path."
  echo ""
  exit 0
fi

#sign mode
if [ $crypto_scheme == 2  ]; then
mode="ec256"
fi
#keys selection
if [ $mode == "ec256" ]; then
key_s=$sbsfu_key_dir"/root-ec-p256.pem"
key_ns=$sbsfu_key_dir"/root-ec-p256_1.pem"
key_enc_pub=$sbsfu_key_dir"/enc-ec256-pub.pem"
fi
sbsfu_s=$projectdir"/../../Secure/Release/SBSFU_Appli_Secure.bin"
sbsfu_ns=$projectdir"/../../NonSecure/Release/SBSFU_Appli_NonSecure.bin"

if [ $primary_only == 1 ]; then
option="--primary-only"
else
option=""
fi

#signing
echo $signing" "$mode" "$option" image_number="$image_number

if [ $image_number == 1 ]; then
echo assemble image
command_ass=" ass -f "$sbsfu_s" -o "$image_s_size"  -i 0x0 "$sbsfu_ns" $sbsfu"
echo $imgtool $command_ass
"$imgtool" $command_ass >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi

# sign assemble binary as secure image, slot size is image_ns_size
let "image_s_size=$image_ns_size"
signing=
sbsfu_s=$sbsfu
sbsfu_s_sign=$sbsfu_sign
sbsfu_s_enc_sign=$sbsfu_enc_sign
fi

echo $signing signing
if [ "$signing" == "nonsecure" ]; then
command_sign=" sign -k "$key_ns" -e little -S "$image_ns_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$sbsfu_ns" "$sbsfu_ns_sign
else
command_sign=" sign -k "$key_s" -e little -S "$image_s_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$sbsfu_s" "$sbsfu_s_sign
fi
"$imgtool" $command_sign >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi

echo $signing encrypting
if [ "$signing" == "nonsecure" ]; then
command_enc=" sign -k "$key_ns" -E "$key_enc_pub" -e little -S "$image_ns_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$sbsfu_ns" "$sbsfu_ns_enc_sign
else
command_enc=" sign -k "$key_s" -E "$key_enc_pub" -e little -S "$image_s_size" -H 0x400 --pad-header "$option" -v "$version" -s auto --align 8 "$sbsfu_s" "$sbsfu_s_enc_sign
fi
"$imgtool" $command_enc >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
exit 0
