#include "analyzer.h"
#include "sheet.h"
#include "automata.h"
using namespace analyzer;

typedef hyperlex::Morpheme tokenstream;
typedef hyperlex::GrammarTree AST;
typedef hyperlex::tree<hyperlex::GrammarTree::TreeInfor> GTNode;
typedef hyperlex::tree<hyperlex::GrammarTree::TreeInfor>::PostIterator GTiterator;


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
        fprintf(stderr, "head: %z\n", ASTree->error_record01);
        fprintf(stderr, "top: %z\n", ASTree->error_record01);
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
    highestOrder = 0;
    XCount = 0;
    // 按照标号顺序存储多项式
    items.resize(max + 1);
    for (size_t i = 0; i < sites.count(); i++)
    {
        items[sites[i]].move(exps[i]);
        int order = items[i].getOrder();
        int XCountNow = items[i].getXCount();
        if(order > highestOrder)
        {
            highestOrder = order;
        }
        if(XCountNow > XCount)
        {
            XCount = XCountNow;
        }
    }
    XCount++;
    // 计算最高次数和次数分布
    
    OrderCount.clear();
    OrderCount.resize(highestOrder + 1);
    for (size_t i = 0; i < items.count(); i++)
    {
        int order = items[i].getOrder();
        OrderCount[order]++;
    }
    
    return 0;
}





