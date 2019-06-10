/*
 * @Description: 
 * @Author: lixuxian
 * @Date: 2019-05-27 14:35:03
 * @LastEditor: lixuxian
 * @LastEditTime: 2019-06-10 10:41:15
 */
#include "mixCompressor.h"
#include "utils.h"
#include "lossyComp.h"
#include "losslessComp.h"
#include "paqCompressor.h"
#include "fileProcess.h"
#include "task.h"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <mutex>
/**
 * @description: 构造函数，根据参数进行相应的初始化
 * @param rel_err 最大相对误差
 * @param avg_err 最大平均误差
 * @param input 输入文件
 * @param mode 运行模式，压缩为COMPRESS，解压为DECOMPRESS
 */
MixCompressor::MixCompressor(CompressConf &cf) : PW_REL_ERR_MAX(cf.MAX_PW_REL_ERR), AVG_ERR_MAX(cf.MAX_AVG_ERR), 
	inputFilepath(cf.paras.inputFile), mode(cf.paras.mode), blockSize(cf.blockSize)
{
	// lossyCompPtr = new LossyCompressor(rel_err, avg_err);
	// losslessCompPtr = new LosslessCompressor();
	// lossyCompPtr = std::make_shared<LossyCompressor>(PW_REL_ERR_MAX, AVG_ERR_MAX);
	lossyCompPtr = std::make_shared<LossyCompressor>(cf);
	// losslessCompPtr = std::make_shared<LosslessCompressor>();
	losslessCompPtr = std::make_shared<LosslessCompressor>(cf);
	// paqCompPtr = std::make_shared<PaqCompressor>(inputFilepath, rel_err, avg_err);

	// blockSize = 3000;
	this->setConf(cf);
	columnSize = 0;
}

/**
 * @description: 析构函数，清理指针和block
 */
MixCompressor::~MixCompressor()
{
	LOG(INFO) << "MixCompressor::~MixCompressor()" << std::endl;
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
		LOG(ERROR) << "MixCompressor::getFileLines(), !in.is_open() " << inputFilepath << std::endl;
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
	// paqCompPtr = std::make_shared<PaqCompressor>(inputFilepath, PW_REL_ERR_MAX, AVG_ERR_MAX);
	if (!isFileSuffix(inputFilepath, ".csv"))
	{
		LOG(ERROR) << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	// get fileLines
	LOG(INFO) << "compress_ppmd(), inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		LOG(ERROR) << "fileLines = " << fileLines << std::endl;
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

	std::string metadata;
	std::string header;
	std::string losslessAlgorithm = "ppmd";
	// columnSize = fileProcPtr->initWork_old();
	columnSize = fileProcPtr->initWork(losslessAlgorithm, metadata, header);
	fileProcPtr->writeOneBlock2Tempfile(metadata);
	fileProcPtr->writeOneBlock2Tempfile(header);
	LOG(INFO) << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;

	while (true)
	{
		block.reserve(columnSize);

		line_num_of_block = fileProcPtr->getOneBlock(block);

		if (line_num_of_block > 0)
		{
			++block_count;
			lossyCompPtr->compressOneBlock(block, line_num_of_block);

			std::string lossless_str;
			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			// TODO 目前是写入中间文件，改成直接无损压缩lossless_str（从内存压缩，减少io）
			fileProcPtr->writeOneBlock2Tempfile(lossless_str);
			// losslessCompPtr->compress_str_paq9a(lossless_str);

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

	std::cout << "lossless encoding, ppmd algorithm..." << std::endl;
	LOG(INFO) << "tempFilepath = " << tempFilepath << ", outputFilepath = " << outputFilepath << std::endl;
	LosslessCompressor::compressFile_ppmd(tempFilepath, outputFilepath);
	// losslessCompPtr->compressFile_ppmd("file/7.csv_0.50.tmp", "file/lxx-7.hw");
	std::cout << "finish compressFile_ppmd" << std::endl;

	if (conf.remove_tmp_file)
	{
		deleteTmpFile(tempFilepath);
	}
	return 1;
}

/**
 * @description: 对输入文件进行压缩
 * @param
 * @return: int 1表示正常
 */
int MixCompressor::compress_paq()
{
	paqCompPtr = std::make_shared<PaqCompressor>(inputFilepath, PW_REL_ERR_MAX, AVG_ERR_MAX);

	if (!isFileSuffix(inputFilepath, ".csv"))
	{
		LOG(ERROR) << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	
	// get fileLines
	std::cout << "inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		LOG(ERROR) << "fileLines = " << fileLines << std::endl;
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
	std::string losslessAlgorithm = "paq";
	columnSize = fileProcPtr->initWork(losslessAlgorithm, metadatas, header);
	std::cout << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;

	paqCompPtr->compress_str_paq9a(metadatas);
	paqCompPtr->compress_str_paq9a(header);

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

			// fileProcPtr->writeOneBlock2Tempfile(block, line_num_of_block);
			// std::string lossless_str;
			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			// TODO 目前是写入中间文件，改成直接无损压缩lossless_str（从内存压缩，减少io）
			// fileProcPtr->writeOneBlock2Tempfile(lossless_str);
			clock_t e = clock();
			LOG(INFO) << "lossyCompPtr->compressOneBlock() time = " << (double)(e - s)/CLOCKS_PER_SEC << std::endl;
			if (lossless_str.length() >= 1024*1024)
			{
				paqCompPtr->compress_str_paq9a(lossless_str);
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
		paqCompPtr->compress_str_paq9a(lossless_str);
		{
			std::string t;
			t.swap(lossless_str);
		}
		lossless_str = "";
	}
	paqCompPtr->compress_paq9a_end();

	in.close();
	tmp_out.close();

	if (conf.remove_tmp_file)
	{
		deleteTmpFile(tempFilepath);
	}

	return 1;
}

void MixCompressor::setTask(std::shared_ptr<Task> &t)
{
	this->task = t;
}

void MixCompressor::setConf(CompressConf &cf)
{
	this->conf = cf;
}


// int nready = 0;
// std::mutex mut;
// std::condition_variable cond;
/**
 * @description: 多线程版本，使用paq算法
 * @param
 * @return: int 1表示正常
 */
int MixCompressor::compress_thread()
{
	LOG(INFO) << "MixCompressor::compress_thread()..." << std::endl;
	if (!isFileSuffix(inputFilepath, ".csv"))
	{
		LOG(ERROR) << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	
	std::string losslessAlgorithm = "paq";

	// get fileLines
	std::cout << "inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		LOG(ERROR) << "fileLines = " << fileLines << std::endl;
		exit(1);
	}
	
	// 计算文件块数目
	blocks = (fileLines - 1) / blockSize + (((fileLines - 1) % blockSize) != 0);

	// 根据误差和输入文件，设置中间文件名、输出文件名
	setPaths(AVG_ERR_MAX, inputFilepath, tempFilepath, outputFilepath);

	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = std::make_shared<FileProcessor>(&in, &tmp_out, blockSize);
	fileProcPtr->setFileLines(fileLines);

	std::string metadatas;
	std::string header;
	columnSize = fileProcPtr->initWork(losslessAlgorithm, metadatas, header); // get metadatas and header of input file
	std::cout << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;

	// 将metadata和header插入队列，待压缩
	{
		std::unique_lock<std::mutex> lck(task->mut);
		task->push_str(metadatas);
		task->push_str(header);
		task->nready += 2;
		task->cond.notify_all();
	}

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
			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			clock_t e = clock();
			LOG(INFO) << "losslessCompPtr->compressOneBlock time = " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;
				// 写入task的队列中，待paq压缩
				{
					std::unique_lock<std::mutex> lck(task->mut);
					task->push_str(lossless_str);
					++task->nready;
					task->cond.notify_all();
				}
				{
					std::string t;
					t.swap(lossless_str);
				}
				lossless_str = "";	
			std::cout << "\rlossy processing... " << std::fixed << (double)block_count / blocks * 100 << " %" << std::endl;
			std::cout.flush();
		}
		else
		{
			std::cout << "\nlossy process " << block_count << " blocks\n"<< std::endl;
			break;
		}
		// 释放内存
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

	// 最后一块
	if (lossless_str.length() > 0)
	{
		task->push_str(lossless_str);
		{
			std::string t;
			t.swap(lossless_str);
		}
		lossless_str = "";
	}

	in.close();
	tmp_out.close();

	if (conf.remove_tmp_file)
	{	
		deleteTmpFile(tempFilepath);
	}
	

	task->setFinish();

	return 1;
}

/**
 * @description: 多个块合并成一块压缩
 * @param 
 * @return: 
 */
int MixCompressor::compress_multi_blocks()
{
	LOG(INFO) << "compress_multi_blocks()..." << std::endl;
	// 计算metadata，包括行数、块数等（块数=行数/每块行数/2）
	// 读第一块
	// 读第二块
	// 合并两块为一块
	// 调用有损、无损压缩接口，压缩合并后的块
	if (!isFileSuffix(inputFilepath, ".csv"))
	{
		LOG(ERROR) << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	
	std::string losslessAlgorithm = "paq";

	// get fileLines
	std::cout << "inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		LOG(ERROR) << "fileLines = " << fileLines << std::endl;
		exit(1);
	}
	
	// 计算文件块数目
	// blocks = (fileLines - 1) / blockSize + (((fileLines - 1) % blockSize) != 0);
	// 两个块合并为一块
	blocks = (fileLines - 1) / blockSize / 2;
	LOG(INFO) << "blocks = " << blocks << std::endl;

	// 根据误差和输入文件，设置中间文件名、输出文件名
	setPaths(AVG_ERR_MAX, inputFilepath, tempFilepath, outputFilepath);

	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = std::make_shared<FileProcessor>(&in, &tmp_out, blockSize);
	fileProcPtr->setFileLines(fileLines);

	std::string metadatas;
	std::string header;
	columnSize = fileProcPtr->initWork(losslessAlgorithm, metadatas, header); // get metadatas and header of input file
	std::cout << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;

	// 将metadata和header插入队列，待压缩
	{
		std::unique_lock<std::mutex> lck(task->mut);
		task->push_str(metadatas);
		task->push_str(header);
		task->nready += 2;
		task->cond.notify_all();
	}

	std::string lossless_str = "";

	while (true)
	{
		block.reserve(columnSize);
		clock_t s = clock();

		line_num_of_block = fileProcPtr->getTwoBlocks(block);
		
		if (line_num_of_block > 0)
		{
			// 一次压缩两个块
			
			++block_count;
			lossyCompPtr->compressOneBlock(block, line_num_of_block);
			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			clock_t e = clock();
			LOG(INFO) << "losslessCompPtr->compressOneBlock time = " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;
				// 写入task的队列中，待paq压缩
				{
					std::unique_lock<std::mutex> lck(task->mut);
					task->push_str(lossless_str);
					++task->nready;
					task->cond.notify_all();
				}
				{
					lossless_str.clear();
					lossless_str.shrink_to_fit();
				}
				lossless_str = "";	
			std::cout << "\rlossy processing... " << std::fixed << (double)block_count / blocks * 100 << " %" << std::endl;
			std::cout.flush();
		}
		else
		{
			std::cout << "\nlossy process " << block_count << " blocks\n"<< std::endl;
			break;
		}
		// 释放内存
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

		for(std::vector<std::string> x2 : block2)
		{
			for (std::string s2 : x2)
			{
				s2.clear();
				s2.shrink_to_fit();
			}
			x2.clear();
			x2.shrink_to_fit();
		}
		block2.clear();
		block2.shrink_to_fit();
	}

	// 最后一块
	if (lossless_str.length() > 0)
	{
		task->push_str(lossless_str);
		{
			std::string t;
			t.swap(lossless_str);
		}
		lossless_str = "";
	}

	in.close();
	tmp_out.close();

	if (conf.remove_tmp_file)
	{	
		deleteTmpFile(tempFilepath);
	}
	

	task->setFinish();

	return 1;
}

/**
 * @description: 计算两个块的相似度
 * @param {type} 
 * @return: 
 */
float MixCompressor::compareTwoBlocks(std::vector<std::vector<std::string> >& b1, std::vector<std::vector<std::string> >& b2, float theta)
{
	size_t line_1 = b1.size();
	size_t col_1 = b1[0].size();
	size_t line_2 = b2.size();
	size_t col_2 = b2[0].size();

	assert(line_1 == line_2 && col_1 == col_2);

	int simCol = 0;
	// 计算列的相似性
	for (size_t i = 0; i < col_1; i++)
	{
		int sameNum = 0;
		for (size_t j = 0; j < line_1; j++)
		{
			if (b1[j][i].compare(b2[j][i]) == 0 )
			{
				++sameNum;
			}
		}
		if (((float)sameNum / (float)line_1) >= theta)
		{
			++simCol;
		}
	}
	LOG(INFO) << "simCol = " << simCol << std::endl;
	return (float)simCol / (float)col_1;
}

/**
 * @description: 对比块之间的相似性
 * @param {type} 
 * @return: 
 */
void MixCompressor::testBlocks()
{
	LOG(INFO) << "MixCompressor::testBlocks()..." << std::endl;
	if (!isFileSuffix(inputFilepath, ".csv"))
	{
		LOG(ERROR) << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	
	std::string losslessAlgorithm = "paq";

	// get fileLines
	std::cout << "inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		LOG(ERROR) << "fileLines = " << fileLines << std::endl;
		exit(1);
	}
	
	// 计算文件块数目
	blocks = (fileLines - 1) / blockSize + (((fileLines - 1) % blockSize) != 0);

	// 根据误差和输入文件，设置中间文件名、输出文件名
	setPaths(AVG_ERR_MAX, inputFilepath, tempFilepath, outputFilepath);

	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = std::make_shared<FileProcessor>(&in, &tmp_out, blockSize);
	fileProcPtr->setFileLines(fileLines);

	std::string metadatas;
	std::string header;
	columnSize = fileProcPtr->initWork(losslessAlgorithm, metadatas, header); // get metadatas and header of input file
	std::cout << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;
	int line_num_of_block2;

	std::string lossless_str = "";

	while (true)
	{
		block.reserve(columnSize);
		block2.reserve(columnSize);

		line_num_of_block = fileProcPtr->getOneBlock(block);
		line_num_of_block2 = fileProcPtr->getOneBlock(block2);
		assert(line_num_of_block2 == line_num_of_block);
		if (line_num_of_block > 0)
		{
			++block_count;
			lossyCompPtr->compressOneBlock(block, line_num_of_block);
			lossyCompPtr->compressOneBlock(block2, line_num_of_block);
			float sim_10 = compareTwoBlocks(block, block2, 1.0);
			float sim_9 = compareTwoBlocks(block, block2, 0.9);
			float sim_8 = compareTwoBlocks(block, block2, 0.8);
			float sim_7 = compareTwoBlocks(block, block2, 0.7);
			float sim_6 = compareTwoBlocks(block, block2, 0.6);
			float sim_5 = compareTwoBlocks(block, block2, 0.5);
			LOG(INFO) << "theta = 1.0, sim = " << sim_10 << std::endl;
			LOG(INFO) << "theta = 0.9, sim = " << sim_9 << std::endl;
			LOG(INFO) << "theta = 0.8, sim = " << sim_8 << std::endl;
			LOG(INFO) << "theta = 0.7, sim = " << sim_7 << std::endl;
			LOG(INFO) << "theta = 0.6, sim = " << sim_6 << std::endl;
			LOG(INFO) << "theta = 0.5, sim = " << sim_5 << std::endl;
		}
		else
		{
			std::cout << "\nlossy process " << block_count << " blocks\n"<< std::endl;
			break;
		}
		// 释放内存
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

		for(std::vector<std::string> x : block2)
		{
			for (std::string s : x)
			{
				s.clear();
				std::string().swap(s);
			}
			x.clear();
			std::vector<std::string>().swap(x);
		}
		block2.clear();
		block2.shrink_to_fit();
	}

	in.close();
	tmp_out.close();

	if (conf.remove_tmp_file)
	{	
		deleteTmpFile(tempFilepath);
		deleteTmpFile(outputFilepath);
	}
	exit(0);
}

int MixCompressor::compress_two_blocks()
{
	LOG(INFO) << "compress_two_blocks()..." << std::endl;

	if (!isFileSuffix(inputFilepath, ".csv"))
	{
		LOG(ERROR) << "Don't support this type of file!!! Only CSV files!!!" << std::endl;
		exit(0);
	}
	
	std::string losslessAlgorithm = "paq";

	// get fileLines
	std::cout << "inputFilepath = " << inputFilepath << std::endl;
	fileLines = getFileLines(inputFilepath);
	if (fileLines < 0)
	{
		LOG(ERROR) << "fileLines = " << fileLines << std::endl;
		exit(1);
	}
	
	// 计算文件块数目
	blocks = (fileLines - 1) / blockSize + (((fileLines - 1) % blockSize) != 0);

	LOG(INFO) << "blocks = " << blocks << std::endl;

	// 根据误差和输入文件，设置中间文件名、输出文件名
	setPaths(AVG_ERR_MAX, inputFilepath, tempFilepath, outputFilepath);

	std::ifstream in(inputFilepath.c_str(), std::ios::in);
	std::ofstream tmp_out(tempFilepath.c_str(), std::ios::out | std::ios::trunc);

	fileProcPtr = std::make_shared<FileProcessor>(&in, &tmp_out, blockSize);
	fileProcPtr->setFileLines(fileLines);

	std::string metadatas;
	std::string header;
	columnSize = fileProcPtr->initWork(losslessAlgorithm, metadatas, header); // get metadatas and header of input file
	std::cout << "columnSize = " << columnSize << std::endl;

	int block_count = 0;
	int line_num_of_block;
	int line_num_of_block2;

	// 将metadata和header插入队列，待压缩
	{
		std::unique_lock<std::mutex> lck(task->mut);
		task->push_str(metadatas);
		task->push_str(header);
		task->nready += 2;
		task->cond.notify_all();
	}

	std::string lossless_str = "";

	while (true)
	{
		block.reserve(columnSize);
		block2.reserve(columnSize);
		clock_t s = clock();

		line_num_of_block = fileProcPtr->getOneBlock(block);
		line_num_of_block2 = fileProcPtr->getOneBlock(block2);
		assert(line_num_of_block2 == line_num_of_block);

		if (line_num_of_block > 0)
		{
			// 一次压缩两个块
			++block_count;
			lossyCompPtr->compressOneBlock(block, line_num_of_block);
			lossyCompPtr->compressOneBlock(block2, line_num_of_block2);

			losslessCompPtr->compressOneBlock(block, line_num_of_block, lossless_str);
			lossless_str += '\n';
			// 根据与第一块的相似性压缩第二块
			losslessCompPtr->compressBlockBySimilarity(block, block2, line_num_of_block2, lossless_str);

			clock_t e = clock();
			LOG(INFO) << "losslessCompPtr->compressOneBlock time = " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;
				// 写入task的队列中，待paq压缩
				{
					std::unique_lock<std::mutex> lck(task->mut);
					task->push_str(lossless_str);
					++task->nready;
					task->cond.notify_all();
				}
				{
					lossless_str.clear();
					lossless_str.shrink_to_fit();
				}
				lossless_str = "";	
			std::cout << "\rlossy processing... " << std::fixed << (double)block_count / blocks * 100 << " %" << std::endl;
			std::cout.flush();
		}
		else
		{
			std::cout << "\nlossy process " << block_count << " blocks\n"<< std::endl;
			break;
		}
		// 释放内存
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

		for(std::vector<std::string> x2 : block2)
		{
			for (std::string s2 : x2)
			{
				s2.clear();
				s2.shrink_to_fit();
			}
			x2.clear();
			x2.shrink_to_fit();
		}
		block2.clear();
		block2.shrink_to_fit();
	}

	// 最后一块
	if (lossless_str.length() > 0)
	{
		task->push_str(lossless_str);
		{
			std::string t;
			t.swap(lossless_str);
		}
		lossless_str = "";
	}

	in.close();
	tmp_out.close();

	if (conf.remove_tmp_file)
	{	
		deleteTmpFile(tempFilepath);
	}
	

	task->setFinish();

	return 1;
}

/**
 * @description: 对输入文件进行解压
 * @param 
 * @return: int 1表示正常 
 */
int MixCompressor::decompress(std::string &losslessAlgorithm)
{
 	if (isFileSuffix(inputFilepath, ".hw")) {
		std::vector<std::string> filename;
		splitString(inputFilepath, filename, ".hw");

		tempFilepath = filename[0] + ".tmp";
		outputFilepath = filename[0] + "_de.csv";
		std::cout << "tempFilepath = " << tempFilepath << std::endl;
		std::cout << "outputFilepath = " << outputFilepath << std::endl;
	} else {
		LOG(ERROR) << "decompress: wrong file type" << std::endl;
		exit(1);
	}

	if (losslessAlgorithm.compare("paq") == 0)
	{
		std::cout << "lossless decoding, paq9a algorithm" << std::endl;
		// paqCompPtr->decompressFile_paq9a(inputFilepath, tempFilepath);
		PaqCompressor::decompressFile_paq9a(inputFilepath, tempFilepath);
		std::cout << "decompress file to " << tempFilepath << std::endl;
	}
	else if (losslessAlgorithm.compare("ppmd") == 0)
	{
		std::cout << "lossless decoding, ppmd algorithm" << std::endl;
		LosslessCompressor::decompressFile_ppmd(inputFilepath, tempFilepath);
		std::cout << "decompress file to " << tempFilepath << std::endl;
	}
	
	
	std::ifstream tmp_in(tempFilepath.c_str(), std::ios::in);
	std::ofstream out(outputFilepath.c_str(), std::ios::out | std::ios::trunc);

	// fileProcPtr = new FileProcessor(&tmp_in, &out, blockSize);
	fileProcPtr = std::make_shared<FileProcessor>(&tmp_in, &out, blockSize);

	fileProcPtr->getMetadata(blockSize, columnSize, fileLines, blocks);
	LOG(INFO) << "blockSize = " << blockSize << std::endl;
	LOG(INFO) << "columnSize = " << columnSize << std::endl;
	LOG(INFO) << "fileLines = " << fileLines << std::endl;
	LOG(INFO) << "blocks = " << blocks << std::endl;

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
		std::cout << "\rlossy processing... " << std::fixed << (double)block_count / blocks * 100 << " %" << std::endl;
		std::cout.flush();
	}

	tmp_in.close();
	out.close();

	if (conf.remove_tmp_file)
	{
		deleteTmpFile(tempFilepath);
	}
	
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
		LOG(INFO) << "multi_thread = " << conf.multi_thread << std::endl;
		if (conf.multi_thread)
		{
			LOG(INFO) << "multi thread, compress_paq()" << std::endl;
			compress_thread();
			// compress_multi_blocks();
			// testBlocks();
			// compress_two_blocks();
		}
		else
		{
			if (conf.lossless_algorithm.compare("paq") == 0)
			{
				LOG(INFO) << "single thread, compress_paq()" << std::endl;
				compress_paq();
			}
			else if (conf.lossless_algorithm.compare("ppmd") == 0)
			{
				LOG(INFO) << "single thread, compress_ppmd()" << std::endl;
				compress_ppmd();
			}
			else {
				LOG(ERROR) << "don't support this lossless algorithm: " << conf.lossless_algorithm << std::endl;
				exit(1);
			}
		}
		clock_t endTime = clock();
		std::cout << "MixCompressor::run() total time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << std::endl;
	}
	else if (mode == DECOMPRESS) {
		clock_t startTime = clock();
		char buf[10] = {0};
		sprintf(buf, "pQ9%c", 1);
		std::string paq_version = buf;
		if (FileProcessor::checkFile(inputFilepath, paq_version))
		{
			// paq
			LOG(INFO) << "decompress paq file" << std::endl;
			std::string lossless = "paq";
			decompress(lossless);
		}
		else
		{
			// ppmd
			LOG(INFO) << "decompress ppmd file" << std::endl;
			std::string lossless = "ppmd";
			decompress(lossless);
		}
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
		LOG(WARNING) << "\nremove tmp file failed" << std::endl;
		return -1;
	}
}
