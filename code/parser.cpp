#include "analyzer.h"
#include "sheet.h"
#include "automata.h"


typedef hyperlex::Morpheme tokenstream;
typedef hyperlex::GrammarTree AST;
typedef hyperlex::tree<hyperlex::GrammarTree::TreeInfor> GTNode;
typedef hyperlex::tree<hyperlex::GrammarTree::TreeInfor>::PostIterator GTiterator;

namespace analyzer
{

void static checkNode(GTNode* GT, sheet::FIG::nonterminal expect, size_t line, const char* msg)
{
    struct errorinfo * errorinfo = new struct errorinfo;
    if(GT != NULL)
        if(sheet::FIG::RulesToSymbol[GT->root().site] == (int)expect)
            return;
    errorinfo->line = line;
    errorinfo->msg = msg;
    errorinfo->symbol = sheet::FIG::RulesName[GT->root().site];
    throw errorinfo;
}

void static checkNode(GTNode* GT, sheet::FIG::rules expect, size_t line, const char* msg)
{
    struct errorinfo * errorinfo = new struct errorinfo;
    if(GT != NULL)
        if((sheet::FIG::rules)GT->root().site == expect)
            return;
    errorinfo->line = line;
    errorinfo->msg = msg;
    errorinfo->symbol = sheet::FIG::RulesName[(sheet::FIG::rules)GT->root().site];
    throw errorinfo;
}

void static NeglectNullToken(tokenstream& eme)
{
	sheet::FIL::regular T;
	sheet::FIL::group G;
	//site = 0;
	for (size_t i = 0; i < eme.GetCount(); i++)
	{
		T = (sheet::FIL::regular)(eme[i].accept);
		G = (sheet::FIL::group)(eme[i].category);
		if (G == sheet::FIL::_format___ || G == sheet::FIL::_anntation___)
			eme.valid(i) = false;
        if(G == sheet::FIL::_division___)
            eme.valid(i) = false;
	}
	return;
}
size_t static returnSiteTOKEN(GTNode* GT, tokenstream& TS)
{
    checkNode(GT, sheet::FIG::_TOKEN_, __LINE__, "returnSiteTOKEN");
    GTNode* offset = GT->child(2);
    return TS.GetInt(offset->root().site);
}

void static buildMonomial(vector<size_t>& monomial, tokenstream& TS, GTNode* GT)
{
    monomial.clear();
    GTiterator iterator;
    iterator.initial(GT);
    while (iterator.still())
    {
        GT = iterator.target();
        if (iterator.state() == 0)//先序遍历
        {
            sheet::FIG::nonterminal RR = (sheet::FIG::nonterminal)(sheet::FIG::RulesToSymbol[GT->root().site]);
            if (GT->root().rules)
            {
                if(RR == sheet::FIG::_TOKEN_)
                {
                    monomial.append(returnSiteTOKEN(GT, TS));
                    iterator.state() = 1;
                }
                
            }
        }
        iterator.next();
    }
}

int static buildSingleExpress(size_t & siteO, FIexpress & expO, tokenstream& TS, GTNode* GT)
{
    siteO = 0;
    int error = 0;
    GTNode* Lvalue = GT->child(0);
    GTNode* Rvalue = GT->child(2);
    checkNode(Lvalue, sheet::FIG::Lvalue_only_, __LINE__, "buildSingleExpress");
    checkNode(Rvalue, sheet::FIG::Rvalue_only_, __LINE__, "buildSingleExpress");
    siteO = returnSiteTOKEN(Lvalue->child(0), TS);
    
    vector<size_t> monomial;

    GTiterator iterator;
    iterator.initial(Rvalue);
    while (iterator.still())
    {
        GT = iterator.target();
        if (iterator.state() == 0)//先序遍历
        {
            sheet::FIG::nonterminal RR = (sheet::FIG::nonterminal)(sheet::FIG::RulesToSymbol[GT->root().site]);
            if (GT->root().rules)
            {
                if(RR == sheet::FIG::_ITEM_)
                {
                   buildMonomial(monomial, TS, GT);
                   error = expO.AppendMonomial(monomial);
                   if(error != 0)
                   {
                       return error;
                   }
                   iterator.state() = 1;
                }
                
            }
        }
        iterator.next();
    }
    return 0;
}

int build_raw(FILE*fp, vector<FIexpress> &exps, vector<size_t> &sites)
{
    tokenstream TS;
    int error = TS.Build<sheet::FIL>(fp);// 构建词法分析结果
    if(error != 0)
    {
        return error;
    }
    NeglectNullToken(TS);// 过滤注释和空格等无效词元

    AST* ASTree = new AST;
    error = ASTree->build<sheet::FIG>(TS);// 构建语法分析树
    if(error != 0)
    {
        //error_record00 = GrammarTree::ActionError;
        fprintf(stderr, "head: %zu\n", ASTree->error_record01);
        fprintf(stderr, "top: %zu\n", ASTree->error_record02);
        delete ASTree;
        return error;
    }

    GTNode* GT = ASTree->GT;
    GTiterator iterator;
    iterator.initial(GT);

    

    while (iterator.still())
    {
        GT = iterator.target();
        if (iterator.state() == 0)//先序遍历
        {
            sheet::FIG::rules RR = (sheet::FIG::rules)GT->root().site;
            if (GT->root().rules)
            {
                if(RR == sheet::FIG::FI_formula_)
                {
                    size_t site = 0;
                    FIexpress exp;
                    error = buildSingleExpress(site, exp, TS, GT);
                    if(error != 0)
                    {
                        return error;
                    }
                    exps.append(exp);
                    sites.append(site);
                    iterator.state() = 1;
                }
                
            }
        }
    
        iterator.next();
    }
    return 0;
}

int FIexpresses::build(FILE*fp)
{
    vector<FIexpress> exps;
    vector<size_t> sites;
    int error = build_raw(fp, exps, sites);
    if(error != 0)
    {
        return error;
    }
    
    // 检查标号是否重复
    array<bool> check;
    size_t max = 0;
    for (size_t i = 0; i < sites.count(); i++)
    {
        if(sites[i] > max)
        {
            max = sites[i];
        }
    }
    
    if(max != sites.count() - 1)
    {
        return -4;
    }
    
    check.Malloc(max + 1);
    check.value(false);
    
    for (size_t i = 0; i < sites.count(); i++)
        check[sites[i]] = true;
    
    // 确保所有标号都被定义
    for (size_t i = 0; i < sites.count(); i++)
    {
        if(!check[i])
        {
            // 标号未定义 or重复必然同时发生了
            return -2;
        }
    }
    
    // 按照标号顺序存储多项式
    items.resize(max + 1);
    for (size_t i = 0; i < sites.count(); i++)
    {
        items[sites[i]].move(exps[i]);
    }
    
    OrderAnalysis();
    
    // 计算按照一阶不变量划分的partions数组
    analyze();

    // 计算CrossItem数组
    CrossItemAnalysis();

    return 0;
}

void FIexpresses::OrderAnalysis(void)
{
    highestOrder = 0;
    size_t newXCount = 0;
    
    for (size_t i = 0; i < items.count(); i++) {
        int order = items[i].getOrder();
        size_t XCountNow = items[i].getXCount();
        
        if (order > highestOrder) {
            highestOrder = order;
        }
        if (XCountNow > newXCount) {
            newXCount = XCountNow;
        }
    }
    
    if (items.count() > 0) {
        XCount = newXCount + 1;
    } else {
        XCount = 0;
    }
    
    OrderCount.recount(highestOrder + 1);
    for(size_t i = 0; i <= (size_t)highestOrder; i++) {
        OrderCount[i] = 0;
    }
        
    for (size_t i = 0; i < items.count(); i++) {
        int order = items[i].getOrder();
        if (order >= 0 && order <= highestOrder) {
            OrderCount[order]++;
        }
    }
}


}


namespace analyzer

{

/**
 * ParserDataMatrix - 解析数据矩阵文件，提取浮点数数据并检查每行数目是否相等
 * 
 * @param inputMat - 输入文件指针
 * @param matrix - 输出矩阵，存储解析的浮点数数据
 * @param row - 输出参数，矩阵的行数
 * @param col - 输出参数，矩阵的列数
 * 
 * @return 0 - 成功，-1 - 行长度不一致，其他非零值 - 词法分析错误
 * 
 * 功能说明：
 * 1. 使用词法分析器解析输入文件，识别浮点数和整数
 * 2. 过滤无效词元（如空格、注释等）
 * 3. 忽略空行
 * 4. 检查每一行的浮点数数目是否相等
 * 5. 将解析的浮点数存储到输出矩阵中
 * 6. 返回矩阵的行数和列数
 */
int ParserDataMatrix(FILE*inputMat,vector<double> & matrix, size_t &row, size_t &col)
{
    tokenstream TS;
    int error = TS.Build<sheet::FIL>(inputMat);
    if(error != 0)
    {
        return error;
    }
    NeglectNullToken(TS);

    size_t currentLine = 0;
    size_t currentCol = 0;
    size_t expectedCols = 0;
    bool firstLine = true;
    vector<double> rowData;
    size_t actualRowCount = 0;

    for (size_t i = 0; i < TS.GetCount(); i++)
    {
        if(!TS.valid(i))
            continue;
        
        sheet::FIL::regular T = (sheet::FIL::regular)(TS[i].accept);
        size_t line = TS[i].line;
        
        if(line > currentLine )
        {
            // 检查当前行是否为空行
            if(currentCol > 0)
            {
                if(firstLine)
                {
                    expectedCols = currentCol;
                    firstLine = false;
                }
                else if(currentCol != expectedCols)
                {
                    return -1; // 行长度不一致
                }
                
                for(size_t j = 0; j < rowData.count(); j++)
                {
                    matrix.append(rowData[j]);
                }
                rowData.clear();
                actualRowCount++;
            }
            
            currentLine = line;
            currentCol = 0;
        }
        
        if(T == sheet::FIL::_real_ || T == sheet::FIL::_integer_)
        {
            double value;
            if(T == sheet::FIL::_real_)
            {
                value = TS.GetReal(i);
            }
            else
            {
                value = (double)TS.GetInt(i);
            }
            rowData.append(value);
            currentCol++;
        }
    }
    
    if(!rowData.empty())
    {
        if(firstLine)
        {
            expectedCols = currentCol;
        }
        else if(currentCol != expectedCols)
        {
            return -1; // 行长度不一致
        }
        
        for(size_t j = 0; j < rowData.count(); j++)
        {
            matrix.append(rowData[j]);
        }
        actualRowCount++;
    }
    
    row = actualRowCount;
    col = expectedCols;
    
    return 0;
}

}

