#!/bin/bash
cd ../file
dir=$(pwd)
echo "dir = $dir"
logfile=../test/decompress_result.txt

size() {
    echo $(ls -l $1 | awk '{print $5}')
}

OS=$(uname -s)
IS_MAC=$(echo $OS | grep "Darwin")
if [ "$IS_MAC" != "" ];then
    BIN="../bin/hwzip-mac"
fi
# echo "IS_MAC = $IS_MAC"
IS_LINUX=$(echo $OS | grep "Linux")
if [ "$IS_LINUX" != "" ];then
    BIN="../bin/hwzip"
fi

for file in $dir/*; do
    if [ "${file##*.}"x = "hw"x ]; then
        filesize=$(size $file)
        echo "file size = $filesize bytes"  # get file size

        startTime_s=`date +%s`
        cmd="$BIN d $file"
        echo "cmd = $cmd"
        $cmd >> $logfile
        endTime_s=`date +%s`
        decompressTime=$((endTime_s - startTime_s))
        if [ $decompressTime == "0" ]; then
            echo "decompress time = 0"
            decompressTime=1;
        fi
        decompress_speed=$[ $filesize/$decompressTime ]
        echo "decompress $file time: $decompressTime, speed: $decompress_speed bps"
    fi
done