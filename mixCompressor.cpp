#include "mixCompressor.h"

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
		std::ifstream in(inputFilepath.c_str(), std::ios::in);
		std::ofstream out(outputFilepath.c_str(), std::ios::out | std::ios::trunc);
		fileProcPtr = new FileProcessor(&in, &out, blockSize);

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
				fileProcPtr->writeOneBlock2Tempfile(block, line_num_of_block);

			}
			else
			{
				std::cout << "finish process block " << block_count << std::endl;
				break;
			}
			// std::cout << "block_count = " << block_count << std::endl;
		}

		in.close();
		out.close();
	}
	
	// lossy compress the block

	// if mode == DECOMRPESS
}