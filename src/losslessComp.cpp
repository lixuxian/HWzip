#include "losslessComp.h"
#include "paq9a.h"
#include "ppmd.h"
#include <cstdio>
#include <iostream>
#include <vector>

LosslessCompressor::LosslessCompressor() : simThreshold(0.90), simRange(12)
{
	
}

LosslessCompressor::~LosslessCompressor()
{

}

/**
 * @description: 暂未使用
 * @param inputFilepath 输入文件
 * @param outputFilepaht 输出文件
 * @return: void
 */
void LosslessCompressor::compress(std::string inputFilepath, std::string outputFilepath)
{

}

/**
 * @description: 暂未使用
 * @param inputFilepath 输入文件
 * @param outputFilepaht 输出文件
 * @return: void
 */
void LosslessCompressor::decompress(std::string inputFilepath, std::string outputFilepath)
{
	
}

/**
 * @description: 对一个块的数据进行无损压缩，结果存入lossless_str中
 * @param block 待压缩的一块数据
 * @param line_num block中的文件行数
 * @param lossless_str 对block的无损压缩结果，稍后存入中间文件
 * @return: int 1表示正常
 */
int LosslessCompressor::compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num, std::string &lossless_str)
{
	// std::cout << "LosslessCompressor::compressOneBlock()..." << std::endl;
	int rowN = line_num;
	if (rowN <= 0)
	{
		perror("rowN <= 0");
		exit(0);
	}
	int colN = block[0].size();

	lossless_str = "";
	// first three col are metadatas
	for (int col = 0; col < colN; ++col)
	{
		std::string col_str = "";
		double similarity;
		int simCol = chooseSimilarColumn(block, line_num, col, similarity);
		if (simCol == -1)
		{
			/* no similar col */
			std::string pre_data = block[0][col];
			col_str += pre_data;
			for (int line = 1; line < line_num; ++line)
			{
				std::string currentData = block[line][col];
				// std::cout << "currentData = " << currentData << std::endl;
				if (pre_data.compare(currentData) == 0) // 相等
				{
					col_str += ",";
				} else {
					col_str += "," + currentData;
					pre_data = currentData;
				}
			}
			col_str += ",";
		}
		else
		{
			/* there is a similar column */
			if (similarity == 1.0) {
				// if (col != 0) {
				// 	col_str += ",";
				// }
				col_str += std::to_string(col - simCol - 1) + "-,";
			}
			else
			{
				createSimilarString(block, line_num, simCol, col, col_str);
			}
		}
		lossless_str += col_str;
	}
	return 1;
}

/**
 * @description: 根据相似列，构造本列的字符串
 * @param block 经过有损处理后的一块数据
 * @param line_num 该block中的文件行数
 * @param simCol 相似列的列索引
 * @param currentCol 当前列的列索引
 * @param result 根据相似列构造出的当前列的字符串
 * @return: int 1表示正常
 */
int LosslessCompressor::createSimilarString(std::vector<std::vector<std::string> > &block, int line_num, int simCol, int currentCol, std::string &result)
{
	result = "";
	int same_len = 0;
	bool distance_stored = false;
	std::string distance = std::to_string(currentCol - simCol - 1);
	for (int i = 0; i < line_num; ++i)
	{
		if (block[i][simCol].compare(block[i][currentCol]) != 0)
		{
			/* 不相等 */
			if (same_len > 1) // 前面有same_len个数字相同
			{
				if(distance_stored)
				{
					result += "--" + std::to_string(same_len) + ","; 
				} else {
					result += distance + "-" + std::to_string(same_len) + ",";
					distance_stored = true;
				}
			} else if (same_len == 1){ // 前面只有一个数字相同
				result += block[i-1][currentCol] + ",";
			}
			same_len = 0;
			result += block[i][currentCol] + ",";
			continue;
		} else {
			/* 相等 */
			++same_len;
		}

		if (i == line_num - 1)
		{
			if (!distance_stored)
			{
				result += distance + "-,";
				distance_stored = true;
			} else {
				result += "-,";
			}
			same_len = 0;
		}
	}
	return 1;
}

/**
 * @description: 从block的列中，选择一个和当前列相似的列
 * 				(选择范围由simRange控制，减少计算量；相似度大于阈值simThreshold才算相似)
 * @param block 经过有损处理后的一块数据
 * @param line_num 该block中的文件行数
 * @param currentCol 当前列的列索引
 * @param similarity 当前列和相似列的相似度(即相同数据所占的比例，如0.9)
 * @return: int 大于0表示相似列的索引，-1表示没有相似列
 */
int LosslessCompressor::chooseSimilarColumn(std::vector<std::vector<std::string> > &block, int line_num, int currentCol, double &similarity)
{
	int firstColIndex = 3;
	int end = currentCol;
	int start = currentCol - simRange >= firstColIndex ? currentCol - simRange : firstColIndex;

	double maxSimilarity = 0.0;
	int maxSimilarityIndex = -1;

	for (int i = start; i < end; ++i)
	{
		double sim = getSimilarity(block, line_num, currentCol, i);
		if (sim >= simThreshold && sim > maxSimilarity)
		{
			maxSimilarity = sim;
			maxSimilarityIndex = i;
		}
	}
	similarity = maxSimilarity;
	return maxSimilarityIndex;
}

/**
 * @description: 计算block中两个列的相似度
 * @param block 一块数据
 * @param line_num 块中的文件行数
 * @param c1 第一个列索引
 * @param c2 第二个列索引
 * @return: double 两个列的相似度
 */
double LosslessCompressor::getSimilarity(std::vector<std::vector<std::string> > &block, int line_num, int c1, int c2)
{
	unsigned int same_count = 0;

	for (int i = 0; i < line_num; ++i)
	{
		if (block[i][c1].compare(block[i][c2]) == 0)
		{
			++same_count;
		}
	}

	return (double)same_count / line_num;
}

/**
 * @description: 最后一步的无损压缩，7z的无损压缩接口，目前通过调用命令的方式实现
 * @param inputFilepath 输入文件路径(即中间文件) 
 * @param outputFilepath 输出文件路径(即最终的压缩文件)
 * @param level 压缩级别(-1 to -9)
 * @return: int 1表示正常
 */
int LosslessCompressor::compressFile_7z(std::string inputFilepath, std::string outputFilepath, int level)
{
	// 7z a outputFile inputFile -mx9
	std::cout << "7z.... input = " << inputFilepath << " output = " << outputFilepath << std::endl;
	std::string cmd_7z = "7z a " + outputFilepath + " " + inputFilepath + " -mx" + std::to_string(level);
	system(cmd_7z.c_str());
	return 1;
}

/**
 * @description: 最后一步的无损压缩，Bzip2的无损压缩接口，目前通过调用命令的方式实现
 * @param inputFilepath 输入文件路径(即中间文件)
 * @param level 压缩级别(-1 to -9)
 * @return: int 1表示正常
 */
int LosslessCompressor::compressFile_bz2(std::string inputFilepath, int level)
{
	std::string cmd_bz2 = "bzip2 -kv " + inputFilepath + " -" + std::to_string(level);
	system(cmd_bz2.c_str());
	return 1;
}

/**
 * @description: 最后一步的无损压缩，paq9a的无损压缩接口(上下文混合模型压缩算法)
 * @param inputFilepath 输入文件路径(即中间文件) 
 * @param outputFilepath 输出文件路径(即最终的压缩文件)
 * @return: int 1表示正常
 */
int LosslessCompressor::compressFile_paq9a(std::string inputFilepath, std::string outputFilepath)
{
	FILE *in = fopen(inputFilepath.c_str(), "rb");
	if (!in)
	{
		std::cout << "LosslessCompressor::compressFile_paq9a(), FILE *in == NULL" << std::endl;
		exit(0);
	}
	FILE *out = fopen(outputFilepath.c_str(), "wb");
	if (!out)
	{
		std::cout << "LosslessCompressor::compressFile_paq9a(), FILE *out == NULL" << std::endl;
		exit(0);
	}
	Mode_paq m = COMPRESS;
	paq9a(in, out, m);

	fclose(in);
	fclose(out);

	return 1;
}

/**
 * @description: 最后一步的无损压缩，ppmd的无损压缩接口，其实现在静态库libppmd中
 * @param inputFilepath 输入文件路径(即中间文件) 
 * @param outputFilepath 输出文件路径(即最终的压缩文件)
 * @return: int 1表示正常
 */
int LosslessCompressor::compressFile_ppmd(std::string inputFilepath, std::string outputFilepath)
{
	ppmd_compress(inputFilepath.c_str(), outputFilepath.c_str());
	return 1;
}

/**
 * @description: ppmd的无损解压接口，其实现在静态库libppmd中
 * @param inputFilepath 输入文件路径(即压缩文件) 
 * @param outputFilepath 输出文件路径(即中间文件)
 * @return: int 1表示正常
 */
int LosslessCompressor::decompressFile_ppmd(std::string inputFilepath, std::string outputFilepath)
{
	ppmd_decompress(inputFilepath.c_str(), outputFilepath.c_str());
	return 1;
}

/**
 * @description: 7z的无损解压接口，目前通过调用命令的方式实现
 * @param inputFilepath 输入文件路径(即压缩文件)
 * @return: int 1表示正常
 */
int LosslessCompressor::decompressFile_7z(std::string inputFilepath)
{
	std::string de_cmd_7z = "7z x " + inputFilepath;
	system(de_cmd_7z.c_str());
	return 1;
}

/**
 * @description: bzip2的无损解压接口，目前通过调用命令的方式实现
 * @param inputFilepath 输入文件路径(即压缩文件)
 * @return: int 1表示正常
 */
int LosslessCompressor::decompressFile_bz2(std::string inputFilepath)
{
	std::string de_cmd_bz2 = "bzip2 -d " + inputFilepath;
	system(de_cmd_bz2.c_str());
	return 1;
}

/**
 * @description: paq9a的无损解压接口，其实现在paq9a.h中
 * @param inputFilepath 输入文件路径(即压缩文件)
 * @param outputFilepath 输出文件路径(即中间文件)
 * @return: int 1表示正常
 */
int LosslessCompressor::decompressFile_paq9a(std::string inputFilepath, std::string outputFilepath)
{
	FILE *in = fopen(inputFilepath.c_str(), "rb");
	if (!in)
	{
		std::cout << "LosslessCompressor::compressFile_paq9a(), FILE *in == NULL" << std::endl;
		exit(0);
	}
	FILE *out = fopen(outputFilepath.c_str(), "wb");
	if (!out)
	{
		std::cout << "LosslessCompressor::compressFile_paq9a(), FILE *out == NULL" << std::endl;
		exit(0);
	}
	Mode_paq m = DECOMPRESS;
	paq9a(in, out, m);

	fclose(in);
	fclose(out);

	return 1;
}
