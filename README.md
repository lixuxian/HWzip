# 无线性能数据有损压缩算法v1.1

## 编译安装
运行`make`进行编译，运行`make install`将可执行程序安装到`/usr/local/bin`目录，运行`make uninstall`卸载。

## 运行
* **压缩命令**  
`./hwzip c [input] [max_pw_rel_err] [max_avg_err]`  
**示例:**  
`./hwzip c file/7.csv 0.10 0.05`

* **解压命令**  
`./hwzip d [input]`  
**示例:**  
`./hwzip d file/7.csv.hw`  

**注意:**
解压的文件名应当以`.hw`结尾

## 测试
进入`./test`目录, 运行`./compress_test.sh`和`./decompress_test.sh`脚本.
注意：测试文件应位于项目根目录下的`file`目录中。

**压缩测试示例：**  
`./compress_test.sh  0.10  0.05`  
**解压测试示例：**  
`./decompress_tese.sh`  

## 联系方式
**mail**: <xuxianli@whu.edu.cn>
