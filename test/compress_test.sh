#!/bin/bash
dir=../file
pw_err=0.05
avg_err=0.05
logfile=./compress_result.txt
for file in $dir/*; do
    startTime_s=`date +%s`
    cmd="../hwzip c $file $pw_err $avg_err"
    echo "cmd = $cmd"
    $cmd >> $logfile
    endTime_s=`date +%s`
    compressTime=$((endTime_s - startTime_s))
    echo "compress $file time: $compressTime"
done