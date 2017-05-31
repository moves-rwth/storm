#pragma once

#include <boost/optional.hpp>

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/logic/Formula.h"

namespace storm {
    namespace transformer {

        
        // Transforms the given continuous model to a discrete time model.
        // If such a transformation does not preserve the given formula, the transformation does not take place and the original model is returned
        // Moreover, the given formula might be changed (e.g. TimeOperatorFormulas become RewardOperatorFormulas).
        template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> transformContinuousToDiscreteModel(std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> markovModel, std::shared_ptr<storm::logic::Formula const>& formula);
        
        // Transforms the given continuous model to a discrete time model IN PLACE (i.e., the continuous model will be invalidated).
        // If such a transformation does not preserve the given formula, the transformation does not take place.
        // Moreover, the given formula might be changed (e.g. TimeOperatorFormulas become RewardOperatorFormulas).
        template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        void transformContinuousToDiscreteModelInPlace(std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>& markovModel, std::shared_ptr<storm::logic::Formula const>& formula);
        
        template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class SparseCtmcToSparseDtmcTransformer {
        public:
            // Transforms the given CTMC to its underlying (aka embedded) DTMC.
            // A reward model for time is added if a corresponding reward model name is given
            static std::shared_ptr<storm::models::sparse::Dtmc<ValueType, RewardModelType>> translate(storm::models::sparse::Ctmc<ValueType, RewardModelType> const& ctmc, boost::optional<std::string> const& timeRewardModelName = boost::none);
            static std::shared_ptr<storm::models::sparse::Dtmc<ValueType, RewardModelType>> translate(storm::models::sparse::Ctmc<ValueType, RewardModelType>&& ctmc, boost::optional<std::string> const& timeRewardModelName = boost::none);
            
            // If this method returns true, the given formula is preserced by the transformation
            static bool transformationPreservesProperty(storm::logic::Formula const& formula);
        };
        
        template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class SparseMaToSparseMdpTransformer {
        public:
            // Transforms the given MA to its underlying (aka embedded) MDP.
            // A reward model for time is added if a corresponding reward model name is given
            static std::shared_ptr<storm::models::sparse::Mdp<ValueType, RewardModelType>> translate(storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType> const& ma, boost::optional<std::string> const& timeRewardModelName = boost::none);
            static std::shared_ptr<storm::models::sparse::Mdp<ValueType, RewardModelType>> translate(storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>&& ma, boost::optional<std::string> const& timeRewardModelName = boost::none);
            
            // If this method returns true, the given formula is preserved by the transformation
            static bool transformationPreservesProperty(storm::logic::Formula const& formula);
        };
    }
}
