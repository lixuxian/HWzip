#include <string>
#include <vector>
#include "errComputer.h"

class SimplifyData
{
public:
	SimplifyData();
	~SimplifyData();

	std::string getDataS0(std::string data, double err, std::vector<int> &cFreq);
	std::string getDataS(std::string s1, std::string s2, std::vector<int> &cFreq);
	std::string simplifyDataCeil(std::string s1, std::string s2);
	std::string simplifyDataFloor(std::string s1, std::string s2);

	std::string simpData(std::string data, double err);
	std::string simplifyFloat(std::string f_data, double err);
	std::string simplifyInt(std::string i_data, double err);
	std::string simplifyInt2(std::string i_data, double err);
	std::string restoreE0(std::string rs_data, double err);

private:
	std::string simplifyFloat_bigger_than_1(std::string data, double err); // 1--10
	std::string simplifyFloat_bigger_than_10(std::string data, double err); // 10--100
	std::string simplifyFloat_bigger_than_100(std::string data, double err); // > 100
	std::string simplifyFloat_smaller_than_1(std::string data, double err); // < 1

	std::string baseIntConvert2(std::string str);

public:

	ErrComputer errComp;
};