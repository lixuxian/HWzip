#include <memory>
#include <string>
#include "compressConf.h"

class PaqCompressor;
class CompressConf;
class Task;



class MultiThread
{
public:
    static bool end;
    static std::shared_ptr<std::mutex> mut_end;
    static std::shared_ptr<std::condition_variable> cv;

public:
    MultiThread();
    ~MultiThread();

    static void thread_lossless(std::shared_ptr<PaqCompressor> paqComp, std::shared_ptr<Task> task);
    static void multi_thread_compress(CompressConf &conf);
};