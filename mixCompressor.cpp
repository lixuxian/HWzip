#include "mixCompressor.h"
#include <cstdio>

MixCompressor::MixCompressor(double rel_err, double avg_err, 
	std::string input, std::string output, char mode) : PW_REL_ERR_MAX(rel_err), AVG_ERR_MAX(avg_err), 
	inputFilepath(input), outputFilepath(output), mode(mode)
{
	// lossyComp = new LossyCompressor(rel_err, avg_err);
	// LosslessCompressor = new LosslessCompressor();
	// fileProc = new FileProcessor();
	lossyCompPtr = new LossyCompressor(rel_err, avg_err);
	losslessCompPtr = new LosslessCompressor();

	blockSize = 3000;
	columnSize = 0;

	tempFilepath = inputFilepath + ".tmp";
}

MixCompressor::~MixCompressor()
{
	if (lossyCompPtr)
	{
		delete lossyCompPtr;
	}
	if (losslessCompPtr)
	{
		delete losslessCompPtr;
	}
	if (fileProcPtr)
	{
		delete fileProcPtr;
	}
	block.clear();
	for (std::vector<std::string> x : block)
	{
		x.clear();
		x.shrink_to_fit();
	}
	block.shrink_to_fit();

	std::cout << "~MixCompressor(): block.size() = " << block.size() << std::endl;
}

int MixCompressor::compress()
{
	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);
	// std::ofstream out(outputFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = new FileProcessor(&in, &tmp_out, blockSize);

	columnSize = fileProcPtr->initWork();
	std::cout << "columnSize = " << columnSize << std::endl;

	block.resize(blockSize);
	for (std::vector<std::string> x : block)
	{
		x.resize(columnSize);
	}

	int block_count = 0;
	int line_num_of_block;

	while (true)
	{
		// std::vector<std::vector<std::string> > block; 
		// block.reserve(blockSize);
		line_num_of_block = fileProcPtr->getOneBlock(block);
		std::cout << "line_num_of_block = " << line_num_of_block << std::endl;
		if (line_num_of_block > 0)
		{
			++block_count;
			lossyCompPtr->compressOneBlock(block, line_num_of_block);
			// fileProcPtr->writeOneBlock2Tempfile(block, line_num_of_block);
	
			std::string lossless_str;
			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			fileProcPtr->writeOneBlock2Tempfile(lossless_str);
		}
		else
		{
			std::cout << "finish process block " << block_count << std::endl;
			break;
		}
		// std::cout << "block_count = " << block_count << std::endl;
	}

	// compress temp file - 7z
	in.close();
	tmp_out.close();

	losslessCompPtr->compressFile_7z(tempFilepath, outputFilepath, 9);
	std::cout << "finish compressFile_7z." << std::endl;

	// // compress temp file - bz2
	// losslessCompPtr->compressFile_bz2(tempFilepath, 9);
	// std::cout << "finish compressFile_bz2." << std::endl;

	// // compress temp file - paq9a
	// std::string outputFilepath_paq = outputFilepath + ".paq9a";
	// losslessCompPtr->compressFile_paq9a(tempFilepath, outputFilepath_paq);
	// std::cout << "finish compressFile_paq9a" << std::endl;

	// deleteTmpFile();

	return 1;
}

int MixCompressor::decompress()
{
	return 1;
}

void MixCompressor::run()
{
	// if mode = COMPRESS

	// file init work
	// get a block
	// loop: compress a block 
	// end
	if (mode == COMPRESS)
	{
		compress();	
	}
	
	// lossy compress the block

	// if mode == DECOMRPESS
}

int MixCompressor::deleteTmpFile()
{
	if (remove(tempFilepath.c_str()) == 0)
	{
		std::cout << "finish remove tmp file" << std::endl;
		return 0;
	} else {
		std::cout << "remove tmp file failed" << std::endl;
		return -1;
	}
}
