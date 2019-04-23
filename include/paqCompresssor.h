#include <memory>
#include <string>

class Task;
class LosslessCompressor;

class PaqCompresssor
{
private:
    std::shared_ptr<Task> task;
    std::string tempFilepath;
    std::string outFilepath;

    std::shared_ptr<LosslessCompressor> losslessComp;

public:
    PaqCompresssor();
    ~PaqCompresssor();

    void setTask(std::shared_ptr<Task> &t);
    void run();
};
