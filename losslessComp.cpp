#include "losslessComp.h"
#include "paq9a.h"
#include <cstdio>
#include <iostream>

LosslessCompressor::LosslessCompressor()
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

int LosslessCompressor::compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num)
{
	return 1;
}

int LosslessCompressor::compressFile_7z(std::string inputFilepath, std::string outputFilepath, int level)
{
	// 7z a outputFile inputFile -mx9
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

int LosslessCompressor::decompressFile_7z(std::string inputFilepath, std::string outputFilepath, int level)
{
	std::string de_cmd_7z = "7z x " + inputFilepath
}

int LosslessCompressor::decompressFile_bz2(std::string inputFilepath, int level)
{

}

int LosslessCompressor::decompressFile_paq9a(std::string inputFilepath, std::string outputFilepath)
{

}
