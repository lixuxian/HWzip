#!/bin/bash
cd ..
dir=$(pwd)/file
echo "dir = $dir"
# pw_err=$1
# avg_err=$2

# if [ "$pw_err" = "" ] || [ "$avg_err" = "" ];then
#     echo "please input max_err and avg_err"
#     echo "  for example:  ./compress_test.sh 0.10 0.05"
#     exit 1
# fi

OS=$(uname -s)
IS_MAC=$(echo $OS | grep "Darwin")
if [ "$IS_MAC" != "" ];then
    BIN="./bin/hwzip-mac"
fi
# echo "IS_MAC = $IS_MAC"
IS_LINUX=$(echo $OS | grep "Linux")
if [ "$IS_LINUX" != "" ];then
    BIN="./bin/hwzip"
fi
# echo "IS_LINUX = $IS_LINUX"
logfile=./test/compress_result.txt

size() {
    echo $(ls -l $1 | awk '{print $5}')
}

for file in $dir/*; do
    if [ "${file##*.}"x = "csv"x ]; then  # only compress *.csv file
        echo $file
        filesize=$(size $file)
        echo "file size = $filesize bytes"  # get file size

        startTime_s=`date +%s`
        cmd="$BIN c $file"
        echo "cmd = $cmd"
        $cmd >> $logfile
        endTime_s=`date +%s`
        compressTime=$((endTime_s - startTime_s))

        if [ $compressTime == "0" ]; then
            echo "compress time = 0"
            compressTime=1;
        fi
        compress_speed=$[ $filesize/$compressTime ]
        echo "compress $file time: $compressTime seconds, speed: $compress_speed bps"
    fi
done
