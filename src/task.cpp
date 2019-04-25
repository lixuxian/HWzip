#include "task.h"

Task::Task(int size) : max_size(size), finish(false)
{
    
}

Task::~Task()
{
}

void Task::push_str(std::string &str)
{
    tasks.push(str);
}

bool Task::pop_str(std::string &result)
{
    bool flag = false;
    if (!tasks.empty())
    {
        result = tasks.front();
        tasks.pop();
        flag = true;
    }
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