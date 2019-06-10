#!/bin/bash
# 批量测试脚本，对file目录中的所有“.csv”文件进行压缩
cd ..
dir=$(pwd)/file
echo "dir = $dir"

BIN="./bin/hwzip"

logfile=./test/compress_test_result.txt

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
