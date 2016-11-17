#ifndef STORM_TRANSFORMER_SUBSYSTEMBUILDER_H
#define STORM_TRANSFORMER_SUBSYSTEMBUILDER_H


#include <memory>
#include <boost/optional.hpp>

#include "src/storm/models/sparse/StandardRewardModel.h"
#include "src/storm/utility/constants.h"
#include "src/storm/utility/graph.h"
#include "src/storm/utility/macros.h"
#include "src/storm/utility/vector.h"
#include "src/storm/models/sparse/Dtmc.h"
#include "src/storm/models/sparse/Mdp.h"
#include "src/storm/models/sparse/Ctmc.h"
#include "src/storm/models/sparse/MarkovAutomaton.h"

#include "src/storm/exceptions/InvalidArgumentException.h"
#include "src/storm/exceptions/InvalidStateException.h"

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
                // Gives for each state in the resulting model the corresponding state in the original model.
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                // marks the actions of the original model that are still available in the subsystem
                storm::storage::BitVector keptActions;
            };
            
            /*
             * Removes all states and actions that are not part of the subsystem.
             * A state is part of the subsystem iff it is selected in subsystemStates.
             * An action is part of the subsystem iff 
             *    * it is selected in subsystemActions AND
             *    * it originates from a state that is part of the subsystem AND
             *    * it does not contain a transition leading to a state outside of the subsystem.
             *
             * If this introduces a deadlock state (i.e., a state without an action) an exception is thrown.
             * 
             * @param originalModel The original model.
             * @param subsystemStates The selected states.
             * @param subsystemActions The selected actions
             */
            static SubsystemBuilderReturnType transform(SparseModelType const& originalModel, storm::storage::BitVector const& subsystemStates, storm::storage::BitVector const& subsystemActions) {
                STORM_LOG_DEBUG("Invoked subsystem builder on model with " << originalModel.getNumberOfStates() << " states.");
                STORM_LOG_THROW(!(originalModel.getInitialStates() & subsystemStates).empty(), storm::exceptions::InvalidArgumentException, "The subsystem would not contain any initial states");
                SubsystemBuilderReturnType result;
                
                uint_fast64_t subsystemStateCount = subsystemStates.getNumberOfSetBits();
                STORM_LOG_THROW(subsystemStateCount != 0, storm::exceptions::InvalidArgumentException, "Invoked SubsystemBuilder for an empty subsystem.");
                if(subsystemStateCount == subsystemStates.size() && subsystemActions.full()) {
                    result.model = std::make_shared<SparseModelType>(originalModel);
                    result.newToOldStateIndexMapping = storm::utility::vector::buildVectorForRange(0, result.model->getNumberOfStates());
                    result.keptActions = storm::storage::BitVector(result.model->getTransitionMatrix().getRowCount(), true);
                    return result;
                }
                
                result.newToOldStateIndexMapping.reserve(subsystemStateCount);
                result.keptActions = storm::storage::BitVector(originalModel.getTransitionMatrix().getRowCount(), false);
                for(auto subsysState : subsystemStates) {
                    result.newToOldStateIndexMapping.push_back(subsysState);
                    bool stateHasOneChoiceLeft = false;
                    for(uint_fast64_t row = subsystemActions.getNextSetIndex(originalModel.getTransitionMatrix().getRowGroupIndices()[subsysState]); row < originalModel.getTransitionMatrix().getRowGroupIndices()[subsysState+1]; row = subsystemActions.getNextSetIndex(row+1)) {
                        bool allRowEntriesStayInSubsys = true;
                        for(auto const& entry : originalModel.getTransitionMatrix().getRow(row)) {
                            if(!subsystemStates.get(entry.getColumn())) {
                                allRowEntriesStayInSubsys = false;
                                break;
                            }
                        }
                        stateHasOneChoiceLeft |= allRowEntriesStayInSubsys;
                        result.keptActions.set(row, allRowEntriesStayInSubsys);
                    }
                     STORM_LOG_THROW(stateHasOneChoiceLeft, storm::exceptions::InvalidArgumentException, "The subsystem would contain a deadlock state.");
                }
                
                // Transform the ingedients of the model
                storm::storage::SparseMatrix<typename SparseModelType::ValueType> matrix = transformMatrix(originalModel.getTransitionMatrix(), subsystemStates, result.keptActions);
                storm::models::sparse::StateLabeling labeling(matrix.getRowGroupCount());
                for(auto const& label : originalModel.getStateLabeling().getLabels()){
                    storm::storage::BitVector newBitVectorForLabel = transformStateBitVector(originalModel.getStateLabeling().getStates(label), subsystemStates);
                    labeling.addLabel(label, std::move(newBitVectorForLabel));
                }
                std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
                for(auto const& rewardModel : originalModel.getRewardModels()){
                    rewardModels.insert(std::make_pair(rewardModel.first, transformRewardModel(rewardModel.second, subsystemStates, result.keptActions)));
                }
                boost::optional<std::vector<storm::models::sparse::LabelSet>> choiceLabeling;
                if(originalModel.hasChoiceLabeling()){
                    choiceLabeling = transformActionValueVector<storm::models::sparse::LabelSet>(originalModel.getChoiceLabeling(), result.keptActions);
                }
                result.model = std::make_shared<SparseModelType>(createTransformedModel(originalModel, subsystemStates, matrix, labeling, rewardModels, choiceLabeling));
                STORM_LOG_DEBUG("Subsystem Builder is done. Resulting model has " << result.model->getNumberOfStates() << " states.");
                return result;
            }
            
        private:
            template<typename ValueType = typename SparseModelType::ValueType, typename RewardModelType = typename SparseModelType::RewardModelType>
            static typename std::enable_if<std::is_same<RewardModelType, storm::models::sparse::StandardRewardModel<ValueType>>::value, RewardModelType>::type
            transformRewardModel(RewardModelType const& originalRewardModel, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& subsystemActions) {
                boost::optional<std::vector<ValueType>> stateRewardVector;
                boost::optional<std::vector<ValueType>> stateActionRewardVector;
                boost::optional<storm::storage::SparseMatrix<ValueType>> transitionRewardMatrix;
                if(originalRewardModel.hasStateRewards()){
                    stateRewardVector = transformStateValueVector(originalRewardModel.getStateRewardVector(), subsystem);
                }
                if(originalRewardModel.hasStateActionRewards()){
                    stateActionRewardVector = transformActionValueVector(originalRewardModel.getStateActionRewardVector(), subsystemActions);
                }
                if(originalRewardModel.hasTransitionRewards()){
                    transitionRewardMatrix = transformMatrix(originalRewardModel.getTransitionRewardMatrix(), subsystem, subsystemActions);
                }
                return RewardModelType(std::move(stateRewardVector), std::move(stateActionRewardVector), std::move(transitionRewardMatrix));
            }
            
            template<typename ValueType = typename SparseModelType::ValueType>
            static storm::storage::SparseMatrix<ValueType> transformMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& subsystemActions) {
                return originalMatrix.getSubmatrix(false, subsystemActions, subsystem);
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
