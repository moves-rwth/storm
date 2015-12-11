#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddDdManager.h"

#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type>
        SymbolicPropositionalModelChecker<Type>::SymbolicPropositionalModelChecker(storm::models::symbolic::Model<Type> const& model) : model(model) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        bool SymbolicPropositionalModelChecker<Type>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPropositionalFormula();
        }
        
        template<storm::dd::DdType Type>
        std::unique_ptr<CheckResult> SymbolicPropositionalModelChecker<Type>::checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) {
            if (stateFormula.isTrueFormula()) {
                return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getReachableStates()));
            } else {
                return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getManager().getBddZero()));
            }
        }
        
        template<storm::dd::DdType Type>
        std::unique_ptr<CheckResult> SymbolicPropositionalModelChecker<Type>::checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) {
            STORM_LOG_THROW(model.hasLabel(stateFormula.getLabel()), storm::exceptions::InvalidPropertyException, "The property refers to unknown label '" << stateFormula.getLabel() << "'.");
            return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getStates(stateFormula.getLabel())));
        }
        
        template<storm::dd::DdType Type>
        std::unique_ptr<CheckResult> SymbolicPropositionalModelChecker<Type>::checkAtomicExpressionFormula(storm::logic::AtomicExpressionFormula const& stateFormula) {
            return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getStates(stateFormula.getExpression())));
        }
        
        template<storm::dd::DdType Type>
        storm::models::symbolic::Model<Type> const& SymbolicPropositionalModelChecker<Type>::getModel() const {
            return model;
        }
        
        template<storm::dd::DdType Type>
        template<typename ModelType>
        ModelType const& SymbolicPropositionalModelChecker<Type>::getModelAs() const {
            return dynamic_cast<ModelType const&>(model);
        }
        
        // Explicitly instantiate the template class.
        template storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD> const& SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD>::getModelAs() const;
        template storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD> const& SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD>::getModelAs() const;
        template storm::models::symbolic::Mdp<storm::dd::DdType::CUDD> const& SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD>::getModelAs() const;
        template class SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD>;
    }
}