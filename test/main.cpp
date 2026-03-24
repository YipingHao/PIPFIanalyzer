#include "../code/dictionary.h"
#include "../code/analyzer.h"
#include <cstring>

int TestEntrance(hyperlex::dictionary&dict);

int static TaskEntrance(hyperlex::dictionary&dict);

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
        return TestEntrance(dict);
    }
    else {
        return TaskEntrance(dict);
    }
    return 0;
}

int static TaskEntrance(hyperlex::dictionary&dict)
{
    const char * DataFileName = dict.search("./data/origin.txt","DataFileName");
    printf("DataFileName: %s\n", DataFileName);
    FILE*fp = fopen(DataFileName, "r");
    if (fp == NULL) {
        printf("Error when opening data file: %s\n", DataFileName);
        return 1123;
    }
    analyzer::FIexpresses expressions;
    int error = expressions.build(fp);
    fclose(fp);
    if (error != 0) {
        printf("Error when reading data file: %s, error: %d\n", DataFileName, error);
        return error;
    }
    else {
        printf("read data file end:\n");
    }

    expressions.demo(stdout);

    
    return 0;
}
