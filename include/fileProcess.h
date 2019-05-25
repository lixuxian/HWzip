/*
 * @Description: 文件处理类，包括处理csv文件头、处理metadata、按块读写等
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 18:10:11
 * @LastEditTime: 2019-05-24 09:24:37
 */

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Metadata
{
private:
	std::string version;
	int blockLines;
	int columnNum;
	int fileLines;
	std::string losslessAlgorithm;

public:
	Metadata();
	~Metadata();

	void setVersion(std::string &ver);
	void setBlockLines(int block_lines);
	void setColumnNum(int column_num);
	void setfileLines(int file_lines);
	void setLosslessAlgorithm(std::string &algorithm);
	void createMetadatString(std::string &meta_string);
};

class FileProcessor
{
public:
	FileProcessor(std::ifstream *in, std::ofstream *out, int blockSize);
	~FileProcessor();

	int initWork_old();
	int initWork(std::string &losslessAlgorithm, std::string &metadatas, std::string &header); // store head、time and ID

	int getOneBlock(std::vector<std::vector<std::string> > &block);
	int writeOneBlock2Tempfile(std::vector<std::vector<std::string> > &block, int line_num);
	int writeOneBlock2Tempfile(std::string &lossless_str);
	void initBlock(std::vector<std::vector<std::string> > &block);

	void getMetadata(int& blockSize, int& columnSize, int& lines, int& blocks);

	void setFileLines(int lines);

	int getOneDecompressedBlock(std::vector<std::vector<std::string> > &block);

	int writeOneBlock2DecompressedFile(std::vector<std::vector<std::string> > &block, int lines);
	int writeHeader2DecompressedFile();

	static bool checkFile(std::string file, std::string filetype);

	void normlizeOneBlock(std::vector<std::vector<std::string> > &block, int line_num);

	std::ifstream *in;
	std::ofstream *out;

	Metadata meta;

private:
	std::string inputFilepath;
	std::string outputFilepath;
	// std::string tempFilepath;
	int blockLines;
	int blockCount;
	int fileLines;
	int columnSize;

	int decompress_block_count;
	

};
