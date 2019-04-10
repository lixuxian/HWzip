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

void LosslessCompressor::compress(std::string inputFilepath, std::string outputFilepath)
{

}

void LosslessCompressor::decompress(std::string inputFilepath, std::string outputFilepath)
{
	
}

int LosslessCompressor::compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num, std::string &lossless_str)
{
	std::cout << "LosslessCompressor::compressOneBlock()..." << std::endl;
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
					// result += ",--" + std::to_string(same_len); 
					result += "--" + std::to_string(same_len) + ","; 
				} else {
					// result += "," +distance + "-" + std::to_string(same_len);
					result += distance + "-" + std::to_string(same_len) + ",";
					distance_stored = true;
				}
			} else if (same_len == 1){ // 前面只有一个数字相同
				// result += "," + block[i-1][currentCol];
				result += block[i-1][currentCol] + ",";
			}
			same_len = 0;
			// result += "," + block[i][currentCol];
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
				// result += "," + distance + "-,";
				result += distance + "-,";
				distance_stored = true;
			} else {
				// result += ",-,";
				result += "-,";
			}
			same_len = 0;
		}
	}
	return 1;
}

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


int LosslessCompressor::compressFile_7z(std::string inputFilepath, std::string outputFilepath, int level)
{
	// 7z a outputFile inputFile -mx9
	std::cout << "7z.... input = " << inputFilepath << " output = " << outputFilepath << std::endl;
	std::string cmd_7z = "7z a " + outputFilepath + " " + inputFilepath + " -mx" + std::to_string(level);
	system(cmd_7z.c_str());
	return 1;
}

int LosslessCompressor::compressFile_bz2(std::string inputFilepath, int level)
{
	std::string cmd_bz2 = "bzip2 -kv " + inputFilepath + " -" + std::to_string(level);
	system(cmd_bz2.c_str());
	return 1;
}

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

int LosslessCompressor::compressFile_ppmd(std::string inputFilepath, std::string outputFilepath)
{
	ppmd_compress(inputFilepath.c_str(), outputFilepath.c_str());
	return 1;
}

int LosslessCompressor::decompressFile_ppmd(std::string inputFilepath, std::string outputFilepath)
{
	ppmd_decompress(inputFilepath.c_str(), outputFilepath.c_str());
	return 1;
}


int LosslessCompressor::decompressFile_7z(std::string inputFilepath)
{
	std::string de_cmd_7z = "7z x " + inputFilepath;
	system(de_cmd_7z.c_str());
	return 1;
}

int LosslessCompressor::decompressFile_bz2(std::string inputFilepath)
{
	std::string de_cmd_bz2 = "bzip2 -d " + inputFilepath;
	system(de_cmd_bz2.c_str());
	return 1;
}

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
