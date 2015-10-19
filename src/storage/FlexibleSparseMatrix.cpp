/* 
 * File:   FlexibleSparseMatrix.cpp
 * 
 * Created on October 19, 2015, 5:19 PM
 */

#include <algorithm>

#include "FlexibleSparseMatrix.h"
#include "src/adapters/CarlAdapter.h"
#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(index_type rows) : data(rows) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne) : data(matrix.getRowCount()) {

            for (index_type rowIndex = 0; rowIndex < matrix.getRowCount(); ++rowIndex) {
                typename storm::storage::SparseMatrix<ValueType>::const_rows row = matrix.getRow(rowIndex);
                reserveInRow(rowIndex, row.getNumberOfEntries());

                for (auto const& element : row) {
                    // If the probability is zero, we skip this entry.
                    if (storm::utility::isZero(element.getValue())) {
                        continue;
                    }

                    if (setAllValuesToOne) {
                        getRow(rowIndex).emplace_back(element.getColumn(), storm::utility::one<ValueType>());
                    } else {
                        getRow(rowIndex).emplace_back(element);
                    }
                }
            }
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
        typename FlexibleSparseMatrix<ValueType>::row_type const& FlexibleSparseMatrix<ValueType>::getRow(index_type index) const {
            return this->data[index];
        }

        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getNumberOfRows() const {
            return this->data.size();
        }

        template<typename ValueType>
        bool FlexibleSparseMatrix<ValueType>::hasSelfLoop(storm::storage::sparse::state_type state) {
            for (auto const& entry : this->getRow(state)) {
                if (entry.getColumn() < state) {
                    continue;
                } else if (entry.getColumn() > state) {
                    return false;
                } else if (entry.getColumn() == state) {
                    return true;
                }
            }
            return false;
        }

        template<typename ValueType>
        void FlexibleSparseMatrix<ValueType>::print() const {
            for (uint_fast64_t index = 0; index < this->data.size(); ++index) {
                std::cout << index << " - ";
                for (auto const& element : this->getRow(index)) {
                    std::cout << "(" << element.getColumn() << ", " << element.getValue() << ") ";
                }
                std::cout << std::endl;
            }
        }


        template<typename ValueType>
        void FlexibleSparseMatrix<ValueType>::eliminateState(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, uint_fast64_t row, FlexibleSparseMatrix& backwardTransitions, boost::optional<std::vector<ValueType>>& stateRewards, bool removeForwardTransitions, bool constrained, storm::storage::BitVector const& predecessorConstraint) {

            bool hasSelfLoop = false;
            ValueType loopProbability = storm::utility::zero<ValueType>();

            // Start by finding loop probability.
            row_type& currentStateSuccessors = matrix.getRow(row);
            for (auto entryIt = currentStateSuccessors.begin(), entryIte = currentStateSuccessors.end(); entryIt != entryIte; ++entryIt) {
                if (entryIt->getColumn() >= state) {
                    if (entryIt->getColumn() == state) {
                        loopProbability = entryIt->getValue();
                        hasSelfLoop = true;

                        // If we do not clear the forward transitions completely, we need to remove the self-loop,
                        // because we scale all the other outgoing transitions with it anyway..
                        if (!removeForwardTransitions) {
                            currentStateSuccessors.erase(entryIt);
                        }
                    }
                    break;
                }
            }

            // Scale all entries in this row with (1 / (1 - loopProbability)) only in case there was a self-loop.
            std::size_t scaledSuccessors = 0;
            if (hasSelfLoop) {
                STORM_LOG_ASSERT(loopProbability != storm::utility::one<ValueType>(), "Must not eliminate state with probability 1 self-loop.");
                loopProbability = storm::utility::one<ValueType>() / (storm::utility::one<ValueType>() - loopProbability);
                storm::utility::simplify(loopProbability);
                for (auto& entry : matrix.getRow(row)) {
                    // Only scale the non-diagonal entries.
                    if (entry.getColumn() != state) {
                        ++scaledSuccessors;
                        entry.setValue(storm::utility::simplify(entry.getValue() * loopProbability));
                    }
                }
                if (!stateRewards) {
                    oneStepProbabilities[row] = oneStepProbabilities[row] * loopProbability;
                }
            }

            STORM_LOG_TRACE((hasSelfLoop ? "State has self-loop." : "State does not have a self-loop."));

            // Now connect the predecessors of the state being eliminated with its successors.
            row_type& currentStatePredecessors = backwardTransitions.getRow(state);
            std::size_t predecessorForwardTransitionCount = 0;

            // In case we have a constrained elimination, we need to keep track of the new predecessors.
            row_type newCurrentStatePredecessors;

            // Now go through the predecessors and eliminate the ones (satisfying the constraint if given).
            for (auto const& predecessorEntry : currentStatePredecessors) {
                uint_fast64_t predecessor = predecessorEntry.getColumn();

                // Skip the state itself as one of its predecessors.
                if (predecessor == row) {
                    assert(hasSelfLoop);
                    continue;
                }

                // Skip the state if the elimination is constrained, but the predecessor is not in the constraint.
                if (constrained && !predecessorConstraint.get(predecessor)) {
                    newCurrentStatePredecessors.emplace_back(predecessor, storm::utility::one<ValueType>());
                    STORM_LOG_TRACE("Not eliminating predecessor " << predecessor << ", because it does not fit the filter.");
                    continue;
                }
                STORM_LOG_TRACE("Eliminating predecessor " << predecessor << ".");

                // First, find the probability with which the predecessor can move to the current state, because
                // the other probabilities need to be scaled with this factor.
                row_type& predecessorForwardTransitions = matrix.getRow(predecessor);
                predecessorForwardTransitionCount += predecessorForwardTransitions.size();
                typename row_type::iterator multiplyElement = std::find_if(predecessorForwardTransitions.begin(), predecessorForwardTransitions.end(), [&](storm::storage::MatrixEntry<index_type, ValueType> const& a) { return a.getColumn() == state; });

                // Make sure we have found the probability and set it to zero.
                STORM_LOG_THROW(multiplyElement != predecessorForwardTransitions.end(), storm::exceptions::InvalidStateException, "No probability for successor found.");
                ValueType multiplyFactor = multiplyElement->getValue();
                multiplyElement->setValue(storm::utility::zero<ValueType>());

                // At this point, we need to update the (forward) transitions of the predecessor.
                typename row_type::iterator first1 = predecessorForwardTransitions.begin();
                typename row_type::iterator last1 = predecessorForwardTransitions.end();
                typename row_type::iterator first2 = currentStateSuccessors.begin();
                typename row_type::iterator last2 = currentStateSuccessors.end();

                row_type newSuccessors;
                newSuccessors.reserve((last1 - first1) + (last2 - first2));
                std::insert_iterator<row_type> result(newSuccessors, newSuccessors.end());

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
                        std::copy_if(first1, last1, result, [&] (storm::storage::MatrixEntry<index_type, ValueType> const& a) { return a.getColumn() != state; } );
                        break;
                    }
                    if (first2->getColumn() < first1->getColumn()) {
                        *result = storm::utility::simplify(std::move(*first2 * multiplyFactor));
                        ++first2;
                    } else if (first1->getColumn() < first2->getColumn()) {
                        *result = *first1;
                        ++first1;
                    } else {
                        *result = storm::storage::MatrixEntry<index_type, ValueType>(first1->getColumn(), storm::utility::simplify(first1->getValue() + storm::utility::simplify(multiplyFactor * first2->getValue())));
                        ++first1;
                        ++first2;
                    }
                }
                for (; first2 != last2; ++first2) {
                    if (first2->getColumn() != state) {
                        *result = storm::utility::simplify(std::move(*first2 * multiplyFactor));
                    }
                }

                // Now move the new transitions in place.
                predecessorForwardTransitions = std::move(newSuccessors);

                if (!stateRewards) {
                    // Add the probabilities to go to a target state in just one step if we have to compute probabilities.
                    oneStepProbabilities[predecessor] += storm::utility::simplify(multiplyFactor * oneStepProbabilities[row]);
                    STORM_LOG_TRACE("Fixed new next-state probabilities of predecessor states.");
                } else {
                    // If we are computing rewards, we basically scale the state reward of the state to eliminate and
                    // add the result to the state reward of the predecessor.
                    if (hasSelfLoop) {
                        stateRewards.get()[predecessor] += storm::utility::simplify(multiplyFactor * loopProbability * stateRewards.get()[row]);
                    } else {
                        stateRewards.get()[predecessor] += storm::utility::simplify(multiplyFactor * stateRewards.get()[row]);
                    }
                }
            }
            
            // Finally, we need to add the predecessor to the set of predecessors of every successor.
            for (auto const& successorEntry : currentStateSuccessors) {
                row_type& successorBackwardTransitions = backwardTransitions.getRow(successorEntry.getColumn());
                
                // Delete the current state as a predecessor of the successor state only if we are going to remove the
                // current state's forward transitions.
                if (removeForwardTransitions) {
                    typename row_type::iterator elimIt = std::find_if(successorBackwardTransitions.begin(), successorBackwardTransitions.end(), [&](storm::storage::MatrixEntry<index_type, ValueType> const& a) { return a.getColumn() == row; });
                    STORM_LOG_ASSERT(elimIt != successorBackwardTransitions.end(), "Expected a proper backward transition, but found none.");
                    successorBackwardTransitions.erase(elimIt);
                }
                
                typename row_type::iterator first1 = successorBackwardTransitions.begin();
                typename row_type::iterator last1 = successorBackwardTransitions.end();
                typename row_type::iterator first2 = currentStatePredecessors.begin();
                typename row_type::iterator last2 = currentStatePredecessors.end();
                
                row_type newPredecessors;
                newPredecessors.reserve((last1 - first1) + (last2 - first2));
                std::insert_iterator<row_type> result(newPredecessors, newPredecessors.end());
                
                if (!constrained) {
                    for (; first1 != last1; ++result) {
                        if (first2 == last2) {
                            std::copy(first1, last1, result);
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            if (first2->getColumn() != row) {
                                *result = *first2;
                            }
                            ++first2;
                        } else {
                            *result = *first1;
                            if (first1->getColumn() == first2->getColumn()) {
                                ++first2;
                            }
                            ++first1;
                        }
                    }
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<index_type, ValueType> const& a) { return a.getColumn() != row; });
                } else {
                    // If the elimination is constrained, we need to be more selective when we set the new predecessors
                    // of the successor state.
                    for (; first1 != last1; ++result) {
                        if (first2 == last2) {
                            std::copy(first1, last1, result);
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            if (first2->getColumn() != row) {
                                *result = *first2;
                            }
                            ++first2;
                        } else {
                            *result = *first1;
                            if (first1->getColumn() == first2->getColumn()) {
                                ++first2;
                            }
                            ++first1;
                        }
                    }
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<index_type, ValueType> const& a) { return a.getColumn() != row && (!constrained || predecessorConstraint.get(a.getColumn())); });
                }
                
                // Now move the new predecessors in place.
                successorBackwardTransitions = std::move(newPredecessors);
            }
            STORM_LOG_TRACE("Fixed predecessor lists of successor states.");
            
            if (removeForwardTransitions) {
                // Clear the eliminated row to reduce memory consumption.
                currentStateSuccessors.clear();
                currentStateSuccessors.shrink_to_fit();
            }
            if (!constrained) {
                currentStatePredecessors.clear();
                currentStatePredecessors.shrink_to_fit();
            } else {
                currentStatePredecessors = std::move(newCurrentStatePredecessors);
            }
        }
        
        template class FlexibleSparseMatrix<int>;
        template class FlexibleSparseMatrix<double>;
#ifdef STORM_HAVE_CARL
        template class FlexibleSparseMatrix<storm::RationalFunction>;
#endif

            
    }
}