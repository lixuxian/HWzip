/*
 * @Description: 误差测算程序
 * @Author: lixuxian
 * @LastEditor: lixuxian
 * @Date: 2018-12-17 10:38:04
 * @LastEditTime: 2019-04-14 14:24:09
 */
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <iomanip>

using namespace std;

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
double cal_point_error(string sf1, string sf2);

/**
 * @description: 对string进行分割
 * @param s 待分割的字符串
 * @param v 存放字符串分割结果
 * @param c 根据c进行分割
 * @return: void
 */
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
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
 * @description: 计算两个数值的相对误差
 * @param sf1 原数值
 * @param sf2 解压后的数值
 * @return: double 相对误差
 */
double cal_point_error(string sf1, string sf2)
{
	// sf1: origin data
	// sf2: decompressed data
	if (sf1 == "" || sf2 == "" || sf1.compare("NA") == 0) {
		return 0.0;
	}

	double f1 = (double)atof(sf1.c_str());
	double f2 = (double)atof(sf2.c_str());

	if (f1 == 0.0 && f2 == 0.0)
	{
		return 0.0;
	}
	// double err = fabs((f1 - f2) / f1);
	double err = (f1 - f2) / f1;
	if (isnan(err))
	{
		cout << "nan: s1 = " << sf1 << " s2 = " << sf2 << endl;
	}
	return err;
}

int main(int argc, char **argv)
{
	char ch;
	bool head = true;

	if (argc != 5)
	{
		cout << "main(): argc != 5" << endl;
		cout << "example: ./cal_error compressed_file decompressed_file max_rel_err max_avg_err" << endl;
		exit(0);
	}
	string compressed_file = argv[1];
	string decompressed_file = argv[2];


	double max_pw_rel_threshold = stod(argv[3]);
	double max_avg_threshold = stod(argv[4]);

	// cout << compressed_file << " vs " << decompressed_file << endl;

    ifstream com_file(compressed_file.c_str());
    ifstream decom_file(decompressed_file.c_str());
    if (!com_file || !decom_file)
    {
        cout << "main(): open file error!" << endl;
        exit(0);
    }

	string tmp1, tmp2;
	std::vector< std::vector<double> > err;

	if (head)
	{
		getline(com_file, tmp1);
		getline(decom_file, tmp2);
	}

	std::vector<string> head_splited;
	SplitString(tmp1, head_splited, ",");

	std::vector<string> de_head_splited;
	SplitString(tmp2, de_head_splited, ",");

	if (head_splited.size() != de_head_splited.size())
	{
		cout << "head_splited.size() != de_head_splited.size()" << endl;
		cout << head_splited.size() << " vs " << de_head_splited.size() << endl;
		exit(0);
	}

	int colN = head_splited.size();

	int line = 0;
	while(getline(com_file, tmp1) && getline(decom_file, tmp2)) {

		line++;
		// cout << "line = " << line << endl;

		std::vector<string> v1;
		std::vector<string> v2;

		SplitString(tmp1, v1, ",");
		SplitString(tmp2, v2, ",");

		size_t size1 = v1.size();
		size_t size2 = v2.size();

		if (size1 < colN)
		{
			for (int i = 0; i < colN - size1; ++i)
			{
				v1.push_back("0");
			}
		}

		if (size2 < colN)
		{
			for (int i = 0; i < colN - size2; ++i)
			{
				v2.push_back("0");
			}
		}

		// cout << "size1 = " << size1 << " size2 = " << size2 << endl;
		size1 = v1.size();
		size2 = v2.size();
		if (size1 != size2)
		{
			cout << "csv1.size() != csv2.size()" << endl;
			cout << "size1 = " << size1 << " size2 = " << size2 << endl;
			cout << "line = " << line << endl;

			exit(0);
		}

		
		// double point_err = cal_point_error(tmp1, tmp2);	
		std::vector<double> err_row;
		for (int i = 3; i < size1; ++i)
		{
			double tmp_err = cal_point_error(v1[i], v2[i]);
			if (tmp_err - max_pw_rel_threshold > 10e-6)
			{
				cout << " col = " << i << " line = " << line << " v1[i] = " << v1[i] << " v2[i] = " << v2[i] << " err = " << tmp_err << endl;
			}
			err_row.push_back(tmp_err);
		}
		err.push_back(err_row);
	}

	size_t row_num = err.size();
	size_t col_num = err[0].size();

	double total_max_err = 0.0;
	double total_err_sum = 0.0;
	double total_avg_err = 0.0;

	cout << setw(10) << "col_num\t" << setw(10) << "avg_err\t" << setw(10) << "max_err\t"<< endl;

	for (int i = 0; i < col_num; ++i)
	{
		double err_sum = 0.0f;
		double max_err = 0.0f;
		for (int j = 0; j < row_num; ++j)
		{
			double pt_err = err[j][i];

			if (isnan(pt_err))
			{
				// cout << "pt_err is nan" << endl;
				continue;
			}
			if (pt_err > max_err)
			{
				max_err = pt_err;
			}
			err_sum += pow(pt_err, 2);
		}
		if (max_err > total_max_err) {
			total_max_err = max_err;
		}
		total_err_sum += err_sum;
		// cout << "col_num[i] err_sum = " << err_sum << " col_num[i] avg_err = " << sqrt(err_sum / row_num) << " max_err = " << max_err << endl;
		cout << setw(10) << i << "\t" << setw(10) << sqrt(err_sum / row_num) << "\t" << setw(10) << max_err << endl;
	}
	cout << "\ntotal max err = " << std::fixed << total_max_err << endl;
	total_avg_err = total_err_sum / col_num / row_num;
	cout << "total avg err = " << std::fixed << total_avg_err << endl;
	return 0;
}
