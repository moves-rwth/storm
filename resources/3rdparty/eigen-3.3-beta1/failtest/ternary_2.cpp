#include "../StormEigen/Core"

using namespace StormEigen;

int main(int argc,char **)
{
  VectorXf a(10), b(10);
#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
  b = argc>1 ? 2*a : a+a;
#else
  b = argc>1 ? VectorXf(2*a) : VectorXf(a+a);
#endif
}
