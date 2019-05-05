#include "multiThread.h"
#include "paqCompressor.h"
#include "mixCompressor.h"
#include "utils.h"
#include "task.h"
#include <thread>
#include <iostream>

MultiThread::MultiThread()
{

}

MultiThread::~MultiThread()
{

}

bool MultiThread::end = false;
std::shared_ptr<std::mutex> MultiThread::mut_end = std::make_shared<std::mutex>();
std::shared_ptr<std::condition_variable> MultiThread::cv = std::make_shared<std::condition_variable>();
// bool end = false;
// std::mutex mut_end;
// std::condition_variable cv;

void MultiThread::thread_lossless(std::shared_ptr<PaqCompressor> paqComp, std::shared_ptr<Task> task)
{
	paqComp->setTask(task);
	paqComp->run();
	{
		std::unique_lock<std::mutex> lock_end(*mut_end);
		end = true;
		cv->notify_all();
	}
}

void MultiThread::multi_thread_compress(CompressConf &conf)
{
	std::shared_ptr<MixCompressor> mixComp = std::make_shared<MixCompressor>(conf);

	std::shared_ptr<PaqCompressor> paqComp = std::make_shared<PaqCompressor>(conf.paras.inputFile, conf.MAX_PW_REL_ERR, conf.MAX_AVG_ERR);

	std::shared_ptr<Task> task = std::make_shared<Task>();

	std::thread lossless(thread_lossless, paqComp, task);
	lossless.detach();

	mixComp->setTask(task);
	mixComp->run();

    {
        std::unique_lock<std::mutex> lock_end(*mut_end);
        while (!end)
        {
            cv->wait(lock_end);
        }
        std::cout << "multi_thread_compress end..." << std::endl;
    }
}