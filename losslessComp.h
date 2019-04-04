#pragma once

#include "baseComp.h"

class LosslessCompressor : public BaseCompressor
{
public:
	LosslessCompressor();
	~LosslessCompressor();

	void compress(std::string inputFilepath, std::string outputFilepath);
	void decompress(std::string inputFilepath, std::string outputFilepath);

	int compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num);

	int compressFile_7z(std::string inputFilepath, std::string outputFilepath, int level);

	int compressFile_bz2(std::string inputFilepath, int level);

	int compressFile_paq9a(std::string inputFilepath, std::string outputFilepath);
	
	int decompressFile_7z(std::string inputFilepath, int level);

	int decompressFile_bz2(std::string inputFilepath, int level);

	int decompressFile_paq9a(std::string inputFilepath, std::string outputFilepath);
	
};
