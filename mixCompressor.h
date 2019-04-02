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
	MixCompressor(double rel_err, double avg_err, std::string input, std::string output, char mode);
	~MixCompressor();

	int compress();
	int decompress();
	
	void run();

private:
	// LossyCompressor *lossyComp;
	// LosslessCompressor *losslessComp;
	// FileProcessor *fileProc;

	LossyCompressor *lossyCompPtr;
	LosslessCompressor *losslessCompPtr;
	FileProcessor *fileProcPtr;

	std::vector<std::vector<std::string> > block; 


	std::string inputFilepath;
	std::string outputFilepath;

	const double PW_REL_ERR_MAX;
	const double AVG_ERR_MAX;

	char mode;
	int blockSize;
	int columnSize;

};