#include "errComputer.h"
#include <cmath>

ErrComputer::ErrComputer()
{

}

ErrComputer::~ErrComputer()
{

}

double ErrComputer::pwRelErr(std::string s1, std::string s2)
{
	double d1 = std::stod(s1);
	double d2 = std::stod(s2);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

double ErrComputer::pwRelErr(double d1, double d2)
{
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

double ErrComputer::pwRelErr(std::string s1, double d2)
{
	double d1 = std::stod(s1);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

double ErrComputer::pwRelErr(double d1, std::string s2)
{
	double d2 = std::stod(s2);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::abs((d1 - d2) / d1);
}

double ErrComputer::pwRelErrSquare(std::string s1, std::string s2)
{
	double d1 = std::stod(s1);
	double d2 = std::stod(s2);
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::pow((d1 - d2) / d1, 2);
}

double ErrComputer::pwRelErrSquare(double d1, double d2)
{
	if (d1 == 0.0 || d2 == 0.0)
	{
		return 0.0;
	}
	return std::pow((d1 - d2) / d1, 2);
}