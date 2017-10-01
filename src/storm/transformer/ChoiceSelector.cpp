#include "storm/transformer/ChoiceSelector.h"
#include "storm/models/sparse/Mdp.h"

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
            return std::make_shared<storm::models::sparse::Mdp<ValueType, RewardModelType>>(std::move(newComponents));
        }

        template class ChoiceSelector<double>;
    }
}
