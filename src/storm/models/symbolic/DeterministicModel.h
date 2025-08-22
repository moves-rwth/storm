#ifndef STORM_MODELS_SYMBOLIC_DETERMINISTICMODEL_H_
#define STORM_MODELS_SYMBOLIC_DETERMINISTICMODEL_H_

#include "storm/models/symbolic/Model.h"

namespace storm {
namespace models {
namespace symbolic {

/*!
 * Base class for all deterministic symbolic models.
 */
template<storm::dd::DdType Type, typename ValueType = double>
class DeterministicModel : public Model<Type, ValueType> {
   public:
    typedef typename Model<Type, ValueType>::RewardModelType RewardModelType;

    DeterministicModel(DeterministicModel<Type, ValueType> const& other) = default;
    DeterministicModel& operator=(DeterministicModel<Type, ValueType> const& other) = default;
    DeterministicModel(DeterministicModel<Type, ValueType>&& other) = default;
    DeterministicModel& operator=(DeterministicModel<Type, ValueType>&& other) = default;

    /*!
     * Constructs a model from the given data.
     *
     * @param modelType The type of the model.
     * @param manager The manager responsible for the decision diagrams.
     * @param reachableStates A DD representing the reachable states.
     * @param initialStates A DD representing the initial states of the model.
     * @param deadlockStates A DD representing the deadlock states of the model.
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param rowVariables The set of row meta variables used in the DDs.
     * @param rowExpressionAdapter An object that can be used to translate expressions in terms of the row
     * meta variables.
     * @param columVariables The set of column meta variables used in the DDs.
     * @param rowColumnMetaVariablePairs All pairs of row/column meta variables.
     * @param labelToExpressionMap A mapping from label names to their defining expressions.
     * @param rewardModels The reward models associated with the model.
     */
    DeterministicModel(storm::models::ModelType const& modelType, std::shared_ptr<storm::dd::DdManager<Type>> manager, storm::dd::Bdd<Type> reachableStates,
                       storm::dd::Bdd<Type> initialStates, storm::dd::Bdd<Type> deadlockStates, storm::dd::Add<Type, ValueType> transitionMatrix,
                       std::set<storm::expressions::Variable> const& rowVariables,
                       std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> rowExpressionAdapter,
                       std::set<storm::expressions::Variable> const& columnVariables,
                       std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                       std::map<std::string, storm::expressions::Expression> labelToExpressionMap = std::map<std::string, storm::expressions::Expression>(),
                       std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>());

    /*!
     * Constructs a model from the given data.
     *
     * @param modelType The type of the model.
     * @param manager The manager responsible for the decision diagrams.
     * @param reachableStates A DD representing the reachable states.
     * @param initialStates A DD representing the initial states of the model.
     * @param deadlockStates A DD representing the deadlock states of the model.
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param rowVariables The set of row meta variables used in the DDs.
     * @param columVariables The set of column meta variables used in the DDs.
     * @param rowColumnMetaVariablePairs All pairs of row/column meta variables.
     * @param labelToBddMap A mapping from label names to their defining BDDs.
     * @param rewardModels The reward models associated with the model.
     */
    DeterministicModel(storm::models::ModelType const& modelType, std::shared_ptr<storm::dd::DdManager<Type>> manager, storm::dd::Bdd<Type> reachableStates,
                       storm::dd::Bdd<Type> initialStates, storm::dd::Bdd<Type> deadlockStates, storm::dd::Add<Type, ValueType> transitionMatrix,
                       std::set<storm::expressions::Variable> const& rowVariables, std::set<storm::expressions::Variable> const& columnVariables,
                       std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                       std::map<std::string, storm::dd::Bdd<Type>> labelToBddMap = std::map<std::string, storm::dd::Bdd<Type>>(),
                       std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>());
};

}  // namespace symbolic
}  // namespace models
}  // namespace storm

#endif /* STORM_MODELS_SYMBOLIC_DETERMINISTICMODEL_H_ */
