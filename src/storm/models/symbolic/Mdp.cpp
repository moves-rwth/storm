#include "storm/models/symbolic/Mdp.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type, typename ValueType>
            Mdp<Type, ValueType>::Mdp(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                      storm::dd::Bdd<Type> reachableStates,
                                      storm::dd::Bdd<Type> initialStates,
                                      storm::dd::Bdd<Type> deadlockStates,
                                      storm::dd::Add<Type, ValueType> transitionMatrix,
                                      std::set<storm::expressions::Variable> const& rowVariables,
                                      std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> rowExpressionAdapter,
                                      std::set<storm::expressions::Variable> const& columnVariables,
                                      std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                      std::set<storm::expressions::Variable> const& nondeterminismVariables,
                                      std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                                      std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : NondeterministicModel<Type, ValueType>(storm::models::ModelType::Mdp, manager, reachableStates, initialStates, deadlockStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, rowColumnMetaVariablePairs, nondeterminismVariables, labelToExpressionMap, rewardModels) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            Mdp<Type, ValueType>::Mdp(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                      storm::dd::Bdd<Type> reachableStates,
                                      storm::dd::Bdd<Type> initialStates,
                                      storm::dd::Bdd<Type> deadlockStates,
                                      storm::dd::Add<Type, ValueType> transitionMatrix,
                                      std::set<storm::expressions::Variable> const& rowVariables,
                                      std::set<storm::expressions::Variable> const& columnVariables,
                                      std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                      std::set<storm::expressions::Variable> const& nondeterminismVariables,
                                      std::map<std::string, storm::dd::Bdd<Type>> labelToBddMap,
                                      std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : NondeterministicModel<Type, ValueType>(storm::models::ModelType::Mdp, manager, reachableStates, initialStates, deadlockStates, transitionMatrix, rowVariables, columnVariables, rowColumnMetaVariablePairs, nondeterminismVariables, labelToBddMap, rewardModels) {
                // Intentionally left empty.
            }
            
            // Explicitly instantiate the template class.
            template class Mdp<storm::dd::DdType::CUDD, double>;
            template class Mdp<storm::dd::DdType::Sylvan, double>;
            
            template class Mdp<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class Mdp<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm
