#include "fileProcess.h"
#include "utils.h"

FileProcessor::FileProcessor(std::ifstream *in, std::ofstream *out, int blockSize) : in(in), out(out), 
blockLines(blockSize), blockCount(0), columnSize(0), decompress_block_count(0)
{

}

FileProcessor::~FileProcessor()
{

}

void FileProcessor::setFileLines(int lines)
{
	this->fileLines = lines;
}

int FileProcessor::initWork()
{
	if (!in->is_open())
	{
		perror("initWork(), input file can't open");
		exit(0);
	}

	if (!out->is_open())
	{
		perror("initWork(), output file can't open");
		exit(0);
	}

	// header
	std::string header;
	getline(*in, header);

	std::vector<std::string> header_splited;
	splitString(header, header_splited, ",");

	int column_num = header_splited.size();
	columnSize = column_num;

	// metadatas
	// blocklines + cols + lossless algorith
	std::string metadatas = std::to_string(blockLines) + "," + std::to_string(column_num) + "," + std::to_string(fileLines);

	*out << metadatas << std::endl;
	*out << header << std::endl;

	return column_num;
}

void FileProcessor::getMetadata(int& blockSize, int& columnSize, int& lines, int& blocks)
{
	if (!in->is_open())
	{
		perror("getMetadata(), input file can't open");
		exit(0);
	}

	if (!out->is_open())
	{
		perror("getMetadata(), output file can't open");
		exit(0);
	}

	std::string metadata;
	getline(*in, metadata);

	std::vector<std::string> metadata_splited;
	splitString(metadata, metadata_splited, ",");

	blockSize = std::stoi(metadata_splited[0]);
	columnSize = std::stoi(metadata_splited[1]);
	lines = std::stoi(metadata_splited[2]);

	blocks = (lines - 1) / blockSize;
	if ((lines - 1) % blockSize != 0) {
		++blocks;
	}

	this->blockLines = blockSize;
	this->blockCount = blocks;
	this->fileLines = lines;
	this->columnSize = columnSize;
}

int FileProcessor::writeHeader2DecompressedFile()
{
	std::string header;
	if (getline(*in, header) )
	{
		(*out) << header << std::endl;
		return 1;
	}
	return -1;
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
			++fileLines;
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

int FileProcessor::getOneDecompressedBlock(std::vector<std::vector<std::string> > &block)
{
	int wait2decompress_lines = (fileLines - 1) - decompress_block_count * blockLines;

	if (wait2decompress_lines <= 0)
	{
		return -1; // all block decompressed
	}

	// 当前应该读取的行数
	int current_block_lines = wait2decompress_lines <= blockLines ? wait2decompress_lines : blockLines;

	std::cout << "current_block_lines = " << current_block_lines << std::endl;

	++decompress_block_count;

	std::string preData = "";

	for(size_t i = 0; i < columnSize; i++)
	{
		int similar_distance = 0;
		int similar_col = 0;

		for(size_t k = 0; k < current_block_lines; k++)
		{
			std::string data = "";
			char ch;
			while ((ch = (*in).get()) != ',')
			{

				if (ch != '\n' && ch != '\r') {
					data += ch;
				} else {//面对单个逗号和非逗号数字这样的结尾，消耗掉'\n'和'\r',暂只考虑win和linux.macbook只有"\r"
					if (ch == '\r') {
						continue;
					} else if (ch == '\n') {
						break;
					}
				}
			}
			// std::cout << "data = " << data << " pre_data = " << preData  << "data.length() = " << data.length() << std::endl;
			if (data.length() == 0) {
				block[k][i] = preData;
			}
			else
			{
				if (i > 2 && data.find("--") != std::string::npos) {
					// contain "--"
					similar_col = i - similar_distance - 1;
					std::vector<std::string> data_splited;
					splitString(data, data_splited, "--");
					int same_len = std::stoi(data_splited[1]);
					for(size_t x = k; x < k + same_len; x++)
					{
						if (x >= current_block_lines) {
							std::cout << "-- x >= current_block_lines, k = " << k << " data = " << data << std::endl;
						}
							
						block[x][i] = block[x][similar_col];
					}
					k = k + same_len - 1;
				} 
				else if (i > 2 && data.find("-") != std::string::npos)
				{
					// contain "-"
					if (data.find("E") != std::string::npos) {
						std::cout << "contain E!!!!" << std::endl;
					}
					
					std::vector<std::string> data_splited;
					splitString(data, data_splited, "-");
					if (data_splited[0] != "") {
						similar_distance = std::stoi(data_splited[0]);
						similar_col = i - similar_distance - 1;
					}
					if (data_splited.size() == 1) {
						// "0-", remain all same to similar col
						for(size_t x = k; x < current_block_lines; x++)
						{
							block[x][i] = block[x][similar_col];
						}
						break;
					}
					else // size = 2 有可能是负数
					{
						if (data_splited[0] == "") {
							// 负数
							block[k][i] = data;
							// std::cout << "222 preData = data = " << data << std::endl;
							preData = data;	
						}
						else
						{
							// "0-10", part same
							int same_len = std::stoi(data_splited.back());
							for(size_t x = k; x < k + same_len; x++)
							{
								if (x >= current_block_lines) {
									// "-63" 注意，这种情况是负数
									std::cout << "-xx x >= current_block_lines, k = " << k << " data = " << data << std::endl;
								}
								
								block[x][i] = block[x][similar_col];
							}
							k = k + same_len - 1;
						}
					}
				} 
				else 
				{
					// normal data
					// std::cout << "111 preData = data = " << data << std::endl;
					// std::cout << "k, i = " << k << " " << i << std::endl;
					// std::cout << "block[k][i] = " << block[k][i] << std::endl; 
					// std::cout << "block.size() = " << block.size() << " block[0].size() = " << block[0].size() << std::endl;
					block[k][i] = data;
					// std::cout << "222 preData = data = " << data << std::endl;
					preData = data;

				}
			}
		}
	}

	return current_block_lines;
}

int FileProcessor::writeOneBlock2DecompressedFile(std::vector<std::vector<std::string> > &block, int lines)
{
	for(size_t i = 0; i < lines; i++)
	{
		std::string line_str = "";
		for(size_t k = 0; k < columnSize - 1; k++)
		{
			line_str += block[i][k] + ",";
		}
		line_str += block[i][columnSize-1]; // last data of a line
		(*out) << line_str << std::endl;
	}
	return 1;
}
