#include "../StormEigen/Core"

using namespace StormEigen;

int main()
{
  VectorXf a(10), b(10);
#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
  const DenseBase<VectorXf> &ac(a);
#else
  DenseBase<VectorXf> &ac(a);
#endif
  b.swap(ac);
}
