#include "src/modelchecker/reachability/SparseSccModelChecker.h"

#include <algorithm>

#include "src/storage/parameters.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace modelchecker {
        namespace reachability {
            
            template<typename ValueType>
            static ValueType&& simplify(ValueType&& value) {
                // In the general case, we don't to anything here, but merely return the value. If something else is
                // supposed to happen here, the templated function can be specialized for this particular type.
                return std::forward<ValueType>(value);
            }
            
            static RationalFunction&& simplify(RationalFunction&& value) {
                // In the general case, we don't to anything here, but merely return the value. If something else is
                // supposed to happen here, the templated function can be specialized for this particular type.
                value.simplify();
                return std::forward<RationalFunction>(value);
            }
            
            template<typename IndexType, typename ValueType>
            static storm::storage::MatrixEntry<IndexType, ValueType>&& simplify(storm::storage::MatrixEntry<IndexType, ValueType>&& matrixEntry) {
                simplify(matrixEntry.getValue());
                return std::move(matrixEntry);
            }
            
            template<typename IndexType, typename ValueType>
            static storm::storage::MatrixEntry<IndexType, ValueType>& simplify(storm::storage::MatrixEntry<IndexType, ValueType>& matrixEntry) {
                matrixEntry.setValue(simplify(matrixEntry.getValue()));
                return matrixEntry;
            }
            
            template<typename ValueType>
            ValueType SparseSccModelChecker<ValueType>::computeReachabilityProbability(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                // First, do some sanity checks to establish some required properties.
                STORM_LOG_THROW(dtmc.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
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
                
                std::cout << "Solving parametric system with " << maybeStates.getNumberOfSetBits() << " states." << std::endl;
                
                // Create a vector for the probabilities to go to a state with probability 1 in one step.
                std::vector<ValueType> oneStepProbabilities = dtmc.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
                
                // Determine the set of initial states of the sub-DTMC.
                storm::storage::BitVector newInitialStates = dtmc.getInitialStates() % maybeStates;
                
                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ValueType> submatrix = dtmc.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                
                // Create a bit vector that represents the subsystem of states we still have to eliminate.
                storm::storage::BitVector subsystem = storm::storage::BitVector(maybeStates.getNumberOfSetBits(), true);
                
                // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
                FlexibleSparseMatrix<ValueType> flexibleMatrix = getFlexibleSparseMatrix(submatrix);
                FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions = getFlexibleSparseMatrix(submatrix.transpose(), true);
                
                // Then, we recursively treat all SCCs.
                treatScc(dtmc, flexibleMatrix, oneStepProbabilities, newInitialStates, subsystem, submatrix, flexibleBackwardTransitions, false, 0);
                
                // Now, we return the value for the only initial state.
                return oneStepProbabilities[initialStateIndex];
            }
            
            template<typename ValueType>
            void SparseSccModelChecker<ValueType>::treatScc(storm::models::Dtmc<ValueType> const& dtmc, FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix<ValueType>& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level) {
                // If the SCCs are large enough, we try to split them further.
                if (scc.getNumberOfSetBits() > SparseSccModelChecker<ValueType>::maximalSccSize) {
                    // Here, we further decompose the SCC into sub-SCCs.
                    storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition(forwardTransitions, scc & ~entryStates, false, false);
                    
                    // To eliminate the remaining one-state SCCs, we need to keep track of them.
                    // storm::storage::BitVector remainingStates(scc);
                    
                    // Store a bit vector of remaining SCCs so we can be flexible when it comes to the order in which
                    // we eliminate the SCCs.
                    storm::storage::BitVector remainingSccs(decomposition.size(), true);
                    
                    // First, get rid of the trivial SCCs.
                    for (uint_fast64_t sccIndex = 0; sccIndex < decomposition.size(); ++sccIndex) {
                        storm::storage::StronglyConnectedComponent const& scc = decomposition.getBlock(sccIndex);
                        if (scc.isTrivial()) {
                            storm::storage::sparse::state_type onlyState = *scc.begin();
                            eliminateState(matrix, oneStepProbabilities, onlyState, backwardTransitions);
                            remainingSccs.set(sccIndex, false);
                        }
                    }
                    
                    // And then recursively treat the remaining sub-SCCs.
                    for (auto sccIndex : remainingSccs) {
                        storm::storage::StronglyConnectedComponent const& newScc = decomposition.getBlock(sccIndex);
                        // If the SCC consists of just one state, we do not explore it recursively, but rather eliminate
                        // it directly.
                        if (newScc.size() == 1) {
                            continue;
                        }
                        
                        // Rewrite SCC into bit vector and subtract it from the remaining states.
                        storm::storage::BitVector newSccAsBitVector(forwardTransitions.getRowCount(), newScc.begin(), newScc.end());
                        // remainingStates &= ~newSccAsBitVector;
                        
                        // Determine the set of entry states of the SCC.
                        storm::storage::BitVector entryStates(dtmc.getNumberOfStates());
                        for (auto const& state : newScc) {
                            for (auto const& predecessor : backwardTransitions.getRow(state)) {
                                if (predecessor.getValue() != storm::utility::constantZero<ValueType>() && !newSccAsBitVector.get(predecessor.getColumn())) {
                                    entryStates.set(state);
                                }
                            }
                        }
                        
                        // Recursively descend in SCC-hierarchy.
                        treatScc(dtmc, matrix, oneStepProbabilities, entryStates, newSccAsBitVector, forwardTransitions, backwardTransitions, true, level + 1);
                    }
                    
                    // If we are not supposed to eliminate the entry states, we need to take them out of the set of
                    // remaining states.
                    // if (!eliminateEntryStates) {
                    //     remainingStates &= ~entryStates;
                    // }
                    //
                    // Now that we eliminated all non-trivial sub-SCCs, we need to take care of trivial sub-SCCs.
                    // Therefore, we need to eliminate all states.
                    // for (auto const& state : remainingStates) {
                    //     eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
                    // }
                } else {
                    // In this case, we perform simple state elimination in the current SCC.
                    storm::storage::BitVector remainingStates = scc;
                    
//                    if (eliminateEntryStates) {
                        remainingStates &= ~entryStates;
//                    }
                    
                    // Eliminate the remaining states.
                    for (auto const& state : remainingStates) {
                        eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
                    }
                    
                    // Finally, eliminate the entry states (if we are allowed to do so).
                    if (eliminateEntryStates) {
                        for (auto state : entryStates) {
                            eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
                        }
                    }
                }
            }
            
            static int chunkCounter = 0;
            static int counter = 0;
            
            template<typename ValueType>
            void SparseSccModelChecker<ValueType>::eliminateState(FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, FlexibleSparseMatrix<ValueType>& backwardTransitions) {
                
                ++counter;
                if (counter > matrix.getNumberOfRows() / 10) {
                    ++chunkCounter;
                    std::cout << "Eliminated " << (chunkCounter * 10) << "% of the states." << std::endl;
                    counter = 0;
                }
                
                bool hasSelfLoop = false;
                ValueType loopProbability = storm::utility::constantZero<ValueType>();
                
                // Start by finding loop probability.
                typename FlexibleSparseMatrix<ValueType>::row_type& currentStateSuccessors = matrix.getRow(state);
                for (auto const& entry : currentStateSuccessors) {
                    if (entry.getColumn() >= state) {
                        if (entry.getColumn() == state) {
                            loopProbability = entry.getValue();
                            hasSelfLoop = true;
                        }
                        break;
                    }
                }
                
                // Scale all entries in this row with (1 / (1 - loopProbability)) only in case there was a self-loop.
                if (hasSelfLoop) {
                    loopProbability = storm::utility::constantOne<ValueType>() / (storm::utility::constantOne<ValueType>() - loopProbability);
                    simplify(loopProbability);
                    for (auto& entry : matrix.getRow(state)) {
                        entry.setValue(simplify(entry.getValue() * loopProbability));
                    }
                    oneStepProbabilities[state] = simplify(oneStepProbabilities[state] * loopProbability);
                }
                
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
                    STORM_LOG_THROW(multiplyElement != predecessorForwardTransitions.end(), storm::exceptions::InvalidStateException, "No probability for successor found.");
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
                            *result = simplify(*first2 * multiplyFactor);
                            ++first2;
                        } else if (first1->getColumn() < first2->getColumn()) {
                            *result = *first1;
                            ++first1;
                        } else {
                            *result = storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type>(first1->getColumn(), simplify(first1->getValue() + simplify(multiplyFactor * first2->getValue())));
                            ++first1;
                            ++first2;
                        }
                    }
                    for (; first2 != last2; ++first2) {
                        if (first2->getColumn() != state) {
                            *result = simplify(*first2 * multiplyFactor);
                        }
                    }
                    
                    // Now move the new transitions in place.
                    predecessorForwardTransitions = std::move(newSuccessors);
                    
                    // Add the probabilities to go to a target state in just one step.
                    oneStepProbabilities[predecessor] = simplify(oneStepProbabilities[predecessor] + simplify(multiplyFactor * oneStepProbabilities[state]));
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
            
            template <typename ValueType>
            bool SparseSccModelChecker<ValueType>::eliminateStateInPlace(storm::storage::SparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, storm::storage::SparseMatrix<ValueType>& backwardTransitions) {
                typename storm::storage::SparseMatrix<ValueType>::iterator forwardElement = matrix.getRow(state).begin();
                typename storm::storage::SparseMatrix<ValueType>::iterator backwardElement = backwardTransitions.getRow(state).begin();
                
                if (forwardElement->getValue() != storm::utility::constantOne<ValueType>() || backwardElement->getValue() != storm::utility::constantOne<ValueType>()) {
                    return false;
                }
                
                std::cout << "eliminating " << state << std::endl;
                std::cout << "fwd element: " << *forwardElement << " and bwd element: " << *backwardElement << std::endl;
                
                // Find the element of the predecessor that moves to the state that we want to eliminate.
                typename storm::storage::SparseMatrix<ValueType>::rows forwardRow = matrix.getRow(backwardElement->getColumn());
                typename storm::storage::SparseMatrix<ValueType>::iterator multiplyElement = std::find_if(forwardRow.begin(), forwardRow.end(), [&](storm::storage::MatrixEntry<typename storm::storage::SparseMatrix<ValueType>::index_type, typename storm::storage::SparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                
                std::cout << "before fwd: " << std::endl;
                for (auto element : matrix.getRow(backwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                
                // Modify the forward probability entry of the predecessor.
                multiplyElement->setValue(multiplyElement->getValue() * forwardElement->getValue());
                multiplyElement->setColumn(forwardElement->getColumn());
                
                // Modify the one-step probability for the predecessor if necessary.
                if (oneStepProbabilities[state] != storm::utility::constantZero<ValueType>()) {
                    oneStepProbabilities[backwardElement->getColumn()] += multiplyElement->getValue() * oneStepProbabilities[state];
                }
                
                // If the forward entry is not at the right position, we need to move it there.
                if (multiplyElement != forwardRow.begin() && multiplyElement->getColumn() < (multiplyElement - 1)->getColumn()) {
                    while (multiplyElement != forwardRow.begin() && multiplyElement->getColumn() < (multiplyElement - 1)->getColumn()) {
                        std::swap(*multiplyElement, *(multiplyElement - 1));
                        --multiplyElement;
                    }
                } else if ((multiplyElement + 1) != forwardRow.end() && multiplyElement->getColumn() > (multiplyElement + 1)->getColumn()) {
                    while ((multiplyElement + 1) != forwardRow.end() && multiplyElement->getColumn() > (multiplyElement + 1)->getColumn()) {
                        std::swap(*multiplyElement, *(multiplyElement + 1));
                        ++multiplyElement;
                    }
                }
                
                std::cout << "after fwd: " << std::endl;
                for (auto element : matrix.getRow(backwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                
                // Find the backward element of the successor that moves to the state that we want to eliminate.
                typename storm::storage::SparseMatrix<ValueType>::rows backwardRow = backwardTransitions.getRow(forwardElement->getColumn());
                typename storm::storage::SparseMatrix<ValueType>::iterator backwardEntry = std::find_if(backwardRow.begin(), backwardRow.end(), [&](storm::storage::MatrixEntry<typename storm::storage::SparseMatrix<ValueType>::index_type, typename storm::storage::SparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                
                std::cout << "before bwd" << std::endl;
                for (auto element : backwardTransitions.getRow(forwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                
                // Modify the predecessor list of the successor and add the predecessor of the state we eliminate.
                backwardEntry->setColumn(backwardElement->getColumn());
                
                // If the backward entry is not at the right position, we need to move it there.
                if (backwardEntry != backwardRow.begin() && backwardEntry->getColumn() < (backwardEntry - 1)->getColumn()) {
                    while (backwardEntry != backwardRow.begin() && backwardEntry->getColumn() < (backwardEntry - 1)->getColumn()) {
                        std::swap(*backwardEntry, *(backwardEntry - 1));
                        --backwardEntry;
                    }
                } else if ((backwardEntry + 1) != backwardRow.end() && backwardEntry->getColumn() > (backwardEntry + 1)->getColumn()) {
                    while ((backwardEntry + 1) != backwardRow.end() && backwardEntry->getColumn() > (backwardEntry + 1)->getColumn()) {
                        std::swap(*backwardEntry, *(backwardEntry + 1));
                        ++backwardEntry;
                    }
                }
                
                std::cout << "after bwd" << std::endl;
                for (auto element : backwardTransitions.getRow(forwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                return true;
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
            typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getNumberOfRows() const {
                return this->data.size();
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
