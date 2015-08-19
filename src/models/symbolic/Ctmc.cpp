#include "src/models/symbolic/Ctmc.h"


#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type>
            Ctmc<Type>::Ctmc(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                             storm::dd::Bdd<Type> reachableStates,
                             storm::dd::Bdd<Type> initialStates,
                             storm::dd::Add<Type> transitionMatrix,
                             std::set<storm::expressions::Variable> const& rowVariables,
                             std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter,
                             std::set<storm::expressions::Variable> const& columnVariables,
                             std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter,
                             std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                             std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                             boost::optional<storm::dd::Add<Type>> const& optionalStateRewardVector,
                             boost::optional<storm::dd::Add<Type>> const& optionalTransitionRewardMatrix)
            : DeterministicModel<Type>(storm::models::ModelType::Ctmc, manager, reachableStates, initialStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, labelToExpressionMap, optionalStateRewardVector, optionalTransitionRewardMatrix) {
                exitRates = this->getTransitionMatrix().sumAbstract(this->getColumnVariables());
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Add<Type> const& Ctmc<Type>::getExitRateVector() const {
                return exitRates;
            }
            
            // Explicitly instantiate the template class.
            template class Ctmc<storm::dd::DdType::CUDD>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm