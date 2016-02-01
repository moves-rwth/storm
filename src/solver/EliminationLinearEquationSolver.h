#ifndef STORM_SOLVER_ELIMINATIONLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_ELIMINATIONLINEAREQUATIONSOLVER_H_

#include "src/solver/LinearEquationSolver.h"

namespace storm {
    namespace solver {
        /*!
         * A class that uses gaussian elimination to implement the LinearEquationSolver interface. In particular
         */
        template<typename ValueType>
        class EliminationLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            
            /*!
             * Constructs a linear equation solver.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             */
            EliminationLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
            
            virtual void solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
            virtual void performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const override;

        private:
            // A reference to the original matrix used for this equation solver.
            storm::storage::SparseMatrix<ValueType> const& A;
            
        };
    }
}

#endif /* STORM_SOLVER_ELIMINATIONLINEAREQUATIONSOLVER_H_ */