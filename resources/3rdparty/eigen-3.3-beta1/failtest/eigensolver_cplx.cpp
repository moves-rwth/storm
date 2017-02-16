#include "../StormEigen/Eigenvalues"

#ifdef STORMEIGEN_SHOULD_FAIL_TO_BUILD
#define SCALAR std::complex<double>
#else
#define SCALAR float
#endif

using namespace StormEigen;

int main()
{
  EigenSolver<Matrix<SCALAR,Dynamic,Dynamic> > eig(Matrix<SCALAR,Dynamic,Dynamic>::Random(10,10));
}
