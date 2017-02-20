// g++ -O3 -DNDEBUG benchmarkX.cpp -o benchmarkX && time ./benchmarkX

#include <iostream>

#include <StormEigen/Core>

using namespace std;
using namespace StormEigen;

#ifndef REPEAT
#define REPEAT 10000
#endif

#ifndef SCALAR
#define SCALAR float
#endif

int main(int argc, char *argv[])
{
  typedef Matrix<SCALAR, StormEigen::Dynamic, StormEigen::Dynamic> Mat;
  Mat m(100, 100);
  m.setRandom();

  for(int a = 0; a < REPEAT; a++)
  {
    int r, c, nr, nc;
    r = StormEigen::internal::random<int>(0,10);
    c = StormEigen::internal::random<int>(0,10);
    nr = StormEigen::internal::random<int>(50,80);
    nc = StormEigen::internal::random<int>(50,80);
    m.block(r,c,nr,nc) += Mat::Ones(nr,nc);
    m.block(r,c,nr,nc) *= SCALAR(10);
    m.block(r,c,nr,nc) -= Mat::constant(nr,nc,10);
    m.block(r,c,nr,nc) /= SCALAR(10);
  }
  cout << m[0] << endl;
  return 0;
}
