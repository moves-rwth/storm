#include <Eigen/Core>
#include <iostream>
using namespace StormEigen;
using namespace std;

template<typename Derived>
StormEigen::VectorBlock<Derived>
segmentFromRange(MatrixBase<Derived>& v, int start, int end)
{
  return StormEigen::VectorBlock<Derived>(v.derived(), start, end-start);
}

template<typename Derived>
const StormEigen::VectorBlock<const Derived>
segmentFromRange(const MatrixBase<Derived>& v, int start, int end)
{
  return StormEigen::VectorBlock<const Derived>(v.derived(), start, end-start);
}

int main(int, char**)
{
  Matrix<int,1,6> v; v << 1,2,3,4,5,6;
  cout << segmentFromRange(2*v, 2, 4) << endl; // calls the const version
  segmentFromRange(v, 1, 3) *= 5;              // calls the non-const version
  cout << "Now the vector v is:" << endl << v << endl;
  return 0;
}
