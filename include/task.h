#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
// #include "../include/boost/lockfree/spsc_queue.hpp"
// #include "boost/lockfree/spsc_queue.hpp"

class Task
{
private:
    std::queue<std::string> tasks;
    bool finish;

public:
    int nready;
    std::mutex mut;
    std::condition_variable cond;

public:
    Task();
    virtual ~Task();

    void push_str(std::string &str);
    bool pop_str(std::string &result);    
    void setFinish();
    bool isFinish();
    bool isEmpty();
};

