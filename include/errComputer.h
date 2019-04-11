/*
 * @Description: 误差计算类，包括计算相对误差、计算误差平方和
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 18:09:06
 * @LastEditTime: 2019-04-11 11:38:45
 */
#pragma once
#include <iostream>
#include <string>
class ErrComputer
{
public:
	ErrComputer();
	~ErrComputer();

	double pwRelErr(std::string s1, std::string s2);
	double pwRelErr(double d1, double d2);
	double pwRelErr(std::string s1, double d2);
	double pwRelErr(double d1, std::string s2);

	double pwRelErrSquare(std::string s1, std::string s2);
	double pwRelErrSquare(double d1, double d2);
	
};
