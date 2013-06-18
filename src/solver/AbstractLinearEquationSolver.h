#ifndef STORM_SOLVER_ABSTRACTLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_ABSTRACTLINEAREQUATIONSOLVER_H_

#include "src/storage/SparseMatrix.h"

#include <vector>

namespace storm {
    namespace solver {
        
        template<class Type>
        class AbstractLinearEquationSolver {
        public:
            
            virtual AbstractLinearEquationSolver<Type>* clone() const = 0;
            
            virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const = 0;
            
            virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type>* b = nullptr, uint_fast64_t n = 1) const = 0;
            
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_ABSTRACTLINEAREQUATIONSOLVER_H_ */
