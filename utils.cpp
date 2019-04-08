#include "utils.h"
#include <iostream>

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

bool isZeroOrNA(std::string str)
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

bool containE(std::string str)
{
    return str.find('e') != std::string::npos || str.find('E') != std::string::npos;
}

std::string convertDouble_old(double value)  
{
  std::ostringstream o;
  if(!(o << value)) 
   return "";
  return o.str();
}

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

double Stod(std::string str)
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