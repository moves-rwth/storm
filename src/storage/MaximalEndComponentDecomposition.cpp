#include "src/storage/MaximalEndComponentDecomposition.h"

#include <list>
#include <queue>

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition() : Decomposition() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(storm::models::AbstractNondeterministicModel<ValueType> const& model) {
            performMaximalEndComponentDecomposition(model);
        }
        
        template<typename ValueType>
        MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(MaximalEndComponentDecomposition const& other) : Decomposition(other) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        MaximalEndComponentDecomposition<ValueType>& MaximalEndComponentDecomposition<ValueType>::operator=(MaximalEndComponentDecomposition const& other) {
            Decomposition::operator=(other);
            return *this;
        }
        
        template<typename ValueType>
        MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(MaximalEndComponentDecomposition&& other) : Decomposition(std::move(other)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        MaximalEndComponentDecomposition<ValueType>& MaximalEndComponentDecomposition<ValueType>::operator=(MaximalEndComponentDecomposition&& other) {
            Decomposition::operator=(std::move(other));
            return *this;
        }
        
        template <typename ValueType>
        void MaximalEndComponentDecomposition<ValueType>::performMaximalEndComponentDecomposition(storm::models::AbstractNondeterministicModel<ValueType> const& model) {
            // Get some references for convenient access.
            storm::storage::SparseMatrix<bool> backwardTransitions = model.getBackwardTransitions();
            std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();
            storm::storage::SparseMatrix<ValueType> const& transitionMatrix = model.getTransitionMatrix();
            
            // Initialize the maximal end component list to be the full state space.
            std::list<StateBlock> endComponentStateSets;
            endComponentStateSets.emplace_back(0, model.getNumberOfStates());
            
            do {
                StronglyConnectedComponentDecomposition<ValueType> sccs(model, true);
                
                // Check for each of the SCCs whether there is at least one action for each state that does not leave the SCC.
                for (auto& scc : sccs) {
                    storm::storage::BitVector statesToCheck(model.getNumberOfStates(), scc.begin(), scc.end());
                    
                    while (!statesToCheck.empty()) {
                        storm::storage::BitVector statesToRemove(model.getNumberOfStates());
                    
                        for (auto state : scc) {
                            bool keepStateInMEC = false;
                            
                            for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                                bool choiceContainedInMEC = true;
                                for (typename storm::storage::SparseMatrix<ValueType>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(choice), successorIte = transitionMatrix.constColumnIteratorEnd(choice); successorIt != successorIte; ++successorIt) {
                                    if (!scc.contains(*successorIt)) {
                                        choiceContainedInMEC = false;
                                        break;
                                    }
                                }
                                
                                // If there is at least one choice whose successor states are fully contained in the MEC, we can leave the state in the MEC.
                                if (choiceContainedInMEC) {
                                    keepStateInMEC = true;
                                    break;
                                }
                            }
                            
                            if (!keepStateInMEC) {
                                statesToRemove.set(state, true);
                            }
                        }
                    
                        // Now erase the states that have no option to stay inside the MEC with all successors.
                        std::vector<uint_fast64_t> statesToRemoveList = statesToRemove.getSetIndicesList();
                        scc.erase(storm::storage::VectorSet<uint_fast64_t>(statesToRemoveList.begin(), statesToRemoveList.end()));

                        // Now check which states should be reconsidered, because successors of them were removed.
                    }
                    
                }
                
                
            } while (true);
        }
        
        template class MaximalEndComponentDecomposition<double>;
    }
}