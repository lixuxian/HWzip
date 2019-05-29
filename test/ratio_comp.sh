#!/bin/bash
cd ..
dir=$(pwd)/file
echo "dir = $dir"


size() {
    echo $(ls -l $1 | awk '{print $5}')
}

for file in $dir/*; do
    if [ "${file##*.}"x = "csv"x ]; then  # only compress *.csv file
        echo $file
        filesize=$(size $file)
        echo "file size = $filesize bytes"  # g

        hw_file=${file}_0.05.hw
        bz2_file=${file}.bz2

        hw_file_size=$(size $hw_file)
        bz2_file_size=$(size $bz2_file)

        compress_ratio=`echo "scale=6; $hw_file_size/$bz2_file_size*100" | bc`

        echo "compress ratio = $hw_file_size / $bz2_file_size = $compress_ratio%"
        echo ""
    fi
done
