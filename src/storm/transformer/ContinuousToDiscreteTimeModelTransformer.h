#pragma once

#include <boost/optional.hpp>

#include "storm/logic/Formula.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
namespace transformer {

template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
class ContinuousToDiscreteTimeModelTransformer {
   public:
    // If this method returns true, the given formula is preserved by the transformation
    static bool preservesFormula(storm::logic::Formula const& formula);

    // Checks whether the given formulas are preserved.
    // Expected time formulas are translated to expected reward formulas.
    // The returned vector only contains formulas that are preserved by the transformation.
    static std::vector<std::shared_ptr<storm::logic::Formula const>> checkAndTransformFormulas(
        std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, std::string const& timeRewardName);

    // Transforms the given CTMC to its underlying (aka embedded) DTMC.
    // A reward model for time is added if a corresponding reward model name is given
    static std::shared_ptr<storm::models::sparse::Dtmc<ValueType, RewardModelType>> transform(
        storm::models::sparse::Ctmc<ValueType, RewardModelType> const& ctmc, boost::optional<std::string> const& timeRewardModelName = boost::none);
    static std::shared_ptr<storm::models::sparse::Dtmc<ValueType, RewardModelType>> transform(
        storm::models::sparse::Ctmc<ValueType, RewardModelType>&& ctmc, boost::optional<std::string> const& timeRewardModelName = boost::none);

    // Transforms the given MA to its underlying (aka embedded) MDP.
    // A reward model for time is added if a corresponding reward model name is given
    static std::shared_ptr<storm::models::sparse::Mdp<ValueType, RewardModelType>> transform(
        storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType> const& ma, boost::optional<std::string> const& timeRewardModelName = boost::none);
    static std::shared_ptr<storm::models::sparse::Mdp<ValueType, RewardModelType>> transform(
        storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>&& ma, boost::optional<std::string> const& timeRewardModelName = boost::none);
};
}  // namespace transformer
}  // namespace storm
