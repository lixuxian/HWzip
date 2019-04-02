#include "mixCompressor.h"
#include <string>
#include <vector>
#include <iostream>

int Help() {
  printf("hwzip v1.0\n");
  printf("How to use hwzip:\n");
  printf("    compress:           hwzip c [input] [output] [max_pw_rel_err] [max_avg_err]\n");
  printf("    decompress:         hwzip d [input] [output]\n");
  return -1;
}

struct Paras
{
	Mode mode;
	double MAX_PW_REL_ERR;
	double MAX_AVG_ERR;
	std::string inputFile;
	std::string outputFile;
} paras;

int getParas(int argc, char const *argv[])
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
		return Help();
	}
	paras.inputFile = argv[2];
	paras.outputFile = argv[3];

	if (argc == 6)
	{
		paras.MAX_PW_REL_ERR = atof(argv[4]);
		paras.MAX_AVG_ERR = atof(argv[5]);	
	}
	return 1;
}

int main(int argc, char const *argv[])
{
	std::cout << "argc = " << argc << std::endl;
	if (argc != 6 && argc != 4)
	{
		return Help();
	}


	if (getParas(argc, argv)  == -1)
	{
		exit(0);
	}

	

	MixCompressor *mixComp = new MixCompressor(paras.MAX_PW_REL_ERR, paras.MAX_AVG_ERR
		, paras.inputFile, paras.outputFile, paras.mode);

	mixComp->run();

	std::cout << "run() finish" << std::endl;
	
	delete mixComp;


	return 0;
}