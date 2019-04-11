/*
 * @Description: ppmd接口头文件，其方法实现在lib/libppmd.a静态库中
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-04-09 17:25:02
 * @LastEditTime: 2019-04-11 11:42:20
 */
int ppmd_compress(const char *input, const char *output);
int ppmd_decompress(const char *input, const char *output);
