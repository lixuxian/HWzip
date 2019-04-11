/*
 * @Description: 有损压缩器，按块压缩
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 17:36:08
 * @LastEditTime: 2019-04-11 11:40:58
 */
#pragma once

#include "baseComp.h"
#include "simplifyData.h"
#include <vector>

class LossyCompressor : public BaseCompressor
{
public:
	LossyCompressor(double rel_err = 0.10, double avg_err = 0.05);
	virtual ~LossyCompressor();

	int compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num);
	int compressFirstBlock(std::vector<std::vector<std::string> > &block, int line_num);
	int compressOtherBlock(std::vector<std::vector<std::string> > &block, int line_num);

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