/*
 * @Description: 数值简化类，对字符串数值进行简化处理
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-04-01 09:34:42
 * @LastEditTime: 2019-04-22 15:40:43
 */
#include <string>
#include <vector>
#include "errComputer.h"

class SimplifyData
{
public:
	SimplifyData();
	~SimplifyData();

	std::string getBestData(std::string data, double err, std::vector<int> &cFreq);
	std::string getBestDataFromInterval(std::string s1, std::string s2, std::vector<int> &cFreq);
	std::string simplifyDataCeil(std::string s1, std::string s2);
	std::string simplifyDataFloor(std::string s1, std::string s2);

	void simpData(std::string data, double err, std::string &result);
	void simplifyFloat(std::string f_data, double err, std::string &result);
	void simplifyInt(std::string i_data, double err, std::string &result);
	void restoreE0(std::string rs_data, double err, std::string &result);

private:
	void simplifyFloat_bigger_than_1(std::string data, double err, std::string &result); // 1--10
	void simplifyFloat_bigger_than_10(std::string data, double err, std::string &result); // 10--100
	void simplifyFloat_bigger_than_100(std::string data, double err, std::string &result); // > 100
	void simplifyFloat_smaller_than_1(std::string data, double err, std::string &result); // < 1

	void trimZeros(std::string str, std::string &result);

public:

	ErrComputer errComp;
};