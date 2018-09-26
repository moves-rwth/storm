#include "storm/transformer/SubsystemBuilder.h"

#include <boost/optional.hpp>
#include <storm/exceptions/UnexpectedException.h>

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
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace transformer {
        
        template <typename ValueType, typename RewardModelType>
        void transformModelSpecificComponents(storm::models::sparse::Model<ValueType, RewardModelType> const& originalModel,
                               storm::storage::BitVector const& subsystem,
                               storm::storage::sparse::ModelComponents<ValueType, RewardModelType>& components) {
            if (originalModel.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                auto const& ma = *originalModel.template as<storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>>();
                components.markovianStates = ma.getMarkovianStates() % subsystem;
                components.exitRates = storm::utility::vector::filterVector(ma.getExitRates(), subsystem);
                components.rateTransitions = false; // Note that originalModel.getTransitionMatrix() contains probabilities
            } else if (originalModel.isOfType(storm::models::ModelType::Ctmc)) {
                auto const& ctmc = *originalModel.template as<storm::models::sparse::Ctmc<ValueType, RewardModelType>>();
                components.exitRates = storm::utility::vector::filterVector(ctmc.getExitRateVector(), subsystem);
                components.rateTransitions = true;
            } else {
                STORM_LOG_THROW(originalModel.isOfType(storm::models::ModelType::Dtmc) || originalModel.isOfType(storm::models::ModelType::Mdp), storm::exceptions::UnexpectedException, "Unexpected model type.");
            }
        }
        
        template<typename RewardModelType>
        RewardModelType transformRewardModel(RewardModelType const& originalRewardModel, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& subsystemActions) {
            boost::optional<std::vector<typename RewardModelType::ValueType>> stateRewardVector;
            boost::optional<std::vector<typename RewardModelType::ValueType>> stateActionRewardVector;
            boost::optional<storm::storage::SparseMatrix<typename RewardModelType::ValueType>> transitionRewardMatrix;
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

        template <typename ValueType, typename RewardModelType>
        SubsystemBuilderReturnType<ValueType, RewardModelType> internalBuildSubsystem(storm::models::sparse::Model<ValueType, RewardModelType> const& originalModel, storm::storage::BitVector const& subsystemStates, storm::storage::BitVector const& subsystemActions) {

            SubsystemBuilderReturnType<ValueType, RewardModelType> result;
            uint_fast64_t subsystemStateCount = subsystemStates.getNumberOfSetBits();
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
            storm::storage::sparse::ModelComponents<ValueType, RewardModelType> components;
            components.transitionMatrix = originalModel.getTransitionMatrix().getSubmatrix(false, result.keptActions, subsystemStates);
            components.stateLabeling = originalModel.getStateLabeling().getSubLabeling(subsystemStates);
            for (auto const& rewardModel : originalModel.getRewardModels()){
                components.rewardModels.insert(std::make_pair(rewardModel.first, transformRewardModel(rewardModel.second, subsystemStates, result.keptActions)));
            }
            if (originalModel.hasChoiceLabeling()) {
                components.choiceLabeling = originalModel.getChoiceLabeling().getSubLabeling(result.keptActions);
            }
            if (originalModel.hasStateValuations()) {
                components.stateValuations = originalModel.getStateValuations().selectStates(subsystemStates);
            }
            if (originalModel.hasChoiceOrigins()) {
                components.choiceOrigins = originalModel.getChoiceOrigins()->selectChoices(result.keptActions);
            }
            
            transformModelSpecificComponents<ValueType, RewardModelType>(originalModel, subsystemStates, components);
            
            result.model = storm::utility::builder::buildModelFromComponents(originalModel.getType(), std::move(components));
            STORM_LOG_DEBUG("Subsystem Builder is done. Resulting model has " << result.model->getNumberOfStates() << " states.");
            return result;
        }
        
        template <typename ValueType, typename RewardModelType>
        SubsystemBuilderReturnType<ValueType, RewardModelType> buildSubsystem(storm::models::sparse::Model<ValueType, RewardModelType> const& originalModel, storm::storage::BitVector const& subsystemStates, storm::storage::BitVector const& subsystemActions, bool keepUnreachableStates) {
            STORM_LOG_DEBUG("Invoked subsystem builder on model with " << originalModel.getNumberOfStates() << " states.");
            storm::storage::BitVector initialStates = originalModel.getInitialStates() & subsystemStates;
            STORM_LOG_THROW(!initialStates.empty(), storm::exceptions::InvalidArgumentException, "The subsystem would not contain any initial states");
            
            STORM_LOG_THROW(!subsystemStates.empty(), storm::exceptions::InvalidArgumentException, "Invoked SubsystemBuilder for an empty subsystem.");
            if (keepUnreachableStates) {
                return internalBuildSubsystem(originalModel, subsystemStates, subsystemActions);
            } else {
                auto actualSubsystem = storm::utility::graph::getReachableStates(originalModel.getTransitionMatrix(), initialStates, subsystemStates, storm::storage::BitVector(subsystemStates.size(), false), false, 0, subsystemActions);
                return internalBuildSubsystem(originalModel, actualSubsystem, subsystemActions);
            }
        }
        
        template SubsystemBuilderReturnType<double> buildSubsystem(storm::models::sparse::Model<double> const& originalModel, storm::storage::BitVector const& subsystemStates, storm::storage::BitVector const& subsystemActions, bool keepUnreachableStates = true);
        template SubsystemBuilderReturnType<double, storm::models::sparse::StandardRewardModel<storm::Interval>> buildSubsystem(storm::models::sparse::Model<double, storm::models::sparse::StandardRewardModel<storm::Interval>> const& originalModel, storm::storage::BitVector const& subsystemStates, storm::storage::BitVector const& subsystemActions, bool keepUnreachableStates = true);
        template SubsystemBuilderReturnType<storm::RationalNumber> buildSubsystem(storm::models::sparse::Model<storm::RationalNumber> const& originalModel, storm::storage::BitVector const& subsystemStates, storm::storage::BitVector const& subsystemActions, bool keepUnreachableStates = true);
        template SubsystemBuilderReturnType<storm::RationalFunction> buildSubsystem(storm::models::sparse::Model<storm::RationalFunction> const& originalModel, storm::storage::BitVector const& subsystemStates, storm::storage::BitVector const& subsystemActions, bool keepUnreachableStates = true);
    }
}
