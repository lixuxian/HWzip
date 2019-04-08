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
