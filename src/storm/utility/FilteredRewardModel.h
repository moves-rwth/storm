#pragma once

#include <memory>

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace utility {
        
        /*
         * This class wraps around a Reward model where certain reward types are disabled.
         */
        template <typename RewardModelType>
        class FilteredRewardModel {
        public:
            FilteredRewardModel(RewardModelType const& baseRewardModel, bool enableStateRewards, bool enableStateActionRewards, bool enableTransitionRewards) {
                if ((baseRewardModel.hasStateRewards() && !enableStateRewards) || (baseRewardModel.hasStateActionRewards() && !enableStateActionRewards) || (baseRewardModel.hasTransitionRewards() && !enableTransitionRewards)) {
                    // One of the available reward types need to be deleted.
                    typename std::remove_const<typename std::remove_reference<decltype(baseRewardModel.getOptionalStateRewardVector())>::type>::type stateRewards;
                    if (enableStateRewards) {
                        stateRewards = baseRewardModel.getOptionalStateRewardVector();
                    }
                    typename std::remove_const<typename std::remove_reference<decltype(baseRewardModel.getOptionalStateActionRewardVector())>::type>::type stateActionRewards;
                    if (enableStateActionRewards) {
                        stateActionRewards = baseRewardModel.getOptionalStateActionRewardVector();
                    }
                    typename std::remove_const<typename std::remove_reference<decltype(baseRewardModel.getOptionalTransitionRewardMatrix())>::type>::type transitionRewards;
                    if (enableTransitionRewards) {
                        transitionRewards = baseRewardModel.getOptionalTransitionRewardMatrix();
                    }
                    localRewardModel = std::unique_ptr<RewardModelType>(new RewardModelType(stateRewards, stateActionRewards, transitionRewards));
                    rewardModel = localRewardModel.get();
                } else {
                    rewardModel = &baseRewardModel;
                }
            }

            RewardModelType const& get() const {
                return *rewardModel;
            }
        private:
                std::unique_ptr<RewardModelType> localRewardModel;
                RewardModelType const* rewardModel;
        };
        
        template <typename RewardModelType, typename FormulaType>
        FilteredRewardModel<RewardModelType> createFilteredRewardModel(RewardModelType const& baseRewardModel, bool isDiscreteTimeModel, FormulaType const& formula) {
            if (formula.hasRewardAccumulation()) {
                auto const& acc = formula.getRewardAccumulation();
                STORM_LOG_THROW(isDiscreteTimeModel || !acc.isExitSet() || !baseRewardModel.hasStateRewards(), storm::exceptions::NotSupportedException, "Exit rewards for continuous time models are not supported.");
                // Check which of the available reward types are allowed.
                bool hasStateRewards = isDiscreteTimeModel ? acc.isExitSet() : acc.isTimeSet();
                bool hasStateActionRewards = acc.isStepsSet();
                bool hasTransitionRewards = acc.isStepsSet();
                return FilteredRewardModel<RewardModelType>(baseRewardModel, hasStateRewards, hasStateActionRewards, hasTransitionRewards);
            } else {
                return FilteredRewardModel<RewardModelType>(baseRewardModel, true, true, true);
            }
        }
        
        template <typename ModelType, typename CheckTaskType>
        FilteredRewardModel<typename ModelType::RewardModelType> createFilteredRewardModel(ModelType const& model, CheckTaskType const& checkTask) {
            auto const& baseRewardModel = checkTask.isRewardModelSet() ? model.getRewardModel(checkTask.getRewardModel()) : model.getUniqueRewardModel();
            return createFilteredRewardModel(baseRewardModel, model.isDiscreteTimeModel(), checkTask.getFormula());
        }
    }
}