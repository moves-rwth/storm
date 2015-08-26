#include "src/models/symbolic/Dtmc.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

#include "src/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type>
            Dtmc<Type>::Dtmc(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                             storm::dd::Bdd<Type> reachableStates,
                             storm::dd::Bdd<Type> initialStates,
                             storm::dd::Add<Type> transitionMatrix,
                             std::set<storm::expressions::Variable> const& rowVariables,
                             std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter,
                             std::set<storm::expressions::Variable> const& columnVariables,
                             std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter,
                             std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                             std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                             std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : DeterministicModel<Type>(storm::models::ModelType::Dtmc, manager, reachableStates, initialStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, labelToExpressionMap, rewardModels) {
                // Intentionally left empty.
            }
            
            // Explicitly instantiate the template class.
            template class Dtmc<storm::dd::DdType::CUDD>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm