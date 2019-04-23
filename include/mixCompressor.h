/*
 * @Description: 混合压缩器，包含了有损压缩、无损压缩
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 17:33:45
 * @LastEditTime: 2019-04-23 16:58:11
 */

// #include "lossyComp.h"
// #include "losslessComp.h"
// #include "fileProcess.h"
#include <memory>
#include <fstream>
#include <string>
#include <vector>

enum Mode
{
	COMPRESS = 1,
	DECOMPRESS
};

class LosslessCompressor;
class LossyCompressor;
class FileProcessor;
class Task;

class MixCompressor
{
public:
	MixCompressor(double rel_err, double avg_err, std::string input, char mode);
	virtual ~MixCompressor();

	int compress_old();
	int compress();
	int compress_thread();
	int decompress();
	
	void run();

	int deleteTmpFile(std::string &tmpfile);

	void setTask(std::shared_ptr<Task> &t);

private:
	// LossyCompressor *lossyComp;
	// LosslessCompressor *losslessComp;
	// FileProcessor *fileProc;

	int getFileLines(std::string inputFilepath);

	// LossyCompressor *lossyCompPtr;
	// LosslessCompressor *losslessCompPtr;
	// FileProcessor *fileProcPtr;

	std::shared_ptr<LossyCompressor> lossyCompPtr;
	std::shared_ptr<LosslessCompressor> losslessCompPtr;
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
};