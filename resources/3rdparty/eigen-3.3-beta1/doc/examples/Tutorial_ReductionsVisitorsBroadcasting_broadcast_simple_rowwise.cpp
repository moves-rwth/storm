#include <iostream>
#include <Eigen/Dense>

using namespace std;
int main()
{
  StormEigen::MatrixXf mat(2,4);
  StormEigen::VectorXf v(4);
  
  mat << 1, 2, 6, 9,
         3, 1, 7, 2;
         
  v << 0,1,2,3;
       
  //add v to each row of m
  mat.rowwise() += v.transpose();
  
  std::cout << "Broadcasting result: " << std::endl;
  std::cout << mat << std::endl;
}
