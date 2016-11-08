#include "src/modelchecker/multiobjective/SparseMaMultiObjectiveModelChecker.h"

#include "src/utility/macros.h"
#include "src/logic/Formulas.h"
#include "src/logic/FragmentSpecification.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/modelchecker/multiobjective/pcaa.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMaModelType>
        SparseMaMultiObjectiveModelChecker<SparseMaModelType>::SparseMaMultiObjectiveModelChecker(SparseMaModelType const& model) : SparseMarkovAutomatonCslModelChecker<SparseMaModelType>(model) {
            // Intentionally left empty.
        }
        
        template<typename SparseMaModelType>
        bool SparseMaMultiObjectiveModelChecker<SparseMaModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            // A formula without multi objective (sub)formulas can be handled by the base class
            if(SparseMarkovAutomatonCslModelChecker<SparseMaModelType>::canHandle(checkTask)) return true;
            //In general, each initial state requires an individual scheduler (in contrast to single objective model checking). Let's exclude this.
            if(this->getModel().getInitialStates().getNumberOfSetBits() > 1) return false;
            if(!checkTask.isOnlyInitialStatesRelevantSet()) return false;
            return checkTask.getFormula().isInFragment(storm::logic::multiObjective().setTimeAllowed(true).setTimeBoundedUntilFormulasAllowed(true));
        }
        
        template<typename SparseMaModelType>
        std::unique_ptr<CheckResult> SparseMaMultiObjectiveModelChecker<SparseMaModelType>::checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) {
            STORM_LOG_ASSERT(this->getModel().getInitialStates().getNumberOfSetBits() == 1, "Multi-objective Model checking on model with multiple initial states is not supported.");
            STORM_LOG_THROW(this->getModel().isClosed(), storm::exceptions::InvalidArgumentException, "Unable to check multi-objective formula in non-closed Markov automaton.");

            return multiobjective::performPcaa(this->getModel(), checkTask.getFormula());
        }
        
        
                
        template class SparseMaMultiObjectiveModelChecker<storm::models::sparse::MarkovAutomaton<double>>;
    //    template class SparseMaMultiObjectiveModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
    }
}
