extern "C" int cuda_basicAdd(int a, int b);

extern "C" void cuda_arrayFmaOptimized(int * const A, int const N, int const M);
extern "C" void cuda_arrayFmaOptimizedHelper(int * const A, int const N);

extern "C" void cuda_arrayFma(int const * const A, int const * const B, int const * const C, int * const D, int const N);
extern "C" void cuda_arrayFmaHelper(int const * const A, int const * const B, int const * const C, int * const D, int const N);

void cpp_cuda_bandwidthTest(int entryCount, int N);