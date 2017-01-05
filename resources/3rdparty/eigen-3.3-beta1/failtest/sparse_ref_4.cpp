#include "../Eigen/Sparse"

using namespace StormEigen;

void call_ref(Ref<SparseMatrix<float> > a) {}

int main()
{
  SparseMatrix<float> A(10,10);
#ifdef EIGEN_SHOULD_FAIL_TO_BUILD
  call_ref(A.transpose());
#else
  call_ref(A);
#endif
}
