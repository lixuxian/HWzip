#include "task.h"
#include "utils.h"

Task::Task() : finish(false), nready(0)
{
    
}

Task::~Task()
{
    LOG(INFO) << "Task::~Task()" << std::endl;
    if (!tasks.empty())
    {
        LOG(WARNING) << "Task::~Task(), !tasks.empty()" << std::endl;
        while (!tasks.empty())
        {
            std::string str = tasks.front();
            str.clear();
            str.shrink_to_fit();
            tasks.pop();
        }
    }
}

void Task::push_str(std::string &str)
{
    tasks.push(str);
}

bool Task::pop_str(std::string &result)
{
    bool flag = false;
    // if (!tasks.empty())
    // {
    //     result = tasks.front();
    //     tasks.pop();
    //     flag = true;
    // }
    result = tasks.front();
    tasks.pop();
    flag = true;
    return flag;
}

void Task::setFinish()
{
    finish = true;
}

bool Task::isFinish()
{
    return finish;
}

bool Task::isEmpty()
{
    return tasks.empty();
}