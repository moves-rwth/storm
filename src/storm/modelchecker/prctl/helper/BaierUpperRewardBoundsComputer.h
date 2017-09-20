#pragma once

#include <vector>

namespace storm {
    namespace storage {
        template<typename ValueType>
        class SparseMatrix;
    }
    
    namespace modelchecker {
        namespace helper {
            
            template<typename ValueType>
            class BaierUpperRewardBoundsComputer {
            public:
                /*!
                 * Creates an object that can compute upper bounds on the *maximal* expected rewards for the provided MDP.
                 *
                 * @param transitionMatrix The matrix defining the transitions of the system without the transitions
                 * that lead directly to the goal state.
                 * @param rewards The rewards of each choice.
                 * @param oneStepTargetProbabilities For each choice the probability to go to a goal state in one step.
                 */
                BaierUpperRewardBoundsComputer(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& rewards, std::vector<ValueType> const& oneStepTargetProbabilities);
                
                /*!
                 * Computes an upper bound on the expected rewards.
                 */
                ValueType computeUpperBound();
                
            private:
                storm::storage::SparseMatrix<ValueType> const& transitionMatrix;
                std::vector<ValueType> const& rewards;
                std::vector<ValueType> const& oneStepTargetProbabilities;
            };
        }
    }
}
