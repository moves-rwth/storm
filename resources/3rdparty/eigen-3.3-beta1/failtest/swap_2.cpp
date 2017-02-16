#include "../StormEigen/Core"

using namespace StormEigen;

int main()
{
  VectorXf a(10), b(10);
  VectorXf const &ac(a);
#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
  b.swap(ac);
#else
  b.swap(ac.const_cast_derived());
#endif
}
