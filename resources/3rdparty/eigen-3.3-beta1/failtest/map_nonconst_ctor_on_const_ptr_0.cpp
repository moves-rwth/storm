#include "../StormEigen/Core"

#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
#define CV_QUALIFIER const
#else
#define CV_QUALIFIER
#endif

using namespace StormEigen;

void foo(CV_QUALIFIER float *ptr){
    Map<Matrix3f> m(ptr);
}

int main() {}
