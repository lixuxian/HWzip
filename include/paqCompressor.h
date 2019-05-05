#include <memory>
#include <string>

class Task;
class LosslessCompressor;
class Encoder;

class PaqCompressor
{
private:

    const double PW_REL_ERR_MAX;
	const double AVG_ERR_MAX;

    std::shared_ptr<Task> task;
    std::string inFilepath;
    std::string tempFilepath;
    std::string outFilepath;

    std::shared_ptr<LosslessCompressor> losslessComp;

    Encoder *e;
    // std::shared_ptr<Encoder> e;

public:
    PaqCompressor(std::string &infile, double rel_err, double avg_err);
    ~PaqCompressor();

    int compress_init_paq9a(std::string inputFilepath, std::string outputFilepath);
	int compress_str_paq9a(std::string str);
	void compress_paq9a_end();

    static int compressFile_paq9a(std::string inputFilepath, std::string outputFilepath);
	static int decompressFile_paq9a(std::string inputFilepath, std::string outputFilepath);

    void setTask(std::shared_ptr<Task> &t);
    void run();
};
