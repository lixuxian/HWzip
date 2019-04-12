#include "mixCompressor.h"
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

/**
 * @description: 帮助信息，关于可执行程序的使用
 * @param
 * @return: int -1表示命令行参数错误
 */
int Help() {
  printf("hwzip v1.0\n");
  printf("How to use hwzip:\n");
  printf("    compress:           hwzip c [input] [max_pw_rel_err] [max_avg_err]\n");
  printf("    decompress:         hwzip d [input]\n");
  return -1;
}

/**
 * @description: 存储参数配置信息，如压缩/解压、最大相对误差、最大平均误差、输入文件路径
 */
struct Paras
{
	Mode mode;
	double MAX_PW_REL_ERR;
	double MAX_AVG_ERR;
	std::string inputFile;
	// std::string outputFile;
} paras;

/**
 * @description: 从命令行参数获取配置信息
 * @param argc 参数个数
 * @param argv 命令行的参数
 * @return: int 1表示正常，-1表示命令行参数出错
 */
int getParas(int argc, char const *argv[])
{
	// get mode
	if (argv[1][0] == 'c')
	{
		paras.mode = COMPRESS;
	}
	else if (argv[1][0] == 'd')
	{
		paras.mode = DECOMPRESS;
	}
	else
	{
		return Help();
	}
	paras.inputFile = argv[2];
	// paras.outputFile = argv[3];

	if (argc == 5)
	{
		paras.MAX_PW_REL_ERR = atof(argv[3]);
		paras.MAX_AVG_ERR = atof(argv[4]);	
		paras.MAX_PW_REL_ERR = std::min(paras.MAX_PW_REL_ERR, paras.MAX_AVG_ERR);
	}
	return 1;
}

/**
 * @description: 程序入口
 * @param argc 参数个数
 * @param argv 命令行的参数
 * @return: int -1表示出错，大于等于0表示正常
 */
int main(int argc, char const *argv[])
{
	if (argc != 5 && argc != 3)
	{
		return Help();
	}


	if (getParas(argc, argv)  == -1)
	{
		exit(-1);
	}

	MixCompressor *mixComp = new MixCompressor(paras.MAX_PW_REL_ERR, paras.MAX_AVG_ERR
		, paras.inputFile, paras.mode);

	mixComp->run();

	std::cout << "run() finish" << std::endl;
	
	delete mixComp;


	return 0;
}