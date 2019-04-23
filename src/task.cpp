#include "task.h"

Task::Task(int size) : max_size(size), finish(false)
{
    
}

Task::~Task()
{
}

void Task::push_str(std::string &str)
{
    q_mutex.lock();

    tasks.push(str);

    q_mutex.unlock();
}

bool Task::pop_str(std::string &result)
{
    bool flag = false;
    q_mutex.lock();
    if (!tasks.empty())
    {
        result = tasks.front();
        tasks.pop();
        flag = true;
    }
    q_mutex.unlock();
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