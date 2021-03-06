#include "simplifyData.h"
#include "utils.h"
#include <cmath>

SimplifyData::SimplifyData()
{

}

SimplifyData::~SimplifyData()
{

}

// 在上下限内进行字符替换，选择一个最优的数值
/**
 * @description: 对当前数据进行简化处理，在其误差区间内选择一个最优的数值
 * @param data 当前数据
 * @param err 误差阈值
 * @param cFreq 每个字符的频率 
 * @return: string 简化处理后的数据
 */
std::string SimplifyData::getBestData(std::string data, double err, std::vector<int> &cFreq)
{
	if (data == "")
	{
		return "0";
	}
	double UP = 1 + err;
	double LOW = 1 - err;
	double lTmp, uTmp;
	double data_d = Stod(data);
	if (data.find('-') != std::string::npos)
	{
		lTmp = data_d * UP;
		uTmp = data_d * LOW;
	}
	else
	{
		lTmp = data_d * LOW;
		uTmp = data_d * UP;
	}

	// TODO
	// 两个函数可以合并为一个
	std::string lTmp_str, uTmp_str;
	convertDouble(lTmp, lTmp_str);
	convertDouble(uTmp, uTmp_str);
	std::string ls = simplifyDataCeil(lTmp_str, uTmp_str);
	std::string us = simplifyDataFloor(	lTmp_str, uTmp_str);

	return getBestDataFromInterval(ls, us, cFreq);
}

/**
 * @description: 从误差允许的区间中，选择一个长度最短、字符频率最高的字符串，替换当前数据
 * @param ls 区间下限
 * @param us 区间上线
 * @param cFreq 字符频率
 * @return: string 处理后的数据
 */
std::string SimplifyData::getBestDataFromInterval(std::string ls, std::string us, std::vector<int> &cFreq)
{
	int ls_len = ls.length();
	int us_len = us.length();
	if (ls_len > us_len)
	{
		return us;
	}
	if (ls_len < us_len)
	{
		return ls;
	}

	std::string lsTmp; 
	trimZeros(ls, lsTmp); // 去除头尾的0
	std::string usTmp; 
	trimZeros(us, usTmp);

	// 若有个数比较短，则返回短的
	if (lsTmp.length() > usTmp.length())
	{
		return us;
	}
	else if (lsTmp.length() < usTmp.length())
	{
		return ls;
	}

	// 两个数长度相同时
	int lInt = 0, uInt = 0;
	int dotInL = -1, dotInU = -1;
	if (lsTmp.find(".") != std::string::npos)
	{
		std::string l = lsTmp;
		dotInL = l.find(".");
		l.erase(dotInL, 1);
		lInt = std::stoi(l);
	} 
	else
	{
		lInt = std::stoi(lsTmp);
	}

	if (usTmp.find(".") != std::string::npos)
	{
		std::string u = usTmp;
		dotInU = u.find(".");
		u.erase(dotInU, 1);
		uInt = std::stoi(u);
	}
	else
	{
		uInt = std::stoi(usTmp);
	}

	if (dotInL != dotInU)
	{
		std::cout << "getBestDataFromInterval(): dotInL != dotInU" << std::endl;
		return ls;
	}

	if (lInt > uInt) // 负数区间
	{
		int tmp = lInt;
		lInt = uInt;
		uInt = tmp;
	}

	std::string result = ls;
	int freq_count = 0;

	for (int i = lInt; i <= uInt; ++i)
	{
		std::string sTmp = std::to_string(i);
		int freq = 0;
		for (int j = 0; j < sTmp.length(); ++j)
		{
			// cout << "ch = " << sTmp[j] - '0' << endl;
			freq += cFreq[sTmp[j] - '0'];
		}

		if (freq > freq_count)
		{
			freq_count = freq;
			if (dotInL > 0) // 小数
			{
				if (dotInL != dotInU)
				{
					// dotInL != dotInU
					return result;
				}
				sTmp.insert(dotInL, ".", 1);
			}
			// 把us中的usTmp换成sTmp
			// result = sTmp;
			std::string u_str = us;
			result = u_str.replace(us.find(usTmp), usTmp.length(), sTmp);
			if (Stod(ls) > Stod(result) || Stod(us) < Stod(result))
			{
				// out of bound
				// std::cout << "getBestDataFromInterval(), result out of bound" << std::endl;
				return ls;
			}
		}
	}
	return result;
}

/**
 * @description: 去除数值头尾的不必要的0和负号，如10.12000变为10.12，0.123变成123
 * @param str 待处理数据
 * @return: string 处理后的数据
 */
void SimplifyData::trimZeros(std::string str, std::string &result)
{
	int start = 0;
	int end = str.length() - 1;
	while(start <= end && !(str[start] >= '1' && str[start] <= '9'))
	{
		++start;
	}
	while(end >= 0 && !(str[end] >= '1' && str[end] <= '9'))
	{
		--end;
	}
	result = str.substr(start, end - start + 1);
	if (result == "")
	{
		result = "0";
	}
}


/**
 * @description: 简化区间下限值
 * @param s1 原区间下限
 * @param s2 原区间上限
 * @return: string 简化后的区间下限
 */
std::string SimplifyData::simplifyDataCeil(std::string s1, std::string s2)
{
	double lower, upper;

	if (isZeroOrNA(s1))
	{
		lower = 0.0;
	}
	else
	{
		lower = Stod(s1);
	}
	if (isZeroOrNA(s2))
	{
		upper = 0.0;
	}
	else
	{
		upper = Stod(s2);
	}

	double lowerTmp = lower;

	char flag_big_than_1 = (std::abs(lower) > 1);

	int dotI = s1.length();
	if (s1.find('.') != std::string::npos)
	{
		dotI = s1.find('.');
	}

	int first_non_zero_index = 0;

	for (int i = 0; i < s1.length(); ++i)
	{
		if (s1[i] >= '1' && s1[i] <= '9')
		{
			first_non_zero_index = i;
			break;
		}
	}

	double base = 0;
	for (int i = flag_big_than_1; i < s1.length() + 1; ++i)
	{
		base = std::pow(10, dotI - first_non_zero_index - i);
		lowerTmp = base * std::ceil(lower / base);
		if (lowerTmp >= lower && lowerTmp <= upper)
		{
			std::string res;
			convertDouble(lowerTmp, res);
			return res;
		}
	}
	return s1;
}

/**
 * @description: 简化区间上限值
 * @param s1 原区间下限
 * @param s2 原区间上限
 * @return: string 简化后的区间上限
 */
std::string SimplifyData::simplifyDataFloor(std::string s1, std::string s2)
{
	double lower, upper;

	if (isZeroOrNA(s1))
	{
		lower = 0.0;
	}
	else
	{
		lower = Stod(s1);
	}
	if (isZeroOrNA(s2))
	{
		upper = 0.0;
	}
	else
	{
		upper = Stod(s2);
	}

	double upTmp = lower;

	char flag_big_than_1 = (std::abs(upper) > 1);

	int dotI = s2.length();
	if (s2.find('.') != std::string::npos)
	{
		dotI = s2.find('.');
	}

	int first_non_zero_index = 0;

	for (int i = 0; i < s2.length(); ++i)
	{
		if (s2[i] >= '1' && s2[i] <= '9')
		{
			first_non_zero_index = i;
			break;
		}
	}

	double base = 0;
	for (int i = flag_big_than_1; i < s2.length() + 1; ++i)
	{
		base = std::pow(10, dotI - first_non_zero_index - i);
		upTmp = base * std::floor(upper / base);
		if (upTmp >= lower && upTmp <= upper)
		{
			std::string res;
			convertDouble(upTmp, res);
			return res;
		}
	}
	return s2;
}

/**
 * @description: 简化当前数值
 * @param data 当前数值
 * @param err 误差阈值
 * @return: string 简化后的数值
 */
void SimplifyData::simpData(std::string data, double err, std::string &result)
{
	
	if (data.find('e') != std::string::npos || data.find('E') != std::string::npos)
	{
		restoreE0(data, err, result);
	}
	else if (data.find('.') != std::string::npos)
	{
		simplifyFloat(data, err, result);
	}
	else
	{
		simplifyInt(data, err, result);
	}
}

/**
 * @description: 简化浮点数
 * @param f_data 原浮点数值
 * @param err 误差阈值
 * @return: string 简化后的浮点数值
 */
void SimplifyData::simplifyFloat(std::string f_data, double err, std::string &result)
{
	int flag = 0;
	if (f_data[0] == '-')
	{
		flag = 1; // 负数
	}
	double abs_f_data = std::abs(Stod(f_data));
	if (abs_f_data > 100)
	{
		// result = simplifyFloat_bigger_than_100(f_data, err);
		simplifyFloat_bigger_than_100(f_data, err, result);
	}
	else if (abs_f_data > 10)
	{
		simplifyFloat_bigger_than_10(f_data, err, result);
	}
	else if (abs_f_data > 1)
	{
		simplifyFloat_bigger_than_1(f_data, err, result);
	}
	else
	{
		simplifyFloat_smaller_than_1(f_data, err, result);
	}
}

/**
 * @description: 从高位到低位依次四舍五入，取满足误差条件的
 * @param i_data 原整型数值
 * @param err 误差阈值
 * @return: string 简化后的整型数值
 */
void SimplifyData::simplifyInt(std::string i_data, double err, std::string &result)
{
	// std::string result = i_data;
	result = i_data;
	int isNav = 0;
	if (i_data.find('.') != std::string::npos)
	{
		isNav = 1;
	}

	int len = i_data.length();
	if (len - isNav == 1) // 个位数
	{
		// return i_data;
		result = i_data;
	}

	// 2位以上,从高位第2位开始四舍五入
	for (int i = len - isNav - 1; i > 0; --i)
	{
		long e = (long)std::pow(10, i);
		long round = (long)std::round(Stod(i_data) / e) * e;
		if (errComp.pwRelErr(i_data, round) <= err)
		{
			result = std::to_string(round);
			break;
		}
	}

	//变成叠数，如210=200,45=44
	if (result[1+isNav] != '0')
	{
		std::string tmp = result;
		tmp[1+isNav] = i_data[0+isNav];
		if (errComp.pwRelErr(i_data, tmp) <= err)
		{
			result = tmp;
		}
	}
	return;
}

/**
 * @description: 对包含e的数字进行处理
 * @param e_data 包含e的数字
 * @param err 误差阈值
 * @return: string 处理后的数值
 */
void SimplifyData::restoreE0(std::string e_data, double err, std::string &result)
{
	// std::string result = "";
	result = "";
	std::vector<std::string> splited_data;
	if (e_data.find("E") != std::string::npos) {
		splitString(e_data, splited_data, "E");
	} else if (e_data.find("e") != std::string::npos)
	{
		splitString(e_data, splited_data, "e");
	}
	if (splited_data[0].find('.') != std::string::npos)
	{
		// splited_data[0] = simplifyFloat(splited_data[0], err);
		simplifyFloat(splited_data[0], err, splited_data[0]);
	}
	else
	{
		// splited_data[0] = simplifyInt(splited_data[0], err);
		simplifyInt(splited_data[0], err, splited_data[0]);
	}
	result = splited_data[0] + "E" + splited_data[1];
	std::string result1 = result;
	
	convertDouble(Stod(result), result);
	
	// return result;
}

/**
 * @description: 处理1-10之间的浮点数
 * @param data 原始数值
 * @param err 误差阈值
 * @return: string 简化后的数值
 */
void SimplifyData::simplifyFloat_bigger_than_1(std::string data, double err, std::string &result)
{
	int flag = 0;
	if (data[0] == '-')
	{
		flag = 1;
	}

	int res = std::round(Stod(data)); // 不保留小数
	if (errComp.pwRelErr(data, res) <= err)
	{
		// std::string res_str;
		convertDouble(res, result);
		return;
	}

	{
		// 叠数
		std::string tmp = data.substr(0, 3 + flag);
		tmp[2+flag] = data[0+flag];
		if (errComp.pwRelErr(data, tmp) <= err)
		{
			result = tmp;
			return;
		}
	}

	double round = (double)(std::round(Stod(data) * 10) / 10); // 保留一位小数
	if (errComp.pwRelErr(data, round) <= err)
	{
		convertDouble(round, result);
		return;
	}
	else
	{
		round = (double)(std::round(Stod(data) * 100) / 100);
		convertDouble(round, result);
		return;
	}
}

/**
 * @description: 处理10-100之间的浮点数
 * @param data 原始数值
 * @param err 误差阈值
 * @return: string 简化后的数值
 */
void SimplifyData::simplifyFloat_bigger_than_10(std::string data, double err, std::string &result)
{
	int flag = 0;
	if (data[0] == '-')
	{
		flag = 1;
	}

	int round1 = std::round(Stod(data) / 10) * 10; // 保留到10位
	if (errComp.pwRelErr(data, (double)round1) > err)
	{
		// 叠数
		{
			// int intTmp = (int)Stod(data);
			std::string dataTmp = data;
			dataTmp[1+flag] = data[0+flag];
			if (errComp.pwRelErr(data, dataTmp) <= err)
			{
				result = dataTmp;
				return;
			}
		}

		round1 = std::round(Stod(data));
		if (errComp.pwRelErr(data, (double)round1) > err)
		{
			convertDouble(std::round(Stod(data) * 10) / 10, result);
			return;
		}

	}
	convertDouble(round1, result);
	return;
}

/**
 * @description: 处理100以上的浮点数，保留到十位或者四舍五入或者约去小数
 * @param data 原始数值
 * @param err 误差阈值
 * @return: string 简化后的数值
 */
void SimplifyData::simplifyFloat_bigger_than_100(std::string data, double err, std::string &result)
{
	// std::string result = data.substr(0, data.find('.'));
	result = data.substr(0, data.find('.'));
	// result = simplifyInt(result, err);
	simplifyInt(result, err, result);
	if (errComp.pwRelErr(data, result) > err)
	{
		int tmp = std::round(Stod(data) / 10) * 10;
		if (errComp.pwRelErr(data, tmp) <= err)
		{
			result = std::to_string(tmp);
		}
		else
		{
			result = std::to_string(std::round(Stod(data)));
		}
	}
	return;
}

/**
 * @description: 处理小于1的浮点数
 * @param data 原始数值
 * @param err 误差阈值
 * @return: string 简化后的数值
 */
void SimplifyData::simplifyFloat_smaller_than_1(std::string data, double err, std::string &result)
{
	if (isZeroOrNA(data)) {
		result = "0";
		return;
	}

	// std::string result;

	int flag = 0;
	if (data[0] == '-')
	{
		flag = 1;
	}

	int b; // 第一个非0数字,在小数点后第b位
	for ( b = 1 + data.find('.'); b < data.length(); ++b)
	{
		if (data[b] != '0')
		{
			break;
		}
	}

	if (b == 1)
	{
		int round = std::round(Stod(data));
		if (errComp.pwRelErr(data, round) <= err)
		{
			result = std::to_string(round);
			return;
		}
	}

	//以下判断保留非0数字后的第0、1、2位是否在误差允许范围之内，0.088889=0.09,0.01254
	for (int i = 0; i < 3; ++i)
	{
		int num = (int)std::pow(10, b + i);
		double round = (double)(std::round(Stod(data) * num) / num);
		if (errComp.pwRelErr(data, round) <= err)
		{
			convertDouble(round, result);
			break;
		}
	}

	//对小数部分非0数字采用叠数，如0.012=0.011
	if (b + 2 + flag < result.length()  && result[b + 2 + flag] != '0')
	{
		std::string tmp = result;
		tmp[2 + b + flag] = data[b + 1 + flag];
		if (errComp.pwRelErr(data, tmp) <= err)
		{
			result = tmp;
			return;
		}
	}

	if (Stod(result) == 0.0)
	{
		result = "0";
		return;
	}
	return;
}