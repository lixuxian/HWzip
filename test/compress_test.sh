#!/bin/bash
cd ../file
dir=$(pwd)
pw_err=$1
avg_err=$2
logfile=../test/compress_result.txt

size() {
    echo $(ls -l $1 | awk '{print $5}')
}

for file in $dir/*; do
    if [ "${file##*.}"x = "csv"x ]; then  # only compress *.csv file
        echo $file
        filesize=$(size $file)
        echo "file size = $filesize bytes"  # get file size

        startTime_s=`date +%s`
        cmd="../hwzip c $file $pw_err $avg_err"
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
