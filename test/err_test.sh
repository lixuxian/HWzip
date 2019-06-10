#!/bin/bash
# 批量误差测算脚本
cd ..
dir=$(pwd)/file
echo "dir = $dir"
logfile=./test/err_resu_testlt.txt

size() {
    echo $(ls -l $1 | awk '{print $5}')
}

OS=$(uname -s)
IS_MAC=$(echo $OS | grep "Darwin")
if [ "$IS_MAC" != "" ];then
    BIN="./calerror/cal_error_mac"
fi
# echo "IS_MAC = $IS_MAC"
IS_LINUX=$(echo $OS | grep "Linux")
if [ "$IS_LINUX" != "" ];then
    BIN="./calerror/cal_error"
fi

for file in $dir/*; do
    if [ "${file##*.}"x = "hw"x ]; then

        file=${file%_0.05.hw} # 这里测试的是误差为0.05的文件
        de_file=${file}_0.05_de.csv
        file_name=${file#/Users/lixuxian/compress/HWzip/file/}
        result=${file_name}.txt
        cmd="$BIN  $file $de_file 0.1 0.05"
        echo "cmd = $cmd"
        $cmd >> $result
    fi
done