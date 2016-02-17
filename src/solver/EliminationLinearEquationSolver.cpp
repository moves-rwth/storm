#include "src/solver/EliminationLinearEquationSolver.h"

#include "src/utility/vector.h"

namespace storm {
    namespace solver {
        template<typename ValueType>
        EliminationLinearEquationSolver<ValueType>::EliminationLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : A(A) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void EliminationLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            // TODO: implement state-elimination here.
        }
        
        template<typename ValueType>
        void EliminationLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* currentX = &x;
            
            bool multiplyResultProvided = true;
            std::vector<ValueType>* nextX = multiplyResult;
            if (nextX == nullptr) {
                nextX = new std::vector<ValueType>(x.size());
                multiplyResultProvided = false;
            }
            std::vector<ValueType> const* copyX = nextX;
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            for (uint_fast64_t i = 0; i < n; ++i) {
                A.multiplyWithVector(*currentX, *nextX);
                std::swap(nextX, currentX);
                
                // If requested, add an offset to the current result vector.
                if (b != nullptr) {
                    storm::utility::vector::addVectors(*currentX, *b, *currentX);
                }
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If the vector for the temporary multiplication result was not provided, we need to delete it.
            if (!multiplyResultProvided) {
                delete copyX;
            }
        }

        template class EliminationLinearEquationSolver<double>;
        
        // TODO: make this work with the proper implementation of solveEquationSystem.
        template class EliminationLinearEquationSolver<storm::CarlRationalNumber>;
        template class EliminationLinearEquationSolver<storm::RationalFunction>;
        
    }
}

