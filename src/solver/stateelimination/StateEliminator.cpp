#include "src/solver/stateelimination/StateEliminator.h"

#include "src/adapters/CarlAdapter.h"

#include "src/storage/BitVector.h"

#include "src/utility/stateelimination.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            using namespace storm::utility::stateelimination;
            
            template<typename ValueType>
            StateEliminator<ValueType>::StateEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions) : transitionMatrix(transitionMatrix), backwardTransitions(backwardTransitions) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            void StateEliminator<ValueType>::eliminateState(storm::storage::sparse::state_type state, bool removeForwardTransitions, storm::storage::BitVector predecessorConstraint) {
                STORM_LOG_TRACE("Eliminating state " << state << ".");
                
                // Start by finding loop probability.
                bool hasSelfLoop = false;
                ValueType loopProbability = storm::utility::zero<ValueType>();
                FlexibleRowType& currentStateSuccessors = transitionMatrix.getRow(state);
                for (auto entryIt = currentStateSuccessors.begin(), entryIte = currentStateSuccessors.end(); entryIt != entryIte; ++entryIt) {
                    if (entryIt->getColumn() >= state) {
                        if (entryIt->getColumn() == state) {
                            loopProbability = entryIt->getValue();
                            hasSelfLoop = true;
                            
                            // If we do not clear the forward transitions completely, we need to remove the self-loop,
                            // because we scale all the other outgoing transitions with it anyway.
                            if (!removeForwardTransitions) {
                                currentStateSuccessors.erase(entryIt);
                            }
                        }
                        break;
                    }
                }
                
                // Scale all entries in this row with (1 / (1 - loopProbability)) only in case there was a self-loop.
                STORM_LOG_TRACE((hasSelfLoop ? "State has self-loop." : "State does not have a self-loop."));
                if (hasSelfLoop) {
                    STORM_LOG_ASSERT(loopProbability != storm::utility::one<ValueType>(), "Must not eliminate state with probability 1 self-loop.");
                    loopProbability = storm::utility::simplify(storm::utility::one<ValueType>() / (storm::utility::one<ValueType>() - loopProbability));
                    for (auto& entry : transitionMatrix.getRow(state)) {
                        // Only scale the non-diagonal entries.
                        if (entry.getColumn() != state) {
                            entry.setValue(storm::utility::simplify(entry.getValue() * loopProbability));
                        }
                    }
                    updateValue(state, loopProbability);
                }
                
                // Now connect the predecessors of the state being eliminated with its successors.
                FlexibleRowType& currentStatePredecessors = backwardTransitions.getRow(state);
                
                // In case we have a constrained elimination, we need to keep track of the new predecessors.
                FlexibleRowType newCurrentStatePredecessors;
                
                std::vector<FlexibleRowType> newBackwardProbabilities(currentStateSuccessors.size());
                for (auto& backwardProbabilities : newBackwardProbabilities) {
                    backwardProbabilities.reserve(currentStatePredecessors.size());
                }
                
                // Now go through the predecessors and eliminate the ones (satisfying the constraint if given).
                for (auto const& predecessorEntry : currentStatePredecessors) {
                    uint_fast64_t predecessor = predecessorEntry.getColumn();
                    STORM_LOG_TRACE("Found predecessor " << predecessor << ".");
                    
                    // Skip the state itself as one of its predecessors.
                    if (predecessor == state) {
                        assert(hasSelfLoop);
                        continue;
                    }
                    
                    // Skip the state if the elimination is constrained, but the predecessor is not in the constraint.
                    if (isFilterPredecessor() && !filterPredecessor(predecessor)) {
                        newCurrentStatePredecessors.emplace_back(predecessorEntry);
                        STORM_LOG_TRACE("Not eliminating predecessor " << predecessor << ", because it does not fit the filter.");
                        continue;
                    }
                    STORM_LOG_TRACE("Eliminating predecessor " << predecessor << ".");
                    
                    // First, find the probability with which the predecessor can move to the current state, because
                    // the forward probabilities of the state to be eliminated need to be scaled with this factor.
                    FlexibleRowType& predecessorForwardTransitions = transitionMatrix.getRow(predecessor);
                    FlexibleRowIterator multiplyElement = std::find_if(predecessorForwardTransitions.begin(), predecessorForwardTransitions.end(), [&](storm::storage::MatrixEntry<typename storm::storage::FlexibleSparseMatrix<ValueType>::index_type, typename storm::storage::FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                    
                    // Make sure we have found the probability and set it to zero.
                    STORM_LOG_THROW(multiplyElement != predecessorForwardTransitions.end(), storm::exceptions::InvalidStateException, "No probability for successor found.");
                    ValueType multiplyFactor = multiplyElement->getValue();
                    multiplyElement->setValue(storm::utility::zero<ValueType>());
                    
                    // At this point, we need to update the (forward) transitions of the predecessor.
                    FlexibleRowIterator first1 = predecessorForwardTransitions.begin();
                    FlexibleRowIterator last1 = predecessorForwardTransitions.end();
                    FlexibleRowIterator first2 = currentStateSuccessors.begin();
                    FlexibleRowIterator last2 = currentStateSuccessors.end();
                    
                    FlexibleRowType newSuccessors;
                    newSuccessors.reserve((last1 - first1) + (last2 - first2));
                    std::insert_iterator<FlexibleRowType> result(newSuccessors, newSuccessors.end());
                    
                    uint_fast64_t successorOffsetInNewBackwardTransitions = 0;
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
                            std::copy_if(first1, last1, result, [&] (storm::storage::MatrixEntry<typename storm::storage::FlexibleSparseMatrix<ValueType>::index_type, typename storm::storage::FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; } );
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            auto successorEntry = storm::utility::simplify(std::move(*first2 * multiplyFactor));
                            *result = successorEntry;
                            newBackwardProbabilities[successorOffsetInNewBackwardTransitions].emplace_back(predecessor, successorEntry.getValue());
                            ++first2;
                            ++successorOffsetInNewBackwardTransitions;
                        } else if (first1->getColumn() < first2->getColumn()) {
                            *result = *first1;
                            ++first1;
                        } else {
                            auto probability = storm::utility::simplify(first1->getValue() + storm::utility::simplify(multiplyFactor * first2->getValue()));
                            *result = storm::storage::MatrixEntry<typename storm::storage::FlexibleSparseMatrix<ValueType>::index_type, typename storm::storage::FlexibleSparseMatrix<ValueType>::value_type>(first1->getColumn(), probability);
                            newBackwardProbabilities[successorOffsetInNewBackwardTransitions].emplace_back(predecessor, probability);
                            ++first1;
                            ++first2;
                            ++successorOffsetInNewBackwardTransitions;
                        }
                    }
                    for (; first2 != last2; ++first2) {
                        if (first2->getColumn() != state) {
                            auto stateProbability = storm::utility::simplify(std::move(*first2 * multiplyFactor));
                            *result = stateProbability;
                            newBackwardProbabilities[successorOffsetInNewBackwardTransitions].emplace_back(predecessor, stateProbability.getValue());
                            ++successorOffsetInNewBackwardTransitions;
                        }
                    }
                    
                    // Now move the new transitions in place.
                    predecessorForwardTransitions = std::move(newSuccessors);
                    STORM_LOG_TRACE("Fixed new next-state probabilities of predecessor state " << predecessor << ".");
                    
                    updatePredecessor(predecessor, multiplyFactor, state);
                    
                    STORM_LOG_TRACE("Updating priority of predecessor.");
                    updatePriority(predecessor);
                }
                
                // Finally, we need to add the predecessor to the set of predecessors of every successor.
                uint_fast64_t successorOffsetInNewBackwardTransitions = 0;
                for (auto const& successorEntry : currentStateSuccessors) {
                    if (successorEntry.getColumn() == state) {
                        continue;
                    }
                    
                    FlexibleRowType& successorBackwardTransitions = backwardTransitions.getRow(successorEntry.getColumn());
                    
                    // Delete the current state as a predecessor of the successor state only if we are going to remove the
                    // current state's forward transitions.
                    if (removeForwardTransitions) {
                        FlexibleRowIterator elimIt = std::find_if(successorBackwardTransitions.begin(), successorBackwardTransitions.end(), [&](storm::storage::MatrixEntry<typename storm::storage::FlexibleSparseMatrix<ValueType>::index_type, typename storm::storage::FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                        STORM_LOG_ASSERT(elimIt != successorBackwardTransitions.end(), "Expected a proper backward transition from " << successorEntry.getColumn() << " to " << state << ", but found none.");
                        successorBackwardTransitions.erase(elimIt);
                    }
                    
                    FlexibleRowIterator first1 = successorBackwardTransitions.begin();
                    FlexibleRowIterator last1 = successorBackwardTransitions.end();
                    FlexibleRowIterator first2 = newBackwardProbabilities[successorOffsetInNewBackwardTransitions].begin();
                    FlexibleRowIterator last2 = newBackwardProbabilities[successorOffsetInNewBackwardTransitions].end();
                    
                    FlexibleRowType newPredecessors;
                    newPredecessors.reserve((last1 - first1) + (last2 - first2));
                    std::insert_iterator<FlexibleRowType> result(newPredecessors, newPredecessors.end());
                    
                    for (; first1 != last1; ++result) {
                        if (first2 == last2) {
                            std::copy(first1, last1, result);
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            if (first2->getColumn() != state) {
                                *result = *first2;
                            }
                            ++first2;
                        } else if (first1->getColumn() == first2->getColumn()) {
                            if (estimateComplexity(first1->getValue()) > estimateComplexity(first2->getValue())) {
                                *result = *first1;
                            } else {
                                *result = *first2;
                            }
                            ++first1;
                            ++first2;
                        } else {
                            *result = *first1;
                            ++first1;
                        }
                    }
                    if (isFilterPredecessor()) {
                        std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename storm::storage::FlexibleSparseMatrix<ValueType>::index_type, typename storm::storage::FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state && filterPredecessor(a.getColumn()); });
                    } else {
                        std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename storm::storage::FlexibleSparseMatrix<ValueType>::index_type, typename storm::storage::FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; });
                    }
                    // Now move the new predecessors in place.
                    successorBackwardTransitions = std::move(newPredecessors);
                    ++successorOffsetInNewBackwardTransitions;
                }
                STORM_LOG_TRACE("Fixed predecessor lists of successor states.");
                
                if (removeForwardTransitions) {
                    // Clear the eliminated row to reduce memory consumption.
                    currentStateSuccessors.clear();
                    currentStateSuccessors.shrink_to_fit();
                }
                if (isFilterPredecessor()) {
                    currentStatePredecessors = std::move(newCurrentStatePredecessors);
                } else {
                    currentStatePredecessors.clear();
                    currentStatePredecessors.shrink_to_fit();
                }
            }
            
            template<typename ValueType>
            void StateEliminator<ValueType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            void StateEliminator<ValueType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            void StateEliminator<ValueType>::updatePriority(storm::storage::sparse::state_type const& state) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            bool StateEliminator<ValueType>::filterPredecessor(storm::storage::sparse::state_type const& state) {
                STORM_LOG_ASSERT(false, "Must not filter predecessors.");
                return false;
            }
            
            template<typename ValueType>
            bool StateEliminator<ValueType>::isFilterPredecessor() const {
                return false;
            }
            
            template class StateEliminator<double>;
            template class StateEliminator<storm::RationalNumber>;
            
#ifdef STORM_HAVE_CARL
            template class StateEliminator<storm::RationalFunction>;
#endif
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
