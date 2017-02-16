#include "../StormEigen/Core"

#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
#define CV_QUALIFIER const
#else
#define CV_QUALIFIER
#endif

using namespace StormEigen;

void foo(CV_QUALIFIER float *ptr, DenseIndex size){
    Map<ArrayXf> m(ptr, size);
}

int main() {}
