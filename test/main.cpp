#include "../code/dictionary.h"
#include <cstring>

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

    int error = dict.build(parameterPath);
    dict.print(stdout);
    if (error != 0) {
        printf("Error when reading parameter file: %s\n", parameterPath);
        return 1;
    }
    
    

    return 0;
}