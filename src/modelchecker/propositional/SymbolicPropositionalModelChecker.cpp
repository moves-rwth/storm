#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/DdManager.h"

#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type, typename ValueType>
        SymbolicPropositionalModelChecker<Type, ValueType>::SymbolicPropositionalModelChecker(storm::models::symbolic::Model<Type, ValueType> const& model) : model(model) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool SymbolicPropositionalModelChecker<Type, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPropositionalFormula();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicPropositionalModelChecker<Type, ValueType>::checkBooleanLiteralFormula(CheckTask<storm::logic::BooleanLiteralFormula> const& checkTask) {
            if (stateFormula.isTrueFormula()) {
                return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getReachableStates()));
            } else {
                return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getManager().getBddZero()));
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicPropositionalModelChecker<Type, ValueType>::checkAtomicLabelFormula(CheckTask<storm::logic::AtomicLabelFormula> const& checkTask) {
            STORM_LOG_THROW(model.hasLabel(stateFormula.getLabel()), storm::exceptions::InvalidPropertyException, "The property refers to unknown label '" << stateFormula.getLabel() << "'.");
            return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getStates(stateFormula.getLabel())));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicPropositionalModelChecker<Type, ValueType>::checkAtomicExpressionFormula(CheckTask<storm::logic::AtomicExpressionFormula> const& checkTask) {
            return std::unique_ptr<CheckResult>(new SymbolicQualitativeCheckResult<Type>(model.getReachableStates(), model.getStates(stateFormula.getExpression())));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::models::symbolic::Model<Type, ValueType> const& SymbolicPropositionalModelChecker<Type, ValueType>::getModel() const {
            return model;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        template<typename ModelType>
        ModelType const& SymbolicPropositionalModelChecker<Type, ValueType>::getModelAs() const {
            return dynamic_cast<ModelType const&>(model);
        }
        
        // Explicitly instantiate the template class.
        template storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double> const& SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD, double>::getModelAs() const;
        template storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD, double> const& SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD, double>::getModelAs() const;
        template storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double> const& SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD, double>::getModelAs() const;
        template class SymbolicPropositionalModelChecker<storm::dd::DdType::CUDD, double>;

        template storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double> const& SymbolicPropositionalModelChecker<storm::dd::DdType::Sylvan, double>::getModelAs() const;
        template storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, double> const& SymbolicPropositionalModelChecker<storm::dd::DdType::Sylvan, double>::getModelAs() const;
        template storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double> const& SymbolicPropositionalModelChecker<storm::dd::DdType::Sylvan, double>::getModelAs() const;
        template class SymbolicPropositionalModelChecker<storm::dd::DdType::Sylvan, double>;

    }
}