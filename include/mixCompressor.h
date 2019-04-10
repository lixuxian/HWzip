#pragma once

#include "lossyComp.h"
#include "losslessComp.h"
#include "fileProcess.h"
#include <memory>
#include <fstream>
#include <string>
#include <vector>

enum Mode
{
	COMPRESS = 1,
	DECOMPRESS
};


class MixCompressor
{
public:
	MixCompressor(double rel_err, double avg_err, std::string input, char mode);
	virtual ~MixCompressor();

	int compress();
	int decompress();
	
	void run();

	int deleteTmpFile();

private:
	// LossyCompressor *lossyComp;
	// LosslessCompressor *losslessComp;
	// FileProcessor *fileProc;

	int getFileLines(std::string inputFilepath);

	LossyCompressor *lossyCompPtr;
	LosslessCompressor *losslessCompPtr;
	FileProcessor *fileProcPtr;

	std::vector<std::vector<std::string> > block; 


	const double PW_REL_ERR_MAX;
	const double AVG_ERR_MAX;

	std::string inputFilepath;
	std::string outputFilepath;
	std::string tempFilepath;

	char mode;
	int blockSize;
	int columnSize;
	int fileLines;
	int blocks;

};