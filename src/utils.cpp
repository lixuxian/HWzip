#include "utils.h"
#include <iostream>

/**
 * @description: 对输入字符串进行分割
 * @param s 待分割的字符串
 * @param v 用于存储分割结果
 * @param c 分割符号，根据c对s进行分割
 * @return: void
 */
void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
    {
        v.push_back(s.substr(pos1));
    }
}

/**
 * @description: 判断数值是否为0或为空
 * @param str 当前数值
 * @return: bool true表示为0或空，否则不是
 */
bool isZeroOrNA(const std::string &str)
{
    if (str == "NA" || str == "" || str == " ")           
    {
        return true;
    }
    double res ;
    try
    {
      res = std::stod(str);
      if (res == 0.0) {
        return true;
      }
    }
    catch(const std::exception& e)
    {
      std::cout << "str = " << str << std::endl;
      std::cerr << e.what() << '\n';
    }
    
    return false;
}

/**
 * @description: 判断当前数值是否包含e或E(即科学计数法)
 * @param str 当前数值 
 * @return: bool true表示包含，否则不包含
 */
bool containE(const std::string &str)
{
    return str.find('e') != std::string::npos || str.find('E') != std::string::npos;
}

/**
 * @description: 浮点数转换为字符串，已废弃，因为精度不够
 * @param value 待转换的浮点数
 * @return: string 转换后的字符串
 */
std::string convertDouble_old(double value)  
{
  std::ostringstream o;
  if(!(o << value)) 
   return "";
  return o.str();
}

/**
 * @description: 浮点数转换为字符串，保留10位小数的精度，并取出结尾不必要的0
 * @param value 待转换的浮点数
 * @return: string 转换后的字符串
 */
std::string convertDouble(double value)  
{
  std::string res;
  char buf[24] = { 0 };
  snprintf(buf, 24, "%.10f", value);
  res = buf;
  if (res.find(".") != std::string::npos)
  {
    int i = res.length() - 1;
    while (i >= 0 && res[i] == '0')
    {
      --i;
    }
    if (i >= 0 && res[i] == '.') {
        --i;
    }
    res = res.substr(0, i + 1);
  }
  return res;
}

/**
 * @description: 将字符串转换为浮点数。是对std::stod函数的封装，增加了输入检测，若输入为0或空则直接返回0.0
 * @param str 当前数值
 * @return: double 转换后的浮点数
 */
double Stod(const std::string &str)
{
  if (isZeroOrNA(str)) {
    return 0.0;
  }
  double res;
  try
  {
    res = std::stod(str);
  }
  catch(const std::exception& e)
  {
    std::cout << "STOD str = " << str << std::endl;
    std::cerr << e.what() << '\n';
  }
  return res;
}