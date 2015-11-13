#include "src/models/symbolic/DeterministicModel.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type, typename ValueType>
            DeterministicModel<Type, ValueType>::DeterministicModel(storm::models::ModelType const& modelType,
                                                                    std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                                                    storm::dd::Bdd<Type> reachableStates,
                                                                    storm::dd::Bdd<Type> initialStates,
                                                                    storm::dd::Add<Type, ValueType> transitionMatrix,
                                                                    std::set<storm::expressions::Variable> const& rowVariables,
                                                                    std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> rowExpressionAdapter,
                                                                    std::set<storm::expressions::Variable> const& columnVariables,
                                                                    std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> columnExpressionAdapter,
                                                                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                                                    std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                                                                    std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : Model<Type>(modelType, manager, reachableStates, initialStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, labelToExpressionMap, rewardModels) {
                // Intentionally left empty.
            }
            
            // Explicitly instantiate the template class.
            template class DeterministicModel<storm::dd::DdType::CUDD>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm