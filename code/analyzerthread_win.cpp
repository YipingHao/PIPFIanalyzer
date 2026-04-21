#include "analyzer.h"

using namespace analyzer;

int FIexpresses::compute(unsigned int threadCount, const double*input, size_t ldi, size_t rowi, size_t coli, double* output, size_t ldo, size_t rowo, size_t colo) const
{
    // Keep behavior deterministic when no threading backend is enabled.
    // This overload is required by callers that pass a thread count.
    (void)threadCount;
    return compute(input, ldi, rowi, coli, output, ldo, rowo, colo);
}