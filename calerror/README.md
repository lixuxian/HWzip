#误差测算程序

##使用方式

**编译**
执行`make`进行编译，linux中可执行程序名为"cal_error"，macOS中名为"cal_error_mac"。

**命令**
`./cal_error compressed_file decompressed_file max_rel_err max_avg_err`
"compress_file"为原csv文件，"decompressed_file"为解压文件，"max_rel_err"为最大相对误差，"max_avg_err"最大平均误差(按每列指标进行计算)。

##说明

程序输出每列的列号、平均误差、最大平均误差。若有超过误差阈值的部分，将单独输出。