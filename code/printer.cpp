#include "analyzer.h"
#include <cstdio>

using namespace analyzer;

int FIexpresses::printCcode(FILE* fp) const {
    // 检查文件指针是否有效
    if (fp == NULL) {
        return -1;
    }
    
    // 输出函数声明
    fprintf(fp, "void compute(const double* input, double* output)\n");
    fprintf(fp, "{\n");
    
    // 遍历每个FIexpress对象
    for (size_t i = 0; i < items.size(); ++i) {
        const FIexpress& expr = items[i];
        int order = expr.getOrder();
        size_t itemCount = expr.getItemCount();
        const vector<size_t>& exprItems = expr.getItems();
        
        // 输出计算当前多项式的代码
        fprintf(fp, "    // 计算第 %zu 个多项式\n", i);
        fprintf(fp, "\tdouble temp = 0;\n");
        
        // 遍历每个单项式
        for (size_t j = 0; j < itemCount; ++j) {
            fprintf(fp, "\t\ttemp += ");
            
            // 遍历单项式的每个变量
            for (int k = 0; k < order; ++k) {
                size_t varIndex = exprItems[j * order + k];
                if (k > 0) {
                    fprintf(fp, " * ");
                }
                fprintf(fp, "input[%zu]", varIndex);
            }
            
            fprintf(fp, ";\n");
        }
        
        fprintf(fp, "\toutput[%zu] = temp;\n", i);
    }
    
    // 输出函数结束
    fprintf(fp, "}\n");
    
    return 0;
}

int FIexpresses::printFortrancode(FILE* fp) const {
    // 检查文件指针是否有效
    if (fp == NULL) {
        return -1;
    }
    
    // 输出函数声明（Fortran风格）
    fprintf(fp, "subroutine compute(input, output)\n");
    fprintf(fp, "    implicit none\n");
    fprintf(fp, "    real*8, intent(in) :: input(:)\n");
    fprintf(fp, "    real*8, intent(out) :: output(:)\n");
    fprintf(fp, "    real*8 :: temp\n");
    fprintf(fp, "    integer :: i\n");
    fprintf(fp, "\n");
    
    // 遍历每个FIexpress对象
    for (size_t i = 0; i < items.size(); ++i) {
        const FIexpress& expr = items[i];
        int order = expr.getOrder();
        size_t itemCount = expr.getItemCount();
        const vector<size_t>& exprItems = expr.getItems();
        
        // 输出计算当前多项式的代码
        fprintf(fp, "    ! 计算第 %zu 个多项式\n", i);
        fprintf(fp, "    temp = 0.0d0\n");
        
        // 遍历每个单项式
        for (size_t j = 0; j < itemCount; ++j) {
            fprintf(fp, "    temp = temp + ");
            
            // 遍历单项式的每个变量（Fortran数组索引从1开始）
            for (int k = 0; k < order; ++k) {
                size_t varIndex = exprItems[j * order + k];
                if (k > 0) {
                    fprintf(fp, " * ");
                }
                fprintf(fp, "input(%zu)", varIndex + 1); // Fortran索引从1开始
            }
            
            fprintf(fp, "\n");
        }
        
        fprintf(fp, "    output(%zu) = temp\n", i + 1); // Fortran索引从1开始
        fprintf(fp, "\n");
    }
    
    // 输出函数结束
    fprintf(fp, "end subroutine compute\n");
    
    return 0;
}
