#ifndef STORM_MODELS_SYMBOLIC_MDP_H_
#define STORM_MODELS_SYMBOLIC_MDP_H_

#include "src/models/symbolic/NondeterministicModel.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            /*!
             * This class represents a discrete-time Markov decision process.
             */
            template<storm::dd::DdType Type>
            class Mdp : public NondeterministicModel<Type> {
            public:
                Mdp(Mdp<Type> const& other) = default;
                Mdp& operator=(Mdp<Type> const& other) = default;
                
#ifndef WINDOWS
                Mdp(Mdp<Type>&& other) = default;
                Mdp& operator=(Mdp<Type>&& other) = default;
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
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 */
                Mdp(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                    storm::dd::Dd<Type> reachableStates,
                    storm::dd::Dd<Type> initialStates,
                    storm::dd::Dd<Type> transitionMatrix,
                    std::set<storm::expressions::Variable> const& rowVariables,
                    std::shared_ptr<storm::adapters::DdExpressionAdapter<Type>> rowExpressionAdapter,
                    std::set<storm::expressions::Variable> const& columnVariables,
                    std::shared_ptr<storm::adapters::DdExpressionAdapter<Type>> columnExpressionAdapter,
                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                    std::set<storm::expressions::Variable> const& nondeterminismVariables,
                    std::map<std::string, storm::expressions::Expression> labelToExpressionMap = std::map<std::string, storm::expressions::Expression>(),
                    boost::optional<storm::dd::Dd<Type>> const& optionalStateRewardVector = boost::optional<storm::dd::Dd<Type>>(),
                    boost::optional<storm::dd::Dd<Type>> const& optionalTransitionRewardMatrix = boost::optional<storm::dd::Dd<Type>>());
                
            };
            
        } // namespace symbolic
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SYMBOLIC_MDP_H_ */
