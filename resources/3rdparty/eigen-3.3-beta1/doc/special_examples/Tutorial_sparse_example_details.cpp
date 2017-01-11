#include <StormEigen/Sparse>
#include <vector>
#include <QImage>

typedef StormEigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double
typedef StormEigen::Triplet<double> T;

void insertCoefficient(int id, int i, int j, double w, std::vector<T>& coeffs,
                       StormEigen::VectorXd& b, const StormEigen::VectorXd& boundary)
{
  int n = int(boundary.size());
  int id1 = i+j*n;

        if(i==-1 || i==n) b(id) -= w * boundary(j); // constrained coefficient
  else  if(j==-1 || j==n) b(id) -= w * boundary(i); // constrained coefficient
  else  coeffs.push_back(T(id,id1,w));              // unknown coefficient
}

void buildProblem(std::vector<T>& coefficients, StormEigen::VectorXd& b, int n)
{
  b.setZero();
  StormEigen::ArrayXd boundary = StormEigen::ArrayXd::LinSpaced(n, 0,M_PI).sin().pow(2);
  for(int j=0; j<n; ++j)
  {
    for(int i=0; i<n; ++i)
    {
      int id = i+j*n;
      insertCoefficient(id, i-1,j, -1, coefficients, b, boundary);
      insertCoefficient(id, i+1,j, -1, coefficients, b, boundary);
      insertCoefficient(id, i,j-1, -1, coefficients, b, boundary);
      insertCoefficient(id, i,j+1, -1, coefficients, b, boundary);
      insertCoefficient(id, i,j,    4, coefficients, b, boundary);
    }
  }
}

void saveAsBitmap(const StormEigen::VectorXd& x, int n, const char* filename)
{
  StormEigen::Array<unsigned char,StormEigen::Dynamic,Eigen::Dynamic> bits = (x*255).cast<unsigned char>();
  QImage img(bits.data(), n,n,QImage::Format_Indexed8);
  img.setColorCount(256);
  for(int i=0;i<256;i++) img.setColor(i,qRgb(i,i,i));
  img.save(filename);
}
