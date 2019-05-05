#include "mixCompressor.h"
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <thread>
#include "task.h"
#include "paqCompressor.h"
#include "compressConf.h"
#include "utils.h"
#include "multiThread.h"
#include "glog/logging.h"

/**
 * @description: 程序入口
 * @param argc 参数个数
 * @param argv 命令行的参数
 * @return: int -1表示出错，大于等于0表示正常
 */
int main(int argc, char const *argv[])
{
	google::InitGoogleLogging(argv[0]);
	google::SetStderrLogging(google::GLOG_INFO);

	if (argc != 3)
	{
		return Help();
	}

	time_t stime = time(NULL);
	CompressConf conf;
	if (conf.getParas(argc, argv) < 0)
	{
		LOG(ERROR) << "command line parameters error" << std::endl;
		exit(1);
	}
	std::string conffile = "conf/config";
	conf.readConf(conffile);
	// double thread version
	
	if (conf.paras.mode == COMPRESS)
	{
		LOG(INFO) << "conf.paras.mode = COMPRESS" << std::endl;
		// doubel thread open, only paq9a is supported
		if (conf.multi_thread)
		{
			LOG(INFO) << "conf.multi_thread == true" << std::endl;
			MultiThread::multi_thread_compress(conf);
		}
		else
		{
			LOG(INFO) << "conf.multi_thread == false" << std::endl;
			std::shared_ptr<MixCompressor> mixComp = std::make_shared<MixCompressor>(conf);
			mixComp->run();
		}
	}
	else if (conf.paras.mode == DECOMPRESS)
	{
		LOG(INFO) << "conf.paras.mode = DECOMPRESS" << std::endl;
		std::shared_ptr<MixCompressor> mixComp = std::make_shared<MixCompressor>(conf);
		mixComp->run();
	}
	else
	{
		LOG(WARNING) << "don't support this mode: " << conf.paras.mode << std::endl;
		exit(1);
	}
	
	time_t etime = time(NULL);
	std::cout << "finish, time = " << etime << " - " << stime << " = " << etime - stime << " seconds" << std::endl;
	return 0;
}