#include "analyzer.h"
#include <pthread.h>

using namespace analyzer;

// 线程参数结构体
typedef struct {
    const FIexpresses* fiExpresses;
    const double* input;
    size_t ldi;
    size_t startRow;
    size_t endRow;
    size_t coli;
    double* output;
    size_t ldo;
    size_t colo;
} ThreadData;

// 线程函数
void* computeThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    const FIexpresses* fiExpresses = data->fiExpresses;
    const double* input = data->input;
    size_t ldi = data->ldi;
    size_t startRow = data->startRow;
    size_t endRow = data->endRow;
    size_t coli = data->coli;
    double* output = data->output;
    size_t ldo = data->ldo;
    size_t colo = data->colo;
    
    // 获取FIexpress对象列表
    const vector<FIexpress>& items = fiExpresses->getItems();
    
    // 处理分配给当前线程的行
    for (size_t i = startRow; i < endRow; ++i) {
        // 计算当前输入数据的起始位置
        const double* currentInput = input + i * ldi;
        // 计算当前输出数据的起始位置
        double* currentOutput = output + i * ldo;
        
        // 对每条数据，计算所有FIexpress对象的值
        for (size_t j = 0; j < items.size(); ++j) {
            // 计算当前FIexpress的值
            double value = items[j].compute_bare(currentInput);
            // 将计算结果存储到输出矩阵的对应位置
            currentOutput[j] = value;
        }
    }
    
    return NULL;
}

int FIexpresses::compute(unsigned int threadCount, const double* input, size_t ldi, size_t rowi, size_t coli, double* output, size_t ldo, size_t rowo, size_t colo) const
{
    // 检查输入输出的行数是否匹配
    if (rowi != rowo) {
        return -1;
    }
    
    // 检查列数是否与输入数据的维度匹配
    if (coli != XCount) {
        return -3;
    }
    
    // 检查输出数据的维度是否与所有FIexpress的次数总和匹配
    if (colo != items.size()) {
        return -4;
    }
    
    // 如果线程数为0或1，直接使用单线程版本
    if (threadCount == 0 || threadCount == 1) {
        return compute(input, ldi, rowi, coli, output, ldo, rowo, colo);
    }
    
    // 计算每个线程处理的行数
    size_t rowsPerThread = rowi / threadCount;
    size_t remainingRows = rowi % threadCount;
    
    // 创建线程和线程数据
    pthread_t* threads = new pthread_t[threadCount];
    ThreadData* threadData = new ThreadData[threadCount];
    
    // 分配任务并创建线程
    size_t currentRow = 0;
    for (unsigned int i = 0; i < threadCount; ++i) {
        // 计算当前线程处理的行数
        size_t threadRows = rowsPerThread + (i < remainingRows ? 1 : 0);
        
        // 设置线程数据
        threadData[i].fiExpresses = this;
        threadData[i].input = input;
        threadData[i].ldi = ldi;
        threadData[i].startRow = currentRow;
        threadData[i].endRow = currentRow + threadRows;
        threadData[i].coli = coli;
        threadData[i].output = output;
        threadData[i].ldo = ldo;
        threadData[i].colo = colo;
        
        // 创建线程
        pthread_create(&threads[i], NULL, computeThread, &threadData[i]);
        
        // 更新当前行
        currentRow += threadRows;
    }
    
    // 等待所有线程完成
    for (unsigned int i = 0; i < threadCount; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    // 释放资源
    delete[] threads;
    delete[] threadData;
    
    return 0;
}


