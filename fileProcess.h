#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class FileProcessor
{
public:
	FileProcessor(std::ifstream *in, std::ofstream *out, int blockSize);
	~FileProcessor();

	int initWork(); // store head、time and ID

	int storeHeader();
	int storeTimeAndId();

	int getOneBlock(std::vector<std::vector<std::string> > &block);
	int writeOneBlock2Tempfile(std::vector<std::vector<std::string> > &block, int line_num);

	std::ofstream *out;
	std::ifstream *in;

private:
	std::string inputFilepath;
	std::string outputFilepath;
	std::string tempFilepath;
	int blockLines;


	
};
