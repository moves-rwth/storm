#ifndef STORM_TRANSFORMER_SUBSYSTEMBUILDER_H
#define STORM_TRANSFORMER_SUBSYSTEMBUILDER_H


#include <memory>
#include <boost/optional.hpp>

#include "src/models/sparse/StandardRewardModel.h"
#include "src/utility/constants.h"
#include "src/utility/graph.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/Ctmc.h"
#include "src/models/sparse/MarkovAutomaton.h"

#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace transformer {
        
        /*
         * Removes all states that are not part of the subsystem
         */
        template <typename SparseModelType>
        class SubsystemBuilder {
        public:

            struct SubsystemBuilderReturnType {
                // The resulting model
                std::shared_ptr<SparseModelType> model;
                // Gives for each state in the resulting model the corresponding state in the original model and vice versa.
                // If a state does not exist in the other model, an invalid index is given.
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                std::vector<uint_fast64_t> oldToNewStateIndexMapping;
                // marks the actions of the original model that are still available in the subsystem
                storm::storage::BitVector subsystemActions;
            };
            
            /*
             * Removes all states that are not part of the subsystem
             * all actions (i.e. rows) that lead outside of the subsystem are erased. Note that this might introduce deadlock states.
             * 
             * @param originalModel The model to be duplicated
             * @param subsystem The states that will be kept
             */
            static SubsystemBuilderReturnType transform(SparseModelType const& originalModel, storm::storage::BitVector const& subsystem) {
                STORM_LOG_DEBUG("Invoked subsystem builder on model with " << originalModel.getNumberOfStates() << " states.");
                SubsystemBuilderReturnType result;
                
                uint_fast64_t subsystemStateCount = subsystem.getNumberOfSetBits();
                STORM_LOG_THROW(subsystemStateCount != 0, storm::exceptions::InvalidArgumentException, "Invoked SubsystemBuilder for an empty subsystem.");
                if(subsystemStateCount == subsystem.size()) {
                    result.model = std::make_shared<SparseModelType>(originalModel);
                    result.newToOldStateIndexMapping = storm::utility::vector::buildVectorForRange(0, result.model->getNumberOfStates());
                    result.oldToNewStateIndexMapping = result.newToOldStateIndexMapping;
                    result.subsystemActions = storm::storage::BitVector(result.model->getTransitionMatrix().getRowCount(), true);
                    return result;
                }
                
                result.newToOldStateIndexMapping.reserve(subsystemStateCount);
                result.oldToNewStateIndexMapping = std::vector<uint_fast64_t>(subsystem.size(), std::numeric_limits<uint_fast64_t>::max());
                result.subsystemActions = storm::storage::BitVector(result.model->getTransitionMatrix().getRowCount(), false);
                for(auto subsysState : subsystem) {
                    result.oldToNewStateIndexMapping[subsysState] = result.newToOldStateIndexMapping.size();
                    result.newToOldStateIndexMapping.push_back(subsysState);
                    for(uint_fast64_t row = originalModel.getTransitionMatrix().getRowGroupIndices()[subsysState]; row < originalModel.getTransitionMatrix().getRowGroupIndices()[subsysState+1]; ++row) {
                        result.subsystemActions.set(row, true);
                        for(auto const& entry : originalModel.getTransitionMatrix().getRow(row)) {
                            if(!subsystem.get(entry.getColumn())) {
                                result.subsystemActions.set(row, false);
                                break;
                            }
                        }
                    }
                }
                
                // Transform the ingedients of the model
                storm::storage::SparseMatrix<typename SparseModelType::ValueType> matrix = transformMatrix(originalModel.getTransitionMatrix(), result);
                storm::models::sparse::StateLabeling labeling(matrix.getRowGroupCount());
                for(auto const& label : originalModel.getStateLabeling().getLabels()){
                    storm::storage::BitVector newBitVectorForLabel = transformStateBitVector(originalModel.getStateLabeling().getStates(label), subsystem);
                    labeling.addLabel(label, std::move(newBitVectorForLabel));
                }
                std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
                for(auto const& rewardModel : originalModel.getRewardModels()){
                    rewardModels.insert(std::make_pair(rewardModel.first, transformRewardModel(rewardModel.second, originalModel.getTransitionMatrix().getRowGroupIndices(), subsystem, result)));
                }
                boost::optional<std::vector<storm::models::sparse::LabelSet>> choiceLabeling;
                if(originalModel.hasChoiceLabeling()){
                    choiceLabeling = transformActionValueVector<storm::models::sparse::LabelSet>(originalModel.getChoiceLabeling(), result.subsystemActions);
                }
                result.model = std::make_shared<SparseModelType>(createTransformedModel(originalModel, subsystem, result, matrix, labeling, rewardModels, choiceLabeling));
                STORM_LOG_DEBUG("Subsystem Builder is done. Resulting model has " << result.model->getNumberOfStates() << " states.");
                return result;
            }
            
        private:
            template<typename ValueType = typename SparseModelType::ValueType, typename RewardModelType = typename SparseModelType::RewardModelType>
            static typename std::enable_if<std::is_same<RewardModelType, storm::models::sparse::StandardRewardModel<ValueType>>::value, RewardModelType>::type
            transformRewardModel(RewardModelType const& originalRewardModel, std::vector<uint_fast64_t> const& originalRowGroupIndices, storm::storage::BitVector const& subsystem, SubsystemBuilderReturnType const& result) {
                boost::optional<std::vector<ValueType>> stateRewardVector;
                boost::optional<std::vector<ValueType>> stateActionRewardVector;
                boost::optional<storm::storage::SparseMatrix<ValueType>> transitionRewardMatrix;
                if(originalRewardModel.hasStateRewards()){
                    stateRewardVector = transformStateValueVector(originalRewardModel.getStateRewardVector(), subsystem);
                }
                if(originalRewardModel.hasStateActionRewards()){
                    stateActionRewardVector = transformActionValueVector(originalRewardModel.getStateActionRewardVector(), result.subsystemActions);
                }
                if(originalRewardModel.hasTransitionRewards()){
                    transitionRewardMatrix = transformMatrix(originalRewardModel.getTransitionRewardMatrix(), result);
                }
                return RewardModelType(std::move(stateRewardVector), std::move(stateActionRewardVector), std::move(transitionRewardMatrix));
            }
            
            template<typename ValueType = typename SparseModelType::ValueType>
            static storm::storage::SparseMatrix<ValueType> transformMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix, SubsystemBuilderReturnType const& result) {
                // Build the builder
                uint_fast64_t const numStates = result.newToOldStateIndexMapping.size();
                uint_fast64_t const numRows = result.subsystemActions.getNumberOfSetBits();
                uint_fast64_t numEntries = 0;
                for(auto row : result.subsystemActions) {
                    numEntries += originalMatrix.getRow(row).getNumberOfEntries();
                }
                storm::storage::SparseMatrixBuilder<ValueType> builder(numRows, numStates, numEntries, true, !originalMatrix.hasTrivialRowGrouping(), originalMatrix.hasTrivialRowGrouping() ? 0 : numStates);
                
                // Fill in the data
                uint_fast64_t newRow = 0;
                for(uint_fast64_t newState = 0; newState < numStates; ++newState){
                    if(!originalMatrix.hasTrivialRowGrouping()){
                        builder.newRowGroup(newRow);
                    }
                    uint_fast64_t oldState = result.newToOldStateIndexMapping[newState];
                    for (uint_fast64_t oldRow = result.subsystemActions.getNextSetIndex(originalMatrix.getRowGroupIndices()[oldState]); oldRow < originalMatrix.getRowGroupIndices()[oldState+1]; oldRow = result.subsystemActions.getNextSetIndex(oldRow+1)){
                        for(auto const& entry : originalMatrix.getRow(oldRow)){
                            builder.addNextValue(newRow, result.oldToNewStateIndexMapping[entry.getColumn()], entry.getValue());
                        }
                        ++newRow;
                    }
                }
                return builder.build();
            }
            
            
            template<typename ValueType = typename SparseModelType::ValueType>
            static std::vector<ValueType> transformActionValueVector(std::vector<ValueType> const& originalVector, storm::storage::BitVector const& subsystemActions) {
                std::vector<ValueType> v;
                v.reserve(subsystemActions.getNumberOfSetBits());
                for(auto action : subsystemActions){
                    v.push_back(originalVector[action]);
                }
                return v;
            }
            
            template<typename ValueType = typename SparseModelType::ValueType>
            static std::vector<ValueType> transformStateValueVector(std::vector<ValueType> const& originalVector, storm::storage::BitVector const& subsystem) {
                std::vector<ValueType> v;
                v.reserve(subsystem.getNumberOfSetBits());
                for(auto state : subsystem){
                    v.push_back(originalVector[state]);
                }
                return v;
            }
            
            static storm::storage::BitVector transformStateBitVector(storm::storage::BitVector const& originalBitVector, storm::storage::BitVector const& subsystem) {
                return originalBitVector % subsystem;
            }
            
            
            template<typename MT = SparseModelType>
            static typename std::enable_if<
            std::is_same<MT,storm::models::sparse::Dtmc<typename SparseModelType::ValueType>>::value ||
            std::is_same<MT,storm::models::sparse::Mdp<typename SparseModelType::ValueType>>::value ||
            std::is_same<MT,storm::models::sparse::Ctmc<typename SparseModelType::ValueType>>::value,
            MT>::type
            createTransformedModel(MT const& originalModel,
                                   storm::storage::BitVector const& subsystem,
                                   SubsystemBuilderReturnType const& result,
                                   storm::storage::SparseMatrix<typename MT::ValueType>& matrix,
                                   storm::models::sparse::StateLabeling& stateLabeling,
                                   std::unordered_map<std::string,
                                   typename MT::RewardModelType>& rewardModels,
                                   boost::optional<std::vector<typename storm::models::sparse::LabelSet>>& choiceLabeling ) {
                return MT(std::move(matrix), std::move(stateLabeling), std::move(rewardModels), std::move(choiceLabeling));
            }
            
            template<typename MT = SparseModelType>
            static typename std::enable_if<
            std::is_same<MT,storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType>>::value,
            MT>::type
            createTransformedModel(MT const& originalModel,
                                   storm::storage::BitVector const& subsystem,
                                   SubsystemBuilderReturnType const& result,
                                   storm::storage::SparseMatrix<typename MT::ValueType>& matrix,
                                   storm::models::sparse::StateLabeling& stateLabeling,
                                   std::unordered_map<std::string,
                                   typename MT::RewardModelType>& rewardModels,
                                   boost::optional<std::vector<typename storm::models::sparse::LabelSet>>& choiceLabeling ) {
                storm::storage::BitVector markovianStates = transformStateBitVector(originalModel.getMarkovianStates(), subsystem);
                std::vector<typename MT::ValueType> exitRates = transformStateValueVector(originalModel.getExitRates(), subsystem);
                return MT(std::move(matrix), std::move(stateLabeling), std::move(markovianStates), std::move(exitRates), true, std::move(rewardModels), std::move(choiceLabeling));
            }
            
            
        };
    }
}
#endif // STORM_TRANSFORMER_SUBSYSTEMBUILDER_H
