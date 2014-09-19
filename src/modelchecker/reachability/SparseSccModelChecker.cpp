#include "src/modelchecker/reachability/SparseSccModelChecker.h"

#include <algorithm>

#include "src/storage/parameters.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace modelchecker {
        namespace reachability {
            
            template<typename ValueType>
            ValueType SparseSccModelChecker<ValueType>::computeReachabilityProbability(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                // First, do some sanity checks to establish some required properties.
                LOG_THROW(dtmc.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
                typename FlexibleSparseMatrix<ValueType>::index_type initialStateIndex = *dtmc.getInitialStates().begin();
                
                // Then, compute the subset of states that has a probability of 0 or 1, respectively.
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(dtmc, phiStates, psiStates);
                storm::storage::BitVector statesWithProbability0 = statesWithProbability01.first;
                storm::storage::BitVector statesWithProbability1 = statesWithProbability01.second;
                storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);

                // If the initial state is known to have either probability 0 or 1, we can directly return the result.
                if (!maybeStates.get(initialStateIndex)) {
                    return statesWithProbability0.get(initialStateIndex) ? storm::utility::constantZero<ValueType>() : storm::utility::constantOne<ValueType>();
                }
                
                // Determine the set of states that is reachable from the initial state without jumping over a target state.
                storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(dtmc.getTransitionMatrix(), dtmc.getInitialStates(), maybeStates, statesWithProbability1);
                
                // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                maybeStates &= reachableStates;
                
                // Otherwise, we build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ValueType> submatrix = dtmc.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                
                // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
                FlexibleSparseMatrix<ValueType> flexibleMatrix = getFlexibleSparseMatrix(submatrix);
                
                // Create a vector for the probabilities to go to a state with probability 1 in one step.
                std::vector<ValueType> oneStepProbabilities = dtmc.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
                
                // Then, we recursively treat all SCCs.
                FlexibleSparseMatrix<ValueType> backwardTransitions = getFlexibleSparseMatrix(submatrix.transpose(), true);
                treatScc(dtmc, flexibleMatrix, oneStepProbabilities, dtmc.getInitialStates() % maybeStates, storm::storage::BitVector(maybeStates.getNumberOfSetBits(), true), submatrix, backwardTransitions, false, 0);
                
                // Now, we return the value for the only initial state.
                return oneStepProbabilities[initialStateIndex];
            }
            
            template<typename ValueType>
            void SparseSccModelChecker<ValueType>::treatScc(storm::models::Dtmc<ValueType> const& dtmc, FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix<ValueType>& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level) {
                if (level <= 3) {
                    // Here, we further decompose the SCC into sub-SCCs.
                    storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition(forwardTransitions, scc & ~entryStates, true, false);

                    // To eliminate the remaining one-state SCCs, we need to keep track of them.
                    storm::storage::BitVector remainingStates(scc);
                    
                    // And then recursively treat all sub-SCCs.
                    for (auto const& newScc : decomposition) {
                        // If the SCC consists of just one state, we do not explore it recursively, but rather eliminate
                        // it directly.
                        if (newScc.size() == 1) {
                            continue;
                        }
                        
                        // Rewrite SCC into bit vector and subtract it from the remaining states.
                        storm::storage::BitVector newSccAsBitVector(forwardTransitions.getRowCount(), newScc.begin(), newScc.end());
                        remainingStates &= ~newSccAsBitVector;
                        
                        // Determine the set of entry states of the SCC.
                        storm::storage::BitVector entryStates(dtmc.getNumberOfStates());
                        for (auto const& state : newScc) {
                            for (auto const& predecessor : backwardTransitions.getRow(state)) {
                                if (predecessor.getValue() > storm::utility::constantZero<ValueType>() && !newSccAsBitVector.get(predecessor.getColumn())) {
                                    entryStates.set(state);
                                }
                            }
                        }
                        
                        // Recursively descend in SCC-hierarchy.
                        treatScc(dtmc, matrix, oneStepProbabilities, entryStates, newSccAsBitVector, forwardTransitions, backwardTransitions, true, level + 1);
                    }

                    // If we are not supposed to eliminate the entry states, we need to take them out of the set of
                    // remaining states.
                    if (!eliminateEntryStates) {
                        remainingStates &= ~entryStates;
                    }
                    
                    // Now that we eliminated all non-trivial sub-SCCs, we need to take care of trivial sub-SCCs.
                    // Therefore, we need to eliminate all states.
                    for (auto const& state : remainingStates) {
                        eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
                    }
                } else {
                    // In this case, we perform simple state elimination in the current SCC.
                    storm::storage::BitVector remainingStates(scc);

                    // If we are not supposed to eliminate the entry states, we need to take them out of the set of
                    // remaining states.
                    if (!eliminateEntryStates) {
                        remainingStates &= ~entryStates;
                    }
                    
                    // Eliminate the remaining states.
                    for (auto const& state : remainingStates) {
                        eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
                    }
                }
            }
            
            template<typename ValueType>
            void SparseSccModelChecker<ValueType>::eliminateState(FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, FlexibleSparseMatrix<ValueType>& backwardTransitions) {
                ValueType loopProbability = storm::utility::constantZero<ValueType>();
                
                // Start by finding loop probability.
                typename FlexibleSparseMatrix<ValueType>::row_type& currentStateSuccessors = matrix.getRow(state);
                for (auto const& entry : currentStateSuccessors) {
                    if (entry.getColumn() >= state) {
                        if (entry.getColumn() == state) {
                            loopProbability = entry.getValue();
                        }
                        break;
                    }
                }
                
                // Scale all entries in this row with (1 / (1 - loopProbability)).
                loopProbability = storm::utility::constantOne<ValueType>() / (storm::utility::constantOne<ValueType>() - loopProbability);
                for (auto& entry : matrix.getRow(state)) {
                    entry.setValue(entry.getValue() * loopProbability);
                }
                oneStepProbabilities[state] *= loopProbability;
                
                // Now connect the predecessors of the state being eliminated with its successors.
                typename FlexibleSparseMatrix<ValueType>::row_type& currentStatePredecessors = backwardTransitions.getRow(state);
                for (auto const& predecessorEntry : currentStatePredecessors) {
                    uint_fast64_t predecessor = predecessorEntry.getColumn();
                    
                    // Skip the state itself as one of its predecessors.
                    if (predecessor == state) {
                        continue;
                    }
                    
                    // First, find the probability with which the predecessor can move to the current state, because
                    // the other probabilities need to be scaled with this factor.
                    typename FlexibleSparseMatrix<ValueType>::row_type& predecessorForwardTransitions = matrix.getRow(predecessor);
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator multiplyElement = std::find_if(predecessorForwardTransitions.begin(), predecessorForwardTransitions.end(), [&](storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                    
                    // Make sure we have found the probability and set it to zero.
                    LOG_THROW(multiplyElement != predecessorForwardTransitions.end(), storm::exceptions::InvalidStateException, "No probability for successor found.");
                    ValueType multiplyFactor = multiplyElement->getValue();
                    multiplyElement->setValue(storm::utility::constantZero<ValueType>());
                    
                    // At this point, we need to update the (forward) transitions of the predecessor.
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first1 = predecessorForwardTransitions.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last1 = predecessorForwardTransitions.end();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first2 = currentStateSuccessors.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last2 = currentStateSuccessors.end();
                    
                    typename FlexibleSparseMatrix<ValueType>::row_type newSuccessors;
                    newSuccessors.reserve((last1 - first1) + (last2 - first2));
                    std::insert_iterator<typename FlexibleSparseMatrix<ValueType>::row_type> result(newSuccessors, newSuccessors.end());
                    
                    // Now we merge the two successor lists. (Code taken from std::set_union and modified to suit our needs).
                    for (; first1 != last1; ++result) {
                        // Skip the transitions to the state that is currently being eliminated.
                        if (first1->getColumn() == state || (first2 != last2 && first2->getColumn() == state)) {
                            if (first1->getColumn() == state) {
                                ++first1;
                            }
                            if (first2 != last2 && first2->getColumn() == state) {
                                ++first2;
                            }
                            continue;
                        }
                        
                        if (first2 == last2) {
                            std::copy_if(first1, last1, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; } );
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            *result = *first2 * multiplyFactor;
                            ++first2;
                        } else if (first1->getColumn() < first2->getColumn()) {
                            *result = *first1;
                            ++first1;
                        } else {
                            *result = storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type>(first1->getColumn(), first1->getValue() + multiplyFactor * first2->getValue());
                            ++first1;
                            ++first2;
                        }
                    }
                    for (; first2 != last2; ++first2) {
                        if (first2->getColumn() != state) {
                            *result = *first2 * multiplyFactor;
                        }
                    }
                    
                    // Now move the new transitions in place.
                    predecessorForwardTransitions = std::move(newSuccessors);
                    
                    // Add the probabilities to go to a target state in just one step.
                    oneStepProbabilities[predecessor] += multiplyFactor * oneStepProbabilities[state];
                }
                
                // Finally, we need to add the predecessor to the set of predecessors of every successor.
                for (auto const& successorEntry : currentStateSuccessors) {
                    typename FlexibleSparseMatrix<ValueType>::row_type& successorBackwardTransitions = backwardTransitions.getRow(successorEntry.getColumn());
                    
                    // Delete the current state as a predecessor of the successor state.
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator elimIt = std::find_if(successorBackwardTransitions.begin(), successorBackwardTransitions.end(), [&](storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                    if (elimIt != successorBackwardTransitions.end()) {
                        successorBackwardTransitions.erase(elimIt);
                    }
                    
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first1 = successorBackwardTransitions.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last1 = successorBackwardTransitions.end();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first2 = currentStatePredecessors.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last2 = currentStatePredecessors.end();
                    
                    typename FlexibleSparseMatrix<ValueType>::row_type newPredecessors;
                    newPredecessors.reserve((last1 - first1) + (last2 - first2));
                    std::insert_iterator<typename FlexibleSparseMatrix<ValueType>::row_type> result(newPredecessors, newPredecessors.end());

                    
                    for (; first1 != last1; ++result) {
                        if (first2 == last2) {
                            std::copy_if(first1, last1, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; });
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            if (first2->getColumn() != state) {
                                *result = *first2;
                            }
                            ++first2;
                        } else {
                            if (first1->getColumn() != state) {
                                *result = *first1;
                            }
                            if (first1->getColumn() == first2->getColumn()) {
                                ++first2;
                            }
                            ++first1;
                        }
                    }
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; });
                    
                    // Now move the new predecessors in place.
                    successorBackwardTransitions = std::move(newPredecessors);
                }

                
                // Clear the eliminated row to reduce memory consumption.
                currentStateSuccessors.clear();
                currentStateSuccessors.shrink_to_fit();
            }
            
            template<typename ValueType>
            FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(index_type rows) : data(rows) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            void FlexibleSparseMatrix<ValueType>::reserveInRow(index_type row, index_type numberOfElements) {
                this->data[row].reserve(numberOfElements);
            }
            
            template<typename ValueType>
            typename FlexibleSparseMatrix<ValueType>::row_type& FlexibleSparseMatrix<ValueType>::getRow(index_type index) {
                return this->data[index];
            }
            
            template<typename ValueType>
            FlexibleSparseMatrix<ValueType> SparseSccModelChecker<ValueType>::getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne) {
                FlexibleSparseMatrix<ValueType> flexibleMatrix(matrix.getRowCount());
                
                for (typename FlexibleSparseMatrix<ValueType>::index_type rowIndex = 0; rowIndex < matrix.getRowCount(); ++rowIndex) {
                    typename storm::storage::SparseMatrix<ValueType>::const_rows row = matrix.getRow(rowIndex);
                    flexibleMatrix.reserveInRow(rowIndex, row.getNumberOfEntries());
                    
                    for (auto const& element : row) {
                        if (setAllValuesToOne) {
                            flexibleMatrix.getRow(rowIndex).emplace_back(element.getColumn(), storm::utility::constantOne<ValueType>());
                        } else {
                            flexibleMatrix.getRow(rowIndex).emplace_back(element);
                        }
                    }
                }
                
                return flexibleMatrix;
            }
            
            template class FlexibleSparseMatrix<double>;
            template class SparseSccModelChecker<double>;
            #ifdef PARAMETRIC_SYSTEMS
            template class FlexibleSparseMatrix<RationalFunction>;
            template class SparseSccModelChecker<RationalFunction>;
            #endif            
        } // namespace reachability
    } // namespace modelchecker
} // namespace storm