#!/bin/bash
cd ../file
dir=$(pwd)
echo "dir = $dir"
logfile=../test/decompress_result.txt
for file in $dir/*; do
    if [ "${file##*.}"x = "hw"x ]; then
        startTime_s=`date +%s`
        cmd="../hwzip d $file"
        echo "cmd = $cmd"
        $cmd >> $logfile
        endTime_s=`date +%s`
        decompressTime=$((endTime_s - startTime_s))
        echo "decompress $file time: $decompressTime"
    fi
done