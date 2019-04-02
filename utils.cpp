#include "utils.h"

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
    if (str == "NA" || str == "" || str == " " || std::stod(str) == 0.0)           
    {
        return true;
    }
    return false;
}

bool containE(std::string str)
{
    return str.find('e') != std::string::npos || str.find('E') != std::string::npos;
}

std::string convertDouble(double value)  
{
  std::ostringstream o;
  if(!(o << value)) 
   return "";
  return o.str();
}