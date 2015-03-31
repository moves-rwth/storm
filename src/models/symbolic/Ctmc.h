#ifndef STORM_MODELS_SYMBOLIC_CTMC_H_
#define STORM_MODELS_SYMBOLIC_CTMC_H_

#include "src/models/symbolic/DeterministicModel.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            /*!
             * This class represents a continuous-time Markov chain.
             */
            template<storm::dd::DdType Type>
            class Ctmc : public DeterministicModel<Type> {
            public:
                Ctmc(Ctmc<Type> const& other) = default;
                Ctmc& operator=(Ctmc<Type> const& other) = default;
                
#ifndef WINDOWS
                Ctmc(Ctmc<Type>&& other) = default;
                Ctmc& operator=(Ctmc<Type>&& other) = default;
#endif
                
                /*!
                 * Constructs a model from the given data.
                 *
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
                 * @param labelToExpressionMap A mapping from label names to their defining expressions.
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 */
                Ctmc(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                     storm::dd::Bdd<Type> reachableStates,
                     storm::dd::Bdd<Type> initialStates,
                     storm::dd::Add<Type> transitionMatrix,
                     std::set<storm::expressions::Variable> const& rowVariables,
                     std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter,
                     std::set<storm::expressions::Variable> const& columnVariables,
                     std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter,
                     std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                     std::map<std::string, storm::expressions::Expression> labelToExpressionMap = std::map<std::string, storm::expressions::Expression>(),
                     boost::optional<storm::dd::Add<Type>> const& optionalStateRewardVector = boost::optional<storm::dd::Dd<Type>>(),
                     boost::optional<storm::dd::Add<Type>> const& optionalTransitionRewardMatrix = boost::optional<storm::dd::Dd<Type>>());
            };
            
        } // namespace symbolic
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SYMBOLIC_CTMC_H_ */
