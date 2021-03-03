#!/bin/bash - 
# arg1 is the build directory
# arg2 is operation
projectdir=$1
operation=$2
binarydir=$projectdir"/../../Binary"
loader_ns=$projectdir"/TFM_Loader.bin"
loader=$binarydir"/loader.bin"
loader_size=0x6000
current_directory=`pwd`
echo $current_directory
cd $projectdir"/../../../../../../../Middlewares/Third_Party/mcuboot"
basedir=`pwd`
cd $current_directory
#Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $binarydir ]; then 
echo "create Binary dir"
mkdir $binarydir
fi
imgtool=$basedir"/scripts/dist/imgtool/imgtool.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [   -e "$imgtool" ]; then
#line for window executable
echo Postbuild with windows executable
cmd=""
else
#line for python
echo Postbuild with python script
imgtool=$basedir"/scripts/imgtool.py"
cmd="python"
fi


#loader without secure part
command=$cmd" "$imgtool" ass  -i "$loader_size" "$loader_ns" "$loader
$command  >> $projectdir"/output.txt"
ret=$?

if [ $ret == 0 ]; then
echo $message
# uncomment these lines to generate version for anti roll back test

#end of generate version for anti roll back test
else
echo "postbuild.sh failed"
fi
