#pragma once

#include "baseComp.h"

class LosslessCompressor : public BaseCompressor
{
public:
	LosslessCompressor();
	~LosslessCompressor();

	void compress(std::string inputFilepath, std::string outputFilepath);
	void decompress(std::string inputFilepath, std::string outputFilepath);
	
	
};
