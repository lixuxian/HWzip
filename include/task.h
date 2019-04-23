#include <queue>
#include <string>
#include <mutex>

class Task
{
private:
    int max_size;
    std::queue<std::string> tasks;
    std::mutex q_mutex;

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

