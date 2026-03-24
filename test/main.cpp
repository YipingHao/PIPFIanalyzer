#include "../code/dictionary.h"
#include "../code/analyzer.h"
#include <cstring>
#include <string>

int TestEntrance(hyperlex::dictionary&dict, const char* outputPath);

int static TaskEntrance(hyperlex::dictionary&dict, const char* outputPath);
std::string static ChangeSuffix(const std::string& file, const char* new_one);

int main(int argc, char* argv[])
{
    hyperlex::dictionary dict;

    const char* parameterPath = "./parameter/input.txt";
    const char* outputPath = "./output/";

    if (argc > 1) {
        parameterPath = argv[1];
    }
    if (argc > 2) {
        outputPath = argv[2];
    }

    FILE*fp = fopen(parameterPath, "r");
    if (fp == NULL) {
        printf("Error when opening parameter file: %s\n", parameterPath);
        return 1123123;
    }

    int error = dict.build(fp);
    fclose(fp);
    dict.print(stdout);
    if (error != 0) {
        printf("Error when reading parameter file: %s, error: %d\n", parameterPath, error);
        return error;
    }
    
    const char * task = dict.search("test","item");
    printf("task: %s\n", task);
    

    if (strcmp(task, "test") == 0) {
        return TestEntrance(dict, outputPath);
    }
    else {
        return TaskEntrance(dict, outputPath);
    }
    return 0;
}

void static CodeGeneration(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions)
{
    bool CcodePrint = dict.search(false,"CcodePrint");
    bool FortranCodePrint = dict.search(false,"FortranCodePrint");
    const char * OutputFileName = dict.search("output","OutputFileName");
    std::string fileName = OutputFileName;


    // 生成C代码
    if (CcodePrint) {
        std::string cFilePath = ChangeSuffix(fileName, ".c");
        hyperlex::FilePath filetemp;
        hyperlex::FilePath OutputFilePath;
        OutputFilePath.build(outputPath);
        filetemp.build(cFilePath.c_str());
        OutputFilePath += filetemp;
        FILE* cFile = fopen(OutputFilePath.path(), "w");
        if (cFile != NULL) {
            expressions.printCcode(cFile);
            fclose(cFile);
            printf("C code generated: %s\n", OutputFilePath.path());
        } else {
            printf("Error opening C code file for writing: %s\n", OutputFilePath.path());
        }
    }

    // 生成Fortran代码
    if (FortranCodePrint) {
        std::string fortranFilePath = ChangeSuffix(fileName, ".f90");
        hyperlex::FilePath filetemp;
        hyperlex::FilePath OutputFilePath;
        OutputFilePath.build(outputPath);
        filetemp.build(fortranFilePath.c_str());
        OutputFilePath += filetemp;
        FILE* fortranFile = fopen(OutputFilePath.path(), "w");
        if (fortranFile != NULL) {
            expressions.printFortrancode(fortranFile);
            fclose(fortranFile);
            printf("Fortran code generated: %s\n", OutputFilePath.path());
        } else {
            printf("Error opening Fortran code file for writing: %s\n", OutputFilePath.path());
        }
    }
}




int static TaskEntrance(hyperlex::dictionary&dict, const char* outputPath)
{
    const char * PIPFileName = dict.search("./data/origin.txt","PIPFileName");
    printf("PIPFileName: %s\n", PIPFileName);
    FILE*fp = fopen(PIPFileName, "r");
    if (fp == NULL) {
        printf("Error when opening PIP file: %s\n", PIPFileName);
        return 1123;
    }
    analyzer::FIexpresses expressions;
    int error = expressions.build(fp);
    fclose(fp);
    if (error != 0) {
        printf("Error when reading PIP file: %s, error: %d\n", PIPFileName, error);
        return error;
    }
    else {
        printf("read PIP file end:\n");
    }

    expressions.demo(stdout);

    CodeGeneration(dict, outputPath, expressions);

    long int threadCount = dict.search((long int)1, "threadCount");
    printf("threadCount: %ld\n", threadCount);

    const char * DataFileName = dict.search("./data/origin.txt","DataFileName");
    printf("DataFileName: %s\n", DataFileName);

    FILE*inputMat = fopen(DataFileName, "r");
    if (inputMat == NULL) {
        printf("Error when opening data file: %s\n", DataFileName);
        return 1234234;
    }


    
    return 0;
}


std::string static ChangeSuffix(const std::string& file, const char* new_one)
{
    size_t i, j;
    std::string name;
    name = "";
    for (i = file.length(); i != 0; i--)
        if (file[i - 1] == '.') break;
    if (i == 0)
    {
        name = file;
        name += '.';
    }
    else
    {
        for (j = 0; j < i; j++)
            name += file[j];
    }
    if (new_one[0] == '.') name += (new_one + 1);
    else name += new_one;
    return name;
}