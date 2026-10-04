#include "../code/dictionary.h"
#include "../code/analyzer.h"
#include <cstring>
#include <string>
#include <climits>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

int TestEntrance(hyperlex::dictionary&dict, const char* outputPath);

int static TaskEntrance(hyperlex::dictionary&dict, const char* outputPath, const char* task);
std::string static ChangeSuffix(const std::string& file, const char* new_one);
int static Benchmark(hyperlex::dictionary& dict, const char* outputPath,
                     analyzer::FIexpresses& expressions,
                     double formulaLoadSeconds, const char* formulaFormat);

static double wallSeconds()
{
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec * 1.0e-6;
#endif
}

static long long peakRssKiB()
{
#ifdef _WIN32
    return -1;
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) != 0) return -1;
#ifdef __APPLE__
    return (long long)(usage.ru_maxrss / 1024);
#else
    return (long long)usage.ru_maxrss;
#endif
#endif
}

static bool endsWith(const std::string& value, const char* suffix)
{
    const size_t suffixLength = std::strlen(suffix);
    return value.size() >= suffixLength &&
           value.compare(value.size() - suffixLength, suffixLength, suffix) == 0;
}

static void printJsonString(FILE* fp, const char* value)
{
    std::fputc('"', fp);
    if (value != NULL)
    {
        for (const unsigned char* p = (const unsigned char*)value; *p; ++p)
        {
            if (*p == '"' || *p == '\\')
            {
                std::fputc('\\', fp);
                std::fputc(*p, fp);
            }
            else if (*p == '\n') std::fputs("\\n", fp);
            else if (*p == '\r') std::fputs("\\r", fp);
            else if (*p == '\t') std::fputs("\\t", fp);
            else if (*p >= 0x20) std::fputc(*p, fp);
        }
    }
    std::fputc('"', fp);
}

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
        return TaskEntrance(dict, outputPath, task);
    }
    return 0;
}

void static CodeGeneration(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions)
{
    bool CcodePrint = dict.search(false,"CodeGenSetting.CcodePrint");
    bool FortranCodePrint = dict.search(false,"CodeGenSetting.FortranCodePrint");
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
int static Cutoff(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions);
int static SFI(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions);

int static TaskEntrance(hyperlex::dictionary&dict, const char* outputPath, const char* task)
{
    const char * PIPFileName = dict.search("./data/origin.txt","PIPFileName");
    const char * PIPFileFormat = dict.search("Auto", "PIPFileFormat");
    const bool benchmarkMode = strcmp(task, "benchmark") == 0;
    bool binaryFormat = false;
    if (strcmp(PIPFileFormat, "Binary") == 0)
        binaryFormat = true;
    else if (strcmp(PIPFileFormat, "Text") == 0)
        binaryFormat = false;
    else if (strcmp(PIPFileFormat, "Auto") == 0)
        binaryFormat = endsWith(PIPFileName, ".pipbin");
    else
    {
        printf("Error: PIPFileFormat must be Auto, Text, or Binary.\n");
        return -30;
    }

    printf("PIPFileName: %s\n", PIPFileName);
    printf("PIPFileFormat: %s\n", binaryFormat ? "Binary" : "Text");
    const double formulaLoadStart = wallSeconds();
    FILE*fp = fopen(PIPFileName, binaryFormat ? "rb" : "r");
    if (fp == NULL) {
        printf("Error when opening PIP file: %s\n", PIPFileName);
        return 1123;
    }
    analyzer::FIexpresses expressions;
    int error = binaryFormat ? expressions.buildBinary(fp, !benchmarkMode)
                             : expressions.build(fp, !benchmarkMode);
    fclose(fp);
    const double formulaLoadSeconds = wallSeconds() - formulaLoadStart;
    if (error != 0) {
        printf("Error when reading PIP file: %s, error: %d\n", PIPFileName, error);
        return error;
    }
    else {
        printf("read PIP file end:\n");
    }

    if (!benchmarkMode)
        expressions.demo(stdout);

    if (strcmp(task, "dataswitch") == 0) 
    {
        return DataMatrixSwitch(dict, outputPath, expressions);
    }
    else if (strcmp(task, "CodeGeneration") == 0)
    {
        CodeGeneration(dict, outputPath, expressions);
        return 0;
    }
    else if (strcmp(task, "cutoff") == 0) 
    {
        return Cutoff(dict, outputPath, expressions);
    }
    else if (strcmp(task, "SFI") == 0) 
    {
        return SFI(dict, outputPath, expressions);
    }
    else if (benchmarkMode)
    {
        return Benchmark(dict, outputPath, expressions, formulaLoadSeconds,
                         binaryFormat ? "Binary" : "Text");
    }
    else
    {
        printf("Unknown task: %s\n", task);
        return -156489;
    }
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

    size_t row, col;
    analyzer::vector<double> matrix;
    int error = analyzer::ParserDataMatrix(inputMat, matrix, row, col);
    fclose(inputMat);
    if (error != 0) {
        printf("Error when parsing data file: %s, error: %d\n", DataFileName, error);
        return error;
    }

    printf("row: %zu, col: %zu\n", row, col);

    bool hasEnergy;
    size_t xCount = expressions.getXCount();
    if(xCount + 1 == col)
    {
        hasEnergy = true;
     
    }
    else if(xCount == col)
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
        fprintf(outputFile, "# Number of data points: %zu\n", row);
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

int static Benchmark(hyperlex::dictionary& dict, const char* outputPath,
                     analyzer::FIexpresses& expressions,
                     double formulaLoadSeconds, const char* formulaFormat)
{
    const long int threadCountValue = dict.search((long int)1, "threadCount");
    const long int batchSizeValue = dict.search((long int)32, "BenchmarkSetting.batchSize");
    const long int warmupCountValue = dict.search((long int)3, "BenchmarkSetting.warmupCount");
    const long int repeatCountValue = dict.search((long int)10, "BenchmarkSetting.repeatCount");
    if (threadCountValue < 1 || (unsigned long)threadCountValue > (unsigned long)UINT_MAX ||
        batchSizeValue < 1 || warmupCountValue < 0 || repeatCountValue < 1 ||
        (unsigned long long)batchSizeValue > (unsigned long long)(size_t)-1 ||
        (unsigned long long)warmupCountValue > (unsigned long long)(size_t)-1 ||
        (unsigned long long)repeatCountValue > (unsigned long long)(size_t)-1)
    {
        printf("Error: benchmark settings require threadCount>=1, batchSize>=1, warmupCount>=0, repeatCount>=1.\n");
        return -40;
    }

    const size_t batchSize = (size_t)batchSizeValue;
    const size_t warmupCount = (size_t)warmupCountValue;
    const size_t repeatCount = (size_t)repeatCountValue;
    const unsigned int threadCount = (unsigned int)threadCountValue;
    const char* dataFileName = dict.search("./data/origin.txt", "DataFileName");

    const double dataLoadStart = wallSeconds();
    FILE* inputMat = fopen(dataFileName, "r");
    if (inputMat == NULL)
    {
        printf("Error when opening data file: %s\n", dataFileName);
        return 1234234;
    }

    size_t row = 0, col = 0;
    analyzer::vector<double> matrix;
    int error = analyzer::ParserDataMatrix(inputMat, matrix, row, col);
    fclose(inputMat);
    const double dataLoadSeconds = wallSeconds() - dataLoadStart;
    if (error != 0)
    {
        printf("Error when parsing data file: %s, error: %d\n", dataFileName, error);
        return error;
    }
    if (row < batchSize)
    {
        printf("Error: data file has %zu rows but benchmark batchSize is %zu.\n", row, batchSize);
        return -41;
    }

    const size_t xCount = expressions.getXCount();
    bool hasEnergy = false;
    if (col == xCount + 1) hasEnergy = true;
    else if (col != xCount)
    {
        printf("Error: input column count %zu does not match feature count %zu.\n", col, xCount);
        return -42;
    }

    const size_t outputCols = expressions.getItems().size();
    if (outputCols != 0 && batchSize > ((size_t)-1) / outputCols)
    {
        printf("Error: benchmark output matrix size overflows size_t.\n");
        return -43;
    }

    analyzer::vector<double> outputMatrix;
    outputMatrix.resize(batchSize * outputCols);
    const size_t inputCols = hasEnergy ? col - 1 : col;

    const double warmupStart = wallSeconds();
    for (size_t i = 0; i < warmupCount; ++i)
    {
        if (threadCount > 1)
            error = expressions.compute(threadCount, matrix.ptr(), col, batchSize, inputCols,
                                        outputMatrix.ptr(), outputCols, batchSize, outputCols);
        else
            error = expressions.compute(matrix.ptr(), col, batchSize, inputCols,
                                        outputMatrix.ptr(), outputCols, batchSize, outputCols);
        if (error != 0)
        {
            printf("Error during benchmark warm-up: %d\n", error);
            return error;
        }
    }
    const double warmupSeconds = wallSeconds() - warmupStart;

    const double evaluationStart = wallSeconds();
    for (size_t i = 0; i < repeatCount; ++i)
    {
        if (threadCount > 1)
            error = expressions.compute(threadCount, matrix.ptr(), col, batchSize, inputCols,
                                        outputMatrix.ptr(), outputCols, batchSize, outputCols);
        else
            error = expressions.compute(matrix.ptr(), col, batchSize, inputCols,
                                        outputMatrix.ptr(), outputCols, batchSize, outputCols);
        if (error != 0)
        {
            printf("Error during timed benchmark evaluation: %d\n", error);
            return error;
        }
    }
    const double evaluationSeconds = wallSeconds() - evaluationStart;

    long double checksum = 0.0L;
    for (size_t i = 0; i < outputMatrix.size(); ++i)
        checksum += (long double)outputMatrix[i];

    unsigned long long factorIndexCount = 0;
    const analyzer::vector<analyzer::FIexpress>& polys = expressions.getItems();
    for (size_t i = 0; i < polys.size(); ++i)
        factorIndexCount += (unsigned long long)polys[i].getOrder() *
                            (unsigned long long)polys[i].getItemCount();

    const double meanRepeatSeconds = evaluationSeconds / (double)repeatCount;
    const double secondsPerPoint = meanRepeatSeconds / (double)batchSize;
    const double pointsPerSecond = secondsPerPoint > 0.0 ? 1.0 / secondsPerPoint : 0.0;
    const long long peakRss = peakRssKiB();

    const char* outputFileName = dict.search("output", "OutputFileName");
    std::string reportName = ChangeSuffix(outputFileName, ".benchmark.json");
    hyperlex::FilePath reportLeaf;
    hyperlex::FilePath reportPath;
    reportPath.build(outputPath);
    reportLeaf.build(reportName.c_str());
    reportPath += reportLeaf;

    FILE* report = fopen(reportPath.path(), "w");
    if (report == NULL)
    {
        printf("Error opening benchmark report: %s\n", reportPath.path());
        return -44;
    }

    fprintf(report, "{\n");
    fprintf(report, "  \"schema_version\": 1,\n");
    fprintf(report, "  \"formula_file\": "); printJsonString(report, dict.search("", "PIPFileName")); fprintf(report, ",\n");
    fprintf(report, "  \"formula_format\": "); printJsonString(report, formulaFormat); fprintf(report, ",\n");
    fprintf(report, "  \"data_file\": "); printJsonString(report, dataFileName); fprintf(report, ",\n");
    fprintf(report, "  \"feature_count\": %zu,\n", xCount);
    fprintf(report, "  \"polynomial_count\": %zu,\n", outputCols);
    fprintf(report, "  \"factor_index_count\": %llu,\n", factorIndexCount);
    fprintf(report, "  \"batch_size\": %zu,\n", batchSize);
    fprintf(report, "  \"thread_count\": %u,\n", threadCount);
    fprintf(report, "  \"warmup_count\": %zu,\n", warmupCount);
    fprintf(report, "  \"repeat_count\": %zu,\n", repeatCount);
    fprintf(report, "  \"formula_load_seconds\": %.9f,\n", formulaLoadSeconds);
    fprintf(report, "  \"data_load_seconds\": %.9f,\n", dataLoadSeconds);
    fprintf(report, "  \"warmup_seconds\": %.9f,\n", warmupSeconds);
    fprintf(report, "  \"evaluation_seconds\": %.9f,\n", evaluationSeconds);
    fprintf(report, "  \"mean_repeat_seconds\": %.9f,\n", meanRepeatSeconds);
    fprintf(report, "  \"seconds_per_point\": %.12f,\n", secondsPerPoint);
    fprintf(report, "  \"points_per_second\": %.9f,\n", pointsPerSecond);
    fprintf(report, "  \"peak_rss_kib\": %lld,\n", peakRss);
    fprintf(report, "  \"checksum\": %.17Lg\n", checksum);
    fprintf(report, "}\n");
    fclose(report);

    printf("Benchmark report generated: %s\n", reportPath.path());
    printf("METRIC formula_load_seconds=%.9f\n", formulaLoadSeconds);
    printf("METRIC data_load_seconds=%.9f\n", dataLoadSeconds);
    printf("METRIC warmup_seconds=%.9f\n", warmupSeconds);
    printf("METRIC evaluation_seconds=%.9f\n", evaluationSeconds);
    printf("METRIC mean_repeat_seconds=%.9f\n", meanRepeatSeconds);
    printf("METRIC seconds_per_point=%.12f\n", secondsPerPoint);
    printf("METRIC points_per_second=%.9f\n", pointsPerSecond);
    printf("METRIC peak_rss_kib=%lld\n", peakRss);
    printf("METRIC checksum=%.17Lg\n", checksum);
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

int static Cutoff(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions)
{
    const char* threshold = dict.search("order", "CutoffSetting.threshold");
    int order = (int)dict.search((long int)-1, "CutoffSetting.order");
    long int workload = dict.search((long int)-1, "CutoffSetting.workload");
    bool crossItem = dict.search(false, "CutoffSetting.CrossItem");

    printf("Cutoff task started:\n");
    printf("  threshold: %s\n", threshold);
    if (strcmp(threshold, "order") == 0) {
        printf("  order: %d\n", order);
        expressions.cutoffByOrder(order, crossItem);
    } else if (strcmp(threshold, "workload") == 0) {
        printf("  workload: %ld\n", workload);
        expressions.cutoffByWorkload((size_t)workload, crossItem);
    } else {
        printf("  Unknown threshold: %s\n", threshold);
    }
    printf("  CrossItem: %s\n", crossItem ? "true" : "false");

    printf("\nCutoff applied successfully.\n");
    expressions.demo(stdout);

    // 输出截断后的原始格式文本
    const char * OutputFileName = dict.search("output", "OutputFileName");
    std::string fileName = OutputFileName;
    std::string printFilePath = ChangeSuffix(fileName, ".txt");
    
    hyperlex::FilePath filetemp;
    hyperlex::FilePath PrintFilePathPath;
    PrintFilePathPath.build(outputPath);
    filetemp.build(printFilePath.c_str());
    PrintFilePathPath += filetemp;

    FILE* pFile = fopen(PrintFilePathPath.path(), "w");
    if (pFile != NULL) {
        expressions.print(pFile);
        fclose(pFile);
        printf("\nPolynomials printed to file: %s\n", PrintFilePathPath.path());
    } else {
        printf("\nError opening file for printing polynomials: %s\n", PrintFilePathPath.path());
    }

    // 生成代码保存截断后的特征
    // CodeGeneration(dict, outputPath, expressions);
    
    return 0;
}

int static SFI(hyperlex::dictionary&dict, const char* outputPath, analyzer::FIexpresses & expressions)
{
    // 这里实现 SFI 任务的逻辑
    printf("SFI task is not implemented yet.\n");
    return 0;
}
