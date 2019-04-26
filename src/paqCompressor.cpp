#include "paqCompresssor.h"
#include "losslessComp.h"
#include "task.h"
#include <iostream>
#include <ctime>
#include <condition_variable>

PaqCompresssor::PaqCompresssor()
{
    tempFilepath = "test.tmp";
    outFilepath = "test.hw";
    losslessComp = std::make_shared<LosslessCompressor>();
    losslessComp->compress_init_paq9a(tempFilepath, outFilepath);
}

PaqCompresssor::~PaqCompresssor()
{
}

void PaqCompresssor::setTask(std::shared_ptr<Task> &t)
{
    this->task = t;
}

extern int nready;
extern std::condition_variable cond;
extern std::mutex mut;

void PaqCompresssor::run()
{
    int loop_times = 0;
    while (!task->isFinish())
    {
        ++loop_times;
        std::string str;
        // if (task->pop_str(str))
        // {
        //     // paq compress
        //     clock_t s = clock();
        //     losslessComp->compress_str_paq9a(str);
        //     clock_t e = clock();
        //     std::cout << "paq compress a string time = " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;
        // }
        // {
        //     std::string t;
        //     t.swap(str);
        // }
        
        {
		    std::unique_lock<std::mutex> lck(mut);
            if (!(nready > 0))
            {
                cond.wait(lck);
            }
            task->pop_str(str);
            --nready;
        }
        clock_t s = clock();
        losslessComp->compress_str_paq9a(str);
        clock_t e = clock();
        std::cout << "losslessComp->compress_str_paq9a, time = " << (double)(e - s)/CLOCKS_PER_SEC << std::endl;
        {
            std::string t;
            t.swap(str);
        }
    }
    losslessComp->compress_paq9a_end();
    std::cout << "compress_paq9a_end(), loop_times = " << loop_times << std::endl;
}
