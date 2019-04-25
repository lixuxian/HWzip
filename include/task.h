#include <queue>
#include <string>
#include <mutex>
// #include "../include/boost/lockfree/spsc_queue.hpp"
#include "boost/lockfree/spsc_queue.hpp"

class Task
{
private:
    const int max_size;
    std::queue<std::string> tasks;
    // boost::lockfree::spsc_queue<std::string, boost::lockfree::capacity<100> > tasks;
    bool finish;

public:
    Task(int size);
    virtual ~Task();

    void push_str(std::string &str);
    bool pop_str(std::string &result);    
    void setFinish();
    bool isFinish();
    bool isEmpty();
};

