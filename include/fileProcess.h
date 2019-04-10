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
	int writeOneBlock2Tempfile(std::string &lossless_str);
	void initBlock(std::vector<std::vector<std::string> > &block);

	void getMetadata(int& blockSize, int& columnSize, int& lines, int& blocks);

	void setFileLines(int lines);

	int getOneDecompressedBlock(std::vector<std::vector<std::string> > &block);

	int writeOneBlock2DecompressedFile(std::vector<std::vector<std::string> > &block, int lines);
	int writeHeader2DecompressedFile();

	std::ifstream *in;
	std::ofstream *out;

private:
	std::string inputFilepath;
	std::string outputFilepath;
	// std::string tempFilepath;
	int blockLines;
	int blockCount;
	int fileLines;
	int columnSize;

	int decompress_block_count;
	
};
