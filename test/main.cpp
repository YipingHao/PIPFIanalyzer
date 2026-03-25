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


int static DataMatrixSwitch(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions);

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
    return DataMatrixSwitch(dict, outputPath, expressions);
}
int static DataMatrixSwitch(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions)
{
    long int threadCount = dict.search((long int)1, "threadCount");
    printf("threadCount: %ld\n", threadCount);

    const char * DataFileName = dict.search("./data/origin.txt","DataFileName");
    printf("DataFileName: %s\n", DataFileName);

    FILE*inputMat = fopen(DataFileName, "r");
    if (inputMat == NULL) {
        printf("Error when opening data file: %s\n", DataFileName);
        return 1234234;
    }

    int row, col;
    analyzer::vector<double> matrix;
    int error = analyzer::ParserDataMatrix(inputMat, matrix, row, col);
    fclose(inputMat);
    if (error != 0) {
        printf("Error when parsing data file: %s, error: %d\n", DataFileName, error);
        return error;
    }

    printf("row: %d, col: %d\n", row, col);

    bool hasEnergy;
    if(expressions.XCount + 1 == col)
    {
        hasEnergy = true;
     
    }
    else if(expressions.XCount == col)
    {
        hasEnergy = false;
    }
    else
    {
        printf("Error: XCount != col && XCount + 1 != col\n");
        return -1;
    }
    // 计算输出矩阵的列数：PIP多项式数量 + (如果有能量则加1)
    size_t outputCols = expressions.getItems().size();
    size_t ldi = col; // 输入矩阵的领先维度
    size_t coli = hasEnergy ? (col - 1) : col; // 输入数据的列数（不包括能量列）
    size_t ldo = hasEnergy ? (outputCols + 1) : outputCols; // 输出矩阵的领先维度

    // 分配输出矩阵
    analyzer::vector<double> outputMatrix;
    outputMatrix.resize(row * ldo);
    // 准备输入和输出参数
    const double* inputData = matrix.ptr();
    double* outputData = outputMatrix.ptr();

    // 计算PIP值
    int computeError;
    if (threadCount > 1) {
        // 使用多线程计算
        computeError = expressions.compute(static_cast<unsigned int>(threadCount), 
                                         inputData, ldi, row, coli, 
                                         outputData, ldo, row, outputCols);
    } else {
        // 使用单线程计算
        computeError = expressions.compute(inputData, ldi, row, coli, 
                                         outputData, ldo, row, outputCols);
    }

    if (computeError != 0) {
        printf("Error when computing PIP values: %d\n", computeError);
        return computeError;
    }

    // 如果有能量列，将能量值复制到输出矩阵的最后一列
    if (hasEnergy) {
        for (size_t i = 0; i < row; ++i) {
            // 输入矩阵的最后一列是能量值
            double energy = matrix[i * ldi + coli];
            // 输出矩阵的最后一列存储能量值
            outputMatrix[i * ldo + outputCols] = energy;
        }
    }

    // 保存结果到输出文件
    const char * OutputFileName = dict.search("output","OutputFileName");
    std::string fileName = OutputFileName;
    std::string outputFilePath = ChangeSuffix(fileName, ".txt");
    hyperlex::FilePath filetemp;
    hyperlex::FilePath OutputFilePath;
    OutputFilePath.build(outputPath);
    filetemp.build(outputFilePath.c_str());
    OutputFilePath += filetemp;

    FILE* outputFile = fopen(OutputFilePath.path(), "w");
    if (outputFile != NULL) {
        // 写入表头信息
        fprintf(outputFile, "# PIP values generated by FIanalyzer\n");
        fprintf(outputFile, "# Number of data points: %d\n", row);
        fprintf(outputFile, "# Number of PIP polynomials: %zu\n", expressions.getItems().size());
        if (hasEnergy) {
            fprintf(outputFile, "# Including energy values\n");
        }
        fprintf(outputFile, "\n");

        // 写入数据
        for (size_t i = 0; i < row; ++i) {
            for (size_t j = 0; j < ldo; ++j) {
                fprintf(outputFile, "%25.16E ", outputMatrix[i * ldo + j]);
            }
            fprintf(outputFile, "\n");
        }

        fclose(outputFile);
        printf("Output file generated: %s\n", OutputFilePath.path());
    } else {
        printf("Error opening output file for writing: %s\n", OutputFilePath.path());
        return 1234;
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



