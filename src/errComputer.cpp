/*
 * @Description: 误差计算类的实现
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 18:09:06
 * @LastEditTime: 2019-05-15 17:29:19
 */
#include "errComputer.h"
#include "utils.h"
#include <cmath>

ErrComputer::ErrComputer()
{

}

ErrComputer::~ErrComputer()
{

}
/**
 * @description: 计算两个string表示的数值的相对误差
 * @param s1 第一个字符串 
 * @param s2 第二个字符串
 * @return: double 表示相对误差
 */
double ErrComputer::pwRelErr(std::string s1, std::string s2)
{
	double d1 = Stod(s1);
	double d2 = Stod(s2);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

/**
 * @description: 计算两个double表示的数值的相对误差
 * @param d1 第一个double数值
 * @param d2 第二个double数值
 * @return: double 表示相对误差
 */
double ErrComputer::pwRelErr(double d1, double d2)
{
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

/**
 * @description: 计算一个string和一个double表示的数值的相对误差
 * @param s1 第一个string数值
 * @param d2 第二个double数值
 * @return: double 表示相对误差
 */
double ErrComputer::pwRelErr(std::string s1, double d2)
{
	double d1 = Stod(s1);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

/**
 * @description: 计算一个double和一个string表示的数值的相对误差
 * @param d1 第一个double数值
 * @param s2 第二个string数值
 * @return: double 表示相对误差
 */
double ErrComputer::pwRelErr(double d1, std::string s2)
{
	double d2 = Stod(s2);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

/**
 * @description: 计算两个string表示的数值的误差平方和
 * @param s1 第一个string数值
 * @param s2 第二个string数值
 * @return: double 表示误差平方和
 */
double ErrComputer::pwRelErrSquare(std::string s1, std::string s2)
{
	double d1 = Stod(s1);
	double d2 = Stod(s2);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::pow((d1 - d2) / d1, 2);
}

/**
 * @description: 计算两个double表示的数值的误差平方和
 * @param d1 第一个double数值
 * @param d2 第二个double数值
 * @return: double 表示误差平方和
 */
double ErrComputer::pwRelErrSquare(double d1, double d2)
{
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::pow((d1 - d2) / d1, 2);
}