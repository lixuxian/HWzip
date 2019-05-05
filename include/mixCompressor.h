/*
 * @Description: 混合压缩器，包含了有损压缩、无损压缩
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 17:33:45
 * @LastEditTime: 2019-05-05 10:55:39
 */

// #include "lossyComp.h"
// #include "losslessComp.h"
// #include "fileProcess.h"
#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include "compressConf.h"

// enum Mode
// {
// 	COMPRESS = 1,
// 	DECOMPRESS
// };

class LosslessCompressor;
class LossyCompressor;
class PaqCompressor;
class FileProcessor;
class Task;
class CompressConf;

class MixCompressor
{
public:
	MixCompressor(CompressConf &cf);
	virtual ~MixCompressor();

	int compress_ppmd();
	int compress_paq();
	int compress_thread();
	int decompress(std::string &losslessAlgorithm);
	
	void run();

	int deleteTmpFile(std::string &tmpfile);

	void setTask(std::shared_ptr<Task> &t);

	void setConf(CompressConf &cf);

private:

	int getFileLines(std::string inputFilepath);

	std::shared_ptr<LossyCompressor> lossyCompPtr;
	std::shared_ptr<LosslessCompressor> losslessCompPtr;
	std::shared_ptr<PaqCompressor> paqCompPtr;
	std::shared_ptr<FileProcessor> fileProcPtr;

	std::vector<std::vector<std::string> > block; 


	const double PW_REL_ERR_MAX;
	const double AVG_ERR_MAX;

	std::string inputFilepath;
	std::string outputFilepath;
	std::string tempFilepath;

	char mode;
	int blockSize;
	int columnSize;
	int fileLines;
	int blocks;

	std::shared_ptr<Task> task;

	CompressConf conf;
};