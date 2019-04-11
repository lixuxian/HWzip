/*
 * @Description: 压缩器的基类，包括compress和decompress方法
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 17:34:10
 * @LastEditTime: 2019-04-11 11:38:12
 */
#pragma once

#include <string>

class BaseCompressor
{
public:
	BaseCompressor()
	{

	}
	~BaseCompressor()
	{

	}
	virtual void compress(std::string inputFilepath, std::string outputFilepath) = 0;
	virtual void decompress(std::string inputFilepath, std::string outputFilepath) = 0;
	
};