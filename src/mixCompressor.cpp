#include "mixCompressor.h"
#include "utils.h"
#include <cstdio>

MixCompressor::MixCompressor(double rel_err, double avg_err, 
	std::string input, char mode) : PW_REL_ERR_MAX(rel_err), AVG_ERR_MAX(avg_err), 
	inputFilepath(input), mode(mode)
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

int MixCompressor::getFileLines(std::string inputFilepath)
{
	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	if (!in.is_open()) {
		std::cout << "MixCompressor::getFileLines(), !in.is_open()" << std::endl;
		return -1;
	}
	std::string line;
	int lines = 0;
	while(getline(in, line)){
		++lines;
	}
	in.close();
	return lines;
}

int MixCompressor::compress()
{
	// get fileLines
	fileLines = getFileLines(inputFilepath);

	tempFilepath = inputFilepath  + "_" + convertDouble(AVG_ERR_MAX) + ".tmp";
	outputFilepath = inputFilepath + "_" + convertDouble(AVG_ERR_MAX) + ".hw";

	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);
	// std::ofstream out(outputFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = new FileProcessor(&in, &tmp_out, blockSize);
	fileProcPtr->setFileLines(fileLines);

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

	deleteTmpFile();

	return 1;
}

int MixCompressor::decompress()
{
	// std::ifstream in(inputFilepath.c_str(), std::ios::in);

	// lossless decompress
	// get metadata
	// loop
	// 		decompress one block
	// end loop

	// decompressFile_7z
	if (inputFilepath.find(".hw") != std::string::npos) {
		// tempFilepath = inputFilepath.replace(inputFilepath.find(".hw"), 3, ".tmp");
		// std::cout << "tempFilepath = " << tempFilepath << std::endl;
		// outputFilepath = inputFilepath.replace(tempFilepath.find(".tmp"), 4, "");
		std::vector<std::string> filename;
		splitString(inputFilepath, filename, ".hw");

		tempFilepath = filename[0] + ".tmp";
		outputFilepath = filename[0] + "_de.csv";
		std::cout << "tempFilepath = " << tempFilepath << std::endl;
		std::cout << "outputFilepath = " << outputFilepath << std::endl;
	} else {
		std::cout << "decompress: wrong file type" << std::endl;
		exit(0);
	}
	
	// tempFilepath = inputFilepath + ".tmp";
	// outputFilepath = inputFilepath + ".hw";

	losslessCompPtr->decompressFile_7z(inputFilepath);

	std::cout << "after decompressFile_7z to " << tempFilepath << std::endl;
	
	std::ifstream tmp_in(tempFilepath.c_str(), std::ios::in);
	std::ofstream out(outputFilepath.c_str(), std::ios::out | std::ios::trunc);

	
	fileProcPtr = new FileProcessor(&tmp_in, &out, blockSize);

	fileProcPtr->getMetadata(blockSize, columnSize, fileLines, blocks);
	std::cout << "blockSize = " << blockSize << std::endl;
	std::cout << "columnSize = " << columnSize << std::endl;
	std::cout << "fileLines = " << fileLines << std::endl;
	std::cout << "blocks = " << blocks << std::endl;

	fileProcPtr->writeHeader2DecompressedFile();
	
	std::vector<std::vector<std::string> > tmp_block(blockSize, std::vector<std::string>(columnSize)); 
	block = tmp_block;

	// block.reserve(blockSize);
	// block.resize(blockSize);
	// for (std::vector<std::string> x : block)
	// {
	// 	std::cout << x.size() << std::endl;
	// }

	int block_count = 0;
	int line_num_of_block;

	while (true)
	{
		// decompress one block
		line_num_of_block = fileProcPtr->getOneDecompressedBlock(block);
		if ( line_num_of_block == -1) {
			break;
		}
		
		// write this decompressed block to output file
		fileProcPtr->writeOneBlock2DecompressedFile(block, line_num_of_block);
		++block_count;
	}

	tmp_in.close();
	out.close();

	std::cout << "finish decompress all " << block_count << " blocks" << std::endl;
	return 1;
}

void MixCompressor::run()
{
	if (mode == COMPRESS)
	{
		compress();	
	}
	else if (mode == DECOMPRESS) {
		decompress();
	}
}

int MixCompressor::deleteTmpFile()
{
	if (remove(tempFilepath.c_str()) == 0)
	{
		std::cout << "remove tmp file" << std::endl;
		return 0;
	} else {
		std::cout << "remove tmp file failed" << std::endl;
		return -1;
	}
}
