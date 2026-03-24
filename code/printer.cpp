#include "analyzer.h"
#include <cstdio>

using namespace analyzer;

/*
 * 生成C语言代码，用于计算PIP多项式的值
 * 
 * 生成的代码布局效果：
 * void compute(const double* input, double* output)
 * {
 *     // 计算第0个多项式
 *     double temp;
 *     temp = 0.0;
 *         temp += input[0] * input[1];
 *         temp += input[2] * input[3];
 *     output[0] = temp;
 *     
 *     // 计算第1个多项式
 *     temp = 0.0;
 *         temp += input[0] * input[2];
 *     output[1] = temp;
 *     
 *     ...
 * }
 * 
 * 特点：
 * - 每个多项式使用独立的临时变量temp累加所有单项式
 * - 每个单项式单独一行，避免单行过长
 * - 使用C风格的数组索引（从0开始）
 * - 使用双精度浮点数进行计算
 */
int FIexpresses::printCcode(FILE* fp) const {
    // 检查文件指针是否有效
    if (fp == NULL) {
        return -1;
    }
    
    // 输出函数声明
    fprintf(fp, "void compute(const double* input, double* output)\n");
    fprintf(fp, "{\n");
    fprintf(fp, "\tdouble temp = 0.0;\n");
    // 遍历每个FIexpress对象
    for (size_t i = 0; i < items.size(); ++i) {
        const FIexpress& expr = items[i];
        int order = expr.getOrder();
        size_t itemCount = expr.getItemCount();
        const vector<size_t>& exprItems = expr.getItems();
        
        // 输出计算当前多项式的代码
        fprintf(fp, "    // 计算第 %zu 个多项式\n", i);
        fprintf(fp, "\ttemp = 0.0;\n");
        
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

/*
 * 生成Fortran语言代码，用于计算PIP多项式的值
 * 
 * 生成的代码布局效果：
 * subroutine compute(input, output)
 *     implicit none
 *     real*8, intent(in) :: input(:)
 *     real*8, intent(out) :: output(:)
 *     real*8 :: temp
 *     integer :: i
 *     
 *     ! 计算第0个多项式
 *     temp = 0.0d0
 *     temp = temp + input(1) * input(2)
 *     temp = temp + input(3) * input(4)
 *     output(1) = temp
 *     
 *     ! 计算第1个多项式
 *     temp = 0.0d0
 *     temp = temp + input(1) * input(3)
 *     output(2) = temp
 *     
 *     ...
 * end subroutine compute
 * 
 * 特点：
 * - 使用Fortran的subroutine声明函数
 * - 使用implicit none避免隐式类型声明
 * - 使用intent(in)和intent(out)明确参数方向
 * - 每个多项式使用独立的临时变量temp累加所有单项式
 * - 每个单项式单独一行，避免单行过长
 * - 使用Fortran风格的数组索引（从1开始）
 * - 使用real*8表示双精度浮点数
 * - 使用0.0d0表示双精度零
 */
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
