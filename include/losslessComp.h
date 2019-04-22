/*
 * @Description: 无损压缩器，包括相似列处理、7z、ppmd、paq等无损算法接口
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 17:36:17
 * @LastEditTime: 2019-04-22 11:11:10
 */

#include "baseComp.h"
// #include "paq9a.h"
#include <string>
#include <vector>

class Encoder;

class LosslessCompressor : public BaseCompressor
{
public:
	LosslessCompressor();
	virtual ~LosslessCompressor();

	void compress(std::string inputFilepath, std::string outputFilepath);
	void decompress(std::string inputFilepath, std::string outputFilepath);

	double getSimilarity(std::vector<std::vector<std::string> > &block, int line_num, int c1, int c2);
	int chooseSimilarColumn(std::vector<std::vector<std::string> > &block, int line_num, int currentCol, double &similarity);
	int createSimilarString(std::vector<std::vector<std::string> > &block, int line_num, int simCol, int currentCol, std::string &result);


	int compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num, std::string &lossless_str);

	int compressFile_7z(std::string inputFilepath, std::string outputFilepath, int level);

	int compressFile_bz2(std::string inputFilepath, int level);

	int compressFile_paq9a(std::string inputFilepath, std::string outputFilepath);

	int compressFile_ppmd(std::string inputFilepath, std::string outputFilepath);
	
	int decompressFile_7z(std::string inputFilepath);

	int decompressFile_bz2(std::string inputFilepath);

	int decompressFile_paq9a(std::string inputFilepath, std::string outputFilepath);

	int decompressFile_ppmd(std::string inputFilepath, std::string outputFilepath);

	int compress_init_paq9a(std::string inputFilepath, std::string outputFilepath);
	int compress_str_paq9a(std::string str);
	void compress_paq9a_end();

private:
	double simThreshold;
	int simRange;
	Encoder *e;
};
