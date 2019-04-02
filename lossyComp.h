#pragma once

#include "baseComp.h"
#include "simplifyData.h"
#include <vector>

class LossyCompressor : public BaseCompressor
{
public:
	LossyCompressor(double rel_err = 0.10, double avg_err = 0.05);
	~LossyCompressor();

	int compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num);
	int compressFirstBlock(std::vector<std::vector<std::string> > &block, int line_num);
	int compressOneColumn(std::vector<std::vector<std::string> > &block, int col);
	// int compressFile(const std::string &inputFilepath);

	void compress(std::string inputFilepath, std::string outputFilepath);
	void decompress(std::string inputFilepath, std::string outputFilepath);
	

private:
	const double PW_REL_ERR_MAX;
	const double AVG_ERR_MAX;
	double UP;
	double LOW;

	std::vector<int> cFreq;

	unsigned int block_count;

	void updateFreq(const std::string &data);

	void processZeroOrE(std::string &data, double err);
	void processIntervalRange(double &low, double &up); // 对区间上下取整
	void processIntervalData(std::vector<std::vector<std::string> > &block, int col, int start, int end);

public:
	SimplifyData simplifyData;
};