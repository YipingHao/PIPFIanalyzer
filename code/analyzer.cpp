#include "analyzer.h"
#include "sheet.h"

using namespace analyzer;

void FIexpress::move(FIexpress& src)
{
    order = src.order;
    ItemCount = src.ItemCount;
    items.move(src.items);
}
double FIexpress::compute_bare(const vector<double>& values) const
{
    double result = 0.0;
    // 遍历每个单项式
    for (size_t i = 0; i < ItemCount; ++i)
    {
        double term = 1.0;  // 当前单项式的值，系数为1.0
        // 每个单项式占order个元素，每个元素是变量的索引
        for (int j = 0; j < order; ++j)
        {
            size_t varIndex = items[i * order + j];  // 获取变量索引
            term *= values[varIndex];                // 乘以对应变量的值
        }
        result += term;  // 累加单项式到结果
    }
    return result;
}
double FIexpress::compute_bare(const double* values) const
{
    double result = 0.0;
    // 遍历每个单项式
    for (size_t i = 0; i < ItemCount; ++i)
    {
        double term = 1.0;  // 当前单项式的值，系数为1.0
        // 每个单项式占order个元素，每个元素是变量的索引
        for (int j = 0; j < order; ++j)
        {
            size_t varIndex = items[i * order + j];  // 获取变量索引
            term *= values[varIndex];                // 乘以对应变量的值
        }
        result += term;  // 累加单项式到结果
    }
    return result;
}


void FIexpresses::move(FIexpress& src)
{
    items.move(src.items);
}