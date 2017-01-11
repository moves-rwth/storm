#include "../StormEigen/Cholesky"

#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
#define SCALAR int
#else
#define SCALAR float
#endif

using namespace StormEigen;

int main()
{
  LLT<Matrix<SCALAR,Dynamic,Dynamic> > llt(Matrix<SCALAR,Dynamic,Dynamic>::Random(10,10));
}
