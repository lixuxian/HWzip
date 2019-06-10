#pragma once

#include <string>
#include <fstream>

enum Mode
{
	COMPRESS = 1,
	DECOMPRESS
};

class CompressConf
{
public:
    struct Paras
    {
        Mode mode;
        std::string inputFile;
    }paras;
    std::string confFilepath;
    
    double MAX_PW_REL_ERR;
	double MAX_AVG_ERR;
    std::string lossless_algorithm;
    bool multi_thread;
    bool remove_tmp_file;
    int blockSize;
    int firstDataIndex;

public:
    CompressConf();
    ~CompressConf();

    int getParas(int argc, char const *argv[]);
    void readConf(std::string &confFile);
};