#ifndef STORM_TRANSFORMER_STATEDUPLICATOR_H
#define STORM_TRANSFORMER_STATEDUPLICATOR_H


#include <memory>
#include <boost/optional.hpp>

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/constants.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"


namespace storm {
    namespace transformer {
        
        /*
         * Duplicates the state space of the given model and redirects the incoming transitions of gateStates of the first copy to the gateStates of the second copy.
         * Only states reachable from the initial states are kept.
         */
        template <typename SparseModelType>
        class StateDuplicator {
        public:

            struct StateDuplicatorReturnType {
                std::shared_ptr<SparseModelType> model;  // The resulting model
                storm::storage::BitVector firstCopy;    // The states of the resulting model that correspond to the first copy
                storm::storage::BitVector secondCopy;   // The states of the resulting model that correspond to the second copy
                storm::storage::BitVector gateStates;   // The gate states of the resulting model
                std::vector<uint_fast64_t> newToOldStateIndexMapping; // Gives for each state in the resulting model the corresponding state in the original model
                std::vector<uint_fast64_t> firstCopyOldToNewStateIndexMapping; //Maps old indices of states in the first copy to their new indices
                std::vector<uint_fast64_t> secondCopyOldToNewStateIndexMapping; //Maps old indices of states in the second copy to their new indices
                storm::storage::BitVector duplicatedStates;   // The states in the original model that have been duplicated
                storm::storage::BitVector reachableStates;   // The states in the original model that are reachable from the initial state
            };
            
            /*
             * Duplicates the state space of the given model and redirects the incoming transitions of gateStates of the first copy to the gateStates of the second copy.
             * 
             * Note that only reachable states are kept.
             * Gate states will always belong to the second copy.
             * Rewards and labels are duplicated accordingly.
             * However, the non-gateStates in the second copy will not get the label for initial states.
             *
             * @param originalModel The model to be duplicated
             * @param gateStates The states for which the incoming transitions are redirected
             */
            static StateDuplicatorReturnType transform(SparseModelType const& originalModel, storm::storage::BitVector const& gateStates) {
                STORM_LOG_DEBUG("Invoked state duplicator on model with " << originalModel.getNumberOfStates() << " states.");
                StateDuplicatorReturnType result;
                
                // Collect some data for the result
                initializeTransformation(originalModel, gateStates, result);
                
                // Transform the ingedients of the model
                storm::storage::SparseMatrix<typename SparseModelType::ValueType> matrix = transformMatrix(originalModel.getTransitionMatrix(), result, gateStates);
                storm::models::sparse::StateLabeling stateLabeling(matrix.getRowGroupCount());
                for (auto const& label : originalModel.getStateLabeling().getLabels()) {
                    storm::storage::BitVector newBitVectorForLabel = transformStateBitVector(originalModel.getStateLabeling().getStates(label), result);
                    if (label=="init") {
                        newBitVectorForLabel &= (result.firstCopy | result.gateStates);
                    }
                    stateLabeling.addLabel(label, std::move(newBitVectorForLabel));
                }
                
                std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
                for (auto const& rewardModel : originalModel.getRewardModels()) {
                    rewardModels.insert(std::make_pair(rewardModel.first, transformRewardModel(rewardModel.second, originalModel.getTransitionMatrix().getRowGroupIndices(), result, gateStates)));
                }
                
                boost::optional<storm::models::sparse::ChoiceLabeling> choiceLabeling;
                if (originalModel.hasChoiceLabeling()) {
                    for (auto const& label : originalModel.getChoiceLabeling().getLabels()) {
                        storm::storage::BitVector newBitVectorForLabel = transformActionBitVector(originalModel.getChoiceLabeling().getChoices(label), originalModel.getTransitionMatrix().getRowGroupIndices(), result);
                        choiceLabeling->addLabel(label, std::move(newBitVectorForLabel));
                    }
                }
                
                result.model = std::make_shared<SparseModelType>(createTransformedModel(originalModel, result, matrix, stateLabeling, rewardModels, choiceLabeling));
                STORM_LOG_DEBUG("State duplicator is done. Resulting model has " << result.model->getNumberOfStates() << " states, where " << result.firstCopy.getNumberOfSetBits() << " are in the first copy.");
                return result;
            }
            
        private:
            
            static void initializeTransformation(SparseModelType const& originalModel, storm::storage::BitVector const& gateStates, StateDuplicatorReturnType& result) {
                
                storm::storage::BitVector noStates(originalModel.getNumberOfStates(), false);
                // Get the states that are reachable without visiting a gateState
                storm::storage::BitVector statesForFirstCopy = storm::utility::graph::getReachableStates(originalModel.getTransitionMatrix(), originalModel.getInitialStates(), ~gateStates, noStates);
                
                // Get the states reachable from gateStates
                storm::storage::BitVector statesForSecondCopy = storm::utility::graph::getReachableStates(originalModel.getTransitionMatrix(), gateStates, ~noStates, noStates);
            
                result.duplicatedStates = statesForFirstCopy & statesForSecondCopy;
                result.reachableStates = statesForFirstCopy | statesForSecondCopy;
            
                uint_fast64_t numStates = statesForFirstCopy.getNumberOfSetBits() + statesForSecondCopy.getNumberOfSetBits();
                result.firstCopy = statesForFirstCopy % result.reachableStates; // only consider reachable states
                result.firstCopy.resize(numStates, false); // the new states do NOT belong to the first copy
                result.secondCopy = (statesForSecondCopy & (~statesForFirstCopy)) % result.reachableStates; // only consider reachable states
                result.secondCopy.resize(numStates, true); // the new states DO belong to the second copy
                STORM_LOG_ASSERT((result.firstCopy^result.secondCopy).full(), "firstCopy and secondCopy do not partition the state space.");
            
                // Get the state mappings.
                // We initialize them with illegal values to assert that we don't get a valid
                // state when given e.g. an unreachable state or a state from the other copy.
                result.newToOldStateIndexMapping = std::vector<uint_fast64_t>(numStates, std::numeric_limits<uint_fast64_t>::max());
                result.firstCopyOldToNewStateIndexMapping = std::vector<uint_fast64_t>(originalModel.getNumberOfStates(), std::numeric_limits<uint_fast64_t>::max());
                result.secondCopyOldToNewStateIndexMapping = std::vector<uint_fast64_t>(originalModel.getNumberOfStates(), std::numeric_limits<uint_fast64_t>::max());
                uint_fast64_t newState = 0;
                for (auto const& oldState : result.reachableStates) {
                    result.newToOldStateIndexMapping[newState] = oldState;
                    if (statesForFirstCopy.get(oldState)) {
                        result.firstCopyOldToNewStateIndexMapping[oldState] = newState;
                    } else {
                        result.secondCopyOldToNewStateIndexMapping[oldState] = newState;
                    }
                    ++newState;
                }
                // The remaining states are duplicates. All these states belong to the second copy.
                
                for (auto const& oldState : result.duplicatedStates) {
                    result.newToOldStateIndexMapping[newState] = oldState;
                    result.secondCopyOldToNewStateIndexMapping[oldState] = newState;
                    ++newState;
                }
                STORM_LOG_ASSERT(newState == numStates, "Unexpected state Indices");
                
                result.gateStates = transformStateBitVector(gateStates, result);
            }
            
            template<typename ValueType = typename SparseModelType::ValueType, typename RewardModelType = typename SparseModelType::RewardModelType>
            static typename std::enable_if<std::is_same<RewardModelType, storm::models::sparse::StandardRewardModel<ValueType>>::value, RewardModelType>::type
            transformRewardModel(RewardModelType const& originalRewardModel, std::vector<uint_fast64_t> const& originalRowGroupIndices, StateDuplicatorReturnType const& result, storm::storage::BitVector const& gateStates) {
                boost::optional<std::vector<ValueType>> stateRewardVector;
                boost::optional<std::vector<ValueType>> stateActionRewardVector;
                boost::optional<storm::storage::SparseMatrix<ValueType>> transitionRewardMatrix;
                if (originalRewardModel.hasStateRewards()) {
                    stateRewardVector = transformStateValueVector(originalRewardModel.getStateRewardVector(), result);
                }
                if (originalRewardModel.hasStateActionRewards()) {
                    stateActionRewardVector = transformActionValueVector(originalRewardModel.getStateActionRewardVector(), originalRowGroupIndices, result);
                }
                if (originalRewardModel.hasTransitionRewards()) {
                    transitionRewardMatrix = transformMatrix(originalRewardModel.getTransitionRewardMatrix(), result, gateStates);
                }
                return RewardModelType(std::move(stateRewardVector), std::move(stateActionRewardVector), std::move(transitionRewardMatrix));
            }
            
            template<typename ValueType = typename SparseModelType::ValueType>
            static storm::storage::SparseMatrix<ValueType> transformMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix, StateDuplicatorReturnType const& result, storm::storage::BitVector const& gateStates) {
                // Build the builder
                uint_fast64_t numStates = result.newToOldStateIndexMapping.size();
                uint_fast64_t numRows = 0;
                uint_fast64_t numEntries = 0;
                for (auto const& oldState : result.newToOldStateIndexMapping) {
                    numRows += originalMatrix.getRowGroupSize(oldState);
                    numEntries += originalMatrix.getRowGroupEntryCount(oldState);
                }
                storm::storage::SparseMatrixBuilder<ValueType> builder(numRows, numStates, numEntries, true, !originalMatrix.hasTrivialRowGrouping(), originalMatrix.hasTrivialRowGrouping() ? 0 : numStates);
                
                // Fill in the data
                uint_fast64_t newRow = 0;
                for (uint_fast64_t newState = 0; newState < numStates; ++newState) {
                    if (!originalMatrix.hasTrivialRowGrouping()) {
                        builder.newRowGroup(newRow);
                    }
                    uint_fast64_t oldState = result.newToOldStateIndexMapping[newState];
                    for (uint_fast64_t oldRow = originalMatrix.getRowGroupIndices()[oldState]; oldRow < originalMatrix.getRowGroupIndices()[oldState+1]; ++oldRow) {
                        for (auto const& entry : originalMatrix.getRow(oldRow)) {
                            if (result.firstCopy.get(newState) && !gateStates.get(entry.getColumn())) {
                                builder.addNextValue(newRow, result.firstCopyOldToNewStateIndexMapping[entry.getColumn()], entry.getValue());
                            } else if (!result.duplicatedStates.get(entry.getColumn())) {
                                builder.addNextValue(newRow, result.secondCopyOldToNewStateIndexMapping[entry.getColumn()], entry.getValue());
                            }
                        }
                        //To add values in the right order, transitions to duplicated states have to be added in a second run.
                        for (auto const& entry : originalMatrix.getRow(oldRow)) {
                            if (result.secondCopy.get(newState) && result.duplicatedStates.get(entry.getColumn())) {
                                builder.addNextValue(newRow, result.secondCopyOldToNewStateIndexMapping[entry.getColumn()], entry.getValue());
                            }
                        }
                        ++newRow;
                    }
                }
                
                return builder.build();
            }
            
            
            template<typename ValueType = typename SparseModelType::ValueType>
            static std::vector<ValueType> transformActionValueVector(std::vector<ValueType>const& originalVector, std::vector<uint_fast64_t> const& originalRowGroupIndices, StateDuplicatorReturnType const& result) {
                uint_fast64_t numActions = 0;
                for (auto const& oldState : result.newToOldStateIndexMapping) {
                    numActions += originalRowGroupIndices[oldState+1] - originalRowGroupIndices[oldState];
                }
                std::vector<ValueType> v;
                v.reserve(numActions);
                for (auto const& oldState : result.newToOldStateIndexMapping) {
                    for (uint_fast64_t oldAction = originalRowGroupIndices[oldState]; oldAction < originalRowGroupIndices[oldState+1]; ++oldAction) {
                        v.push_back(originalVector[oldAction]);
                    }
                }
                STORM_LOG_ASSERT(v.size() == numActions, "Unexpected vector size.");
                return v;
            }
            
            template<typename ValueType = typename SparseModelType::ValueType>
            static std::vector<ValueType> transformStateValueVector(std::vector<ValueType> const& originalVector, StateDuplicatorReturnType const& result) {
                uint_fast64_t numStates = result.newToOldStateIndexMapping.size();
                std::vector<ValueType> v;
                v.reserve(numStates);
                for (auto const& oldState : result.newToOldStateIndexMapping) {
                    v.push_back(originalVector[oldState]);
                }
                STORM_LOG_ASSERT(v.size() == numStates, "Unexpected vector size.");
                return v;
            }
            
            static storm::storage::BitVector transformActionBitVector(storm::storage::BitVector const& originalBitVector, std::vector<uint_fast64_t> const& originalRowGroupIndices, StateDuplicatorReturnType const& result) {
                uint_fast64_t numActions = 0;
                for (auto const& oldState : result.newToOldStateIndexMapping) {
                    numActions += originalRowGroupIndices[oldState+1] - originalRowGroupIndices[oldState];
                }
                storm::storage::BitVector bv(numActions, false);
                uint_fast64_t newAction = 0;
                for (auto const& oldState : result.newToOldStateIndexMapping) {
                    for (uint_fast64_t oldAction = originalRowGroupIndices[oldState]; oldAction < originalRowGroupIndices[oldState+1]; ++oldAction) {
                        if (originalBitVector.get(oldAction)) {
                            bv.set(newAction, true);
                        }
                        ++newAction;
                    }
                }
                STORM_LOG_ASSERT(newAction == numActions, "Unexpected vector size.");
                return bv;
            }
            
           static storm::storage::BitVector transformStateBitVector(storm::storage::BitVector const& originalBitVector, StateDuplicatorReturnType const& result) {
                uint_fast64_t numStates = result.newToOldStateIndexMapping.size();
                storm::storage::BitVector bv(numStates);
                for (uint_fast64_t newState = 0; newState < numStates; ++newState) {
                    uint_fast64_t oldState = result.newToOldStateIndexMapping[newState];
                    bv.set(newState, originalBitVector.get(oldState));
                }
                return bv;
            }
            
            
            template<typename MT = SparseModelType>
            static typename std::enable_if<
            std::is_same<MT,storm::models::sparse::Dtmc<typename SparseModelType::ValueType>>::value ||
            std::is_same<MT,storm::models::sparse::Mdp<typename SparseModelType::ValueType>>::value ||
            std::is_same<MT,storm::models::sparse::Ctmc<typename SparseModelType::ValueType>>::value,
            MT>::type
            createTransformedModel(MT const& /*originalModel*/,
                                   StateDuplicatorReturnType const& /*result*/,
                                   storm::storage::SparseMatrix<typename MT::ValueType>& matrix,
                                   storm::models::sparse::StateLabeling& stateLabeling,
                                   std::unordered_map<std::string,
                                   typename MT::RewardModelType>& rewardModels,
                                   boost::optional<storm::models::sparse::ChoiceLabeling>& choiceLabeling ) {
                return MT(std::move(matrix), std::move(stateLabeling), std::move(rewardModels), std::move(choiceLabeling));
            }
            
            template<typename MT = SparseModelType>
            static typename std::enable_if<
            std::is_same<MT,storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType>>::value,
            MT>::type
            createTransformedModel(MT const& originalModel,
                                   StateDuplicatorReturnType const& result,
                                   storm::storage::SparseMatrix<typename MT::ValueType>& matrix,
                                   storm::models::sparse::StateLabeling& stateLabeling,
                                   std::unordered_map<std::string,
                                   typename MT::RewardModelType>& rewardModels,
                                   boost::optional<storm::models::sparse::ChoiceLabeling>& choiceLabeling ) {
                storm::storage::BitVector markovianStates = transformStateBitVector(originalModel.getMarkovianStates(), result);
                std::vector<typename MT::ValueType> exitRates = transformStateValueVector(originalModel.getExitRates(), result);
                return MT(std::move(matrix), std::move(stateLabeling), std::move(markovianStates), std::move(exitRates), true, std::move(rewardModels), std::move(choiceLabeling));
            }
            
            
        };
    }
}
#endif // STORM_TRANSFORMER_STATEDUPLICATOR_H
