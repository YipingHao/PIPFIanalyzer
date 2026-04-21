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
    fprintf(fp, "FIexpress: order=%d, ItemCount=%zu, polynomial:\n\t", order, ItemCount);
    for (size_t i = 0; i < ItemCount; ++i) 
    {
            for (int j = 0; j < order; ++j) {
                size_t varIndex = items[i * order + j];
                fprintf(fp, "x[%zu]", varIndex);
                if (j + 1 < order) {
                    fprintf(fp, " * ");
                }
            }
            if (i + 1 < ItemCount) {
                fprintf(fp, " + ");
            }
            else
                fprintf(fp, ";\n");
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
            if (j + 1 < order) {
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
    fprintf(fp, "FIexpresses: total=%zu, highestOrder=%d, XCount=%zu, Order1Count=%zu\n", items.size(), highestOrder, XCount, Order1Count);
    
    size_t accLength = 0;
    for (size_t i = 0; i < items.size(); ++i) {
        size_t currentLength = items[i].order * items[i].ItemCount;
        accLength += currentLength;
        fprintf(fp, "  FIexpress %zu (length=%zu, acc=%zu):", i, currentLength, accLength);
        if (items[i].order == 1) {
            items[i].demo(fp);
        } else {
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

    if (partions.size() > 0) {
        fprintf(fp, "  Partions:\n");
        for (size_t i = 0; i < partions.size(); ++i) {
            fprintf(fp, "    FIexpress %zu partion: [", i);
            for (size_t j = 0; j < partions[i].size(); ++j) {
                fprintf(fp, "%zu", partions[i][j]);
                if (j + 1 < partions[i].size()) {
                    fprintf(fp, ", ");
                }
            }
            fprintf(fp, "]\n");
        }
    }
    
    fflush(fp);
}

void errorinfo::demo(FILE*fp) const
{
    fprintf(fp, "line %d, %s, expect %s\n", line, msg, symbol);
    fflush(fp);
}

void FIexpresses::SortByOrder(void)
{
    // 使用简单的冒泡排序对items进行排序，按照order从小到大
    for (size_t i = 0; i < items.count(); ++i) {
        for (size_t j = 0; j < items.count() - 1 - i; ++j) {
            if (items[j].getOrder() > items[j + 1].getOrder()) {
                // 交换items[j]和items[j + 1]
                FIexpress temp;
                temp.move(items[j]);
                items[j].move(items[j + 1]);
                items[j + 1].move(temp);
            }
        }
    }
}

void FIexpresses::cutoffByOrder(int max_order, bool enable_crossitem)
{
    vector<FIexpress> newItems;
    for (size_t i = 0; i < items.count(); ++i) {
        bool keep = true;
        if (items[i].getOrder() > max_order) {
            keep = false;
        }

        // Check crossitem condition
        if (keep && enable_crossitem) {
            if (i < CrossItem.size()) {
                keep = CrossItem[i];
            }
        }
        
        if (keep) {
            newItems.append(items[i]);
        }
    }
    
    // Update items
    items.move(newItems);
    
    // Recalculate highestOrder and OrderCount
    OrderAnalysis();
    
    // Re-analyze partions
    analyze();
}

void FIexpresses::cutoffByWorkload(size_t max_workload, bool enable_crossitem)
{
    vector<FIexpress> newItems;
    size_t accLength = 0;
    
    for (size_t i = 0; i < items.count(); ++i) {
        bool keep = true;
        size_t currentLength = items[i].getOrder() * items[i].getItemCount();
        
        if (accLength + currentLength > max_workload) {
            keep = false; 
        }

        // Check crossitem condition
        if (keep && enable_crossitem) {
            if (i < CrossItem.size()) {
                keep = CrossItem[i];
            }
        }
        
        if (keep) {
            newItems.append(items[i]);
            accLength += items[i].getOrder() * items[i].getItemCount();
        }
    }
    
    // Update items
    items.move(newItems);
    
    // Recalculate highestOrder and OrderCount
    OrderAnalysis();
    
    // Re-analyze partions
    analyze();
}

void FIexpresses::CrossItemAnalysis(void)
{
    CrossItem.recount(items.count());
    
    for (size_t i = 0; i < items.count(); ++i) {
        bool hasDuplicate = false;
        if (items[i].getItemCount() > 0) {
            int order = items[i].getOrder();
            const vector<size_t>& exprItems = items[i].getItems();
            
            // Just check the first monomial (first `order` elements in exprItems)
            for (int p = 0; p < order; ++p)
            {
                for (int q = p + 1; q < order; ++q) 
                {
                    if (exprItems[p] == exprItems[q]) {
                        hasDuplicate = true;
                        break;
                    }
                }
                if (hasDuplicate) break;
            }
        }
        
        CrossItem[i] = !hasDuplicate;
    }
}

void FIexpresses::analyze(void)
{
    // 该函数的目的是通过一阶不变量来划分 N 阶多项式。
    // partions[i][k] 记录了第 i 个多项式的某个代表性单项式中，
    // 包含了多少个属于第 k 个一阶多项式的自变量。
    
    SortByOrder();
    Order1Count = 0;
    partions.recount(0); // 清空以前的划分，以免反复追加
    
    for (size_t i = 0; i < items.count(); i++)
    {
        const FIexpress& expre = items[i];
        if(expre.getOrder() == 1)
        {
            Order1Count++;
        }
    }

    for (size_t i = 0; i < items.count(); i++)
    {
        vector<size_t> partion;
        const FIexpress& expre = items[i];
        size_t order = expre.getOrder();
        
        // 利用置换不变性：一阶不变量的 N 阶单项式构成了 N 阶不变量多项式的一个划分。
        // 因此，只需要检查该多项式的任何一个单项式（这里用了第一个单项式，即前 order 个变量），
        // 就可以计算出它属于哪个一阶不变量单项式的划分。
        
        partion.recount(Order1Count);
        partion.value(0);

        for (size_t j = 0; j < order; j++)
        {
            for (size_t k = 0; k < Order1Count; k++)
            {
                const FIexpress& Order1now_ = items[k];
                size_t site = Order1now_.items.search(expre.items[j]);
                if(site != (size_t)(-1))
                {
                    partion[k]++;
                    break;
                }
            }
        }
        partions.append(partion);
    }
  
    CrossItemAnalysis();
}










