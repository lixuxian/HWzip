#include "compressConf.h"
#include "utils.h"
#include <iostream>

CompressConf::CompressConf() : confFilepath("conf/config"), MAX_PW_REL_ERR(0.1), MAX_AVG_ERR(0.05), lossless_algorithm("paq"), multi_thread(true)
{

}

CompressConf::~CompressConf()
{

}

void CompressConf::readConf(std::string &confFile)
{
    if (confFile.length() > 0)
    {
        confFilepath = confFile;
    }
    std::cout << "file path = " << confFilepath << std::endl;
    std::ifstream in(confFile, std::ios::in);
    if (!in.is_open())
    {
        perror("Conf::readConf(), !in.is_open");
        exit(1);
    }
    std::string line = "";
    while (std::getline(in, line))
    {
        trim(line);
        if (line.length() == 0 || line[0] == '#')
        {
            continue;
        }
        else
        {
            std::vector<std::string> v;   
            splitString(line, v, "=");
            if (v.size() != 2)
            {
                LOG(ERROR) << "line: " << line << std::endl;
                exit(1);
            }
            trim(v[0]);
            trim(v[1]);
            if (v[0].compare("max_pw_rel_err") == 0)
            {
                MAX_PW_REL_ERR = std::stod(v[1]);
                LOG(INFO) << "MAX_PW_REL_ERR = " << MAX_PW_REL_ERR << std::endl;
            }
            else if (v[0].compare("max_avg_err") == 0)
            {
                MAX_AVG_ERR = std::stod(v[1]);
                LOG(INFO) << "MAX_AVG_ERR = " << MAX_AVG_ERR << std::endl;
            }
            else if (v[0].compare("lossless_encoding") == 0)
            {
                lossless_algorithm = v[1];
                LOG(INFO) << "lossless_algorithm = " << lossless_algorithm << std::endl;
            }
            else if (v[0].compare("multi_thread") == 0)
            {
                if (v[1].compare("true") == 0)
                {
                    multi_thread = true;
                } else {
                    multi_thread = false;
                }
                LOG(INFO) << "multi_thread = " << multi_thread << std::endl;
            }
            else if (v[0].compare("block_size") == 0)
            {
                try
                {
                    blockSize = std::stoi(v[1]);
                    LOG(INFO) << "blockSize = " << blockSize << std::endl;
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            else if (v[0].compare("remove_tmp_file") == 0)
            {
                if (v[1].compare("true") == 0)
                {
                    remove_tmp_file = true;
                } else {
                    remove_tmp_file = false;
                }
                LOG(INFO) << "remove_tmp_file = " << remove_tmp_file << std::endl;
            }
            else
            {
                LOG(WARNING) << "do not support this config: " << line << std::endl;
            }
        }   
    }
    // MAX_PW_REL_ERR = std::min(MAX_PW_REL_ERR, MAX_AVG_ERR);
    if (lossless_algorithm.compare("ppmd") == 0)
    {
        multi_thread = false;
    }
}

/**
 * @description: 从命令行参数获取配置信息
 * @param argc 参数个数
 * @param argv 命令行的参数
 * @return: int 1表示正常，-1表示命令行参数出错
 */
int CompressConf::getParas(int argc, char const *argv[])
{
	// get mode
	if (argv[1][0] == 'c')
	{
		paras.mode = COMPRESS;
	}
	else if (argv[1][0] == 'd')
	{
		paras.mode = DECOMPRESS;
	}
	else
	{
		LOG(ERROR) << "don't support this mode: " << argv[1][0] << std::endl << std::endl;
		return Help();
	}
	paras.inputFile = argv[2];
	return 1;
}