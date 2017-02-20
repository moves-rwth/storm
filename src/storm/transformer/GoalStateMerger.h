#pragma once

#include <limits>
#include <memory>
#include <boost/optional.hpp>

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace transformer {
        
        /*
         * Merges the given target and sink states into a single state with a selfloop
         */
        template <typename SparseModelType>
        class GoalStateMerger {
        public:
                                    
            /* Computes a submodel of the specified model that only considers the states given by maybeStates as well as
             *  * one target state to which all transitions to a state selected by targetStates are redirected and
             *  * one sink state to which all transitions to a state selected by sinkStates are redirected.
             *  If there is no transition to either target or sink, the corresponding state will not be created.
             *
             *  The two given bitvectors targetStates and sinkStates are modified such that they represent the corresponding state in the obtained submodel.
             *
             *  Notes:
             *  * The resulting model will not have any rewardmodels, labels (other then "init") etc.
             *  * It is assumed that the given set of maybeStates can only be left via either a target or a sink state. Otherwise an exception is thrown.
             *  * It is assumed that maybeStates, targetStates, and sinkStates are all disjoint. Otherwise an exception is thrown.
             */
            static std::shared_ptr<SparseModelType> mergeTargetAndSinkStates(SparseModelType const& model, storm::storage::BitVector const& maybeStates, storm::storage::BitVector& targetStates, storm::storage::BitVector& sinkStates) {
                STORM_LOG_THROW(maybeStates.isDisjointFrom(targetStates) && targetStates.isDisjointFrom(sinkStates) && sinkStates.isDisjointFrom(maybeStates), storm::exceptions::InvalidArgumentException, "maybestates, targetstates, and sinkstates are assumed to be disjoint when creating the submodel. However, this is not the case.");
                storm::storage::SparseMatrix<typename SparseModelType::ValueType> const& origMatrix = model.getTransitionMatrix();
                
                // Get the number of rows, cols and entries that the resulting transition matrix will have.
                uint_fast64_t resNumStates(maybeStates.getNumberOfSetBits()), resNumActions(0), resNumTransitions(0);
                bool targetStateRequired = !model.getInitialStates().isDisjointFrom(targetStates);
                bool sinkStateRequired = !model.getInitialStates().isDisjointFrom(targetStates);
                for( auto state : maybeStates) {
                    resNumActions += origMatrix.getRowGroupSize(state);
                    bool hasTransitionToTarget(false), hasTransitionToSink(false);
                    auto const& endOfRowGroup = origMatrix.getRowGroupIndices()[state+1];
                    for (uint_fast64_t row = origMatrix.getRowGroupIndices()[state]; row < endOfRowGroup; ++row) {
                        for (auto const& entry : origMatrix.getRow(row)) {
                            if(maybeStates.get(entry.getColumn())) {
                                ++resNumTransitions;
                            } else if (targetStates.get(entry.getColumn())) {
                                hasTransitionToTarget = true;
                            } else if (sinkStates.get(entry.getColumn())) {
                                hasTransitionToSink = true;
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "There is a transition originating from a maybestate that does not lead to a maybe-, target-, or sinkstate.");
                            }
                        }
                        if(hasTransitionToTarget) {
                            ++resNumTransitions;
                            targetStateRequired = true;
                        }
                        if(hasTransitionToSink) {
                            ++resNumTransitions;
                            sinkStateRequired = true;
                        }
                    }
                }
                
                // Get the index of the target/ sink state in the resulting model (if these states will exist)
                uint_fast64_t targetState(std::numeric_limits<uint_fast64_t>::max()), sinkState(std::numeric_limits<uint_fast64_t>::max()); // init with some invalid state
                if(targetStateRequired) {
                    targetState = resNumStates;
                    ++resNumStates;
                    ++resNumActions;
                    ++resNumTransitions;
                }
                if(sinkStateRequired) {
                    sinkState = resNumStates;
                    ++resNumStates;
                    ++resNumActions;
                    ++resNumTransitions;
                }
                
                // Get a Mapping that yields for each column in the old matrix the corresponding column in the new matrix
                std::vector<uint_fast64_t> oldToNewIndexMap(maybeStates.size(), std::numeric_limits<uint_fast64_t>::max()); // init with some invalid state
                uint_fast64_t newStateIndex = 0;
                for (auto maybeState : maybeStates) {
                    oldToNewIndexMap[maybeState] = newStateIndex;
                    ++newStateIndex;
                }
                
                // Build the transition matrix
                storm::storage::SparseMatrixBuilder<typename SparseModelType::ValueType> builder(resNumActions, resNumStates, resNumTransitions, true, true, resNumStates);
                uint_fast64_t currRow = 0;
                for (auto state : maybeStates) {
                    builder.newRowGroup(currRow);
                    boost::optional<typename SparseModelType::ValueType> targetProbability, sinkProbability;
                    auto const& endOfRowGroup = origMatrix.getRowGroupIndices()[state+1];
                    for (uint_fast64_t row = origMatrix.getRowGroupIndices()[state]; row < endOfRowGroup; ++row) {
                        for (auto const& entry : origMatrix.getRow(row)) {
                            if(maybeStates.get(entry.getColumn())) {
                                builder.addNextValue(currRow, oldToNewIndexMap[entry.getColumn()], entry.getValue());
                            } else if (targetStates.get(entry.getColumn())) {
                                targetProbability = targetProbability.is_initialized() ? *targetProbability + entry.getValue() : entry.getValue();
                            } else if (sinkStates.get(entry.getColumn())) {
                                sinkProbability = sinkProbability.is_initialized() ? *sinkProbability + entry.getValue() : entry.getValue();
                            } else {
                                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "There is a transition originating from a maybestate that does not lead to a maybe-, target-, or sinkstate.");
                            }
                        }
                        if(targetProbability) {
                            builder.addNextValue(currRow, targetState, storm::utility::simplify(*targetProbability));
                        }
                        if(sinkProbability) {
                            builder.addNextValue(currRow, sinkState, storm::utility::simplify(*sinkProbability));
                        }
                        ++currRow;
                    }
                }
                // Add the selfloops at target and sink
                if(targetStateRequired) {
                    builder.newRowGroup(currRow);
                    builder.addNextValue(currRow, targetState, storm::utility::one<typename SparseModelType::ValueType>());
                    ++currRow;
                }
                if(sinkStateRequired) {
                    builder.newRowGroup(currRow);
                    builder.addNextValue(currRow, sinkState, storm::utility::one<typename SparseModelType::ValueType>());
                    ++currRow;
                }
                
                // Get the labeling for the initial states
                storm::storage::BitVector initialStates = model.getInitialStates() % maybeStates;
                initialStates.resize(resNumStates, false);
                if(!model.getInitialStates().isDisjointFrom(targetStates)) {
                    initialStates.set(targetState, true);
                }
                if(!model.getInitialStates().isDisjointFrom(sinkStates)) {
                    initialStates.set(sinkState, true);
                }
                storm::models::sparse::StateLabeling labeling(resNumStates);
                labeling.addLabel("init", std::move(initialStates));
                
                // modify the given target and sink states
                targetStates = storm::storage::BitVector(resNumStates, false);
                if(targetStateRequired) {
                    targetStates.set(targetState, true);
                }
                sinkStates = storm::storage::BitVector(resNumStates, false);
                if(sinkStateRequired) {
                    sinkStates.set(sinkState, true);
                }
                
                // Return the result
                return std::make_shared<SparseModelType>(builder.build(), std::move(labeling));
            }
        };
    }
}
