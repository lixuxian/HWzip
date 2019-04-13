/*
 * @Description: 工具类，包含了一些简单的操作，如字符串分割、浮点数转字符串、字符串转浮点数等
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2019-03-29 22:45:38
 * @LastEditTime: 2019-04-13 22:53:46
 */
#ifndef __UTILS__
#define __UTILS__
#include <string>
#include <vector>
#include <sstream>

void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

bool isZeroOrNA(const std::string &str);
bool containE(const std::string &str);

std::string convertDouble_old(double value);
std::string convertDouble(double value);

double Stod(const std::string &str);


#endif