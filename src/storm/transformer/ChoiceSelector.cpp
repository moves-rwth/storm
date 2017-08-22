#include <storm/exceptions/NotImplementedException.h>
#include "storm/transformer/ChoiceSelector.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"

namespace  storm {
    namespace transformer {
        template <typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::models::sparse::NondeterministicModel<ValueType, RewardModelType>> ChoiceSelector<ValueType, RewardModelType>::transform(storm::storage::BitVector const& enabledActions) const
        {
            storm::storage::sparse::ModelComponents<ValueType, RewardModelType> newComponents(inputModel.getTransitionMatrix().restrictRows(enabledActions));
            newComponents.stateLabeling = inputModel.getStateLabeling();
            for (auto const& rewardModel : inputModel.getRewardModels()) {
                newComponents.rewardModels.emplace(rewardModel.first, rewardModel.second.restrictActions(enabledActions));
            }
            if (inputModel.hasChoiceLabeling()) {
                newComponents.choiceLabeling = inputModel.getChoiceLabeling().getSubLabeling(enabledActions);
            }
            newComponents.stateValuations = inputModel.getOptionalStateValuations();
            if (inputModel.hasChoiceOrigins()) {
                newComponents.choiceOrigins = inputModel.getChoiceOrigins()->selectChoices(enabledActions);
            }
            STORM_LOG_THROW(inputModel.getType() != storm::models::ModelType::MarkovAutomaton, storm::exceptions::NotImplementedException, "Selecting choices is not implemented for MA.");
            if(inputModel.getType() == storm::models::ModelType::Pomdp) {
                newComponents.observabilityClasses = static_cast<storm::models::sparse::Pomdp<ValueType,RewardModelType> const&>(inputModel).getObservations();
                return std::make_shared<storm::models::sparse::Pomdp<ValueType, RewardModelType>>(std::move(newComponents));
            } else {
                return std::make_shared<storm::models::sparse::Mdp<ValueType, RewardModelType>>(std::move(newComponents));
            }

        }

        template class ChoiceSelector<double>;
        template class ChoiceSelector<storm::RationalNumber>;
    }
}
