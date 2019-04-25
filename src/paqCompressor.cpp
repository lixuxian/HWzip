#include "paqCompresssor.h"
#include "losslessComp.h"
#include "task.h"
#include <iostream>
#include <ctime>

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

void PaqCompresssor::run()
{
    while (!task->isFinish())
    {
        std::string str;
        if (task->pop_str(str))
        {
            // paq compress
            clock_t s = clock();
            losslessComp->compress_str_paq9a(str);
            clock_t e = clock();
            std::cout << "paq compress a string time = " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;
        }
        {
            std::string t;
            t.swap(str);
        }
    }
    losslessComp->compress_paq9a_end();
    std::cout << "compress_paq9a_end()" << std::endl;
}
