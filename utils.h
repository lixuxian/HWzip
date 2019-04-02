#ifndef __UTILS__
#define __UTILS__
#include <string>
#include <vector>
#include <sstream>

void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

bool isZeroOrNA(std::string str);
bool containE(std::string str);

std::string convertDouble(double value);


#endif