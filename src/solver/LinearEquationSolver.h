#ifndef STORM_SOLVER_LINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_LINEAREQUATIONSOLVER_H_

#include <vector>

#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace solver {
        
        /*!
         * An interface that represents an abstract linear equation solver. In addition to solving a system of linear
         * equations, the functionality to repeatedly multiply a matrix with a given vector is provided.
         */
        template<class Type>
        class LinearEquationSolver {
        public:
            /*!
             * Solves the equation system A*x = b. The matrix A is required to be square and have a unique solution.
             * The solution of the set of linear equations will be written to the vector x. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param x The solution vector that has to be computed. Its length must be equal to the number of rows of A.
             * @param b The right-hand side of the equation system. Its length must be equal to the number of rows of A.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             */
            virtual void solveEquationSystem(std::vector<Type>& x, std::vector<Type> const& b, std::vector<Type>* multiplyResult = nullptr) const = 0;
            
            /*!
             * Performs repeated matrix-vector multiplication, using x[0] = x and x[i + 1] = A*x[i] + b. After
             * performing the necessary multiplications, the result is written to the input vector x. Note that the
             * matrix A has to be given upon construction time of the solver object.
             *
             * @param x The initial vector with which to perform matrix-vector multiplication. Its length must be equal
             * to the number of rows of A.
             * @param b If non-null, this vector is added after each multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             */
            virtual void performMatrixVectorMultiplication(std::vector<Type>& x, std::vector<Type> const* b = nullptr, uint_fast64_t n = 1, std::vector<Type>* multiplyResult = nullptr) const = 0;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_LINEAREQUATIONSOLVER_H_ */
