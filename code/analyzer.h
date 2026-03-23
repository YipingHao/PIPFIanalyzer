#include "template.h"
//C++03 语法
namespace analyzer
{
    using namespace TemplateSelf;
    class FIexpress
    {
        public:
            typedef void _hx_need_explicit_move_tag;
            FIexpress() = default;
            ~FIexpress() = default;
            FIexpress(const FIexpress&) = default;
            FIexpress& operator=(const FIexpress&) = default;
            void move(FIexpress& src);
        protected:
            int order;//齐次多项式的次数
            size_t ItemCount;//单项式项数
            vector<size_t> items;//大小为ItemCount * order
            /*
            多项式的存储格式:
            1. 放弃存储常数项，所有项的系数都设为1.0
               - FI(基本不变量)是多元多项式
            2. items数组存储单项式的指数信息
               - 大小为 ItemCount * order
               - 每个单项式必定占 order 个元素，代表order个自变元
               - 例如: order=3 时，items[i*3], items[i*3+1], items[i*3+2] 中存储的三个整数
                 表示第i个单项式 的每一个自变元是对应哪一个变量的索引（名称）
            3. compute_bare()方法计算齐次多项式的值
               - 输入: FI的值向量(values)
               - 输出: 多项式计算结果(系数为1.0的各单项式之和)
            */
        public:
            int getOrder() const { return order; }
            size_t getItemCount() const { return ItemCount; }
            const vector<size_t>& getItems() const { return items; }
            void setOrder(int order) { this->order = order; }
            void setItemCount(size_t itemCount) { this->ItemCount = itemCount; }
            void setItems(const vector<size_t>& items) { this->items = items; }

            double compute_bare(const vector<double>& values) const;
            inline double compute_bare(const double* values) const
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
            //不做任何检查，直接计算
    };
    class FIexpresses
    {
        public:
        typedef void _hx_need_explicit_move_tag;
            FIexpresses() = default;
            ~FIexpresses() = default;
            FIexpresses(const FIexpresses&) = default;
            FIexpresses& operator=(const FIexpresses&) = default;
            void move(FIexpresses& src);
        protected:
            vector<FIexpress> items;
            int highestOrder;//最高次数
            vector<size_t> OrderCount;//次数i的多项式数量
            vector<size_t> OrderIndex;//次数i的多项式索引
            size_t XCount;//变量数量

        public:
            const vector<FIexpress>& getItems() const { return items; }
            void setItems(const vector<FIexpress>& items) { this->items = items; }
            int getHighestOrder() const { return highestOrder; }
            const vector<size_t>& getOrderCount() const { return OrderCount; }
            const vector<size_t>& getOrderIndex() const { return OrderIndex; }
            size_t getXCount() const { return XCount; }

        public:
            int compute(const double*input, size_t ldi, size_t rowi, size_t coli, double* output, size_t ldo, size_t rowo, size_t colo) const;
            int compute(unsigned int threadCount, const double*input, size_t ldi, size_t rowi, size_t coli, double* output, size_t ldo, size_t rowo, size_t colo) const;
            /*
            数据转换函数，将多元多项式的输入转为输出，并且一次转换多条数据，每条数据占据矩阵的一行的开头位置。
            数据矩阵的存储方式说明：坚定的使用行主元存储，即每个元素的索引为 row * ld + col
            
            参数说明：
            - input: 输入矩阵指针，存储原始数据（如键长坐标）
            - ldi: 输入矩阵的领先维度（每行的元素个数）
            - rowi: 输入数据的行数（要处理的数据条数）
            - coli: 输入数据的列数（每条数据的维度）
            - output: 输出矩阵指针，存储转换后的数据（PIP多项式值）
            - ldo: 输出矩阵的领先维度（每行的元素个数）
            - rowo: 输出数据的行数（应与rowi相同）
            - colo: 输出数据的列数（PIP多项式的个数）
            - threadCount: 线程数量（默认1）
            工作原理：
            1. 遍历每条输入数据
            2. 对每条数据，计算所有FIexpress对象的值
            3. 将计算结果存储到输出矩阵的对应位置
            
            检查：
            - 输入输出的行数应匹配（rowi == rowo） 违反则返回-1
            - 输入数据的维度应满足所有FIexpress的计算需求 违反则返回-2
            - 列数应与输入数据的维度匹配（coli == XCount） 违反则返回-3
            - 输出数据的维度应与所有FIexpress的次数总和匹配（colo == items.size()) 违反则返回-4
            
            返回值：
            - 处理的成功状态或错误码（如果有）
            */
            int build(FILE*fp);
            /*
            从文件中读取FIexpress对象的表达式，构建FIexpresses对象。
            参数说明：
            - fp: 输入文件指针，用于读取表达式
            - 处理的成功状态或错误码（如果有）
            文件的文法记录在data/FIlex.txt中，文法分析使用LR解析，解析动作表格和词法分析器表格在
            code/sheet.cpp中
            */
            int printCcode(FILE*fp) const;
            /*
            将这个FIexpresses对象转换为C语言代码，用于计算PIP多项式的值。
            参数说明：
            - fp: 输出文件指针，用于存储生成的C语言代码
            输出的行数的声明是
                void compute(const double* inpput, double* output)
                {
                }
            逐一计算每个多项式的值并写入output数组
            */
            int printFortrancode(FILE*fp) const;
            /*
            将这个FIexpresses对象转换为Fortran语言代码，用于计算PIP多项式的值。
            参数说明：
            - fp: 输出文件指针，用于存储生成的Fortran语言代码
            生成的代码布局：
                subroutine compute(input, output)
                    implicit none
                    real*8, intent(in) :: input(:)
                    real*8, intent(out) :: output(:)
                    real*8 :: temp
                    integer :: i
                    
                    ! 计算第0个多项式
                    temp = 0.0d0
                    temp = temp + input(1) * input(2)
                    temp = temp + input(3) * input(4)
                    output(1) = temp
                    
                    ! 计算第1个多项式
                    temp = 0.0d0
                    temp = temp + input(1) * input(3)
                    output(2) = temp
                    
                    ...
                end subroutine compute
            每个多项式占一个独立的计算块，使用临时变量temp累加所有单项式的值，
            最后将结果写入output数组的对应位置。
            */
    };
}