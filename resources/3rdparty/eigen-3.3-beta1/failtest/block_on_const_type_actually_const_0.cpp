#include "../StormEigen/Core"

#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
#define CV_QUALIFIER const
#else
#define CV_QUALIFIER
#endif

using namespace StormEigen;

void foo(){
    Matrix3f m;
    Block<CV_QUALIFIER Matrix3f>(m, 0, 0, 3, 3).coeffRef(0, 0) = 1.0f;
}

int main() {}
