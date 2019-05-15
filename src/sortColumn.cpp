#include "sortColumn.h"
#include "utils.h"
#include <algorithm>

void sortColumn::analyseColumn(std::vector<std::vector<std::string> > &block, int line_num, int col_index, colFeature &featrue)
{
    float min = 0.0f, max = 0.0f, mean = 0.0f;
    float sum = 0.0f;
    for (size_t i = 0; i < line_num; i++)
    {
        float num = Stod(block[i][col_index]);
        if (num == 0.0f)
        {
            continue;
        }
        else
        {
            sum += num;
            if (num > max)
            {
                max = num;
            }
            else if (num < min)
            {
                min = num;
            }
        }
    }
    mean = sum / line_num;

    featrue.min = min;
    featrue.max = max;
    featrue.mean = mean;
}

int sortColumn::sortBlock(std::vector<std::vector<std::string> > &block, int line_num, std::vector<size_t> &index)
{
    assert(line_num > 0);
    size_t colSize = block[0].size();
    assert(colSize > 0);
    
    std::vector<std::pair<size_t, colFeature> > fp(colSize);
    for (size_t i = 0; i < colSize; i++)
    {
        fp[i].first = i;
    }
    

    // first 3 col is not value data
    for (size_t i = 3; i < colSize; i++)
    {
        analyseColumn(block, line_num, i, fp[i].second);
    }
    // sort column indices
    sortByFeature(fp);
    LOG(INFO) << "complete sort index by feature" << std::endl;
    for (size_t i = 0; i < colSize; i++)
    {
        // std::cout << fp[i].first << "," << std::endl;
        index[i] = fp[i].first;
    }

}

float sortColumn::fusionFeature(colFeature cf)
{
    return 2 * cf.max - cf.min + cf.mean;
}

void sortColumn::sortByFeature(std::vector<std::pair<size_t, colFeature> > &fp)
{
    size_t colSize = fp.size();
    // skip first 3 column
    sort(fp.begin() + 3, fp.end(), [ = ](std::pair<size_t, colFeature> f1, std::pair<size_t, colFeature> f2){
        return fusionFeature(f1.second) < fusionFeature(f2.second);
    });
}

