#include "src/models/symbolic/Mdp.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type>
            Mdp<Type>::Mdp(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                           storm::dd::Bdd<Type> reachableStates,
                           storm::dd::Bdd<Type> initialStates,
                           storm::dd::Add<Type> transitionMatrix,
                           std::set<storm::expressions::Variable> const& rowVariables,
                           std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter,
                           std::set<storm::expressions::Variable> const& columnVariables,
                           std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter,
                           std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                           std::set<storm::expressions::Variable> const& nondeterminismVariables,
                           std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                           boost::optional<storm::dd::Add<Type>> const& optionalStateRewardVector,
                           boost::optional<storm::dd::Add<Type>> const& optionalTransitionRewardMatrix)
            : NondeterministicModel<Type>(storm::models::ModelType::Mdp, manager, reachableStates, initialStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, nondeterminismVariables, labelToExpressionMap, optionalStateRewardVector, optionalTransitionRewardMatrix) {
                // Intentionally left empty.
            }
            
            // Explicitly instantiate the template class.
            template class Mdp<storm::dd::DdType::CUDD>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm