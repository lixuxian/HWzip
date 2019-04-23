#include "paqCompresssor.h"
#include "losslessComp.h"
#include "task.h"
#include <iostream>

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
        if (!task->isEmpty())
        {
            std::string str;
            if (task->pop_str(str))
            {
                // paq compress
                losslessComp->compress_str_paq9a(str);
                std::cout << "paq compress a string" << std::endl;
            }
            {
                std::string t;
                t.swap(str);
            }
        }
    }
    losslessComp->compress_paq9a_end();
    std::cout << "compress_paq9a_end()" << std::endl;
}
