#!/bin/bash - 
# arg1 is the build directory
# arg2 is the version
# arg3 is the binary type (1 nonsecure, 2 secure)
projectdir=$1
version=$2
signing=$3
userAppBinary=$projectdir"/../../../Binary"
tfm_ns_sign=$userAppBinary"/sbsfu_ns_sign.bin"
tfm_s_sign=$userAppBinary"/sbsfu_s_sign.bin"
current_directory=`pwd`
echo $current_directory
cd $projectdir"/../../../../../../../../Middlewares/Third_Party/trustedfirmware/bl2/ext/mcuboot"
basedir=`pwd`
cd $current_directory
preprocess_ns_file=`pwd`/"image_macros_preprocessed_ns.c"
preprocess_s_file=`pwd`/"image_macros_preprocessed_s.c"
#Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $userAppBinary ]; then 
echo "create Binary dir"
mkdir $userAppBinary
fi
#line for window executeable
echo Postbuild with windows executable
imgtool=$basedir"/scripts/dist/imgtool/imgtool.exe"
assemble=$basedir"/scripts/dist/assemble/assemble.exe"
cmd=""

#line for python
#echo Postbuild with python script
#imgtool=$basedir"/scripts/impgtool.py"
#assemble=$basedir"/scripts/assemble.py"
#cmd="python"

key_s=$basedir/root-rsa-3072.pem
key_ns=$basedir/root-rsa-3072_1.pem
#to change 
tfm_ns=$projectdir"/SBSFU_Appli_NonSecure.bin"
tfm_s=$projectdir"/SBSFU_Appli_Secure.bin"
#command=$cmd" "$assemble" -l "$flashlayout" -s "$tfm_s" -n "$tfm_ns" -o "$tfm_full
#echo $command
#$command  >> $projectdir"/output.txt"
#ret=$?
#if [ $ret == 0 ]; then
#echo "assemble done"
if [ $signing == "nonsecure" ]; then
message="non secure sign done"
command=$cmd" "$imgtool" sign --layout "$preprocess_ns_file" -k "$key_ns" --public-key-format "hash" --align 1 -v "$version" -H 0x400  "$tfm_ns" "$tfm_ns_sign
elif [ $signing == "secure" ]; then
message="secure sign done"
command=$cmd" "$imgtool" sign --layout "$preprocess_s_file" -k "$key_s" --public-key-format "hash" --align 1 -v "$version" -H 0x400  "$tfm_s" "$tfm_s_sign
fi
$command  >> $projectdir"/output.txt"
ret=$?

if [ $ret == 0 ]; then
echo $message
# uncomment these lines to generate version for anti roll back test

#end of generate version for anti roll back test
else
echo "postbuild.sh failed"
fi
