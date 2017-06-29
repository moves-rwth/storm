#ifndef STORM_TRANSFORMER_SUBSYSTEMBUILDER_H
#define STORM_TRANSFORMER_SUBSYSTEMBUILDER_H


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
#include "storm/storage/sparse/ModelComponents.h"
#include "storm/utility/builder.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidStateException.h"

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
                if (subsystemStateCount == subsystemStates.size() && subsystemActions.full()) {
                    result.model = std::make_shared<SparseModelType>(originalModel);
                    result.newToOldStateIndexMapping = storm::utility::vector::buildVectorForRange(0, result.model->getNumberOfStates());
                    result.keptActions = storm::storage::BitVector(result.model->getTransitionMatrix().getRowCount(), true);
                    return result;
                }
                
                result.newToOldStateIndexMapping.reserve(subsystemStateCount);
                result.keptActions = storm::storage::BitVector(originalModel.getTransitionMatrix().getRowCount(), false);
                for (auto subsysState : subsystemStates) {
                    result.newToOldStateIndexMapping.push_back(subsysState);
                    bool stateHasOneChoiceLeft = false;
                    for (uint_fast64_t row = subsystemActions.getNextSetIndex(originalModel.getTransitionMatrix().getRowGroupIndices()[subsysState]); row < originalModel.getTransitionMatrix().getRowGroupIndices()[subsysState+1]; row = subsystemActions.getNextSetIndex(row+1)) {
                        bool allRowEntriesStayInSubsys = true;
                        for (auto const& entry : originalModel.getTransitionMatrix().getRow(row)) {
                            if (!subsystemStates.get(entry.getColumn())) {
                                allRowEntriesStayInSubsys = false;
                                break;
                            }
                        }
                        stateHasOneChoiceLeft |= allRowEntriesStayInSubsys;
                        result.keptActions.set(row, allRowEntriesStayInSubsys);
                    }
                     STORM_LOG_THROW(stateHasOneChoiceLeft, storm::exceptions::InvalidArgumentException, "The subsystem would contain a deadlock state.");
                }
                
                // Transform the components of the model
                storm::storage::sparse::ModelComponents<typename SparseModelType::ValueType, typename SparseModelType::RewardModelType> components;
                components.transitionMatrix = originalModel.getTransitionMatrix().getSubmatrix(false, result.keptActions, subsystemStates);
                components.stateLabeling = originalModel.getStateLabeling().getSubLabeling(subsystemStates);
                for (auto const& rewardModel : originalModel.getRewardModels()){
                    components.rewardModels.insert(std::make_pair(rewardModel.first, transformRewardModel(rewardModel.second, subsystemStates, result.keptActions)));
                }
                if (originalModel.hasChoiceLabeling()) {
                    components.choiceLabeling = originalModel.getChoiceLabeling().getSubLabeling(result.keptActions);
                }
                if (originalModel.hasStateValuations()) {
                    components.stateValuations = originalModel.getStateValuations().selectStates(subsystemStateCount);
                }
                if (originalModel.hasChoiceOrigins()) {
                    components.choiceOrigins = originalModel.getChoiceOrigins().selectChoices(result.keptActions);
                }
                
                transformModelSpecificComponents(originalModel, subsystemStates, components);
                
                result.model = storm::utility::builder::buildModelFromComponents(originalModel.getType(), std::move(components));
                STORM_LOG_DEBUG("Subsystem Builder is done. Resulting model has " << result.model->getNumberOfStates() << " states.");
                return result;
            }
            
        private:
            template<typename ValueType = typename SparseModelType::RewardModelType::ValueType, typename RewardModelType = typename SparseModelType::RewardModelType>
            static RewardModelType transformRewardModel(RewardModelType const& originalRewardModel, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& subsystemActions) {
                boost::optional<std::vector<ValueType>> stateRewardVector;
                boost::optional<std::vector<ValueType>> stateActionRewardVector;
                boost::optional<storm::storage::SparseMatrix<ValueType>> transitionRewardMatrix;
                if (originalRewardModel.hasStateRewards()){
                    stateRewardVector = storm::utility::vector::filterVector(originalRewardModel.getStateRewardVector(), subsystem);
                }
                if (originalRewardModel.hasStateActionRewards()){
                    stateActionRewardVector = storm::utility::vector::filterVector(originalRewardModel.getStateActionRewardVector(), subsystemActions);
                }
                if (originalRewardModel.hasTransitionRewards()){
                    transitionRewardMatrix = originalRewardModel.getTransitionRewardMatrix().getSubmatrix(false, subsystemActions, subsystem);
                }
                return RewardModelType(std::move(stateRewardVector), std::move(stateActionRewardVector), std::move(transitionRewardMatrix));
            }
            
            template<typename MT = SparseModelType>
            static typename std::enable_if<
            std::is_same<MT,storm::models::sparse::Dtmc<typename SparseModelType::ValueType>>::value ||
            std::is_same<MT,storm::models::sparse::Mdp<typename SparseModelType::ValueType>>::value,
            MT>::type
            transformModelSpecificComponents(MT const&,
                                   storm::storage::BitVector const& /*subsystem*/,
                                   storm::storage::sparse::ModelComponents<typename SparseModelType::ValueType, typename SparseModelType::RewardModelType>&) {
                // Intentionally left empty
            }
            
            template<typename MT = SparseModelType>
            static typename std::enable_if<
            std::is_same<MT,storm::models::sparse::Ctmc<typename SparseModelType::ValueType>>::value,
            MT>::type
            transformModelSpecificComponents(MT const& originalModel,
                                   storm::storage::BitVector const& subsystem,
                                   storm::storage::SparseMatrix<typename MT::ValueType>& matrix,
                                   storm::storage::sparse::ModelComponents<typename SparseModelType::ValueType, typename SparseModelType::RewardModelType>& components) {
                components.exitRates = storm::utility::vector::filterVector(originalModel.getExitRateVector(), subsystem);
                components.rateTransitions = true;
            }
        
            template<typename MT = SparseModelType>
            static typename std::enable_if<
            std::is_same<MT,storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType>>::value,
            MT>::type
            transformModelSpecificComponents(MT const& originalModel,
                                   storm::storage::BitVector const& subsystem,
                                   storm::storage::SparseMatrix<typename MT::ValueType>& matrix,
                                   storm::storage::sparse::ModelComponents<typename SparseModelType::ValueType, typename SparseModelType::RewardModelType>& components) {
                components.markovianStates = originalModel.getMarkovianStates() % subsystem;
                components.exitRates = storm::utility::vector::filterVector(originalModel.getExitRates(), subsystem);
                components.rateTransitions = false; // Note that originalModel.getTransitionMatrix() contains probabilities
            }
            
        };
    }
}
#endif // STORM_TRANSFORMER_SUBSYSTEMBUILDER_H
