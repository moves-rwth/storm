#ifndef STORM_MODELS_SYMBOLIC_STANDARDREWARDMODEL_H_
#define STORM_MODELS_SYMBOLIC_STANDARDREWARDMODEL_H_

#include <boost/optional.hpp>

namespace storm {
    namespace models {
        namespace symbolic {
            
            template <storm::dd::DdType Type, typename ValueType>
            class StandardRewardModel {
                /*!
                 * Builds a reward model by copying with the given reward structures.
                 *
                 * @param stateRewardVector The state reward vector.
                 * @param stateActionRewardVector The vector of state-action rewards.
                 * @param transitionRewardMatrix The matrix of transition rewards.
                 */
                explicit StandardRewardModel(boost::optional<storm::dd::Add<Type>> const& stateRewardVector, boost::optional<storm::dd::Add<Type>> const& stateActionRewardVector, boost::optional<storm::dd::Add<Type>> const& transitionRewardMatrix);

            private:
                // The state reward vector.
                boost::optional<storm::dd::Add<Type>> stateRewardVector;
                
                // A vector of state-action-based rewards.
                boost::optional<storm::dd::Add<Type>> stateActionRewardVector;

                // A matrix of transition rewards.
                boost::optional<storm::dd::Add<Type>> transitionRewardMatrix;
            };
            
        }
    }
}

#endif /* STORM_MODELS_SYMBOLIC_STANDARDREWARDMODEL_H_ */