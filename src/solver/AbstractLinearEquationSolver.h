#ifndef STORM_SOLVER_ABSTRACTLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_ABSTRACTLINEAREQUATIONSOLVER_H_

#include <vector>

#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that represents an abstract linear equation solver. In addition to solving a system of linear
         * equations, the functionality to repeatedly multiply a matrix with a given vector is provided.
         */
        template<class Type>
        class AbstractLinearEquationSolver {
        public:
            /*!
             * Makes a copy of the linear equation solver.
             *
             * @return A pointer to a copy of the linear equation solver.
             */
            virtual AbstractLinearEquationSolver<Type>* clone() const = 0;
            
            /*!
             * Solves the equation system A*x = b. The matrix A is required to be square and have a unique solution.
             * The solution of the set of linear equations will be written to the vector x.
             *
             * @param A The coefficient matrix of the equation system.
             * @param x The solution vector that has to be computed. Its length must be equal to the number of rows of A.
             * @param b The right-hand side of the equation system. Its length must be equal to the number of rows of A.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             */
            virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b, std::vector<Type>* multiplyResult = nullptr) const = 0;
            
            /*!
             * Performs repeated matrix-vector multiplication, using x[0] = x and x[i + 1] = A*x[i] + b. After
             * performing the necessary multiplications, the result is written to the input vector x.
             *
             * @param A The matrix to use for matrix-vector multiplication.
             * @param x The initial vector with which to perform matrix-vector multiplication. Its length must be equal
             * to the number of rows of A.
             * @param b If non-null, this vector is added after each multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             */
            virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type>* b = nullptr, uint_fast64_t n = 1, std::vector<Type>* multiplyResult = nullptr) const = 0;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_ABSTRACTLINEAREQUATIONSOLVER_H_ */
