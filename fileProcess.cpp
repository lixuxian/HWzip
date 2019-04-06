#include "fileProcess.h"
#include "utils.h"

FileProcessor::FileProcessor(std::ifstream *in, std::ofstream *out, int blockSize) : in(in), out(out), blockLines(blockSize)
{
	tempFilepath = "temp/temp_col.csv";
	std::ofstream temp(tempFilepath.c_str(), std::ios::out | std::ios::trunc);
	if (!temp.is_open())
	{
		perror("open temp file error");
		exit(0);
	}
}

FileProcessor::~FileProcessor()
{

}

int FileProcessor::initWork()
{
	if (!in->is_open())
	{
		perror("input file can't open");
		exit(0);
	}

	if (!out->is_open())
	{
		perror("output file can't open");
		exit(0);
	}

	// header
	std::string header;
	getline(*in, header);

	*out << header << std::endl;

	std::vector<std::string> header_splited;
	splitString(header, header_splited, ",");


	return header_splited.size();

}

int FileProcessor::storeHeader()
{
	return 1;
}

int FileProcessor::storeTimeAndId()
{
	return 1;
}

// return line num of the block
int FileProcessor::getOneBlock(std::vector<std::vector<std::string> > &block)
{
	int i = 0;

	for (; i < blockLines; ++i)
	{
		std::string line;
		std::vector<std::string> line_splited;
		// std::vector<std::string> line_splited;
		if(getline(*in, line))
		{
			// std::cout << "get one line" << i << std::endl;
			if (line[line.length()-1] == '\n' || line[line.length()-1] == '\r')
	        {
	            line[line.length()-1] = '\0';
	        }
			splitString(line, line_splited, ",");
			block[i] = line_splited;
			for (int x = 0; x < block[i].size(); ++x)
			{
				if (block[i][x] == "")
				{
					block[i][x] = "0";
				}
			}
		}
		else
		{
			return i;
		}
	}
	return i;
}

void FileProcessor::initBlock(std::vector<std::vector<std::string> > &block)
{
	int rowN = block.size();
	int colN = block[0].size();
	for (int i = 0; i < rowN; ++i)
	{
		for (int j = 0; j < colN; ++j)
		{
			block[i][j] = "0";
		}
	}
}

int FileProcessor::writeOneBlock2Tempfile(std::string &lossless_str)
{
	if (lossless_str == "") {
		perror("FileProcessor::writeOneBlock2Tempfile(), lossless_str == NULL");
		return -1;
	}

	*out << lossless_str;
	
	return 1;
}


int FileProcessor::writeOneBlock2Tempfile(std::vector<std::vector<std::string> > &block, int line_num)
{
	// 
	std::cout << "writeOneBlock2Tempfile()..." << std::endl;
	// int rowN = block.size();
	int rowN = line_num;
	int colN = block[0].size();

	for (int i = 0; i < rowN; ++i)
	{
		for (int j = 0; j < colN - 1; ++j)
		{
			*out << block[i][j] << ",";
		}
		*out << block[i][colN-1] << std::endl;
	}
	// clear current block for next step
	initBlock(block);
	std::cout << "finish writeOneBlock2Tempfile..." << std::endl;
	return 1;
}

