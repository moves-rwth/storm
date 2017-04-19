#include "storm/transformer/GoalStateMerger.h"

#include <limits>
#include <memory>

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace transformer {
        
        template <typename SparseModelType>
        GoalStateMerger<SparseModelType>::GoalStateMerger(SparseModelType const& model) : originalModel(model) {
            // Intentionally left empty
        }
            
        template <typename SparseModelType>
        std::shared_ptr<SparseModelType> GoalStateMerger<SparseModelType>::mergeTargetAndSinkStates(storm::storage::BitVector const& maybeStates, storm::storage::BitVector& targetStates, storm::storage::BitVector& sinkStates, std::vector<std::string> const& selectedRewardModels) {
            STORM_LOG_THROW(maybeStates.isDisjointFrom(targetStates) && targetStates.isDisjointFrom(sinkStates) && sinkStates.isDisjointFrom(maybeStates), storm::exceptions::InvalidArgumentException, "maybestates, targetstates, and sinkstates are assumed to be disjoint when creating the submodel. However, this is not the case.");
  
            boost::optional<uint_fast64_t> targetState, sinkState;
            auto builder = initializeTransitionMatrixBuilder(maybeStates, targetStates, sinkStates, targetState, sinkState);
            auto transitionMatrix = buildTransitionMatrix(maybeStates, targetStates, sinkStates, targetState, sinkState, builder);
            
            uint_fast64_t resNumStates = transitionMatrix.getRowGroupCount();
                
            // Get the labeling for the initial states
            storm::storage::BitVector initialStates = originalModel.getInitialStates() % maybeStates;
            initialStates.resize(resNumStates, false);
            if(!originalModel.getInitialStates().isDisjointFrom(targetStates)) {
                initialStates.set(*targetState, true);
            }
            if(!originalModel.getInitialStates().isDisjointFrom(sinkStates)) {
                initialStates.set(*sinkState, true);
            }
            storm::models::sparse::StateLabeling labeling(resNumStates);
            labeling.addLabel("init", std::move(initialStates));
                
            // Get the reward models
            std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
            for (auto rewardModelName : selectedRewardModels) {
                auto origTotalRewards = originalModel.getRewardModel(rewardModelName).getTotalRewardVector(originalModel.getTransitionMatrix());
                auto transitionsOfMaybeStates = originalModel.getTransitionMatrix().getRowIndicesOfRowGroups(maybeStates);
                auto resTotalRewards = storm::utility::vector::filterVector(origTotalRewards, transitionsOfMaybeStates);
                resTotalRewards.resize(transitionMatrix.getRowCount(), storm::utility::zero<typename SparseModelType::RewardModelType::ValueType>());
                rewardModels.insert(std::make_pair(rewardModelName, typename SparseModelType::RewardModelType(boost::none, resTotalRewards)));
            }
                
            // modify the given target and sink states
            targetStates = storm::storage::BitVector(resNumStates, false);
            if(targetState) {
                targetStates.set(*targetState, true);
            }
            sinkStates = storm::storage::BitVector(resNumStates, false);
            if(sinkState) {
                sinkStates.set(*sinkState, true);
            }
                
            // Return the result
            return std::make_shared<SparseModelType>(std::move(transitionMatrix), std::move(labeling), std::move(rewardModels));
        }
        
        template <typename SparseModelType>
        storm::storage::SparseMatrixBuilder<typename SparseModelType::ValueType> GoalStateMerger<SparseModelType>::initializeTransitionMatrixBuilder(storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& sinkStates, boost::optional<uint_fast64_t>& newTargetState, boost::optional<uint_fast64_t>& newSinkState) {
            
            storm::storage::SparseMatrix<typename SparseModelType::ValueType> const& origMatrix = originalModel.getTransitionMatrix();
                
            // Get the number of rows, cols and entries that the resulting transition matrix will have.
            uint_fast64_t resNumStates(maybeStates.getNumberOfSetBits()), resNumActions(0), resNumTransitions(0);
            bool targetStateRequired = !originalModel.getInitialStates().isDisjointFrom(targetStates);
            bool sinkStateRequired = !originalModel.getInitialStates().isDisjointFrom(sinkStates);
            for( auto state : maybeStates) {
                resNumActions += origMatrix.getRowGroupSize(state);
                auto const& endOfRowGroup = origMatrix.getRowGroupIndices()[state+1];
                for (uint_fast64_t row = origMatrix.getRowGroupIndices()[state]; row < endOfRowGroup; ++row) {
                    bool hasTransitionToTarget(false), hasTransitionToSink(false);
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
            if(targetStateRequired) {
                newTargetState = resNumStates;
                ++resNumStates;
                ++resNumActions;
                ++resNumTransitions;
            }
            if(sinkStateRequired) {
                newSinkState = resNumStates;
                ++resNumStates;
                ++resNumActions;
                ++resNumTransitions;
            }
            
            return storm::storage::SparseMatrixBuilder<typename SparseModelType::ValueType>(resNumActions, resNumStates, resNumTransitions, true, true, resNumStates);

        }
        
        template <typename SparseModelType>
        storm::storage::SparseMatrix<typename SparseModelType::ValueType> GoalStateMerger<SparseModelType>::buildTransitionMatrix(storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& sinkStates, boost::optional<uint_fast64_t> const& newTargetState, boost::optional<uint_fast64_t>const& newSinkState, storm::storage::SparseMatrixBuilder<typename SparseModelType::ValueType>& builder) {
        
            // Get a Mapping that yields for each column in the old matrix the corresponding column in the new matrix
            std::vector<uint_fast64_t> oldToNewIndexMap(maybeStates.size(), std::numeric_limits<uint_fast64_t>::max()); // init with some invalid state
            uint_fast64_t newStateIndex = 0;
            for (auto maybeState : maybeStates) {
                oldToNewIndexMap[maybeState] = newStateIndex;
                ++newStateIndex;
            }
                
            // Build the transition matrix
            storm::storage::SparseMatrix<typename SparseModelType::ValueType> const& origMatrix = originalModel.getTransitionMatrix();
            uint_fast64_t currRow = 0;
            for (auto state : maybeStates) {
                builder.newRowGroup(currRow);
                auto const& endOfRowGroup = origMatrix.getRowGroupIndices()[state+1];
                for (uint_fast64_t row = origMatrix.getRowGroupIndices()[state]; row < endOfRowGroup; ++row) {
                    boost::optional<typename SparseModelType::ValueType> targetProbability, sinkProbability;
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
                        assert(newTargetState);
                        builder.addNextValue(currRow, *newTargetState, storm::utility::simplify(*targetProbability));
                    }
                    if(sinkProbability) {
                        assert(newSinkState);
                        builder.addNextValue(currRow, *newSinkState, storm::utility::simplify(*sinkProbability));
                    }
                    ++currRow;
                }
            }
            // Add the selfloops at target and sink
            if(newTargetState) {
                builder.newRowGroup(currRow);
                builder.addNextValue(currRow, *newTargetState, storm::utility::one<typename SparseModelType::ValueType>());
                ++currRow;
            }
            if(newSinkState) {
                builder.newRowGroup(currRow);
                builder.addNextValue(currRow, *newSinkState, storm::utility::one<typename SparseModelType::ValueType>());
                ++currRow;
            }
            
            return builder.build();
        }
        
        template class GoalStateMerger<storm::models::sparse::Dtmc<double>>;
        template class GoalStateMerger<storm::models::sparse::Mdp<double>>;
        template class GoalStateMerger<storm::models::sparse::Dtmc<storm::RationalNumber>>;
        template class GoalStateMerger<storm::models::sparse::Mdp<storm::RationalNumber>>;
        template class GoalStateMerger<storm::models::sparse::Dtmc<storm::RationalFunction>>;
        template class GoalStateMerger<storm::models::sparse::Mdp<storm::RationalFunction>>;


    }
}
