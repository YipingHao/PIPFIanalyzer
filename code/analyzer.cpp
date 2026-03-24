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
    XCount = src.XCount;
    highestOrder = src.highestOrder;
    OrderCount.move(src.OrderCount);
}
size_t FIexpress::getXCount() const
{
    if (items.count() == 0)
    {
        return 0;
    }
    
    size_t maxIndex = 0;
    for (size_t i = 0; i < items.count(); i++)
    {
        if (items[i] > maxIndex)
        {
            maxIndex = items[i];
        }
    }
    
    return maxIndex;
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

void FIexpress::demo(FILE*fp) const
{
    fprintf(fp, "FIexpress: order=%d, ItemCount=%zu\n", order, ItemCount);
    if (ItemCount < 32) {
        for (size_t i = 0; i < ItemCount; ++i) {
            fprintf(fp, "  ");
            for (int j = 0; j < order; ++j) {
                size_t varIndex = items[i * order + j];
                fprintf(fp, "x[%zu]", varIndex);
                if (j < order - 1) {
                    fprintf(fp, " * ");
                }
            }
            if (i < ItemCount - 1) {
                fprintf(fp, " +");
            }
            fprintf(fp, "\n");
        }
    } else {
        fprintf(fp, "  Too many monomials (%zu), showing basic info only\n", ItemCount);
    }
    fflush(fp);
}

void FIexpress::DemoSimple(FILE*fp) const
{
    fprintf(fp, "FIexpress: order=%d, ItemCount=%zu\n", order, ItemCount);
    if (ItemCount > 0) {
        fprintf(fp, "  ");
        for (int j = 0; j < order; ++j) {
            size_t varIndex = items[j];
            fprintf(fp, "x[%zu]", varIndex);
            if (j < order - 1) {
                fprintf(fp, " * ");
            }
        }
        if (ItemCount > 1) {
            fprintf(fp, " + ...");
        }
        fprintf(fp, "\n");
    }
    fflush(fp);
}

void FIexpresses::demo(FILE*fp) const
{
    fprintf(fp, "FIexpresses: total=%zu, highestOrder=%d, XCount=%zu\n", items.size(), highestOrder, XCount);
    
    if (items.size() < 32) {
        for (size_t i = 0; i < items.size(); ++i) {
            fprintf(fp, "  FIexpress %zu:", i);
            items[i].demo(fp);
        }
    } else {
        fprintf(fp, "  Too many FIexpress objects (%zu), showing first 32 with simple info\n", items.size());
        for (size_t i = 0; i < 32; ++i) {
            fprintf(fp, "  FIexpress %zu:", i);
            items[i].DemoSimple(fp);
        }
    }
    
    // 无论什么情况都要打印 OrderCount
    fprintf(fp, "  Order distribution:\n");
    for (size_t i = 0; i < OrderCount.size(); ++i) {
        if (OrderCount[i] > 0) {
            fprintf(fp, "    Order %zu: %zu polynomials\n", i, OrderCount[i]);
        }
    }
    fflush(fp);
}

void errorinfo::demo(FILE*fp) const
{
    fprintf(fp, "line %d, %s, expect %s\n", line, msg, symbol);
    fflush(fp);
}