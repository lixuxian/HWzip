#include "mixCompressor.h"
#include "utils.h"
#include "lossyComp.h"
#include "losslessComp.h"
#include "fileProcess.h"
#include <cstdio>
#include <ctime>
#include <iostream>

/**
 * @description: 构造函数，根据参数进行相应的初始化
 * @param rel_err 最大相对误差
 * @param avg_err 最大平均误差
 * @param input 输入文件
 * @param mode 运行模式，压缩为COMPRESS，解压为DECOMPRESS
 */
MixCompressor::MixCompressor(double rel_err, double avg_err, 
	std::string input, char mode) : PW_REL_ERR_MAX(rel_err), AVG_ERR_MAX(avg_err), 
	inputFilepath(input), mode(mode)
{
	// lossyCompPtr = new LossyCompressor(rel_err, avg_err);
	// losslessCompPtr = new LosslessCompressor();
	lossyCompPtr = std::make_shared<LossyCompressor>(rel_err, avg_err);
	losslessCompPtr = std::make_shared<LosslessCompressor>();

	blockSize = 3000;
	columnSize = 0;
}

/**
 * @description: 析构函数，清理指针和block
 */
MixCompressor::~MixCompressor()
{
	block.clear();
	for (std::vector<std::string> x : block)
	{
		x.clear();
		x.shrink_to_fit();
	}
	block.shrink_to_fit();

	// std::cout << "~MixCompressor(): block.size() = " << block.size() << std::endl;
}

/**
 * @description: 获取文件的行数，用于计算文件块数，及最后一块的行数
 * @param inputFilepath 输入文件
 * @return: int 大于0表示文件行数，-1表示读文件出错
 */
int MixCompressor::getFileLines(std::string inputFilepath)
{
	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	if (!in.is_open()) {
		std::cout << "MixCompressor::getFileLines(), !in.is_open() " << inputFilepath << std::endl;
		return -1;
	}
	std::string line;
	int lines = 0;
	while(std::getline(in, line)){
		++lines;
	}
	in.close();
	return lines;
}

/**
 * @description: 旧版本的压缩接口，采用写入中间文件的方式
 * @param
 * @return: 
 */
int MixCompressor::compress_ppmd()
{
	// TODO 
	// 封装成一个函数，用于检测文件后缀名
	std::string suffix = inputFilepath.substr(inputFilepath.length()-4);
	if (suffix.compare(".csv") != 0)
	{
		std::cout << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	
	// get fileLines
	std::cout << "inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		std::cout << "fileLines = " << fileLines << std::endl;
		exit(1);
	}
	
	blocks = (fileLines - 1) / blockSize + (((fileLines - 1) % blockSize) != 0);

	std::string avg_err_str;
	convertDouble(AVG_ERR_MAX, avg_err_str);
	tempFilepath = inputFilepath  + "_" + avg_err_str + ".tmp";
	outputFilepath = inputFilepath + "_" + avg_err_str + ".hw";

	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = std::make_shared<FileProcessor>(&in, &tmp_out, blockSize);
	fileProcPtr->setFileLines(fileLines);

	columnSize = fileProcPtr->initWork_old();
	std::cout << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;

	while (true)
	{
		block.reserve(columnSize);

		line_num_of_block = fileProcPtr->getOneBlock(block);

		if (line_num_of_block > 0)
		{
			++block_count;
			// 有损处理一个block的文件，直接修改block中的数据
			lossyCompPtr->compressOneBlock(block, line_num_of_block);

			std::string lossless_str;
			// 无损处理一个block的文件，结果存放于lossless_str
			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			// 将处理后的block写入中间文件中，有待后续无损压缩编码
			fileProcPtr->writeOneBlock2Tempfile(lossless_str);
			{
				std::string t;
				t.swap(lossless_str);
			}
			std::cout << "\rlossy processing... " << std::fixed << (double)block_count / blocks * 100 << " %";
			std::cout.flush();
		}
		else
		{
			std::cout << "\nlossy process " << block_count << " blocks\n"<< std::endl;
			break;
		}
		for(std::vector<std::string> x : block)
		{
			for (std::string s : x)
			{
				s.clear();
				std::string().swap(s);
			}
			x.clear();
			std::vector<std::string>().swap(x);
		}
		block.clear();
		block.shrink_to_fit();
	}


	in.close();
	tmp_out.close();

	// std::cout << "lossless encoding, paq9a algorithm..." << std::endl;
	// losslessCompPtr->compressFile_paq9a(tempFilepath, outputFilepath);
	// std::cout << "finish compressFile_paq9a" << std::endl;

	std::cout << "lossless encoding, ppmd algorithm..." << std::endl;
	losslessCompPtr->compressFile_ppmd(tempFilepath, outputFilepath);
	std::cout << "finish compressFile_ppmd" << std::endl;

	deleteTmpFile(tempFilepath);

	return 1;
}

/**
 * @description: 对输入文件进行压缩
 * @param
 * @return: int 1表示正常
 */
int MixCompressor::compress()
{
	std::string suffix = inputFilepath.substr(inputFilepath.length()-4);
	if (suffix.compare(".csv") != 0)
	{
		std::cout << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	
	// get fileLines
	std::cout << "inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		std::cout << "fileLines = " << fileLines << std::endl;
		exit(1);
	}
	
	blocks = (fileLines - 1) / blockSize + (((fileLines - 1) % blockSize) != 0);

	std::string avg_err_str;
	convertDouble(AVG_ERR_MAX, avg_err_str);
	tempFilepath = inputFilepath  + "_" + avg_err_str + ".tmp";
	outputFilepath = inputFilepath + "_" + avg_err_str + ".hw";

	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = std::make_shared<FileProcessor>(&in, &tmp_out, blockSize);
	fileProcPtr->setFileLines(fileLines);

	std::string metadatas;
	std::string header;
	columnSize = fileProcPtr->initWork(metadatas, header);
	std::cout << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;

	losslessCompPtr->compress_init_paq9a(tempFilepath, outputFilepath);
	losslessCompPtr->compress_str_paq9a(metadatas);
	losslessCompPtr->compress_str_paq9a(header);

	std::string lossless_str = "";

	while (true)
	{
		block.reserve(columnSize);
		clock_t s = clock();
		line_num_of_block = fileProcPtr->getOneBlock(block);

		if (line_num_of_block > 0)
		{
			++block_count;
			lossyCompPtr->compressOneBlock(block, line_num_of_block);

			// std::string lossless_str;
			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			// TODO 目前是写入中间文件，改成直接无损压缩lossless_str（从内存压缩，减少io）
			// fileProcPtr->writeOneBlock2Tempfile(lossless_str);
			clock_t e = clock();
			std::cout << "losslessCompPtr->compressOneBlock() time = " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;
			if (lossless_str.length() >= 1024*1024)
			{
				losslessCompPtr->compress_str_paq9a(lossless_str);
				{
					std::string t;
					t.swap(lossless_str);
				}
				lossless_str = "";
			}

			
			std::cout << "\rlossy processing... " << std::fixed << (double)block_count / blocks * 100 << " %";
			std::cout.flush();
		}
		else
		{
			std::cout << "\nlossy process " << block_count << " blocks\n"<< std::endl;
			break;
		}
		for(std::vector<std::string> x : block)
		{
			for (std::string s : x)
			{
				s.clear();
				std::string().swap(s);
			}
			x.clear();
			std::vector<std::string>().swap(x);
		}
		block.clear();
		block.shrink_to_fit();
	}

	if (lossless_str.length() > 0)
	{
		losslessCompPtr->compress_str_paq9a(lossless_str);
		{
			std::string t;
			t.swap(lossless_str);
		}
		lossless_str = "";
	}
	losslessCompPtr->compress_paq9a_end();

	in.close();
	tmp_out.close();

	// std::cout << "lossless encoding, paq9a algorithm..." << std::endl;
	// losslessCompPtr->compressFile_paq9a(tempFilepath, outputFilepath);
	// std::cout << "finish compressFile_paq9a" << std::endl;

	deleteTmpFile(tempFilepath); // 实际上是个空文件

	return 1;
}

/**
 * @description: 对输入文件进行解压
 * @param 
 * @return: int 1表示正常 
 */
int MixCompressor::decompress()
{
	// std::ifstream in(inputFilepath.c_str(), std::ios::in);

	// lossless decompress
	// get metadata
	// loop
	// 		decompress one block
	// end loop
	std::string suffix = inputFilepath.substr(inputFilepath.length()-3);
	if (suffix.compare(".hw") == 0) {
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


	// std::cout << "lossless decoding, PPMD algorithm" << std::endl;
	// losslessCompPtr->decompressFile_ppmd(inputFilepath, tempFilepath);


	std::cout << "lossless decoding, paq9a algorithm" << std::endl;
	losslessCompPtr->decompressFile_paq9a(inputFilepath, tempFilepath);
	std::cout << "decompress file to " << tempFilepath << std::endl;
	
	std::ifstream tmp_in(tempFilepath.c_str(), std::ios::in);
	std::ofstream out(outputFilepath.c_str(), std::ios::out | std::ios::trunc);

	// fileProcPtr = new FileProcessor(&tmp_in, &out, blockSize);
	fileProcPtr = std::make_shared<FileProcessor>(&tmp_in, &out, blockSize);

	fileProcPtr->getMetadata(blockSize, columnSize, fileLines, blocks);
	std::cout << "blockSize = " << blockSize << std::endl;
	std::cout << "columnSize = " << columnSize << std::endl;
	std::cout << "fileLines = " << fileLines << std::endl;
	std::cout << "blocks = " << blocks << std::endl;

	fileProcPtr->writeHeader2DecompressedFile();
	
	std::vector<std::vector<std::string> > tmp_block(blockSize, std::vector<std::string>(columnSize)); 
	block = tmp_block;

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
		std::cout << "\rlossy processing... " << std::fixed << (double)block_count / blocks * 100 << " %";
		std::cout.flush();
	}

	tmp_in.close();
	out.close();

	deleteTmpFile(tempFilepath);
	std::cout << "\nfinish decompress all " << block_count << " blocks" << std::endl;
	std::cout << "decompress file " << inputFilepath << " to file " << outputFilepath << std::endl;
	return 1;
}

/**
 * @description: 压缩器/解压器运行入口，根据mode对文件进行压缩或解压
 * @param
 * @return: void
 */
void MixCompressor::run()
{
	if (mode == COMPRESS)
	{
		clock_t startTime = clock();
		//compress_ppmd();
		compress();
		clock_t endTime = clock();
		std::cout << "compress total time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << std::endl;
	}
	else if (mode == DECOMPRESS) {
		clock_t startTime = clock();
		decompress();
		clock_t endTime = clock();
		std::cout << "decompress total time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << std::endl;
	}
}

/**
 * @description: 删除中间文件
 * @param 
 * @return: int 1表示删除成功，-1表示删除失败
 */
int MixCompressor::deleteTmpFile(std::string &tmpfile)
{
	if (remove(tmpfile.c_str()) == 0)
	{
		return 0;
	} else {
		std::cout << "\nremove tmp file failed" << std::endl;
		return -1;
	}
}
