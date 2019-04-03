#include "lossyComp.h"
// #include "errComputer.h"
#include "utils.h"
#include <iostream>

LossyCompressor::LossyCompressor(double rel_err, double avg_err) : PW_REL_ERR_MAX(rel_err), AVG_ERR_MAX(avg_err), cFreq(10), block_count(0)
{
	for (int i = 0; i < 10; ++i)
	{
		std::cout << "cFreq[" << i << "] = " << cFreq[i] << std::endl;
	}
	UP = 1 + PW_REL_ERR_MAX;
	LOW = 1 - PW_REL_ERR_MAX;

}

LossyCompressor::~LossyCompressor()
{

}

void LossyCompressor::updateFreq(const std::string &data)
{
	for (int i = 0; i < data.length(); i++) {
		if (data[i] >= '0' && data[i] <= '9') {
			++cFreq[(int)data[i] - '0'];
		}
	}
}

void LossyCompressor::processZeroOrE(std::string &data, double err)
{
	if (isZeroOrNA(data))
	{
		data = "0";
	} else if (containE(data))
	{
		data = simplifyData.restoreE0(data, err);
	} else {
		perror("LossyCompressor::processZeroOrE(), not zero or E!");
	}
	updateFreq(data);
}

void LossyCompressor::processIntervalRange(double &low, double &up)
{
	if (up < low)
	{
		std::swap(up, low); // 负数
	}
	std::string low_tmp = simplifyData.simplifyDataCeil(std::to_string(low), std::to_string(up));
	if (low_tmp == "")
	{
		low = 0;
	} else {
		low = std::stod(low_tmp);
	}
	
	std::string up_tmp = simplifyData.simplifyDataFloor(std::to_string(low), std::to_string(up));
	if (up_tmp == "")
	{
		up = 0;
	} else {
		up = std::stod(up_tmp);
	}
	// std::cout << "low = " << low << " up = " << up << std::endl;
}

void LossyCompressor::processIntervalData(std::vector<std::vector<std::string> > &block, int col, int start, int end)
{

}

int LossyCompressor::compressOneBlock(std::vector<std::vector<std::string> > &block, int line_num)
{
	++block_count;

	if (block_count == 1)
	{
		compressFirstBlock(block, line_num);
	}
	else
	{
		compressOtherBlock(block, line_num);	
	}
	return 1;
}

int LossyCompressor::compressOtherBlock(std::vector<std::vector<std::string> > &block, int line_num)
{
	std::cout << "compressOtherBlock()..." << std::endl;
	int rowN = line_num;
	if (rowN <= 0)
	{
		perror("rowN <= 0");
		exit(0);
	}
	int colN = block[0].size();
	// first three col are metadatas
	for (int j = 3; j < colN; ++j)
	{
		std::cout << "compress column " << j << std::endl;
		int start = 0, end = 0; // 区间开始、结束下标
		double up1 = std::stod(block[0][j]) * UP;
		double low1 = std::stod(block[0][j]) * LOW;

		processIntervalRange(low1, up1);

		for (int k = 1; k < rowN; ++k)
		{
			// std::cout << "k = " << k << " data = " << block[k][j] << std::endl;
			// if 区间停止
			// 		处理区间中所有值，更新start、end、up1、low1
			//		continue；
			// else
			// 		判断是否可扩展区间
			//		if 可扩展
			// 			更新end，up1、low1
			//		else
			//			处理区间start到end的值，更新start、end为当前点，更新up、low
			std::string currentData = block[k][j];
			if (isZeroOrNA(currentData) || containE(currentData))
			{
				// 区间结束
				// TODO 更改处理方式，使用字符替换的方式
				processZeroOrE(block[k][j], PW_REL_ERR_MAX);
				// process interval
				std::string bestData = simplifyData.getDataS(std::to_string(low1), std::to_string(up1), cFreq);
				for (int i = start; i <= end; ++i)
				{		
					// block[i][j] = std::to_string(up1);
					// TODO 更改处理方式，使用字符替换的方式
					// block[i][j] = convertDouble(up1);
					block[i][j] = bestData;
					updateFreq(block[i][j]);
				}
				// update
				++k;
				start = k; // 下一个数作为初始区间
				end = k;
				// std::cout << "start = " << start << " end = " << end  << std::endl;
				if (k > rowN - 1)
				{
					break;
				}
				else if (k == rowN - 1) // 最后一个数
				{
					up1 = std::stod(block[k][j]) * UP;
					low1 = std::stod(block[k][j]) * LOW;
					processIntervalRange(low1, up1);
					// block[k][j] = convertDouble(up1);
					block[k][j] = simplifyData.getDataS0(block[k][j], PW_REL_ERR_MAX, cFreq);
					updateFreq(block[k][j]);
					break;
				}
				up1 = std::stod(block[k][j]) * UP;
				low1 = std::stod(block[k][j]) * LOW;
				processIntervalRange(low1, up1);

			}
			else // 普通数值，非0且不含E
			{
				double low_cur = std::stod(currentData) * UP;
				double up_cur = std::stod(currentData) * LOW;
				processIntervalRange(low_cur, up_cur);
				// 判断是否可扩展
				if ((low_cur <= up1 && up_cur >= low1)) // 有交集
				{
					up1 = std::min(up1, up_cur);
					low1 = std::max(low1, low_cur);
					end = k;
				} else { // 无交集
					// 处理区间内数据
					// 更新start、end、up1、low1
					std::string bestData = simplifyData.getDataS(std::to_string(low1), std::to_string(up1), cFreq);
					for (int i = start; i <= end; ++i)
					{
						// block[i][j] = std::to_string(up1);
						// TODO 更改处理方式，使用字符替换的方式
						// block[i][j] = convertDouble(up1);
						// block[i][j] = simplifyData.getDataS0(block[i][j], err, cFreq);
						block[i][j] = bestData;
						updateFreq(block[i][j]);
					}
					// update
					start = k; // 当前数作为初始区间
					end = k;
					
					// std::cout << "start = " << start << " end = " << end  << std::endl;
					if (k > rowN - 1)
					{
						break;
					}
					else if (k == rowN - 1) // 最后一个数
					{
						up1 = std::stod(block[k][j]) * UP;
						low1 = std::stod(block[k][j]) * LOW;
						processIntervalRange(low1, up1);
						// TODO 更改处理方式，使用字符替换的方式
						// block[k][j] = convertDouble(up1);
						block[k][j] = simplifyData.getDataS0(block[k][j], PW_REL_ERR_MAX, cFreq);
						updateFreq(block[k][j]);
						break;
					}
					// update
					up1 = std::stod(block[k][j]) * UP;
					low1 = std::stod(block[k][j]) * LOW;
					processIntervalRange(low1, up1);
				}
			}
		}
	}
	return 1;
}


int LossyCompressor::compressFirstBlock(std::vector<std::vector<std::string> > &block, int line_num)
{
	std::cout << "compressOtherBlock()..." << std::endl;
	int rowN = line_num;
	if (rowN <= 0)
	{
		perror("rowN <= 0");
		exit(0);
	}
	int colN = block[0].size();
	// first three col are metadatas
	for (int j = 3; j < colN; ++j)
	{
		std::cout << "compress column " << j << std::endl;
		int start = 0, end = 0; // 区间开始、结束下标
		double up1 = std::stod(block[0][j]) * UP;
		double low1 = std::stod(block[0][j]) * LOW;

		processIntervalRange(low1, up1);

		for (int k = 1; k < rowN; ++k)
		{
			// std::cout << "k = " << k << " data = " << block[k][j] << std::endl;
			// if 区间停止
			// 		处理区间中所有值，更新start、end、up1、low1
			//		continue；
			// else
			// 		判断是否可扩展区间
			//		if 可扩展
			// 			更新end，up1、low1
			//		else
			//			处理区间start到end的值，更新start、end为当前点，更新up、low
			std::string currentData = block[k][j];
			if (isZeroOrNA(currentData) || containE(currentData))
			{
				// 区间结束
				processZeroOrE(block[k][j], PW_REL_ERR_MAX);
				// 处理前面区间的数值
				for (int i = start; i <= end; ++i)
				{		
					// block[i][j] = std::to_string(up1);
					block[i][j] = convertDouble(up1);
					updateFreq(block[i][j]);
				}
				// update
				++k;
				start = k; // 下一个数作为初始区间
				end = k;
				// std::cout << "start = " << start << " end = " << end  << std::endl;
				if (k > rowN - 1)
				{
					break;
				}
				else if (k == rowN - 1) // 最后一个数
				{
					up1 = std::stod(block[k][j]) * UP;
					low1 = std::stod(block[k][j]) * LOW;
					processIntervalRange(low1, up1);
					block[k][j] = convertDouble(up1);
					break;
				}
				up1 = std::stod(block[k][j]) * UP;
				low1 = std::stod(block[k][j]) * LOW;
				processIntervalRange(low1, up1);

			}
			else // 普通数值，非0且不含E
			{
				double low_cur = std::stod(currentData) * UP;
				double up_cur = std::stod(currentData) * LOW;
				processIntervalRange(low_cur, up_cur);
				// 判断区间是否可扩展
				if ((low_cur <= up1 && up_cur >= low1)) // 有交集，可扩展
				{
					up1 = std::min(up1, up_cur);
					low1 = std::max(low1, low_cur);
					end = k;
				} else { // 无交集，不可扩展
					// 处理区间内数据
					// 更新start、end、up1、low1
					for (int i = start; i <= end; ++i)
					{
						// block[i][j] = std::to_string(up1);
						block[i][j] = convertDouble(up1);
						updateFreq(block[i][j]);
					}
					// update
					start = k; // 当前数作为初始区间
					end = k;
					
					// std::cout << "start = " << start << " end = " << end  << std::endl;
					if (k > rowN - 1)
					{
						break;
					}
					else if (k == rowN - 1) // 最后一个数
					{
						up1 = std::stod(block[k][j]) * UP;
						low1 = std::stod(block[k][j]) * LOW;
						processIntervalRange(low1, up1);
						block[k][j] = convertDouble(up1);
						break;
					}
					// update
					up1 = std::stod(block[k][j]) * UP;
					low1 = std::stod(block[k][j]) * LOW;
					processIntervalRange(low1, up1);
				}
			}
		}
	}
	return 1;
}

void LossyCompressor::compress(std::string inputFilepath, std::string outputFilepath)
{

}

void LossyCompressor::decompress(std::string inputFilepath, std::string outputFilepath)
{

}





