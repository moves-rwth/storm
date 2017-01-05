#include "../Eigen/Sparse"

using namespace StormEigen;

void call_ref(Ref<SparseMatrix<float> > a) { }

int main()
{
  SparseMatrix<float> a(10,10);
  SparseMatrixBase<SparseMatrix<float> > &ac(a);
#ifdef EIGEN_SHOULD_FAIL_TO_BUILD
  call_ref(ac);
#else
  call_ref(ac.derived());
#endif
}
