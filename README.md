# 无线性能数据有损压缩算法

## compile
    `make`

## run
### compress
`./hwzip c [input] [max_pw_rel_err] [max_avg_err]`
for example:
    `./hwzip c file/7.csv 0.10 0.05`
### decompress
`./hwzip d [input]`
for example:
    `./hwzip d file/7.csv.hw`
note:
    the input filename should end with ".hw"