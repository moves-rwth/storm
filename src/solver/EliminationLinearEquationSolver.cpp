#include "src/solver/EliminationLinearEquationSolver.h"

#include <numeric>

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/EliminationSettings.h"

#include "src/solver/stateelimination/StatePriorityQueue.h"
#include "src/solver/stateelimination/PrioritizedStateEliminator.h"

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/stateelimination.h"

namespace storm {
    namespace solver {
        
        using namespace stateelimination;
        using namespace storm::utility::stateelimination;
        
        template<typename ValueType>
        EliminationLinearEquationSolver<ValueType>::EliminationLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : A(A) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void EliminationLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            STORM_LOG_WARN_COND(multiplyResult == nullptr, "Providing scratch memory will not improve the performance of this solver.");
            
            // We need to revert the transformation into an equation system matrix, because the elimination procedure
            // and the distance computation is based on the probability matrix instead.
            storm::storage::SparseMatrix<ValueType> transitionMatrix(A);
            transitionMatrix.convertToEquationSystem();
            storm::storage::SparseMatrix<ValueType> backwardTransitions = A.transpose();

            // Initialize the solution to the right-hand side of the equation system.
            x = b;
            
            // Translate the matrix and its transpose into the flexible format.
            storm::storage::FlexibleSparseMatrix<ValueType> flexibleMatrix(transitionMatrix, false);
            storm::storage::FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions(backwardTransitions, true);
            
            storm::settings::modules::EliminationSettings::EliminationOrder order = storm::settings::getModule<storm::settings::modules::EliminationSettings>().getEliminationOrder();
            boost::optional<std::vector<uint_fast64_t>> distanceBasedPriorities;
            if (eliminationOrderNeedsDistances(order)) {
                // Compute the distance from the first state.
                // FIXME: This is not exactly the initial states.
                storm::storage::BitVector initialStates = storm::storage::BitVector(A.getRowCount());
                initialStates.set(0);
                distanceBasedPriorities = getDistanceBasedPriorities(transitionMatrix, backwardTransitions, initialStates, b, eliminationOrderNeedsForwardDistances(order), eliminationOrderNeedsReversedDistances(order));
            }
            
            std::shared_ptr<StatePriorityQueue> priorityQueue = createStatePriorityQueue<ValueType>(distanceBasedPriorities, flexibleMatrix, flexibleBackwardTransitions, b, storm::storage::BitVector(x.size(), true));
            
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

