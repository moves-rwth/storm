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
            storm::storage::BitVector statesToCheck(model.getNumberOfStates());
            
            for (std::list<StateBlock>::const_iterator mecIterator = endComponentStateSets.begin(); mecIterator != endComponentStateSets.end();) {
                StateBlock const& mec = *mecIterator;
                
                // Keep track of whether the MEC changed during this iteration.
                bool mecChanged = false;
                
                // Get an SCC decomposition of the current MEC candidate.
                StronglyConnectedComponentDecomposition<ValueType> sccs(model, mec, true);
                mecChanged |= sccs.size() > 1;
                
                // Check for each of the SCCs whether there is at least one action for each state that does not leave the SCC.
                for (auto& scc : sccs) {
                    statesToCheck.set(scc.begin(), scc.end());
                    
                    while (!statesToCheck.empty()) {
                        storm::storage::BitVector statesToRemove(model.getNumberOfStates());
                        
                        for (auto state : statesToCheck) {
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
                        mecChanged |= !statesToRemove.empty();
                        std::vector<uint_fast64_t> statesToRemoveList = statesToRemove.getSetIndicesList();
                        scc.erase(storm::storage::VectorSet<uint_fast64_t>(statesToRemoveList));
                        
                        // Now check which states should be reconsidered, because successors of them were removed.
                        statesToCheck.clear();
                        for (auto state : statesToRemove) {
                            for (typename storm::storage::SparseMatrix<ValueType>::ConstIndexIterator predIt = backwardTransitions.constColumnIteratorBegin(state), predIte = backwardTransitions.constColumnIteratorEnd(state); predIt != predIte; ++predIt) {
                                if (scc.contains(*predIt)) {
                                    statesToCheck.set(*predIt);
                                }
                            }
                        }
                    }
                }
                
                // If the MEC changed, we delete it from the list of MECs and append the possible new MEC candidates to the list instead.
                if (mecChanged) {
                    std::list<StateBlock>::const_iterator eraseIterator(mecIterator);
                    for (StateBlock& scc : sccs) {
                        if (!scc.empty()) {
                            endComponentStateSets.push_back(std::move(scc));
                            ++mecIterator;
                        }
                    }
                    endComponentStateSets.erase(eraseIterator);
                } else {
                    // Otherwise, we proceed with the next MEC candidate.
                    ++mecIterator;
                }
                
            } // end of loop over all MEC candidates
            
            // Now that we computed the underlying state sets of the MECs, we need to properly identify the choices contained in the MEC.
            this->blocks.reserve(endComponentStateSets.size());
            for (auto const& mecStateSet : endComponentStateSets) {
                MaximalEndComponent newMec;
                
                for (auto state : mecStateSet) {
                    std::vector<uint_fast64_t> containedChoices;
                    for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                        bool choiceContained = true;
                        for (typename storm::storage::SparseMatrix<ValueType>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(choice), successorIte = transitionMatrix.constColumnIteratorEnd(choice); successorIt != successorIte; ++successorIt) {
                            if (!mecStateSet.contains(*successorIt)) {
                                choiceContained = false;
                                break;
                            }
                        }
                        
                        if (choiceContained) {
                            containedChoices.push_back(choice);
                        }
                    }
                    
                    newMec.addState(state, std::move(containedChoices));
                }
                
                this->blocks.emplace_back(std::move(newMec));
            }
            
            LOG4CPLUS_INFO(logger, "Computed MEC decomposition of size " << this->size() << ".");
        }
        
        template class MaximalEndComponentDecomposition<double>;
    }
}