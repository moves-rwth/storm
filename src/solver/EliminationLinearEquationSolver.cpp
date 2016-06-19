#include "src/solver/EliminationLinearEquationSolver.h"

#include <numeric>

#include "src/solver/stateelimination/StatePriorityQueue.h"
#include "src/solver/stateelimination/PrioritizedStateEliminator.h"

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/stateelimination.h"

namespace storm {
    namespace solver {
        
        using namespace stateelimination;
        
        template<typename ValueType>
        EliminationLinearEquationSolver<ValueType>::EliminationLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : A(A) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void EliminationLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            STORM_LOG_WARN_COND(multiplyResult == nullptr, "Providing scratch memory will not improve the performance of this solver.");
            
//            std::cout << "input:" << std::endl;
//            std::cout << "A:" << std::endl;
//            std::cout << A << std::endl;
//            std::cout << "b:" << std::endl;
//            for (auto const& e : b) {
//                std::cout << e << std::endl;
//            }
            
            // Create a naive priority queue.
            // TODO: improve.
            std::vector<storm::storage::sparse::state_type> allRows(x.size());
            std::iota(allRows.begin(), allRows.end(), 0);
            std::shared_ptr<StatePriorityQueue> priorityQueue = storm::utility::stateelimination::createStatePriorityQueue(allRows);
            
            // Initialize the solution to the right-hand side of the equation system.
            x = b;
            
            // Translate the matrix and its transpose into the flexible format.
            // We need to revert the matrix from the equation system format to the probability matrix format. That is,
            // from (I-P), we construct P. Note that for the backwards transitions, this does not need to be done, as all
            // entries are set to one anyway.
            storm::storage::FlexibleSparseMatrix<ValueType> flexibleMatrix(A, false, true);
            storm::storage::FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions(A.transpose(), true, true);
            
//            std::cout << "intermediate:" << std::endl;
//            std::cout << "flexibleMatrix:" << std::endl;
//            std::cout << flexibleMatrix << std::endl;
//            std::cout << "backward:" << std::endl;
//            std::cout << flexibleBackwardTransitions << std::endl;
            
            // Create a state eliminator to perform the actual elimination.
            PrioritizedStateEliminator<ValueType> eliminator(flexibleMatrix, flexibleBackwardTransitions, priorityQueue, x);
            
            std::cout << "eliminating" << std::endl;
            while (priorityQueue->hasNext()) {
                auto state = priorityQueue->pop();
                eliminator.eliminateState(state, false);
            }
            
//            std::cout << "output:" << std::endl;
//            std::cout << "x:" << std::endl;
//            for (auto const& e : x) {
//                std::cout << e << std::endl;
//            }
            
            std::cout << "done" << std::endl;
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
        template class EliminationLinearEquationSolver<storm::RationalNumber>;
        template class EliminationLinearEquationSolver<storm::RationalFunction>;
        
    }
}

