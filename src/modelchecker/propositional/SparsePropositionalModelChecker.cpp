#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"

#include "src/adapters/CarlAdapter.h"

#include "src/models/Dtmc.h"
#include "src/models/Mdp.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparsePropositionalModelChecker<ValueType>::SparsePropositionalModelChecker(storm::models::AbstractModel<ValueType> const& model) : model(model) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparsePropositionalModelChecker<ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPropositionalFormula();
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparsePropositionalModelChecker<ValueType>::checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) {
            if (stateFormula.isTrueFormula()) {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates(), true)));
            } else {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates())));
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparsePropositionalModelChecker<ValueType>::checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) {
            STORM_LOG_THROW(model.hasAtomicProposition(stateFormula.getLabel()), storm::exceptions::InvalidPropertyException, "The property refers to unknown label '" << stateFormula.getLabel() << "'.");
            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(model.getLabeledStates(stateFormula.getLabel())));
        }
        
        template<typename ValueType>
        storm::models::AbstractModel<ValueType> const& SparsePropositionalModelChecker<ValueType>::getModel() const {
            return model;
        }
        
        template<typename ValueType>
        template<typename ModelType>
        ModelType const& SparsePropositionalModelChecker<ValueType>::getModelAs() const {
            return dynamic_cast<ModelType const&>(model);
        }
        
        // Explicitly instantiate the template class.
        template storm::models::Dtmc<double> const& SparsePropositionalModelChecker<double>::getModelAs() const;
        template storm::models::Mdp<double> const& SparsePropositionalModelChecker<double>::getModelAs() const;
        template class SparsePropositionalModelChecker<double>;
        
#ifdef STORM_HAVE_CARL
        template storm::models::Dtmc<storm::RationalFunction> const& SparsePropositionalModelChecker<storm::RationalFunction>::getModelAs() const;
        template storm::models::Mdp<storm::RationalFunction> const& SparsePropositionalModelChecker<storm::RationalFunction>::getModelAs() const;
        template class SparsePropositionalModelChecker<storm::RationalFunction>;
#endif
    }
}