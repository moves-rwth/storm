#ifndef STORM_MODELS_SYMBOLIC_NONDETERMINISTICMODEL_H_
#define STORM_MODELS_SYMBOLIC_NONDETERMINISTICMODEL_H_

#include "src/models/symbolic/Model.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            /*!
             * Base class for all nondeterministic symbolic models.
             */
            template<storm::dd::DdType Type, typename ValueType = double>
            class NondeterministicModel : public Model<Type, ValueType> {
            public:
                typedef typename Model<Type, ValueType>::RewardModelType RewardModelType;

                NondeterministicModel(NondeterministicModel<Type, ValueType> const& other) = default;
                NondeterministicModel& operator=(NondeterministicModel<Type, ValueType> const& other) = default;
                
#ifndef WINDOWS
                NondeterministicModel(NondeterministicModel<Type, ValueType>&& other) = default;
                NondeterministicModel& operator=(NondeterministicModel<Type, ValueType>&& other) = default;
#endif
                
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param modelType The type of the model.
                 * @param manager The manager responsible for the decision diagrams.
                 * @param reachableStates A DD representing the reachable states.
                 * @param initialStates A DD representing the initial states of the model.
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param rowVariables The set of row meta variables used in the DDs.
                 * @param rowExpressionAdapter An object that can be used to translate expressions in terms of the row
                 * meta variables.
                 * @param columVariables The set of column meta variables used in the DDs.
                 * @param columnExpressionAdapter An object that can be used to translate expressions in terms of the
                 * column meta variables.
                 * @param rowColumnMetaVariablePairs All pairs of row/column meta variables.
                 * @param nondeterminismVariables The meta variables used to encode the nondeterminism in the model.
                 * @param labelToExpressionMap A mapping from label names to their defining expressions.
                 * @param rewardModels The reward models associated with the model.
                 */
                NondeterministicModel(storm::models::ModelType const& modelType,
                                      std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                      storm::dd::Bdd<Type> reachableStates,
                                      storm::dd::Bdd<Type> initialStates,
                                      storm::dd::Add<Type, ValueType> transitionMatrix,
                                      std::set<storm::expressions::Variable> const& rowVariables,
                                      std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> rowExpressionAdapter,
                                      std::set<storm::expressions::Variable> const& columnVariables,
                                      std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> columnExpressionAdapter,
                                      std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                      std::set<storm::expressions::Variable> const& nondeterminismVariables,
                                      std::map<std::string, storm::expressions::Expression> labelToExpressionMap = std::map<std::string, storm::expressions::Expression>(),
                                      std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>());
                
                /*!
                 * Retrieves the number of nondeterministic choices in the model.
                 *
                 * @return The number of nondeterministic choices in the model.
                 */
                uint_fast64_t getNumberOfChoices() const;
                
                /*!
                 * Retrieves the meta variables used to encode the nondeterminism in the model.
                 *
                 * @return The meta variables used to encode the nondeterminism in the model.
                 */
                std::set<storm::expressions::Variable> const& getNondeterminismVariables() const;
                
                /*!
                 * Retrieves a BDD characterizing all illegal nondeterminism encodings in the model.
                 *
                 * @return A BDD characterizing all illegal nondeterminism encodings in the model.
                 */
                storm::dd::Bdd<Type> const& getIllegalMask() const;
                
                virtual void printModelInformationToStream(std::ostream& out) const override;
                
            protected:
            
                virtual void printDdVariableInformationToStream(std::ostream& out) const override;
                
            private:
                
                // The meta variables encoding the nondeterminism in the model.
                std::set<storm::expressions::Variable> nondeterminismVariables;
                
                // A mask that characterizes all legal nondeterministic choices.
                storm::dd::Bdd<Type> illegalMask;
            };
            
        } // namespace symbolic
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SYMBOLIC_NONDETERMINISTICMODEL_H_ */
