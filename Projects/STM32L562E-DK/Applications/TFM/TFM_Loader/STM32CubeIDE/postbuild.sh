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
#line for window executeable
echo Postbuild with windows executable
cmd=""
else
#line for python
echo Postbuild with python script
imgtool=$basedir"/scripts/imgtool.py"
#determine/check python version command
cmd="python3"
$cmd --version  &> /dev/null
ret=$?
if [ $ret != 0 ]; then
  cmd="python"
  $cmd --version  &> /dev/null
  ret=$?
  if [ $ret != 0 ]; then
    echo "This script requires python 3.0 or greater"
    exit 1
  fi
  ver=$(python -V 2>&1 | sed 's/.* \([0-9]\).\([0-9]\).*/\1\2/')
  if [ "$ver" -lt "30" ]; then
    echo "This script requires python 3.0 or greater"
    exit 1
  fi
fi
fi


#loaader without secure part 
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
