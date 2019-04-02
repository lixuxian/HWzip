#pragma once

#include <string>

class BaseCompressor
{
public:
	BaseCompressor()
	{

	}
	~BaseCompressor()
	{

	}
	virtual void compress(std::string inputFilepath, std::string outputFilepath) = 0;
	virtual void decompress(std::string inputFilepath, std::string outputFilepath) = 0;
	
};