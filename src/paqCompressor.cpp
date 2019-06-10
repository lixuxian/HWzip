#include "paqCompressor.h"
#include "losslessComp.h"
#include "task.h"
#include "utils.h"
#include "paq9a.h"
#include <iostream>
#include <ctime>

PaqCompressor::PaqCompressor(std::string &infile, double rel_err, double avg_err) : PW_REL_ERR_MAX(rel_err), AVG_ERR_MAX(avg_err), inFilepath(infile), e(nullptr)
{
    // tempFilepath = "test.tmp";
    // outFilepath = "test.hw";
    // generateFilepath();
    setPaths(avg_err, inFilepath, tempFilepath, outFilepath);
    // losslessComp = std::make_shared<LosslessCompressor>();
    // losslessComp->compress_init_paq9a(tempFilepath, outFilepath);
    compress_init_paq9a(tempFilepath, outFilepath);
}

PaqCompressor::~PaqCompressor()
{
	LOG(INFO) << "PaqCompressor::~PaqCompressor()" << std::endl;
	if (this->e != NULL)
	{
		delete e;
		this->e == NULL;
	}
}

void PaqCompressor::setTask(std::shared_ptr<Task> &t)
{
    this->task = t;
}

void PaqCompressor::run()
{
    int loop_times = 0;
    while (!task->isFinish())
    {
        ++loop_times;
        std::string str;
        {
		    std::unique_lock<std::mutex> lck(task->mut);
            if (!(task->nready > 0))
            {
                task->cond.wait(lck);
            }
            task->pop_str(str);
            --task->nready;
        }
        clock_t s = clock();
        // losslessComp->compress_str_paq9a(str);
        compress_str_paq9a(str);
        clock_t e = clock();
        std::cout << "losslessComp->compress_str_paq9a, time = " << (double)(e - s)/CLOCKS_PER_SEC << std::endl;
        {
            std::string t;
            t.swap(str);
        }
    }
    // losslessComp->compress_paq9a_end();
    compress_paq9a_end();
    std::cout << "compress_paq9a_end(), loop_times = " << loop_times << std::endl;
}


int PaqCompressor::compress_init_paq9a(std::string inputFilepath, std::string outputFilepath)
{
	FILE *out = fopen(outputFilepath.c_str(), "rb");
	if (out)
	{
		LOG(ERROR) << "Cannot overwrite archive " << outputFilepath << std::endl;
		exit(1);
	}
	out = fopen(outputFilepath.c_str(), "wb");
	if (!out)
	{
		LOG(ERROR) << "LosslessCompressor::compressFile_paq9a(), FILE *out == NULL" << std::endl;
		exit(1);
	}
	Mode_paq mode = COMPRESS;
	e = paq9a_compress_init(inputFilepath.c_str(), out, mode);
	return 1;
}

int PaqCompressor::compress_str_paq9a(std::string str)
{
	if (this->e == NULL)
	{
		std::cout << "LosslessCompressor::compress_str_paq9a(), this->e == NULL" << std::endl;
		exit(1);
	}
    paq9a_compress_str(str.c_str(), e);
	return 1;
}

void PaqCompressor::compress_paq9a_end()
{
	paq9a_compress_end(e);
}

/**
 * @description: 最后一步的无损压缩，paq9a的无损压缩接口(上下文混合模型压缩算法)
 * @param inputFilepath 输入文件路径(即中间文件) 
 * @param outputFilepath 输出文件路径(即最终的压缩文件)
 * @return: int 1表示正常
 */
int PaqCompressor::compressFile_paq9a(std::string inputFilepath, std::string outputFilepath)
{
	FILE *out = fopen(outputFilepath.c_str(), "rb");
	if (out)
	{
		std::cout << "Cannot overwrite archive " << outputFilepath << std::endl;
		exit(1);
	}
	out = fopen(outputFilepath.c_str(), "wb+");
	if (!out)
	{
		std::cout << "LosslessCompressor::compressFile_paq9a(), FILE *out == NULL" << std::endl;
		exit(1);
	}
	compress_paq9a(inputFilepath.c_str(), out);
	if (out)
	{
		fclose(out);
	}
	return 1;
}

/**
 * @description: paq9a的无损解压接口，其实现在paq9a.h中
 * @param inputFilepath 输入文件路径(即压缩文件)
 * @param outputFilepath 输出文件路径(即中间文件)
 * @return: int 1表示正常
 */
int PaqCompressor::decompressFile_paq9a(std::string inputFilepath, std::string outputFilepath)
{

	decompress_paq9a(inputFilepath.c_str(), outputFilepath.c_str());

	return 1;
}