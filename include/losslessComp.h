#pragma once

#include "baseComp.h"

class LosslessCompressor : public BaseCompressor
{
public:
	LosslessCompressor();
	~LosslessCompressor();

	void compress(std::string inputFilepath, std::string outputFilepath);
	void decompress(std::string inputFilepath, std::string outputFilepath);

	double getSimilarity(std::vector<std::vector<std::string> > &block, int line_num, int c1, int c2);
	int chooseSimilarColumn(std::vector<std::vector<std::string> > &block, int line_num, int currentCol, double &similarity);
	int createSimilarString(std::vector<std::vector<std::string> > &block, int line_num, int simCol, int currentCol, std::string &result);


	int compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num, std::string &lossless_str);

	int compressFile_7z(std::string inputFilepath, std::string outputFilepath, int level);

	int compressFile_bz2(std::string inputFilepath, int level);

	int compressFile_paq9a(std::string inputFilepath, std::string outputFilepath);
	
	int decompressFile_7z(std::string inputFilepath);

	int decompressFile_bz2(std::string inputFilepath);

	int decompressFile_paq9a(std::string inputFilepath, std::string outputFilepath);
	
private:
	double simThreshold;
	int simRange;
};
