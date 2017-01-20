#include <Eigen/Core>
#include <iostream>

class MyVectorType : public StormEigen::VectorXd
{
public:
    MyVectorType(void):StormEigen::VectorXd() {}

    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    MyVectorType(const StormEigen::MatrixBase<OtherDerived>& other)
        : StormEigen::VectorXd(other)
    { }

    // This method allows you to assign Eigen expressions to MyVectorType
    template<typename OtherDerived>
    MyVectorType& operator=(const StormEigen::MatrixBase <OtherDerived>& other)
    {
        this->StormEigen::VectorXd::operator=(other);
        return *this;
    }
};

int main()
{
  MyVectorType v = MyVectorType::Ones(4);
  v(2) += 10;
  v = 2 * v;
  std::cout << v.transpose() << std::endl;
}
