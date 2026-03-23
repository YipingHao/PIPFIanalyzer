#include"libFIanalyzer.h"
#include <stdio.h>
#include<iostream>
#include<stdlib.h>
using namespace FIanalyzer;
int static test_entrance(const char* output_path);
int static entrance(void);
int main()
{
	return test_entrance("./output/");
	return entrance();
}



int static entrance(void)
{
	int error;
	return error;
}

class parameter
{
public:
	parameter();
	~parameter();
    void build(hyperlex::ParaFile& pf);
    std::string FullInputName(void) const;
    std::string FullOutputName(void) const;
    std::string FullDataName(void) const;
    std::string SequenceFILE(void) const;
    std::string EnergyFILE(void) const;
    void Demo(FILE* fp)const;
    void groupGive(vector<size_t>& group, size_t L) const;
    size_t thread;
    char* funcName;

    double interval;
    size_t OutputCount;
private:
    char* inputPath;
    char* outputPath;

    char* inputName;
    char* outputName;

    char* DataFilePath; 
    char* DataFileName;

    char* SequenceFile;
    char* EnergyFile;
    size_t length;
    hyperlex::IntegerDefault* permutation;

    
};
static char* CopyMalloc(const char* s);
std::string static path_add_file_name(const std::string& path, const std::string& name);
parameter::parameter()
{
    inputPath = NULL;
    outputPath = NULL;

    inputName = NULL;
    outputName = NULL;

    funcName = NULL;

    DataFilePath = NULL;
    DataFileName = NULL;

    permutation = NULL;

    SequenceFile = NULL;
    EnergyFile = NULL;
}
parameter::~parameter()
{
    free(inputPath);
    free(outputPath);

    free(inputName);
    free(outputName);

    free(funcName);

    free(DataFilePath);
    free(DataFileName);

    free(permutation);

    free(SequenceFile);
    free(EnergyFile);
}
std::string parameter::FullInputName(void) const
{
    std::string path;
    std::string file;
    std::string out;
    path = inputPath;
    file = inputName;
    out = path_add_file_name(path, file);
    return out;
}
std::string parameter::FullOutputName(void) const
{
    std::string path;
    std::string file;
    std::string out;
    path = outputPath;
    file = outputName;
    out = path_add_file_name(path, file);
    return out;
}
std::string parameter::FullDataName(void) const
{
    std::string path;
    std::string file;
    std::string out;
    path = DataFilePath;
    file = DataFileName;
    out = path_add_file_name(path, file);
    return out;
}
std::string parameter::SequenceFILE(void) const
{
    std::string path;
    std::string file;
    std::string out;
    path = outputPath;
    file = SequenceFile;
    out = path_add_file_name(path, file);
    return out;
}
std::string parameter::EnergyFILE(void) const
{
    std::string path;
    std::string file;
    std::string out;
    path = outputPath;
    file = EnergyFile;
    out = path_add_file_name(path, file);
    return out;
}
void parameter::build(hyperlex::ParaFile& pf)
{
    size_t sitePF, i;
    std::string temp;
    sitePF = pf.SearchKey("thread", hyperlex::ParaFile::Int);
    if (sitePF != pf.Amount())
        thread = pf.FirstInt(sitePF);
    else
        thread = 1;

    temp = pf.GetString("InputFileName", "C10H8.txt");
    inputName = CopyMalloc(temp.c_str());

    temp = pf.GetString("OutputFileName", "fi.c");
    outputName = CopyMalloc(temp.c_str());
    
    temp = pf.GetString("InputFilePath", "/share/fayelulu/FIanalyzer/data/");
    inputPath = CopyMalloc(temp.c_str());

    temp = pf.GetString("OutputFilePath", "/share/fayelulu/FIanalyzer/data/");
    outputPath = CopyMalloc(temp.c_str());

    temp = pf.GetString("funcName", "RtoFI");
    funcName = CopyMalloc(temp.c_str());

    temp = pf.GetString("DataFilePath", "/share/fayelulu/FIanalyzer/data/");
    DataFilePath = CopyMalloc(temp.c_str());

    temp = pf.GetString("DataFileName", "fort.1000");
    DataFileName = CopyMalloc(temp.c_str());

    temp = pf.GetString("SequenceFile", "sequence.txt");
    SequenceFile = CopyMalloc(temp.c_str());

    temp = pf.GetString("EnergyFile", "energy.txt");
    EnergyFile = CopyMalloc(temp.c_str());

    sitePF = pf.SearchKey("permutation", hyperlex::ParaFile::Int);
    pf.IntList(permutation, length, sitePF);


    sitePF = pf.SearchKey("OutputCount", hyperlex::ParaFile::Int);
    if (sitePF != pf.Amount())
        OutputCount = pf.FirstInt(sitePF);
    else
        OutputCount = 1;

    sitePF = pf.SearchKey("interval", hyperlex::ParaFile::Real);
    if (sitePF != pf.Amount())
        interval = pf.FirstReal(sitePF);
    else
        interval = 2.0;
}
void parameter::Demo(FILE* fp)const
{
    size_t i;
    fprintf(fp, "thread: %zu\n", thread);
    fprintf(fp, "inputPath: %s\n", inputPath);
    fprintf(fp, "outputPath: %s\n", outputPath);
    fprintf(fp, "inputName: %s\n", inputName);
    fprintf(fp, "outputName: %s\n", outputName);
    fprintf(fp, "funcName: %s\n", funcName);
    fprintf(fp, "DataFilePath: %s\n", DataFilePath);
    fprintf(fp, "DataFileName: %s\n", DataFileName);

    fprintf(fp, "SequenceFile: %s\n", SequenceFile);
    fprintf(fp, "EnergyFile: %s\n", EnergyFile);

    fprintf(fp, "permutation length: %zu\n[", length);
    for (i = 0; i + 1 < length; i++)
    {
        fprintf(fp, "%zu, ", permutation[i]);
    }
    if(length != 0) fprintf(fp, "%zu]\n", permutation[i]);
    else fprintf(fp, "]\n");

    fprintf(fp, "OutputCount: %zu\n", OutputCount);
    fprintf(fp, "Interval: %lf\n", interval);
}
void parameter::groupGive(vector<size_t>& group, size_t L) const
{
    size_t i;
    group.recount(L);
    for (i = 0; i < length && i < L; i++)
    {
        group[i] = permutation[i];
    }
}

int static test01(const parameter & para);
int static test02(const parameter & para);
int static test03(const parameter & para);
int static test04(const parameter & para);
int static test05(const parameter & para);
int static test06(const parameter & para);
int static test07(const parameter & para);
int static test08(const parameter & para);
int static test09(const parameter & para);
int static test10(const parameter & para);
int static test11(const parameter & para);
int static test12(const parameter & para);
int static test13(const parameter & para);
int static test14(const parameter & para);
int static test15(const parameter & para);
int static test16(const parameter& para);
int test_entrance(const char* output_path)
{
    int item;
    int info;
    hyperlex::ParaFile pf;
    size_t sitePF;
    parameter para;
    pf.initial("./parameter/test_item.txt");
    sitePF = pf.SearchKey("item", hyperlex::ParaFile::Int);
    if (sitePF != pf.Amount())
        item = pf.FirstInt(sitePF);
    else
        item = 1;
    std::cout << "item: " << item << std::endl;
    info = 0;
    para.build(pf);
    std::cout << "======================== parameter Demo ========================" << std::endl;
    para.Demo(stdout);
    switch (item)
    {
    case 1:
        info = test01(para);
        break;
    case 2:
        info = test02(para);
        break;
    case 3:
        info = test03(para);
        break;
    case 4:
        info = test04(para);
        break;
    case 5:
        std::cout << "Test RegTree; " << std::endl;
        info = test05(para);
        break;
    case 6:
        std::cout << "Test RegTree To nfa; " << std::endl;
        info = test06(para);
        break;
    case 7:
        std::cout << "Test DFA , nfa; " << std::endl;
        info = test07(para);
        break;
    case 8:
        std::cout << "Test grammer 11; " << std::endl;
        info = test08(para);
        break;
    case 9:
        std::cout << "test print; " << std::endl;
        info = test09(para);
        break;
    case 10:
        std::cout << "Test xyz; " << std::endl;
        info = test10(para);
        break;
    case 11:
        std::cout << "use xyz to bondlength; " << std::endl;
        info = test11(para);
        break;
    case 12:
        std::cout << "Test Mersenne Twister; " << std::endl;
        info = test12(para);
        break;
    case 13:
        std::cout << "xyz panel; " << std::endl;
        info = test13(para);
        break;
    case 14:
        std::cout << "NN output analysis; " << std::endl;
        info = test14(para);
        break;
    case 15:
        std::cout << "Test grammer LR0 from input file; " << std::endl;
        info = test15(para);
        break;
    case 16:
        std::cout << "Test grammer LR1 from input file; " << std::endl;
        info = test16(para);
        break;
    default:
        info = test01(para);
        break;
    }
    return info;
}
int static test01(const parameter & para)
{
	FIformula formula;
	formula.input("./data/A2B4.txt");
	formula.print(stdout);
    return 0;
}
int static test02(const parameter & para)
{
	FIunit FI;
	hyperlex::CFile CF;
	FILE* fp;
	fp = CF.OpenRead("./data/A2B4.txt");
	
	hyperlex::CharBuffer fi_buffer;
	fi_buffer << fp;
	fi_buffer.Print(stdout);
	FI.build(fi_buffer);
	printf("=========================================\n");
	FI.print(stdout);
	fclose(fp);
    return 0;
}
int static test03(const parameter & para)
{
	FIformula formula, formula2;
	formula.input("./data/AB3.txt");
	formula.print(stdout);
	printf("=========================================\n");
	formula2.input("./data/AB3PIP.txt");
	formula2.print(stdout);
	printf("=========================================\n");
	formula.Append(formula2);
	formula.print(stdout);
	printf("=========================================\n");
	formula.Sort();
	formula.print(stdout);
	printf("=========================================\n");
    return 0;
}
int static test04(const parameter & para)
{
	FIformula formula, formula2;
	formula.input("./data/AB3.txt");
	formula.print(stdout);
	printf("=========================================\n");
	formula2.input("./data/AB3PIP.txt");
	formula2.print(stdout);
	printf("=========================================\n");
	formula.Append(formula2);
	formula.Sort();
	formula.print(stdout);
	printf("=========================================\n");
    return 0;
}
int static test05(const parameter & para)
{
	FIformula formula, formula1, formula2;
	double R[6];
	formula1.input("./data/AB3.txt");
	formula.input(formula1);
	formula.print(stdout);
	printf("=========================================\n");
	formula2.input("./data/AB3PIP.txt");
	formula2.print(stdout);
	printf("=========================================\n");
	formula.Append(formula2);
	formula.Sort();
	formula.print(stdout);
	printf("=========================================\n");
	formula1.compare(stdout, formula);
	printf("=========================================\n");
	formula2.compare(stdout, formula);

	R[0] = 1.0;
	R[1] = 2.0;
	R[2] = 3.0;
	R[3] = 4.0;
	R[4] = 5.0;
	R[5] = 6.0;
	printf("formula.compute(21, R) = %lf\n", formula.compute(21, R));
	printf("formula.compute(1, R) = %lf\n", formula.compute(1, R));
	printf("formula.compute(2, R) = %lf\n", formula.compute(2, R));
	printf("formula.compute(5, R) = %lf\n", formula.compute(5, R));
    return 0;
}
int static test06(const parameter & para)
{
	//FItransform("./data/AB3.txt", "./data/AB3", "./output/AB3");
	FItransform("/share/fayelulu/FIanalyzer/data/C10H8.txt", "/share/fayelulu/FIanalyzer/data/fort.100", "./output/C10H8");
    return 0;
}
int static test07(const parameter & para)
{


	FItransformPthread(32, "/share/fayelulu/FIanalyzer/data/C10H8.txt", "/share/fayelulu/FIanalyzer/data/fort.100", "./output/C10H8");
    return 0;
}
int static test08(const parameter & para)
{
	FItransformCsingle(para.thread, para.FullInputName().c_str(), para.funcName, para.FullOutputName().c_str());

    return 0;
}
int static test09(const parameter & para)
{
    if (para.thread <= 1)
    {
        FItransform(para.FullInputName().c_str(), para.FullDataName().c_str(), para.FullOutputName().c_str());
    }
    else
    {
        FItransformPthread(para.thread, para.FullInputName().c_str(), para.FullDataName().c_str(), para.FullOutputName().c_str());
    }
    return 0;
}
int static build(const char* path, const char* name, xyzFile& out);
int static build(const char* name, xyzFile& out);
int static build(FILE* fp, xyzFile& out);
int static test10(const parameter & para)
{
    xyzFile out;
    int infor;
    infor = build("/share2/Hyper/FId/MD17", "ethanol.xyz", out);
    std::cout << "infor: " << infor << std::endl;
    out.tail(stdout, 16);
    std::cout << "infor: " << infor << std::endl;
    out.demo(stdout, 0, 16);

    std::cout << "check: " << out.check() << std::endl;
    return 0;
}

class MersenneTwisters
{
public:
    MersenneTwisters();
    ~MersenneTwisters();
    void SetSeed(unsigned long long seed);
    unsigned long long  rand(void);
    double real(void);
    double RealCloseCLose(void);
    double RealOpenOpen(void);
    size_t select(size_t N);//[0, N)
private:
    unsigned long long mt[312];
    int mti;
    void twist(void);
};





int static test11(const parameter& para)
{
    int infor;
    xyzFile out;
    vector<double> output;
    vector<size_t> group;
    vector<double> FIE;
    vector<double> Energy;

    size_t i, ld, row, column, FIdim;
    FIformula formula;

    hyperlex::MatFile MF;
    hyperlex::CFile CF;
    FILE* OutputFile;
    time_t t;

    double Lower, Upper, temp;
    size_t InterCount;
    vector<size_t> distribute;

    time(&t);
    fprintf(stdout, "Data loading: %s", ctime(&t));
    infor = build(para.FullDataName().c_str(), out);
    std::cout << "infor = build(const char*, xyzFile&): " << infor << std::endl;
    Lower = out.EnerguLower();
    Upper = out.EnergyUpper();
    std::cout << "Lower: " << Lower << ", Upper: " << Upper << ", range: " << Upper - Lower << std::endl;
    std::cout << "check: " << out.check() << std::endl;
    std::cout << "first data: " << std::endl;
    out.demo(stdout, 0, 1);
    std::cout << "last data: " << std::endl;
    out.tail(stdout, 1);

    out.OutputErange(Energy);
    temp = 1.0 / para.interval;
    InterCount = (size_t)((Upper - Lower) * temp) + 1;
    distribute.recount(InterCount);
    distribute.value(0);
    for (i = 0; i < Energy.count(); i++)
    {
        distribute[(size_t)(Energy[i] * temp)] += 1;
    }
    for (i = 0; i < distribute.count(); i++)
    {
        temp = (double)i * para.interval;
        fprintf(stdout, "[%+8.8lf, %+8.8lf): %8zu\n", temp, temp + para.interval, distribute[i]);
    }

    OutputFile = CF.OpenWriteAtRear(para.EnergyFILE().c_str());
    MF.MatrixSet(Energy.array(), 1, Energy.count(), 1, OutputFile);
    fclose(OutputFile);

    time(&t);
    fprintf(stdout, "Data loading end, permutation begins: %s", ctime(&t));
    para.groupGive(group, out[0]->AtomCount());
    infor = out.permutation(group);
    std::cout << "infor = permutation(const vector<size_t> &): " << infor << std::endl;
    std::cout << "check: " << out.check() << std::endl;

    std::cout << "first data: " << std::endl;
    out.demo(stdout, 0, 1);
    std::cout << "last data: " << std::endl;
    out.tail(stdout, 1);

    time(&t);
    fprintf(stdout, "Permutation end, bondlength computation begins: %s", ctime(&t));
    out.PrintR(output);

    row = out.count();
    column = out[0]->AtomCount() * (out[0]->AtomCount() - 1);
    column = column / 2;
    ld = column + 1;

    time(&t);
    fprintf(stdout, "Analysis begin: %s, thread: %d\n", ctime(&t), para.thread);

    formula.input(para.FullInputName().c_str());
    FIdim = formula.FICount();

    fprintf(stdout, "FIdim: %zu, column: %zu, FI bond count: %zu\n", FIdim, column, formula.BondCount());

    time(&t);
    fprintf(stdout, "Analysis end, computation begins: %s", ctime(&t));
    infor = FItransformPthread(para.thread, row, column, output, ld, FIE, &formula);
    std::cout << "infor = FItransformPthread(const char*, xyzFile&): " << infor << std::endl;
    time(&t);
    fprintf(stdout, "Computation end,  writng begings: %s", ctime(&t));

    if (infor == 0)
    {
        if (para.OutputCount >= out.count())
        {
            OutputFile = CF.OpenWriteAtRear(para.FullOutputName().c_str());
            MF.MatrixSet(FIE.array(), FIdim, row, FIdim, OutputFile);
            fclose(OutputFile);
        }
        else if (para.OutputCount != 0)
        {
            //i = 0;
            //while (i < para.OutputCount)
            //{

            // }
        }
    }
    fprintf(stdout, "File writing end: %s", ctime(&t));
    return 0;
}


int static test12(const parameter & para)
{
    MersenneTwisters MT;
    size_t i, all, range;
    FILE* fp;
    vector<size_t> count;
    range = 10000;
    count.recount(range);
    count.value(0);
    fp = stdout;
    all = 100000000;
    for (i = 0; i < 100; i++)
    {
        fprintf(fp, "%20llu, %zu, %lf\n", MT.rand(), MT.select(4), MT.real());
    }

    for (i = 0; i < all; i++)
    {
        count[MT.select(range)] += 1;
    }
    for (i = 0; i < count.count(); i++)
    {
        fprintf(stdout, "count[%6zu]: %10zu, percent %lf.\n", i, count[i], (100.0 * (double)(count[i]) / (double)all));
    }

    range = 2;
    count.recount(range);
    count.value(0);
    all = 100000000;
    for (i = 0; i < all; i++)
    {
        count[MT.select(range)] += 1;
    }
    for (i = 0; i < count.count(); i++)
    {
        fprintf(stdout, "count[%6zu]: %10zu, percent %lf.\n", i, count[i], (100.0 * (double)(count[i]) / (double)all));
    }

    range = 4;
    count.recount(range);
    count.value(0);
    all = 100000000;
    for (i = 0; i < all; i++)
    {
        count[MT.select(range)] += 1;
    }
    for (i = 0; i < count.count(); i++)
    {
        fprintf(stdout, "count[%6zu]: %10zu, percent %lf.\n", i, count[i], (100.0 * (double)(count[i]) / (double)all));
    }
    return 0;
}
class XYZpanel
{
public:
    XYZpanel();
    ~XYZpanel();
    void initial(const parameter& para);
    void EnergyPrint(const char* name);
    void permutation(void);
    void FIcompute(const parameter& para);
    void FIprint(const parameter& para);

    int AllInOne(const parameter& para);
private:
    int infor;
    xyzFile out;
    
    vector<size_t> group;
   
    vector<double> Energy;

   
    FIformula formula;

    hyperlex::MatFile MF;
    hyperlex::CFile CF;
    FILE* OutputFile;
    time_t t;

    double Lower, Upper, temp;
    size_t InterCount;
    vector<size_t> distribute;

    vector<double> output;
    vector<double> FIE;
    vector<double> OutputForPrint;

};

XYZpanel::XYZpanel()
{
}
XYZpanel::~XYZpanel()
{
}
void XYZpanel::initial(const parameter& para)
{
    size_t i;
    time(&t);
    fprintf(stdout, "Data loading: %s", ctime(&t));
    infor = build(para.FullDataName().c_str(), out);
    std::cout << "infor = build(const char*, xyzFile&): " << infor << std::endl;
    Lower = out.EnerguLower();
    Upper = out.EnergyUpper();
    std::cout << "Lower: " << Lower << ", Upper: " << Upper << ", range: " << Upper - Lower << std::endl;
    std::cout << "check: " << out.check() << std::endl;
    std::cout << "first data: " << std::endl;
    out.demo(stdout, 0, 1);
    std::cout << "last data: " << std::endl;
    out.tail(stdout, 1);

    out.OutputErange(Energy);
    temp = 1.0 / para.interval;
    InterCount = (size_t)((Upper - Lower) * temp) + 1;
    distribute.recount(InterCount);
    distribute.value(0);
    for (i = 0; i < Energy.count(); i++)
    {
        distribute[(size_t)(Energy[i] * temp)] += 1;
    }
    for (i = 0; i < distribute.count(); i++)
    {
        temp = (double)i * para.interval;
        fprintf(stdout, "[%+8.8lf, %+8.8lf): %8zu\n", temp, temp + para.interval, distribute[i]);
    }


    para.groupGive(group, out[0]->AtomCount());
}
void XYZpanel::EnergyPrint(const char* name)
{
    OutputFile = CF.OpenWriteAtRear(name);
    MF.MatrixSet(Energy.array(), 1, Energy.count(), 1, OutputFile);
    fclose(OutputFile);
}
void XYZpanel::permutation(void)
{
    time(&t);
    fprintf(stdout, "Data loading end, permutation begins: %s", ctime(&t));
    if(group.count() == out[0]->AtomCount())
        infor = out.permutation(group);
    else
    {
        infor = 1;
        std::cout << "infor = permutation(const vector<size_t> &): ERROR" << infor << std::endl;
    }
    std::cout << "infor = permutation(const vector<size_t> &): " << infor << std::endl;
    std::cout << "check: " << out.check() << std::endl;

    std::cout << "first data: " << std::endl;
    out.demo(stdout, 0, 1);
    std::cout << "last data: " << std::endl;
    out.tail(stdout, 1);

    time(&t);
    fprintf(stdout, "Permutation end, bondlength computation begins: %s", ctime(&t));
}
void XYZpanel::FIcompute(const parameter& para)
{
    size_t ld, row, column, FIdim;
    out.PrintR(output);

    row = out.count();
    column = out[0]->AtomCount() * (out[0]->AtomCount() - 1);
    column = column / 2;
    ld = column + 1;

    time(&t);
    fprintf(stdout, "Analysis begin: %s, thread: %d\n", ctime(&t), para.thread);

    formula.input(para.FullInputName().c_str());
    FIdim = formula.FICount();

    fprintf(stdout, "FIdim: %zu, column: %zu, FI bond count: %zu\n", FIdim, column, formula.BondCount());

    time(&t);
    fprintf(stdout, "Analysis end, computation begins: %s", ctime(&t));
    infor = FItransformPthread(para.thread, row, column, output, ld, FIE, &formula);
    std::cout << "infor = FItransformPthread(const char*, xyzFile&): " << infor << std::endl;
    time(&t);
    fprintf(stdout, "Computation end,  writng begings: %s", ctime(&t));
}
void static shuffle(vector<size_t>& source, MersenneTwisters& MT)
{
    size_t i, form, temp;
    for (i = 0; i < source.count(); i++)
    {
        form = MT.select(source.count());
        if (form != i)
        {
            temp = source[i];
            source[i] = source[form];
            source[form] = temp;
        }
    }
}
void static MatRowShuffle(size_t row, size_t column, size_t NewCount, vector<double>& output, size_t ldo, vector<double>& input, size_t ldi, vector<size_t>& source)
{
    size_t i, site, j;
    output.recount(NewCount * ldo);
    for (i = 0; i < NewCount; i++)
    {
        site = source[i];
        for (j = 0; j < column; j++)
        {
            output[i * ldo + j] = input[site * ldi + j];
        }
    }
}

void XYZpanel::FIprint(const parameter& para)
{
    vector<size_t> source;
    size_t i;
    MersenneTwisters MT;
    size_t ld, row, column, FIdim;
    row = out.count();
    FIdim = formula.FICount();
    ld = FIdim + 1;
    if (infor == 0)
    {
        if (para.OutputCount >= out.count())
        {
            OutputFile = CF.OpenWritePlus(para.FullOutputName().c_str());
            MF.MatrixSet(FIE.array(), ld, row, ld, OutputFile);
            fclose(OutputFile);
        }
        else if (para.OutputCount != 0)
        {
            source.recount(row);
            for (i = 0; i < row; i++) source[i] = i;
            shuffle(source, MT);
            MatRowShuffle(row, ld, para.OutputCount, OutputForPrint, ld, FIE, ld, source);

            OutputFile = CF.OpenWritePlus(para.FullOutputName().c_str());
            MF.MatrixSet(OutputForPrint.array(), ld, para.OutputCount, ld, OutputFile);
            fclose(OutputFile);

        }
    }
    fprintf(stdout, "File writing end: %s", ctime(&t));
}
int XYZpanel::AllInOne(const parameter& para)
{
    initial(para);
    EnergyPrint(para.EnergyFILE().c_str());
    permutation();
    FIcompute(para);
    FIprint(para);

    return infor;
}

int static test13(const parameter & para)
{
    XYZpanel panel;
    return panel.AllInOne(para);
}

class NNoutputAnalysis
{
public:
    NNoutputAnalysis();
    ~NNoutputAnalysis();
    int build(const parameter& para);
    void ErrorCount(void)const;
private:
    vector<double> output;
    vector<double> data;
    vector<size_t> sequence;
    size_t trainCount;
    double E_trans;

    double* mat;
    size_t row;
    size_t column;
    int build_output(FILE* fp);
    int build_data(FILE* fp);
    int build_sequence(FILE* fp);
};
#include<cmath>
NNoutputAnalysis::NNoutputAnalysis()
{
    mat = NULL;
}
NNoutputAnalysis::~NNoutputAnalysis()
{
    free(mat);
}
int NNoutputAnalysis::build(const parameter& para)
{
    int infor;
    hyperlex::CFile CF;
    FILE* fp;
    fp = CF.OpenRead(para.FullInputName().c_str());
    infor = build_output(fp);
    fclose(fp);
    
    fp = CF.OpenRead(para.FullDataName().c_str());
    infor = build_data(fp);
    fclose(fp);

    fp = CF.OpenRead(para.SequenceFILE().c_str());
    infor = build_sequence(fp);
    std::cout << "build_sequence: " << infor << std::endl;
    fclose(fp);

    trainCount = para.OutputCount;
    E_trans = para.interval;
    
    return 0;
}
int NNoutputAnalysis::build_output(FILE* fp)
{
    hyperlex::CharBuffer CB;
    hyperlex::UnitBuffer UB;
    size_t i, lower;

    CB << fp;
    UB.build(CB);

    for (i = 0; i < UB.Site(); i++)
        if (UB[i].type == 1) output.append(UB[i].C.real);

    std::cout << "output count: " << output.count() << std::endl;

    for (i = 0; i < 8 && i < output.count(); i++)
        std::cout << "\toutput[" << i << "] = " << output[i] << std::endl;
    i = output.count() < 16 ? 8 : output.count() - 8;
    for (; i < output.count(); i++)
        std::cout << "\toutput[" << i << "] = " << output[i] << std::endl;
    return 0;
}
int NNoutputAnalysis::build_data(FILE* fp)
{
    hyperlex::MatFile MT;
    size_t i;
    MT.MatrixGet(&mat, &row, &column, fp);

    std::cout << "row: " << row << std::endl;
    std::cout << "column: " << column << std::endl;
    for (i = 0; i < row; i++)
        data.append(mat[i * column + column - 1]);

    std::cout << "data count: " << data.count() << std::endl;

    for (i = 0; i < 8 && i < data.count(); i++)
        std::cout << "\tdata[" << i << "] = " << data[i] << std::endl;
    i = data.count() < 16 ? 8 : data.count() - 8;
    for (; i < data.count(); i++)
        std::cout << "\tdata[" << i << "] = " << data[i] << std::endl;
    //free(list);
    return 0;
}
int NNoutputAnalysis::build_sequence(FILE* fp)
{
    hyperlex::CharBuffer CB;
    hyperlex::UnitBuffer UB;
    size_t i, lower;
    vector<bool>label_;
    CB << fp;
    UB.build(CB);
    
    for (i = 0; i < UB.Site(); i++)
        if (UB[i].type == 0) sequence.append(UB[i].C.integer);

    std::cout << "sequence count: " << sequence.count() << std::endl;

    for (i = 0; i < 8 && i < sequence.count(); i++)
        std::cout << "\tsequence[" << i << "] = " << sequence[i] << std::endl;
    i = sequence.count() < 16 ? 8 : sequence.count() - 8;
    for (; i < sequence.count(); i++)
        std::cout << "\tsequence[" << i << "] = " << sequence[i] << std::endl;

    label_.recount(sequence.count());
    label_.value(true);
    for (i = 0; i < sequence.count(); i++)
        if (sequence[i] < sequence.count()) label_[sequence[i]] = false;
    for (i = 0; i < sequence.count(); i++)
        if (label_[i]) return i;
    return 0;
}
void NNoutputAnalysis::ErrorCount(void)const
{
    size_t i;
    double rmse, mae, temp;
    rmse = 0.0;
    mae = 0.0;
    for (i = 0; i < trainCount; i++)
    {
        temp = output[sequence[i]] - data[sequence[i]];
        rmse += temp * temp;
        mae += temp > 0.0 ? temp : -temp;
    }
    temp = 1.0 / (double)trainCount;
    rmse = temp * rmse;
    mae = temp * mae;
    std::cout << "trainCount: " << trainCount << std::endl;
    std::cout << "E_trans: " << E_trans << std::endl;
    std::cout << "rmse: " << std::sqrt(rmse) << ", mae: " << mae << std::endl;
    std::cout << "rmse: " << std::sqrt(rmse) * E_trans << ", mae: " << mae * E_trans << std::endl;
    rmse = 0.0;
    mae = 0.0;
    for (i = trainCount; i < sequence.count(); i++)
    {
        temp = output[sequence[i]] - data[sequence[i]];
        rmse += temp * temp;
        mae += temp > 0.0 ? temp : -temp;
    }
    if (sequence.count() - trainCount != 0)
    {
        temp = 1.0 / (double)(sequence.count() - trainCount);
        rmse = temp * rmse;
        mae = temp * mae;
        std::cout << "valid: " << sequence.count() - trainCount << std::endl;
        std::cout << "rmse: " << std::sqrt(rmse) << ", mae: " << mae << std::endl;
        std::cout << "rmse: " << std::sqrt(rmse) * E_trans << ", mae: " << mae * E_trans << std::endl;
    }
    
}

int static test14(const parameter & para)
{
    NNoutputAnalysis NA;
    int infor;
    infor = NA.build(para);
    NA.ErrorCount();
    return 0;
}
int static test15(const parameter& para)
{
    return 0;
}
int static test16(const parameter& para)
{
    return 0;
}

int static build(const char* path, const char* name, xyzFile & out)
{
    std::string temp1, temp2, temp3;
    hyperlex::CFile CF;
    temp2 = path;
    temp3 = name;
    temp1 = CF.FusePathAndName(temp2, temp3);
    return build(temp1.c_str(), out);
}
int static build(const char* name, xyzFile& out)
{
    hyperlex::CFile CF;
    FILE* fp;
    int infor;
    fp = CF.OpenRead(name);
    infor = build(fp, out);
    fclose(fp);
    return infor;
}
int static build(FILE* fp, xyzFile& out)
{
    hyperlex::CharBuffer CB;
    hyperlex::UnitBuffer UB;
    xyz*temp;
    int state;
    size_t count, dim, dim_record;
    size_t i, amount;
    size_t length;
    vector<char*> name;
    vector<double> XYZ;
    double Energy;
    

    CB << fp;
    UB.build(CB);

    //CB.Print(stdout);
    //UB.print(stdout, 80);

    state = 0;
    count = 0;
    amount = 0;
    for (i = 0; i < UB.Site(); i++)
    {
        switch (state)
        {
        case 0:
            if (UB[i].type == 0)
            {
                length = UB[i].C.integer;
                name.recount(length);
                XYZ.recount(length * 6);
                name.clear();
                XYZ.clear();
                count = 0;
                state = 1;
                amount += 1;
            }
            else if (UB[i].lexical == 5 || UB[i].lexical == 11) return 1;
            else if (UB[i].type == 1)  return 2;
            break;
        case 1:
            if (UB[i].type == 1)
            {
                Energy = UB[i].C.real;
                state = 2;
                
            }
            else if (UB[i].lexical == 5 || UB[i].lexical == 11) return 3;
            else if (UB[i].type == 0) return 4;
            break;
        case 2:
            if (UB[i].lexical == 5 || UB[i].lexical == 11)
            {
                name.append(CopyMalloc(UB[i].C.S));
                state = 3;
                count += 1;
                dim = 0;
            }
            else if (UB[i].type == 0) return 5;
            else if (UB[i].type == 1) return 6;
            break;
        case 3:
            if (UB[i].type == 1)
            {
                XYZ.append(UB[i].C.real);
                dim++;
            }
            else if (UB[i].lexical == 63)
            {
                if (count == 1) dim_record = dim;
                else if (dim != dim_record) return 7;
                if (length == count)
                {
                    state = 0;
                    temp = new xyz;
                    temp->build(Energy, name, XYZ);
                    //if (amount < 2) temp->demo(stdout);
                    out.append(temp);
                }
                else state = 2;
            }
            else if (UB[i].lexical == 5 || UB[i].lexical == 11) return 8;
            else if (UB[i].type == 0) return 9;
            break;
        default:
            break;
        }
    }
    if (state != 0)
    {
        if (state == 3)
        {
            if (count == 1) dim_record = dim;
            else if (dim != dim_record) return 10;
            if (length == count)
            {
                temp = new xyz;
                temp->build(Energy, name, XYZ);
                out.append(temp);
            }
        }
        else return 11;
    }
    return 0;
}






#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL // upper 33
#define LM 0x7FFFFFFFULL // lower 31
//ref: https://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/VERSIONS/C-LANG/mt19937-64.c
/*
   A C-program for MT19937-64 (2004/9/29 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/

MersenneTwisters::MersenneTwisters()
{
    SetSeed(19650218ULL);
    mti = 0;
}
MersenneTwisters::~MersenneTwisters()
{
}
void MersenneTwisters::SetSeed(unsigned long long seed)
{
    mt[0] = seed;
    for (mti = 1; mti < NN; mti++)
        mt[mti] = (6364136223846793005ULL * (mt[mti - 1] ^ (mt[mti - 1] >> 62)) + mti);
}
void MersenneTwisters::twist(void)
{
    int i;
    unsigned long long x;
    for (i = 0; i < NN - MM; i++)
    {
        x = (mt[i] & UM) | (mt[i + 1] & LM);
        mt[i] = mt[i + MM] ^ (x >> 1) ^ ((x & 1ULL) == 0ULL ? 0ULL : MATRIX_A);
    }
    for (; i < NN - 1; i++)
    {
        x = (mt[i] & UM) | (mt[i + 1] & LM);
        mt[i] = mt[i + (MM - NN)] ^ (x >> 1) ^ ((x & 1ULL) == 0ULL ? 0ULL : MATRIX_A);
    }
    x = (mt[NN - 1] & UM) | (mt[0] & LM);
    mt[NN - 1] = mt[MM - 1] ^ (x >> 1) ^ ((x & 1ULL) == 0ULL ? 0ULL : MATRIX_A);

    mti = 0;
}
unsigned long long  MersenneTwisters::rand(void)
{
    unsigned long long  x;

    if (mti >= NN)  twist();
    x = mt[mti];

    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);
    mti += 1;
    return x;
}
double MersenneTwisters::real(void)
{
    return (rand() >> 11) * (1.0 / 9007199254740992.0);
}
double MersenneTwisters::RealCloseCLose(void)
{
    return (rand() >> 11) * (1.0 / 9007199254740991.0);
}
double MersenneTwisters::RealOpenOpen(void)
{
    return ((rand() >> 12) + 0.5) * (1.0 / 4503599627370496.0);
}
size_t MersenneTwisters::select(size_t N)
{
    return rand() % N;
}

#undef NN
#undef MM
#undef MATRIX_A
#undef UM
#undef LM



#include <string.h>
static char* CopyMalloc(const char* s)
{
    char* v;
    size_t size;
    size = strlen(s);
    v = (char*)malloc(sizeof(char) * (size + 4));
    strcpy(v, s);
    return v;
}
std::string static path_add_file_name(const std::string& path, const std::string& name)
{
    std::string fusion;
    if (path[path.length() - 1] == '/')
        fusion = path + name;
    else
        fusion = path + '/' + name;
    return fusion;
}