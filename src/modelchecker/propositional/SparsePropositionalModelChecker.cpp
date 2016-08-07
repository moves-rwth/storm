#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"

#include "src/adapters/CarlAdapter.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseModelType>
        SparsePropositionalModelChecker<SparseModelType>::SparsePropositionalModelChecker(SparseModelType const& model) : model(model) {
            // Intentionally left empty.
        }
        
        template<typename SparseModelType>
        bool SparsePropositionalModelChecker<SparseModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::propositional());
        }
        
        template<typename SparseModelType>
        std::unique_ptr<CheckResult> SparsePropositionalModelChecker<SparseModelType>::checkBooleanLiteralFormula(CheckTask<storm::logic::BooleanLiteralFormula, ValueType> const& checkTask) {
            storm::logic::BooleanLiteralFormula const& stateFormula = checkTask.getFormula();
            if (stateFormula.isTrueFormula()) {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates(), true)));
            } else {
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(storm::storage::BitVector(model.getNumberOfStates())));
            }
        }
        
        template<typename SparseModelType>
        std::unique_ptr<CheckResult> SparsePropositionalModelChecker<SparseModelType>::checkAtomicLabelFormula(CheckTask<storm::logic::AtomicLabelFormula, ValueType> const& checkTask) {
            storm::logic::AtomicLabelFormula const& stateFormula = checkTask.getFormula();
            STORM_LOG_THROW(model.hasLabel(stateFormula.getLabel()), storm::exceptions::InvalidPropertyException, "The property refers to unknown label '" << stateFormula.getLabel() << "'.");
            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(model.getStates(stateFormula.getLabel())));
        }
        
        template<typename SparseModelType>
        SparseModelType const& SparsePropositionalModelChecker<SparseModelType>::getModel() const {
            return model;
        }
        
        // Explicitly instantiate the template class.
        template class SparsePropositionalModelChecker<storm::models::sparse::Model<double>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Dtmc<double>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Ctmc<double>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Mdp<double>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::MarkovAutomaton<double>>;
        
#ifdef STORM_HAVE_CARL
        template class SparsePropositionalModelChecker<storm::models::sparse::Mdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>>;

        template class SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalNumber>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Dtmc<storm::RationalNumber>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Ctmc<storm::RationalNumber>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;

        template class SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Ctmc<storm::RationalFunction>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>>;
        template class SparsePropositionalModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalFunction>>;
#endif
    }
}
