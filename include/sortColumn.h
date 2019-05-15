#pragma once
#include <vector>
#include <string>

struct colFeature {
    float max = 0.0f;
    float min = 0.0f;
    float mean = 0.f;
};

class sortColumn
{
private:
    static void analyseColumn(std::vector<std::vector<std::string> > &block, int line_num, int col_index, colFeature &featrue);
    static void sortByFeature(std::vector<std::pair<size_t, colFeature> > &fp);
    static float fusionFeature(colFeature cf);
public:
    static int sortBlock(std::vector<std::vector<std::string> > &block, int line_num, std::vector<size_t> &index);
};