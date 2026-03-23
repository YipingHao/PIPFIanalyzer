#include "analyzer.h"


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

int FIexpress::AppendMonomial(const vector<size_t>& monomial)
{
    // 如果items数组为空，则设置order为monomial的长度
    if (items.empty())
    {
        order = monomial.size();
    }
    // 如果items数组不为空，且monomial的长度与order不相同，则返回-1
    else if (monomial.size() != static_cast<size_t>(order) || monomial.empty())
    {
        return -1;
    }
    
    // 将单项式的指数信息追加到items数组
    items.append(monomial);
    // 增加ItemCount
    ItemCount++;
    
    return 0;
}

void FIexpresses::move(FIexpresses& src)
{
    items.move(src.items);
}

int FIexpresses::compute(const double*input, size_t ldi, size_t rowi, size_t coli, double* output, size_t ldo, size_t rowo, size_t colo) const
{
    // 检查输入输出的行数是否匹配
    if (rowi != rowo) {
        return -1;
    }
    
    // 检查列数是否与输入数据的维度匹配
    if (coli != XCount) {
        return -3;
    }
    
    // 检查输出数据的维度是否与所有FIexpress的次数总和匹配
    if (colo != items.size()) {
        return -4;
    }
    
    // 遍历每条输入数据
    for (size_t i = 0; i < rowi; ++i) {
        // 计算当前输入数据的起始位置
        const double* currentInput = input + i * ldi;
        // 计算当前输出数据的起始位置
        double* currentOutput = output + i * ldo;
        
        // 对每条数据，计算所有FIexpress对象的值
        for (size_t j = 0; j < items.size(); ++j) {
            // 计算当前FIexpress的值
            double value = items[j].compute_bare(currentInput);
            // 将计算结果存储到输出矩阵的对应位置
            currentOutput[j] = value;
        }
    }
    
    return 0;
}

void errorinfo::demo(FILE*fp) const
{
    fprintf(fp, "line %d, %s, expect %s\n", line, msg, symbol);
    fflush(fp);
}